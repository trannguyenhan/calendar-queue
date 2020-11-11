#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "node.c"

typedef struct CalendarQueue {
    node** buckets;
    long int width;
    long int nbuckets;
    long int resizeenable;
    long int qsize;
    long int lastprio;
    long int lastbucket;
    long int buckettop;
    long int bot_threshold;
    long int top_threshold;
} CalendarQueue;

void insert(CalendarQueue* q, node* entry);
node* removeFirst(CalendarQueue* q);
double newwidth(CalendarQueue* q);
void resize(CalendarQueue* q, long int newsize);
void localInit(CalendarQueue* q, long int nbuck, long int bwidth, long int startprio);
CalendarQueue* initqueue();
void enqueue(CalendarQueue* q, node* entry);
node* dequeue(CalendarQueue* q);
void printBucket(node* n);
void printBuckets(CalendarQueue* q);


void insert(CalendarQueue* q, node* entry){
    long int priority = entry->endTime;

    // i la vi tri bucket ma entry chen vao
    long int i;
    i = priority / q->width;
    i = i % q->nbuckets;

    // tren i vao vi tri hop li tren buckets[i]
    if(q->buckets[i] == NULL || q->buckets[i]->endTime >= priority){
        entry->next = q->buckets[i];
        q->buckets[i] = entry;
    } else {
        node* current = q->buckets[i];
        while(current->next != NULL ){
            if(current->next->endTime < priority)
                current = current->next;
            else break;
        }

        entry->next = current->next;
        current->next = entry;
    }

    if(priority < q->lastprio){
        long int n = priority / q->width;
        q->buckettop = (n+1)*q->width /*+ 0.5*q->width*/;
    }

    // cap nhat qsize : so event cua hang doi
    q->qsize++;

    return;
}

node* removeFirst(CalendarQueue* q){
    long int i;
    if(q->qsize == 0) return NULL;

    i = q->lastbucket;
    while(1){
        if(q->buckets[i] != NULL && q->buckets[i]->endTime < q->buckettop){
            node* tmp = q->buckets[i];
            q->buckets[i] = tmp->next;

            q->lastbucket = i;
            q->lastprio = tmp->endTime;
            q->qsize--;

            return tmp;
        } else {
            i++; if(i==q->nbuckets) i=0;
            q->buckettop += q->width;
            if(i == q->lastbucket) break;
        }
    }

    // neu khong tim thay gia tri nho nhat trong nam
    // quay lai tim gia tri nho nhat trong tat ca cac gia tri dau cua buckets
    long int minbucket;
    double minpri;

    // start : vi tri dau tien buckets[i] != NULL
    long int start;
    for(start=0; start<q->nbuckets; start++)
        if(q->buckets[start] != NULL){
            q->lastbucket = start;
            q->lastprio = q->buckets[start]->endTime;
            minpri = q->buckets[start]->endTime;
            minbucket = start;
            break;
        }

    // tim vi tri buckets[i] != NULL ma nho nhat
    for(long int i = start+1; i<q->nbuckets; i++)
        if(q->buckets[i] != NULL){
            if(q->buckets[i]->endTime < minpri){
                q->lastbucket = i;
                q->lastprio = q->buckets[i]->endTime;
                minpri = q->buckets[i]->endTime;
                minbucket = i;
            }
        }

    node* foo = q->buckets[minbucket];
    q->buckets[minbucket] = foo->next;

    long int n = q->lastprio / q->width;
    q->buckettop = (n+1) * q->width /*+ 0.5*q->width*/;
    q->qsize--;

    return foo;
}

double newwidth(CalendarQueue* q){
    int nsamples;

    if(q->qsize < 2) return 1.0;
    if(q->qsize <= 5)
        nsamples = q->qsize;
    else
        nsamples = 5 + q->qsize/10;

    if(nsamples > 25) nsamples = 25;

    long int oldlastprio = q->lastprio;
    long int oldlastbucket = q->lastbucket;
    long int oldbuckkettop = q->buckettop;


    // lay ra nsamples gia tri mau
    // luc lay ra mau ngan chan viec resize, resizeenable = false
    q->resizeenable = 0;
    node* save = (node*) calloc(nsamples,sizeof(node));
    for(long int i=0; i<nsamples; i++){
        node* tmp = removeFirst(q);
        save[i] = *tmp;
    }
    q->resizeenable = 1;

    //  tra lai cac gia tri da lay ra trong hang doi
    for(long int i=0; i<nsamples; i++){
        insert(q,&save[i]);
    }
    q->lastprio = oldlastprio;
    q->lastbucket = oldlastbucket;
    q->buckettop = oldbuckkettop;

    // tinh toan gia tri cho new witdh
    long int totalSeparation = 0;
    long int end = nsamples;
    long int cur = 0;
    long int next = cur + 1;
    while(next != end){
        totalSeparation += save[next].endTime - save[cur].endTime;
        cur++;
        next++;
    }
    long int twiceAvg = totalSeparation / (nsamples - 1) * 2 + 1;

    totalSeparation = 0;
    end = nsamples;
    cur = 0;
    next = cur + 1;
    while(next != end){
        long int diff = save[next].endTime - save[cur].endTime;
        if(diff <= twiceAvg){
            totalSeparation += diff;
        }
        cur++;
        next++;
    }

    // gia tri width moi = 3 lan do phan tach gia tri trung binh
    totalSeparation *= 3;
    totalSeparation = totalSeparation<1? 1 : totalSeparation;

    return totalSeparation;
}

