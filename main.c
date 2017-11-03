/*
    author : Bulloni Lucas & Fleury Malik
    date : 03.11.2017
    description : Laboratoire 1 - tri bulle concurrent
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

void swapValue(int* array, int a, int b);
void fillRandom(int* array, int size);
void printArray(int* array, int size);
void sortWithBubbleSort(int* array, int size);

#define SIZE 3361

int main()
{
    int* tab = malloc(sizeof(int)*SIZE);
    fillRandom(tab, SIZE);

    free(tab);
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

void sortWithBubbleSort(int* array, int size)
{
    int i;
    //Bubble sorting algorithm
    for(i=size-2; i >= 0; i--)
    {
        printf("le i est %d\n", i);
        int j;
        for(j=0; j<=i; j++)
        {
            if(array[j] > array[j+1])
            {
                swapValue(array, i, j);
            }
        }
    }
}
