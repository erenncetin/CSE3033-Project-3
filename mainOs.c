#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

double global_sqrt_sum = 0;
pthread_mutex_t mutex;

// Sum sqrt of numbers in the given range 
void* sum_Sqrt(void * args) {

    long long int start = ((long long int*)args)[0];
    long long int end = ((long long int*)args)[1];
    long long int option = ((long long int*)args)[2];

    // For option one, There is no need to mutex
    if(option == 1) {
        for(long long int i = start ; i < end ; i++) {
            global_sqrt_sum += sqrt(i);
        }
        free(args);
        return NULL;
    }

    // For option two, There is a mutex
    else if(option == 2) {
        pthread_mutex_lock(&mutex);
        for(long long int i = start ; i < end ; i++) {
            global_sqrt_sum += sqrt(i);
        }
        free(args);
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    // For option three, There is a mutex and local variable
    else {
        pthread_mutex_lock(&mutex);
        double local_sum = 0;
        for(long long int i = start ; i < end ; i++) {
            local_sum += sqrt(i);
        }
        free(args);
        pthread_mutex_unlock(&mutex);
        double* result = malloc(sizeof(double));
        *result = local_sum;
        return (void *) result;
    }
}


void creationThread(long long int a, long long int b, long long int c, long long int d) {

    pthread_t th[c];
    pthread_mutex_init(&mutex,NULL);
    long long int range = (b - a) / c;

    // Check range is 0 or not
    if(range == 0) {
        fprintf(stderr,"The number of thread is bigger than range of numbers\n");
        exit(0);
    }

    // Determine subrange for each thread, create threads and finish execution of these threads
    for(long long int i = 0; i < c ; i++) {
        long long int* args = (long long int*)malloc(3 * sizeof(long long int));
        if(i == c - 1) {
            args[0] = a + (i * range);
            args[1] = b + 1;
            args[2] = d;
        }
        else {
            args[0] = a + (i * range);
            args[1] = a + ((i + 1) * range);
            args[2] = d;
        }

        if(pthread_create(th + i,NULL,&sum_Sqrt,args) != 0) {
            printf("There is an error while creating thread!");
            exit(0);
        }             
    }

    // The Method type is 1 or 2, we don't need to return value from sum_Sqrt function
    if(d == 1 || d == 2) {
        for(int i = 0 ; i < c ; i++) {
            if(pthread_join(th[i],NULL) != 0) {
                printf("There is an error waiting the thread!");
                exit(0);
            }
        }
    }

    // The Method type is 3, we need to keep return value from sum_Sqrt function
    if(d == 3) {
        for(int i = 0 ; i < c ; i++) {
            double *ressult = malloc(sizeof(double));
            if(pthread_join(th[i],(void **)&ressult) != 0) {
                printf("There is an error waiting the thread!");
                exit(0);
            }
            global_sqrt_sum += *ressult;
            free(ressult);
        }  
    }  
    pthread_mutex_destroy(&mutex);
} 

int main(int argc, char **argv) {
          
    // Check number of given arguments 
    if(argc != 5) {
        fprintf(stderr,"The input format must be  a(long long int) b(long long int) c(int) d(int)\n");
        exit(0);
    }

    long long int a = atoll(argv[1]);
    long long int b = atoll(argv[2]);
    int c = atoi(argv[3]);
    int d = atoi(argv[4]);

    // Check first, second number is positive and second is greater than first one
    if(!(a >= 0 && b > 0 && b > a)) {
        fprintf(stderr,"The first and second parameter must be positive and the second parameter must be bigger than first parameter\n");
        exit(0);
    }

    // Check c (number of thread) is positive or not
    if(!(c > 0)) {
        fprintf(stderr,"The third parameter must be positive number.\n");
        exit(0);
    }

    // Check d (type of method) is valid or not
    if(1 <= d && d <= 3) {
        creationThread(a,b,c,d);
        printf("The global value is : %e\n",global_sqrt_sum);
    }
    else {
        fprintf(stderr,"Please enter 1,2 or 3 for the fourth variable(d).\n");
    }
}