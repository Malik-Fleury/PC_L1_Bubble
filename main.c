/*
    author : Bulloni Lucas & Fleury Malik
    date : 03.11.2017
    description : Laboratoire 1 - tri bulle concurrent
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include "section.h"

void swapValue(int* array, int a, int b);
void fillRandom(int* array, int size);
void printArray(int* array, int size);
void bubbleSort(int* array, int size);
void* multiThreadBubbleSort(void* param);

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
    int sizeSubArray = (sizeArray + numberOfThread - 1) / numberOfThread;
    int modSize = sizeArray+numberOfThread-1 % numberOfThread;
    int i;
    for(i = 0; i < numberOfThread; i++)
    {
        sizesArrays[i] = sizeSubArray;
        if(modSize > 0)
        {
            sizesArrays[i]++;
            modSize--;
        }
    }

    //creation du tableau de données, malloc pour pouvoir faire un grand tableau
    int* arrData = malloc(sizeof(int)*sizeArray);
    fillRandom(arrData, sizeArray);

    pthread_t* arrThreads = malloc(sizeof(pthread_t)*numberOfThread);
    int numberOfMutex = numberOfThread-1;
    pthread_mutex_t* arrMutexes = malloc(sizeof(pthread_mutex_t)*numberOfMutex);

    int* subArray = arrData;

    for(i = 0; i < numberOfThread; i++)
    {
        Section* data;
        initSection(data, subArray, sizesArrays[i], arrThreads[i]);
        pthread_create(&arrThreads, NULL, bubbleSort, &data);
        pthread_mutex_t* leftMutex = NULL;
        pthread_mutex_t* rightMutex = NULL;

        if(i > 0)
        {
            leftMutex = arrMutexes[i-1];
        }
        if(i < numberOfThread - 1)
        {
            rightMutex = arrMutexes[i];
        }

        initSection(data, subArray, sizesArrays[i], leftMutex, rightMutex);

        subArray += sizesArrays[i];
    }

    //free memory
    free(arrData);
    free(arrThreads);
    return 0;
}

/*void prototypeThreadedSort(int threadsCount, int array[], int sizeArray)
{
    int mutexCount = threadsCount-1;
    pthread_t thread[threadsCount];
    pthread_mutex_t mutex[mutexCount];
    Section section[threadsCount];

    int sizeSection = sizeArray / threadsCount;
    int rest = sizeArray % threadsCount;

    int cnt;
    int leftIndex = 0;
    int rightIndex = sizeSection;

    initSection(section, array, leftIndex, rightIndex,
                thread, NULL, mutex);
    for(cnt = 1;cnt < (threadsCount-1); cnt++)
    {
        leftIndex = rightIndex + 2;
        rightIndex = leftIndex + (sizeSection);
        initSection(section+cnt, array, leftIndex, rightIndex,
                    thread+cnt, mutex+(cnt-1), mutex+cnt);
    }
    initSection(section+threadsCount-1, array, rightIndex+1-1, sizeArray-1,
                thread+(threadsCount-1),mutex+(mutexCount-1), NULL);

    int sectionCount = threadsCount;
    for(cnt = 0;cnt < sectionCount; cnt++)
    {
        pthread_create(section[cnt].thread, NULL, worker, section+cnt);
        printSection(section+cnt);
    }

    for(cnt = 0;cnt < sectionCount; cnt++)
    {
        pthread_join(*(section[cnt].thread), NULL);
        printf("Thread %d has joined\n",(cnt+1));
    }
}*/

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

void* multiThreadBubbleSort(void* param)
{
    Section* section = (Section*)param;

    int i;
    //Bubble sorting algorithm
    //changer en while
    for(i = 0; i < section->size; i++)
    {
        int j;
        for(j = 0; j < section->size; j++)
        {
            if(section->array[j] > section->array[j+1])
            {
                if(i == 0 && section->leftMutex != NULL)
                {
                    pthread_mutex_lock(section->leftMutex);
                }
                else if(i == section->size-1 && section->rightMutex != NULL)
                {
                    pthread_mutex_lock(section->rightMutex);
                }

                swapValue(section->array, j, j+1);

                if(i == 0 && section->leftMutex != NULL)
                {
                    pthread_mutex_unlock(section->leftMutex);
                }
                else if(i == section->size-1 && section->rightMutex != NULL)
                {
                    pthread_mutex_unlock(section->rightMutex);
                }
            }
        }
    }
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
