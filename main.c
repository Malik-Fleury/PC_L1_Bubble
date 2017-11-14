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
void checkIsLastWorking(char* arrWorking, pthread_mutex_t* arrMutexWorking, int sizeArrayWorking);

int main()
{
    int sizeArray, numberOfThread;
    printf("Entrez la taille du tableau : ");
    scanf("%d", &sizeArray);

    while(numberOfThread > sizeArray)
    {
        printf("Entrez le nombre de threads : ");
        scanf("%d", &numberOfThread);
    }

    //calculer la taille des sous-tableaux
    int sizesArrays[numberOfThread];
    int sizeSubArray = (sizeArray + numberOfThread - 1) / numberOfThread;
    int modSize = sizeArray+numberOfThread-1 % numberOfThread;

    //initiation des tableaux
    char end = 0;
    pthread_mutex_t mutexEnd = PTHREAD_MUTEX_INITIALIZER;
    char* working = malloc(sizeof(char)*numberOfThread);
    pthread_mutex_t* mutexWorking = malloc(sizeof(char)*numberOfThread);

    pthread_t* arrThreads = malloc(sizeof(pthread_t)*numberOfThread);

    int numberOfMutex = numberOfThread - 1;
    pthread_mutex_t* arrMutexes = malloc(sizeof(pthread_mutex_t)*numberOfMutex);

    int i;
    for(i = 0; i < numberOfMutex; i++)
    {
        arrMutexes[i] = PTHREAD_MUTEX_INITIALIZER;
    }

    for(i = 0; i < numberOfThread; i++)
    {
        working[i] = 1;
        mutexWorking[i] = PTHREAD_MUTEX_INITIALIZER;

        sizesArrays[i] = sizeSubArray;

        if(modSize > 0)
        {
            sizesArrays[i]++;
            modSize--;
        }
    }

    //creation du tableau de données, malloc pour pouvoir faire un grand tableau (plus grand que demandé dans le labo)
    int* arrData = malloc(sizeof(int)*sizeArray);
    fillRandom(arrData, sizeArray);
    int* subArray = arrData;

    for(i = 0; i < numberOfThread; i++)
    {
        //mutex pour la valeur commune de gauche et droite
        pthread_mutex_t* leftMutex = NULL;
        pthread_mutex_t* rightMutex = NULL;

        if(i > 0)
        {
            leftMutex = &arrMutexes[i-1];
        }
        if(i < numberOfThread - 1)
        {
            rightMutex = &arrMutexes[i];
        }

                //création de la structure
        Section* data;
        initSection(data, i, subArray, sizesArrays[i], leftMutex, rightMutex, &end, &mutexEnd, working, numberOfThread, mutexWorking);

        if (pthread_create(&arrThreads[i],NULL,bubbleSort, data) == 0)
        {
            pthread_join(arrThreads[i], NULL);
        }
        else
        {
            exit(-1);
        }

        //On passe au prochain sous-tableau
        subArray += sizesArrays[i];
    }

    //free memory
    free(arrData);
    free(arrThreads);
    free(mutexWorking);
    free(working);
    free(arrMutexes);

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

void* multiThreadBubbleSort(void* param)
{
    Section* section = (Section*)param;

    int i;
    //Bubble sorting algorithm
    //changer en while
    while(section->end == 0)
    {
        char hasSwapped = 0;
        int j;
        for(j = 0; j < section->size; j++)
        {
            //section cririque si c'est une valeure partagée -> locker les mutex dans ce cas
            if(i == 0 && section->leftMutex != NULL)
            {
                pthread_mutex_lock(section->leftMutex);
            }
            else if(i == section->size-1 && section->rightMutex != NULL)
            {
                pthread_mutex_lock(section->rightMutex);
            }

            if(section->array[j] > section->array[j+1])
            {
                swapValue(section->array, j, j+1);
                hasSwapped = 1;
            }

            //unlock des mutex si c'est une valeur partagée
            if(i ==0 && section->leftMutex != NULL)
            {
                pthread_mutex_unlock(section->leftMutex);
            }
            else if(i == section->size-1 && section->rightMutex != NULL)
            {
                pthread_mutex_unlock(section->rightMutex);
            }
        }

        if(hasSwapped == 0)
        {
            pthread_mutex_lock(section->arrayMutexWorking);
            section->arrayWorking[section->tId] = 0;
            pthread_mutex_unlock(section->arrayMutexWorking);

            checkIsLastWorking(section->arrayWorking, section->arrayMutexWorking, section->sizeArrayWorking);

        }
    }
}

void checkIsLastWorking(char* arrWorking, pthread_mutex_t* arrMutexWorking, int sizeArrayWorking)
{

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
