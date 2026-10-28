#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<pthread.h>


typedef struct Cart{
    unsigned int suit;
    unsigned int rank;
} Card;

typedef struct SimulationParameters{
    unsigned int numOfSimulations;
    unsigned int seed;
} SimulationParameters;

void Swap(Card* a, Card* b){
    Card temp = *b;
    *b = *a;
    *a = temp;
}


int AreSameRanksOnTop(unsigned int* seed){
    Card cards[52];

    for(unsigned int suit = 0; suit < 4; ++suit){
        for(unsigned int rank = 0; rank < 13; ++rank){
            unsigned int index = suit * 13 + rank;
            cards[index].suit = suit;
            cards[index].rank = rank;
        }
    }

    for(unsigned int i = 52; i > 0;){
        --i;
        unsigned int swapIndex = rand_r(seed) % (i + 1);
        Swap(&cards[i], &cards[swapIndex]);
    }

    return cards[0].rank == cards[1].rank;
}


void* Simulate(void* argument){
    SimulationParameters parameters = *(SimulationParameters*)argument;

    free(argument);

    unsigned int succesfulSimulations = 0;

    for(unsigned int i = 0; i < parameters.numOfSimulations; ++i){
        succesfulSimulations += AreSameRanksOnTop(&parameters.seed);
    } 

    unsigned int* result = (unsigned int*)malloc(sizeof(unsigned int));
    *result = succesfulSimulations;
    return (void*)result;
}


int main(int argc, char* argv[]){
    unsigned int numOfThreads = 1;
    unsigned int totalSimulations = 100000000;


    if(argc >= 2){
        unsigned long value = strtoul(argv[1], NULL, 0);
        if(value > 1024 || value < 1){
            printf("Num of threads should be between 1 and 1024\n");
            return 1;
        }

        numOfThreads = (unsigned int)value;
    }

    if(argc >= 3){
        unsigned long value = strtoul(argv[2], NULL, 0);

        if(value > 1000000000 || value < numOfThreads){
            printf("Num of simulations should be between num of threads and 1000000000\n");
            return 1;
        }

        totalSimulations = (unsigned int)value;
    }


    

    
    

    printf("Threads: %u\n", numOfThreads);
    printf("Simulations: %u\n", totalSimulations);

    unsigned int simulationsPerThread = totalSimulations / numOfThreads;
    totalSimulations = simulationsPerThread * numOfThreads;

    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * numOfThreads);

    srand(time(NULL));
    for(unsigned int i = 0; i < numOfThreads; ++i){
        SimulationParameters* parameters = (SimulationParameters*)malloc(sizeof(SimulationParameters));

        parameters->seed = rand();
        parameters->numOfSimulations = simulationsPerThread;

        if(pthread_create(&threads[i], NULL, Simulate, parameters) != 0){
            perror("Thread error\n");
            return 1;
        }

    }

    unsigned int succesfulSimulations = 0;

    for(unsigned int i = 0; i < numOfThreads; ++i){
        void* threadResult = NULL;
        if(pthread_join(threads[i], &threadResult) != 0){
            perror("Thread join error\n");
            return 1;
        }

        succesfulSimulations += *(unsigned int*)threadResult;

        free(threadResult);

    }


    printf("Succecful simulations: %d/%d\n", succesfulSimulations, totalSimulations);
    printf("Succes probability: %f\n", (float)succesfulSimulations/totalSimulations);


    free(threads);
    return 0;
}