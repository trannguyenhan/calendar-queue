#include <stdio.h>
#include <stdlib.h>
#include "CalendarQueue.c"
#include "timing.c"

int main()
{
    initQueue();
    double wc1 = 0, wc2 = 0, cpuT = 0;
    int i = 0;
    long count = 0;
    timing(&wc1, &cpuT);
    int currentTime = 0;
    int endTime = 1000*1000;


    enqueue(0);
    node * ev = dequeue();
    while(currentTime <= endTime && ev->endTime != -1)
    {
        if(ev->endTime == currentTime)
        {
            count++;
            /*printf("%d)Event type = %d at %d with endTime = %d\n"
                , count, ev->type, ev->idElementInGroup, ev->endTime
                    );*/
            enqueue(currentTime+2);
            ev->endTime = -1;
            ev = dequeue();

            currentTime = ev->endTime;
        }
    }

    timing(&wc2, &cpuT);
    printf("Time: %f ms with count = %ld\n", (wc2 - wc1)*1000, count);
    printf("================================\n");

    return 0;
}
