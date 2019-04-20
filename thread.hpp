#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <vector>

#include "grid.hpp"
#include "util.h"
#include "tuple.h"

//using namespace Utility;

#ifndef _THREAD_FUNCTIONS_
#define _THREAD_FUNCTIONS_

namespace ThreadFunctions{
    void * ColumnChecker(void * args){
        unsigned test_array[9] = {0};
        unsigned & column = *((unsigned*)args);
        static Tuple invalid_position = Tuple(-1,-1);
        for (unsigned i=0;i<9;++i){
            if (Soduko::grid[i][column] > 9 || Soduko::grid[i][column] < 1){
                invalid_position = Tuple(i,column);
            }
            else if (++test_array[Soduko::grid[i][column]-1] != 1){
                invalid_position = Tuple(i,column);
            }
        }
        pthread_exit(&invalid_position);
    }

    void * RowChecker(void * args){
        unsigned test_array[9] = {0};
        unsigned & row = *((unsigned*)args);
        static Tuple invalid_position = Tuple(-1,-1);
        for (unsigned i=0;i<9;++i){
            if (Soduko::grid[row][i] > 9 || Soduko::grid[row][i] < 1){
                invalid_position = Tuple(row,i);
            }
            else if (++test_array[Soduko::grid[row][i]-1] != 1){
                invalid_position = Tuple(row,i);
            }
        }
        pthread_exit(&invalid_position);
    }

    void * RegionChecker(void * args){
        unsigned test_array[9] = {0};
        Tuple & tuple  = *((Tuple*)args);
        static Tuple invalid_position = Tuple(-1,-1);
        for (unsigned i=tuple.row*3,k=0;k<3;++i,++k){
            for (unsigned j=tuple.col*3,l=0;l<3;++j,++l){
                if (Soduko::grid[i][j] < 1 || Soduko::grid[i][j] > 9){
                    invalid_position = Tuple(i,j);
                }
                else if (++test_array[Soduko::grid[i][j]-1] != 1){
                    invalid_position = Tuple(i,j);
                }
            }
        }
        pthread_exit(&invalid_position);
    }
}

namespace ThreadStructs{
    pthread_t row_threads[9],column_threads[9],region_threads[9];
}

namespace GLOBAL_DATA{
    sem_t novux;
    Validity boxes_status[27] = {Validity::Valid};
    std::vector <Tuple> invalid_boxes;
}

void CreateThreads(){
    using namespace ThreadStructs;
    static unsigned array[9] = {0,1,2,3,4,5,6,7,8};
    static Tuple region[9];
    for (short i=0,j=-1;i<9;++i){
        j = i%3==0?++j:j;
        region[i] = Tuple(j,i%3);
        pthread_create(&row_threads[i],NULL,ThreadFunctions::RowChecker,&array[i]);
        pthread_create(&column_threads[i],NULL,ThreadFunctions::ColumnChecker,&array[i]);
        pthread_create(&region_threads[i],NULL,ThreadFunctions::RegionChecker,&region[i]);
    }
}

bool JoinThreads(){
    void * status;
    Validity current_status = Validity::Valid;
    for (short i=0;i<9;++i){
        pthread_join(ThreadStructs::row_threads[i],&status);
        Tuple & received_position = *((Tuple*)status);
        if (received_position.row != -1 && received_position.col != -1){
            if (!Utility::CheckIfExists(GLOBAL_DATA::invalid_boxes,received_position))
                GLOBAL_DATA::invalid_boxes.push_back(received_position);
            current_status = Validity::Invalid;
            GLOBAL_DATA::boxes_status[i] = Validity::Invalid;
        }
        pthread_join(ThreadStructs::column_threads[i],&status);
        if (received_position.row != -1 && received_position.col != -1){
            if (!Utility::CheckIfExists(GLOBAL_DATA::invalid_boxes,received_position))
                GLOBAL_DATA::invalid_boxes.push_back(received_position);
            current_status = Validity::Invalid;
            GLOBAL_DATA::boxes_status[i+9] = Validity::Invalid;
        }
        pthread_join(ThreadStructs::region_threads[i],&status);
        if (received_position.row != -1 && received_position.col != -1){
            if (!Utility::CheckIfExists(GLOBAL_DATA::invalid_boxes,received_position))
                GLOBAL_DATA::invalid_boxes.push_back(received_position);
            current_status = Validity::Invalid;
            GLOBAL_DATA::boxes_status[i+18] = Validity::Invalid;
        }
    }
    return current_status;
}

#endif