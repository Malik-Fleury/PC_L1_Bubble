#ifndef SECTION_H_INCLUDED
#define SECTION_H_INCLUDED

#include <pthread.h>
#include <stdbool.h>

struct Section_t
{
    int* array;
    int size;
    pthread_mutex_t* leftMutex;
    pthread_mutex_t* rightMutex;
};

typedef struct Section_t Section;

#endif // SECTION_H_INCLUDED
