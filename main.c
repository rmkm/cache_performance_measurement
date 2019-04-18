#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

double lp_time(int, int, int);
double second(void);
unsigned long long int rdtsc();

long array[64000000];
int rsum;
double sec;

int main(int argc, char **argv){
    unsigned int i;
    int n, step, mode;
    double time;

    n = 10000; // num of access
    if(argc != 3){
        fprintf(stderr, "Usage: a.exe mode step\n");
        exit(1);
    }

    sscanf(argv[1], "%d", &mode);
    if((mode < 0 || mode > 2 )){
        fprintf(stderr, "Mode=%d, is not supported\n", mode);
        exit(1);
    }

    sscanf(argv[2], "%d", &step);
    if((abs(step) > 6400 || step == 0 )){
        fprintf(stderr, "Step=%d, is 0 or too large\n", step);
        exit(1);
    }

    fprintf(stdout, "mode=%d, step=%d, n=%d", mode, step, n);

    for(i = 0; i < 20; i++){ // measure 20 times
        time = lp_time(mode, step, n)/(double)n;
        fprintf(stdout, "time=%e [cycle] with mode, step=%d, %d\n", time, mode, step);
    }
}

__inline__ unsigned long long int rdtsc(){
    unsigned long long int x;
    __asm__ volatile("rdtsc":"=A"(x));
    return x;
}

double lp_time(int mode, int step, int n){
    int sum = 0;
    int i,j;
    unsigned long long int start, end;

    if(mode == 0){ // read array[0] to array[7]
        for(i = 0; i < n; i++){
            sum += array[i&0x7];
        }
        start = rdtsc();
        for(i = 0; i < n; i++){ // read the same cache line n times
            sum += array[i&0x7];
        }
        end = rdtsc();
    }else if(mode == 1){ // read array every steps
        for(i = j = 0; i < n; i ++, j += step){
            sum += array[j];
        }
        start = rdtsc();
        for(i = j = 0; i < n; i ++, j += step){
            sum += array[j];
        }
        end = rdtsc();
    }else{ // mode = 2; 
        sum = 0;
        for(i = 0; i < n; i++){
            array[sum] = sum + step;
            sum += step;
        }
        sum = 0;
        start = rdtsc();
        for(i = 0; i < n; i++){
            sum = array[sum];
        }
        end = rdtsc();

    }

    rsum = sum;
    return (double)(end - start);
}
