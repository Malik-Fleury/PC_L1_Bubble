#ifndef SECTION_H_INCLUDED
#define SECTION_H_INCLUDED

#include <pthread.h>

struct Section_t
{
    int* array;
    int leftIndex;
    int rightIndex;
    pthread_t* thread;
    pthread_mutex_t* leftMutex;
    pthread_mutex_t* rightMutex;
};

typedef struct Section_t Section;

#endif // SECTION_H_INCLUDED
