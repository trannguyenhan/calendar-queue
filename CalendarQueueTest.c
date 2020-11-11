#include <stdio.h>
#include <stdlib.h>
#include "CalendarQueue.c"
#include "timing.c"

int main()
{
    CalendarQueue* q = initqueue();
    double wc1 = 0, wc2 = 0, cpuT = 0;
    int i = 0;
    long count = 0;
    timing(&wc1, &cpuT);
    double currentTime = 0;
    double endTime = 10000*1000;

    for(i = 0; i < 6750; i++)
    {
        enqueue(q,new_node(A, i, 0, 0));
    }


    node * ev = dequeue(q);
    while(currentTime <= endTime && ev->endTime != -1)
    {   //printf("hello world");
        if(ev->endTime == currentTime)
        {
            count++;
            /*printf("%d)Event type = %d at %d with endTime = %d\n"
                , count, ev->type, ev->idElementInGroup, ev->endTime
                    );*/
            i = ev->idElementInGroup;//Lay id cua host trong danh sach cac hosts
            if(ev->type == A)
            {
                enqueue(q,new_node(A, i, 0, currentTime + 10000));
                enqueue(q,new_node(B, i, 0, currentTime));
            }
            else if(ev->type == B)
            {
                enqueue(q,new_node(C, i, 0, currentTime));
            }
            ev->endTime = -1;
            ev = dequeue(q);

            currentTime = ev->endTime;
        }
    }

    timing(&wc2, &cpuT);
    printf("Time: %f ms with count = %ld\n", (wc2 - wc1)*1000, count);
    printf("================================\n");

    return 0;
}
