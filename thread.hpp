#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

#include "grid.hpp"
#include "util.h"
#include "tuple.h"

using namespace Utility;

#ifndef _THREAD_FUNCTIONS_
#define _THREAD_FUNCTIONS_

namespace ThreadFunctions{
    void * ColumnChecker(void * args){
        unsigned short test_array[9] = {0};
        unsigned column = *((unsigned*)args);
        for (unsigned i=0;i<9;++i){
            test_array[Soduko::grid[i][column]]++;
        }
        bool status = Utility::CheckArray(test_array,9);
        pthread_exit(&status);
    }

    void * RowChecker(void * args){
        unsigned short test_array[9] = {0};
        unsigned row = *((unsigned*)args);
        for (unsigned i=0;i<9;++i){
            test_array[Soduko::grid[row][i]]++;
        }
        bool status = Utility::CheckArray(test_array,9);
        pthread_exit(&status);
    }

    void * RegionChecker(void * args){

    }
}

#endif