void resize(CalendarQueue* q, long int newsize){
    long int bwidth;
    long int i;
    long int oldnbuckets;
    node** oldbuckets;

    if(!q->resizeenable) return;

    bwidth = newwidth(q);
    oldbuckets = q->buckets;
    oldnbuckets = q->nbuckets;

    localInit(q,newsize,bwidth,q->lastprio);

    // them lai cac phan tu vao calendar moi
    for(long int i=0; i<oldnbuckets; i++){
        node* foo = oldbuckets[i];
        while(foo!=NULL){ // tranh vien lap vo han
            node* tmp = new_node(foo->type,foo->idElementInGroup,foo->portID,foo->endTime);
            insert(q,tmp);
            foo = foo->next;
        }
    }

    free(oldbuckets);
    return;
}

void localInit(CalendarQueue* q, long int nbuck, long int bwidth, long int startprio){
    long int i;
    long int n;

    // khoi tao cac tham so
    q->buckets = (node**) calloc(nbuck,sizeof(node));
    //buckets = malloc(sizeof * buckets * nbuckets);
    q->width = bwidth;
    q->nbuckets = nbuck;

    // khoi tao cac bucket
    q->qsize = 0;
    for(long int i=0; i<q->nbuckets; i++){
        q->buckets[i] = NULL;
    }

    // khoi tao cac chi so ban dau cua bucket dau tien
    q->lastprio = startprio;
    n = startprio / q->width;
    q->lastbucket = n % q->nbuckets;
    q->buckettop = (n+1)*q->width /*+ 0.5*q->width*/;

    // khoi tao 2 linh canh dau vao cuoi
    q->bot_threshold = q->nbuckets/2 - 2;
    q->top_threshold = 2*q->nbuckets;
}

CalendarQueue* initqueue(){
    CalendarQueue* init_q = (CalendarQueue*)malloc(sizeof(CalendarQueue));
    localInit(init_q,2,1,0);
    init_q->resizeenable = 1;
    return init_q;
}

// enqueue
void enqueue(CalendarQueue* q, node* entry){
    insert(q,entry);

    // nhan doi so luong calendar neu can
    if(q->qsize>q->top_threshold) resize(q,2*q->nbuckets);
}

// dequeue
node* dequeue(CalendarQueue* q){
    node* tmp = removeFirst(q);

    /*thu hep so luong cua calendar neu can*/
    if(q->qsize < q->bot_threshold) resize(q,q->nbuckets/2);
    return tmp;
}

/*in ra man hinh lich*/
void printBucket(node* n){
    while(n!=NULL){
        printf("%.d ",n->endTime);
        n = n->next;
    }
    return;
}
void printBuckets(CalendarQueue* q){
    for(long int i=0; i<q->nbuckets; i++){
        printf("Day %d : ",i);
        node* tmp = q->buckets[i];
        printBucket(tmp);
        printf("\n");
    }
    printf("\nCount of event : %d\n",q->qsize);
    printf("so luong bucket : %d\n",q->nbuckets);
    printf("buckettop : %.d\n",q->buckettop);
    printf("lastbuckket : %d\n",q->lastbucket);
    printf("lastprio : %d\n",q->lastprio);
    printf("width : %d\n",q->width);
    printf("bot : %d\n",q->bot_threshold);
    printf("top : %d",q->top_threshold);
}

/*
int main(){
    CalendarQueue* q = initqueue();
    enqueue(q,new_node(A,0,0,16));
    enqueue(q,new_node(A,0,0,15));
    enqueue(q,new_node(A,0,0,17));
    printf("%.d \n",dequeue(q)->endTime);
    enqueue(q,new_node(A,0,0,13));
    printf("%.d \n",dequeue(q)->endTime);
    enqueue(q,new_node(A,0,0,12));
    enqueue(q,new_node(A,0,0,18));
    enqueue(q,new_node(A,0,0,20));
    enqueue(q,new_node(A,0,0,25));
    enqueue(q,new_node(A,0,0,34));
    enqueue(q,new_node(A,0,0,10));
    enqueue(q,new_node(A,0,0,19));
    //enqueue(new_node(A,0,0,20.7));
    //if(qsize>top_threshold) resize(2*nbuckets);
    //enqueue(new_node(A,0,0,13.7));
    //resize(nbuckets*2);
    //printf("%.1f \n",newwidth());
    printf("%d \n",dequeue(q)->endTime);
    printf("%d \n",dequeue(q)->endTime);
    enqueue(q,new_node(A,0,0,29));
    //printf("%.1f \n",dequeue()->endTime);
    //printf("%.1f \n",dequeue()->endTime);
    //printf("%.1f \n",dequeue()->endTime);
    //printf("%.1f \n",dequeue()->endTime);
    printBuckets(q);
}
*/
