#ifndef SECTION_H_INCLUDED
#define SECTION_H_INCLUDED

#include <pthread.h>
#include <stdbool.h>

struct Section_t
{
    int tId;
    int* array;
    int size;
    pthread_mutex_t* leftMutex;
    pthread_mutex_t* rightMutex;
    //bool mais pas possible alors char, plus petite variable en espace mémoire
    //si tous les tableaux sont triés
    bool* end;
    pthread_mutex_t* mutexEnd;
    //listes des tableaux qui si en trains de triés
    bool* arrayWorking;
    int sizeArrayWorking;
};

typedef struct Section_t Section;

void initSection(Section* section, int tId, int* array, int size,
                 pthread_mutex_t* leftMutex, pthread_mutex_t* rightMutex,
                 bool* end, pthread_mutex_t* mutexEnd,
                 bool* arrayWorking, int sizeArrayWorking);

#endif // SECTION_H_INCLUDED
