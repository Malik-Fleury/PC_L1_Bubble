/**
    @author Bulloni Lucas & Fleury Malik
    @date   03.11.2017
    @brief  Laboratoire 1 - tri bulle concurrent
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "section.h"

void swapValue(int* array, int a, int b);
void fillRandom(int* array, int size);
void printArray(int* array, int size);
void bubbleSort(int* array, int size);
void* multiThreadBubbleSort(void* param);
bool checkIsLastWorking(bool* arrWorking, int sizeArrayWorking);

/**
@brief  Fonction principale du programme
*/
int main()
{
    int sizeData, numberOfThread;

    // Demande à l'utilisateur le nombre de données et de threads
    printf("Entrez la taille du tableau : ");
    scanf("%d", &sizeData);

    do
    {
        printf("Entrez le nombre de threads : ");
        scanf("%d", &numberOfThread);
    }while(numberOfThread > sizeData);

    printf("Taille du tableau : %d nombre de thread : %d\n", sizeData, numberOfThread);

    //creation du tableau de données, malloc pour pouvoir faire un grand tableau (plus grand que demandé dans le labo)
    int* arrData = malloc(sizeof(int)*sizeData);
    fillRandom(arrData, sizeData);
    int* subArray = arrData;

    clock_t clockStart = clock();

    // Calcule la taille des sous-tableaux
    int sizesArrays[numberOfThread];
    int sizeSubArray = (sizeData + numberOfThread - 1) / numberOfThread;
    int modSize = (sizeData+numberOfThread-1) % numberOfThread;

    // Initialise les tableaux
    bool end = 0;
    pthread_mutex_t mutexEnd = PTHREAD_MUTEX_INITIALIZER;
    bool* working = malloc(sizeof(char)*numberOfThread);

    int numberOfMutex = numberOfThread - 1;
    pthread_mutex_t* arrMutexes = malloc(sizeof(pthread_mutex_t)*numberOfMutex);

    pthread_t* arrThreads = malloc(sizeof(pthread_t)*numberOfThread);

    int i;
    for(i = 0; i < numberOfMutex; i++)
    {
        arrMutexes[i] = PTHREAD_MUTEX_INITIALIZER;
    }

    // Initialisation du tableau de booléens indiquant les threads qui trient
    for(i = 0; i < numberOfThread; i++)
    {
        working[i] = true;

        sizesArrays[i] = sizeSubArray;

        if(modSize > 0)
        {
            sizesArrays[i]++;
            modSize--;
        }
    }

    // Préparation des données pour chaque partie de tableau à trier et lancement des tries
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
        Section* data = malloc(sizeof(Section));
        initSection(data, i, subArray, sizesArrays[i], leftMutex, rightMutex, &end, &mutexEnd, working, numberOfThread);

        if (pthread_create(&arrThreads[i], NULL, multiThreadBubbleSort, data) != 0)
        {
            exit(-1);
        }

        //On passe au prochain sous-tableau
        subArray += sizesArrays[i] - 1;
    }

    // Attend que tous les threads aient terminés leur travail
    for(i = 0; i < numberOfThread; i++)
    {
        pthread_join(arrThreads[i], NULL);
    }

    // Affiche le temps écoulé pour le tri multi-threads
    clock_t clockEnd = clock();
    double timeMultiThread = (double)(clockEnd - clockStart) / CLOCKS_PER_SEC;
    printf("\ntemps ecoule en multithread : %f secondes", timeMultiThread);

    // Lance le tri bubblesort monothread et affiche le résultat
    fillRandom(arrData, sizeData);
    clockStart = clock();
    bubbleSort(arrData, sizeData);
    clockEnd = clock();
    double timeMonothread = (double)(clockEnd - clockStart) / CLOCKS_PER_SEC;
    printf("\ntemps ecoule en monothread : %f secondes", timeMonothread);

    //free memory
    free(arrData);
    free(arrThreads);
    free(working);
    free(arrMutexes);

    return 0;
}

