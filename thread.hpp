#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

#include "grid.hpp"
#include "util.h"
#include "tuple.h"

//using namespace Utility;

#ifndef _THREAD_FUNCTIONS_
#define _THREAD_FUNCTIONS_

namespace ThreadFunctions{
    void * ColumnChecker(void * args){
        unsigned short test_array[9] = {0};
        unsigned & column = *((unsigned*)args);
        for (unsigned i=0;i<9;++i){
            test_array[Soduko::grid[i][column] - 1]++;
        }
        bool status = Utility::CheckArray(test_array,9);
        pthread_exit(&status);
    }

    void * RowChecker(void * args){
        unsigned short test_array[9] = {0};
        unsigned & row  = *((unsigned*)args);
        for (unsigned i=0;i<9;++i){
            test_array[Soduko::grid[row][i] - 1]++;
        }
        bool status = Utility::CheckArray(test_array,9);
        pthread_exit(&status);
    }

    void * RegionChecker(void * args){
        unsigned short test_array[9] = {0};
        Tuple & tuple  = *((Tuple*)args);
        for (unsigned i=tuple.row*3,k=0;k<3;++i,++k)
            for (unsigned j=tuple.col*3,l=0;l<3;++j,++l)
                test_array[Soduko::grid[i][j] - 1]++;
        bool status = Utility::CheckArray(test_array,9);
        pthread_exit(&status);
    }
}

namespace ThreadStructs{
    pthread_t row_threads[9],column_threads[9],region_threads[9];
}

void CreateThreads(){
    using namespace ThreadStructs;
    int array[9] = {1,2,3,4,5,6,7,8,9};
    Tuple region[9];
    for (short i=0,j=-1;i<9;++i){
        j = i%3==0?++j:j;
        region[i] = Tuple(j,i%3);
        pthread_create(&row_threads[i],NULL,ThreadFunctions::RowChecker,&array[i]);
        pthread_create(&column_threads[i],NULL,ThreadFunctions::ColumnChecker,&array[i]);
        pthread_create(&region_threads[i],NULL,ThreadFunctions::RegionChecker,&region[i]);
    }
}

#endif