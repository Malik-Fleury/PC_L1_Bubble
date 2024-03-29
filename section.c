//----------------------------------------------------------------
//  |M 1 M| 6 || 7 || 4 || 3 || 9 || 1 || 0 || 5 || 3 || 2 |M 2 M|
//----------------------------------------------------------------
//
//  Ce sch�ma repr�sente une section de tri.
//  Un seul thread s'occupe de cette section.
//  Il comporte au maximum 2 mutex par section (qui sont partag�s entre plusieurs sections)
//      - 1 mutex --> Premi�re section et derni�re section
//      - 2 mutex --> Toutes les sections qui se trouvent entre la premi�re et la derni�re
//

#include <stdio.h>
#include "section.h"

void initSection(Section* section, int tId, int* array, int size,
                 pthread_mutex_t* leftMutex, pthread_mutex_t* rightMutex,
                 bool* end, pthread_mutex_t* mutexEnd,
                 bool* arrayWorking, int sizeArrayWorking)
{
    section->tId = tId;
    section->array = array;
    section->size = size;
    section->leftMutex = leftMutex;
    section->rightMutex = rightMutex;
    section->end = end;
    section->mutexEnd = mutexEnd;
    section->arrayWorking = arrayWorking;
    section->sizeArrayWorking = sizeArrayWorking;
}

void printSection(Section* section)
{
    printf("-------------------------------------------------------------DEBUG\n");
    printf("ID du thread : %d", section->tId);
    printf("Adresse du tableau : %p\n",section->array);
    printf("Taille du tableau : %d\n",section->size);
    printf("Adresse du mutex gauche : %p\n",section->leftMutex);
    printf("Adresse du mutex droit : %p\n",section->rightMutex);
    printf("Adresse du flag de fin de tri : %p\n", section->end);
    printf("Adresse du mutex de fin de tri : %p\n", section->mutexEnd);
    printf("Adresse du tableau de flags de tri : %p\n", section->arrayWorking);
    printf("Taille du tableau de flags : %d\n", section->sizeArrayWorking);
}