/**
@brief  Permet d'afficher les valeurs d'un tableau
@param  array   Tableau à afficher
@param  size    Taille du tableau
*/
void printArray(int* array, int sizeArray) {
    printf("\n");

    int i;
    for(i = 0; i < sizeArray; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}

/**
@brief  Permet d'échanger deux valeurs dans un tableau
@param  array   Tableau dans lequel échangé deux valeurs
@param  a       Index de la première valeurs à échanger
@param  b       Index de la seconde valeurs à échanger
*/
void swapValue(int* array, int a, int b)
{
	int tmp = array[a];
	array[a] = array[b];
	array[b] = tmp;
}

/**
@brief  Permet de remplir le tableau avec des valeurs aléatoires
@param  array   Tableau à remplir avec des valeurs aléatoires
@param  size    Taille du tableau
*/
void fillRandom(int* array, int size)
{
	srand(time(0)*getpid());
	int i;
	for (i=0; i < size; i++)
    {
        array[i] = rand()%(size*3);
	}
}

pthread_mutex_t modifyWorking = PTHREAD_MUTEX_INITIALIZER;

/**
@brief  Effectue le tri bubblesort avec plusieurs threads
@param  param   Structure "Section" contenant toutes les informations concernant
                le tri d'une partie de tableau
*/
void* multiThreadBubbleSort(void* param)
{
    Section* section = (Section*)param;

    //Bubble sorting algorithm
    //changer en while
    while(*(section->end) == false)
    {
        char hasSwapped = 0;

        int j;
        for(j = 0; j < section->size; j++)
        {
            //section cririque si c'est une valeure partagée -> locker les mutex dans ce cas
            if(j == 0 && section->leftMutex != NULL)
            {
                pthread_mutex_lock(section->leftMutex);
            }
            else if(j == section->size-1 && section->rightMutex != NULL)
            {
                pthread_mutex_lock(section->rightMutex);
            }

            if(j < section->size - 1 && section->array[j] > section->array[j+1])
            {
                swapValue(section->array, j, j+1);
                hasSwapped = 1;
            }

            //unlock des mutex si c'est une valeur partagée
            if(j == 0 && section->leftMutex != NULL)
            {
                pthread_mutex_unlock(section->leftMutex);
            }
            else if(j == section->size-1 && section->rightMutex != NULL)
            {
                pthread_mutex_unlock(section->rightMutex);
            }
        }

        if(hasSwapped == 0)
        {
            // MUTEX - MF
            pthread_mutex_lock(&modifyWorking);
            section->arrayWorking[section->tId] = 0;
            pthread_mutex_unlock(&modifyWorking);

            //SI c'est le dernier qui travaille on s'arrête
            if(checkIsLastWorking(section->arrayWorking, section->sizeArrayWorking) == true)
            {
                pthread_mutex_lock(section->mutexEnd);
                *(section->end) = true;
                pthread_mutex_unlock(section->mutexEnd);
            }
            else
            {
                // MUTEX - MF
                pthread_mutex_lock(&modifyWorking);
                if(section->tId > 0)
                {
                    section->arrayWorking[section->tId - 1] = true;
                }
                if(section->tId < section->sizeArrayWorking - 1)
                {
                    section->arrayWorking[section->tId + 1] = true;
                }
                pthread_mutex_unlock(&modifyWorking);
            }
            while(section->arrayWorking[section->tId] == false && section->end == false){}
        }
    }

    free(section);

    return NULL;
}

pthread_mutex_t checkWorking = PTHREAD_MUTEX_INITIALIZER;

/**
@brief  Vérifie si tous les threads travaillent encore
@param  arrWorking          Tableau de booléens indiquant quels threads travaillent
@param  sizeArrayWorking    Taille du tableau de booléens
*/
bool checkIsLastWorking(bool* arrWorking, int sizeArrayWorking)
{
    bool isSomeoneWorking = false;
    int i;

    // MUTEX - MF
    pthread_mutex_lock(&checkWorking);
    for(i = 0; i < sizeArrayWorking; i++)
    {
        if(arrWorking[i] == true)
        {
            isSomeoneWorking = true;
        }
    }
    pthread_mutex_unlock(&checkWorking);

    /*
    if(isSomeoneWorking != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
    */
    return isSomeoneWorking == false;
}

/**
@brief  Alogorithme de tri bubblesort monothread
@param  array   Tableau à trier
@param  size    Taille du tableau à trier
*/
void bubbleSort(int* array, int size)
{
    int i, j, temp;
    //Bubble sorting algorithm
    for(i=size-2; i>= 0; i--){
        for(j=0; j<=i; j++){
            //Swap
            if(array[j] > array[j+1])
            {
                temp = array[j];
                array[j] = array[j+1]; array[j+1]= temp;
            }
        }
    }
}
