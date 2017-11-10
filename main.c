/*
    author : Bulloni Lucas & Fleury Malik
    date : 03.11.2017
    description : Laboratoire 1 - tri bulle concurrent
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

struct structSubArrToSort {
   int* array;
   int  size;
};
typedef struct structSubArrToSort subArrToSort;

void swapValue(int* array, int a, int b);
void fillRandom(int* array, int size);
void printArray(int* array, int size);
void bubbleSort(int* array, int left, int right);

/*
    TODO : RECHANGER bubbleSort en tab + size avec arithmétique des pointeurs, plus simple pour passer au thread
    TODO : Optimisation de la taille des threads
*/

int main()
{
    int sizeArray, numberOfThread;
    printf("Entrez la taille du tableau : ");
    scanf("%d", &sizeArray);
    printf("Entrez le nombre de threads : ");
    scanf("%d", &numberOfThread);

    //calculate the size of the subarray for each thread
    int sizesArrays[numberOfThread];
    int modSize = sizeArray % numberOfThread;
    int i;
    for(i = 0; i <= numberOfThread; i++)
    {
        sizeArray[i] = sizeArray / numberOfThread;
        if(modSize > 0)
        {
            sizeArray[i]++;
            modSize--;
        }
    }

    //creation du tableau de données, malloc pour pouvoir faire un grand tableau
    int* arrData = malloc(sizeof(int)*sizeArray);
    fillRandom(arrData, sizeArray);

    pthread_t* arrThreads = malloc(sizeof(pthread_t)*numberOfThread);

    int* subArray = arrData;

    int i;
    for(i = 0; i < numberOfThread; i++)
    {
        subArrToSort data;
        data.array = subArray;
        data.size = sizesArrays[i];

        pthread_create(&arrThreads, NULL, bubbleSort, &data);
        subArray += sizesArrays[i];
    }

    //free memory
    free(arrData);
    free(arrThreads);
    return 0;
}

void swapValue(int* array, int a, int b)
{
	int tmp = array[a];
	array[a] = array[b];
	array[b] = tmp;
}

void fillRandom(int* array, int size)
{
	srand(time(0)*getpid());
	int i;
	for (i=0; i < size; i++)
    {
        array[i] = rand()%(size*3);
	}
}

void printArray(int* array, int size)
{
    int i;
    for(i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

double getTime()
{
	static double start = 0;
	struct timeval tv;
	double now;

	gettimeofday(&tv, 0);
	now = tv.tv_sec + tv.tv_usec / 1e6L;

	if (start)
    {
		now -= start;
	}
	else
	{
		start = now;
		now = 0.L;
	}
	return now;
}

void bubbleSort(int* array, int size)
{
    int i;
    //Bubble sorting algorithm
    for(i = 0; i <= size; i++)
    {
        int j;
        for(j=0; j < size-i; j++)
        {
            if(array[j] > array[j+1])
            {
                swapValue(array, j, j+1);
            }
        }
    }
}
