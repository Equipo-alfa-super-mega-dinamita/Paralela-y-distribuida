#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#define ITERATION 1000000
#define N_THREAD 10
static int part = ITERATION/N_THREAD;
struct piInfo
{
    int initIteration;
    int endIteration;
    double *piTotal;
};
void *calculaPi (void *args){
    struct piInfo *paramenters;
    paramenters = (struct piInfo *)args;
    int i = paramenters -> initIteration;
    do{
        *(paramenters-> piTotal) = *(paramenters-> piTotal) + (double)(4.0/((i*2)+1));
        i++;
        *(paramenters-> piTotal) = *(paramenters-> piTotal) - (double)(4.0/((i*2)+1));
        i++;
    }while(i < (paramenters-> endIteration));
}

struct piInfo myThread(double *piTotal, int iniIteration, int endIteration){
   struct piInfo myPiInfo;
   myPiInfo.piTotal = piTotal;
   myPiInfo.endIteration =endIteration;
   myPiInfo.initIteration = iniIteration;
   return myPiInfo;
}
int main() {
   // printf() displays the string inside quotation
   double pi = 0;
   int ans;
   pthread_t thread_array[N_THREAD];
   for(int i = 0; i< N_THREAD; i++){
        struct piInfo newInfo = myThread(&pi,part*i,part*(i+1));
        ans = pthread_create(&thread_array[i],NULL,calculaPi,(void *) &newInfo);
        pthread_join(thread_array[i],NULL);
        if(ans<0){
            perror("Error en pthread create");
        }
    }
    printf("fin %f \n",pi);
   return 0;
}