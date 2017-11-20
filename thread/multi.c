#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/time.h>
#include<pthread.h>

#define T_NUM 40 // 1,2,4,8,10,20,40

#define ARR_SIZE 4000
#define TIME 1000000

int **A;
int **B;
int **C;

pthread_barrier_t bar;

/* Matrix multiplication C=A*B */
void multi (int from, int to){
    int i,j,z;
    int sum = 0;

    for(z=from; z<to; z++){
       for(i=0; i<ARR_SIZE; i++){
          for(j=0; j<ARR_SIZE; j++){
             sum += A[z][j]*B[j][i];
          }
          C[z][i] = sum;
          sum =0;
       }
    }
	pthread_barrier_wait(&bar);
}

/* Thread Function*/ 
void * thr_fn(void * arg){
    int num = ARR_SIZE / T_NUM;

    multi(num*(long)arg, num*((long)arg+1));
}


int main(){
    struct timeval START, END;
    pthread_t tid[T_NUM];
    uint64_t diff,sum = 0;
		


	pthread_barrier_init(&bar,NULL,T_NUM+1);
/*  initializing MAtrix A, B  */

    FILE *fp = fopen("sample1.txt", "r");
    FILE *fp2 = fopen("sample2.txt", "r");

    A = (int**)malloc(sizeof(int*)*ARR_SIZE);
    B = (int**)malloc(sizeof(int*)*ARR_SIZE);
    C = (int**)malloc(sizeof(int*)*ARR_SIZE);

    long i;
	int j,numA, numB;

    for(int i=0; i<ARR_SIZE; i++){
        A[i] = (int*)malloc(sizeof(int)*ARR_SIZE);
        B[i] = (int*)malloc(sizeof(int)*ARR_SIZE);
        C[i] = (int*)malloc(sizeof(int)*ARR_SIZE);
    }

    for(i=0; i<ARR_SIZE; i++){
        for(j=0; j<ARR_SIZE; j++){
             fscanf(fp, "%d", &numA);
             A[i][j] = numA;
             fscanf(fp2, "%d", &numB);
             B[i][j] = numB;
        }
    }


    
/* time checking Matrix multiplication & calcultaing sum */

    gettimeofday(&START,NULL);

    /* Thread = T_NUM */
    for(i=0; i<T_NUM; i++){
       pthread_create(&tid[i], NULL, thr_fn, (void *)i);  
    }

	pthread_barrier_wait(&bar);

    for(i=0; i<T_NUM; i++){               
        pthread_join(tid[i], NULL);
    }      

	pthread_barrier_destroy(&bar);
    
    for(i=0; i<ARR_SIZE; i++){
        for(j=0; j<ARR_SIZE; j++){
            sum += C[i][j];
       }
    }

    gettimeofday(&END,NULL);

    printf("** sum = %llu\n",(long long unsigned int) sum);
    diff = TIME * (END.tv_sec-START.tv_sec) + END.tv_usec-START.tv_usec;
    printf("** time = %llu.%llu sec\n", (long long unsigned int) diff / 1000000, ((long long unsigned int) diff/1000)%1000);
    

return 0;
}
