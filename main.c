#include <stdio.h>
#include <stdlib.h>
#include "time.c"
const int MAX = 365;

// Moi bucket la mot cay nhi phan mak con ben phai > cha, con ben trai < cha
node** buckets;
int nbuckets;
double width; // thoi gian gia dinh cho 1 ngay
int qsize;
int lastBucket;
double lastPrio;
double bucketTop;

node* insertNode(node* T, double item){
    if(T == NULL) T = new_node(item);
    else if(item<T->endTime){
        T->left = insertNode(T->left,item);
    } else {
        T->right = insertNode(T->right,item);
    }
    return T;
}

void enqueue(double priority){
    int i;
    i = priority / width;
    i = i % nbuckets;

    if(buckets[i] == NULL){
        buckets[i] = new_node(priority);
        qsize++;
        return;
    } else {
        // Them node entry vao trong buckets[i-1]
        node* tmp;
        tmp = buckets[i];

        if(priority < tmp->endTime)
            tmp = insertNode(tmp,priority);
        else tmp = insertNode(tmp,priority);
        qsize++;
    }
}

// Xoa di node co khoa be nhat va tra ve con tro toi khoa do
// Khoa be nhat o la phan tu trai nhat cua cay nhi phan
// Neu Calendar rong -> tra ve NULL
node* dequeue(){
    int i;
    i = lastBucket;
    while(1){
        // Neu ma bucket[i] co event va cac event phai nam trong nam nay thi tim va xoa event co khoa nho nhat
        // Khong thi chuyen tiep toi bucket tiep theo
        if(buckets[i] != NULL){
            // Duyet while de tim phan tu trai nhat (cung la phan tu nho nhat)
            node* min;
            min = buckets[i];
            while(min->left!= NULL)
                min = min->left;

            if(min->endTime < bucketTop){
                qsize--; // Giam so luong event di

                if(min == buckets[i]){
                    /*free*/
                    buckets[i] = buckets[i]->right;
                } else {
                    node* tmp = buckets[i];
                    while(tmp->left != min){
                        tmp = tmp->left;
                    }
                    tmp->left = min->right;
                }

                // Cap nhat lai cac bien
                lastBucket = i;
                lastPrio = min->endTime;
                return min;
            } else {
                i++; if(i==nbuckets) i=0;
                bucketTop += width;
                if(i == lastBucket) break; // Tim 1 vong, neu khong tim thay -> thoat vong lap
            }
        } else {
            i++; if(i==nbuckets) i=0;
            bucketTop += width;
            if(i == lastBucket) break; // Tim 1 vong, neu khong tim thay -> thoat vong lap
        }
    }
    return NULL;
}

void localinit(int nbuck, double bwidth, double startPrio){
    qsize = 0; // Ban dau cha co event nao
    nbuckets = nbuck;
    width = bwidth;

    buckets = (node*) calloc(nbuckets, sizeof(node));
    for(int i=0; i<nbuckets; i++){
        buckets[i] = NULL;
    }

    lastPrio = startPrio;
    /*Them bot_threshold*/
    /*Them top_threshold*/
    int n = startPrio / width;
    lastBucket = n % nbuckets;
    bucketTop = (n+1) * width + 0.5*width;

}

void printBucket(node* n){
    if(n == NULL){
        return;
    } else {
        printBucket(n->left);
        printf(" %.1f ",n->endTime);
        printBucket(n->right);
    }
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
    localinit(8,0.5,14.5);
}

int main()
{
    initQueue();
    enqueue(16.2);
    enqueue(16.6);
    enqueue(17.8);
    enqueue(14.5);
    enqueue(14.7);
    enqueue(14.8);
    enqueue(15.2);
    enqueue(15.3);
    enqueue(19.1);
    enqueue(15.9);
    printBuckets();
    return 0;
}
