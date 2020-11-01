#include <stdio.h>
#include <stdlib.h>
#include "time.c"
const int QSPACE = 15000;

node** a;
node** buckets;
int nbuckets; // so luong cua bucket
double width; // thoi gian gia dinh cho 1 ngay
int qsize;  // so luong event trong hang doi
int lastBucket; // vi tri bucket cuoi cung ma event duoc xoa khoi hang doi
double lastPrio; // khoa cuoi cung ma event bi xoa khoi hang doi
double bucketTop; // khoa cao nhat trong bucket do
int top_threshold; // chan so luong dau tren
int bot_threshold; // chan so luong dau duoi
int firstsub;
int resizeenabled; // true or false : cho phep cho resize hay khong

void enqueue(node* entry){
    double priority = entry->endTime;

    int i;
    i = priority / width;
    i = i % nbuckets; // => i duoc chen vao bucket thu i

    if(buckets[i] == NULL){
        buckets[i] = entry;
    } else {
        node* tmp = buckets[i];
        if(tmp->endTime > priority){ // neu header->pri > pri thi thay the entry vao header
            entry->next = tmp;
            buckets[i-1] = entry;
        } else {
            while(tmp->next != NULL && tmp->next->endTime < priority){
                tmp = tmp->next;
            }

            if(tmp->next == NULL){
                tmp->next = entry;
            } else {
                entry = tmp->next;
                tmp->next = entry;
            }
        }
    }
    qsize++;

    /*nhan doi so luong cua calendar neu can*/
    return;
}

// Xoa di node co khoa be nhat va tra ve con tro toi khoa do
// Neu Calendar rong -> tra ve NULL
node* dequeue(){
    int i;
    i = lastBucket;
    while(1){
        if(buckets[i] != NULL && buckets[i]->endTime < bucketTop){
            node* tmp = buckets[i];
            buckets[i] = tmp->next;

            lastBucket = i;
            lastPrio = tmp->endTime;
            qsize--;
            /*thu hep so luong cua calendar neu can*/

            return tmp;
        } else {
            i++; if(i==nbuckets) i=0;
            bucketTop += width;
            if(i == lastBucket) break;
        }
    }
    return NULL;
}

void localinit(int qbase, int nbuck, double bwidth, double startPrio){
    firstsub = qbase;
    buckets = &a[qbase]; //
    qsize = 0; // Ban dau cha co event nao
    nbuckets = nbuck;
    width = bwidth;

    for(int i=0; i<nbuckets; i++){
        buckets[i] = NULL;
    }

    lastPrio = startPrio;
    int n = startPrio / width;
    lastBucket = n % nbuckets;
    bucketTop = (n+1) * width + 0.5*width;

    bot_threshold = nbuckets/2 - 2;
    top_threshold = 2 * nbuckets;
}

/*in ra man hinh lich*/
void printBucket(node* n){
    while(n!=NULL){
        printf("%.1f ",n->endTime);
        n = n->next;
    }
    return;
}
void printBuckets(){
    for(int i=0; i<nbuckets; i++){
        printf("Day %d : ",i);
        node* tmp = buckets[i];
        printBucket(tmp);
        printf("\n");
    }
    printf("Count of event : %d",qsize);
}

void initQueue(){
    a = (node*) calloc(QSPACE, sizeof(node));
    localinit(0,64,1,0);
    resizeenabled = 1;
}

double newwidth(){
    int nsamples;
    if(qsize < 2) return(1.0);
    if(qsize <= 5){
        nsamples = qsize;
    } else {
        nsamples = 5 + qsize/10;
    }
    if(nsamples > 25) nsamples = 25;

    // luu lai cac gia tri lastPrio, lastBucket, bucketTop
    double oldlastprio = lastPrio;
    int oldlastBucket = lastBucket;
    double oldbucketTop = bucketTop;

    // tao mang luu tru mau
    double* savePri;
    savePri = malloc(sizeof(double)*nsamples);
    node** saveNode;
    saveNode = (node*) calloc(nsamples, sizeof(node));

    // luu lai mau
    int j = 0;
    while(nsamples > 0){
        node* tmp = dequeue();
        savePri[j] = tmp->endTime;
        saveNode[j] = tmp;
        j++;
        nsamples--;
    }

    // tra lai cac gia tri cho hang doi va cac gia tri lastPrio, lastBucket, bucketTop
    for(int i=0; i<j; i++){
        enqueue(saveNode[i]);
    }
    lastPrio = oldlastprio;
    lastBucket = oldlastBucket;
    bucketTop = oldbucketTop;

    // tinh toan gia tri cho width

    return 0;
}

/*
int main(){
    initQueue();
    enqueue(new_node(A,0,0,16.2));
    enqueue(new_node(A,0,0,16.6));
    enqueue(new_node(A,0,0,17.8));
    enqueue(new_node(A,0,0,14.5));
    enqueue(new_node(A,0,0,14.7));
    enqueue(new_node(A,0,0,14.8));
    enqueue(new_node(A,0,0,15.2));
    enqueue(new_node(A,0,0,15.3));
    enqueue(new_node(A,0,0,19.1));
    enqueue(new_node(A,0,0,15.9));
    dequeue();
    dequeue();
    dequeue();
    dequeue();
    //newwidth();
    printBuckets();
    return 0;
}
*/
