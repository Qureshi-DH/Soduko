#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <vector>

#include "grid.hpp"
#include "util.hpp"
#include "tuple.hpp"

//using namespace Utility;

#ifndef _THREAD_FUNCTIONS_
#define _THREAD_FUNCTIONS_

namespace ThreadStructs{
    pthread_t row_threads[9],column_threads[9],region_threads[9];
}

namespace GLOBAL_DATA{
    sem_t novux;
    int boxes_status[27] = {-1};
    std::vector <Tuple> invalid_boxes;
    int error_count[Soduko::dimension][Soduko::dimension] = {{0}};
}

namespace ThreadFunctions{
    Tuple invalid_position[27];
    void * ColumnChecker(void * args){
        unsigned test_array[9] = {0};
        unsigned & column = *((unsigned*)args);
        for (unsigned i=0;i<9;++i){
            if (Soduko::grid[i][column] > 9 || Soduko::grid[i][column] < 1){
                invalid_position[column+9] = Tuple(i,column);
            }
            else if (++test_array[Soduko::grid[i][column]-1] != 1){
                invalid_position[column+9] = Tuple(i,column);
            }
        }
        for (short i=0;i<9;++i){
            if (test_array[Soduko::grid[i][column]-1] == 2){
                sem_wait(&GLOBAL_DATA::novux);
                ++GLOBAL_DATA::error_count[i][column];
                sem_post(&GLOBAL_DATA::novux);
            }
            else if (test_array[Soduko::grid[i][column]-1] == 0){
                GLOBAL_DATA::boxes_status[i+9] = Soduko::grid[i][column];
            }
        }
        pthread_exit(&invalid_position[column+9]);
    }

    void * RowChecker(void * args){
        unsigned test_array[9] = {0};
        unsigned & row = *((unsigned*)args);
        for (unsigned i=0;i<9;++i){
            if (Soduko::grid[row][i] > 9 || Soduko::grid[row][i] < 1){
                invalid_position[row] = Tuple(row,i);
            }
            else if (++test_array[Soduko::grid[row][i]-1] != 1){
                invalid_position[row] = Tuple(row,i);
            }
        }
        for (short i=0;i<9;++i){
            if (test_array[Soduko::grid[row][i]-1] == 2){
                sem_wait(&GLOBAL_DATA::novux);
                ++GLOBAL_DATA::error_count[row][i];
                sem_post(&GLOBAL_DATA::novux);
            }
            else if (test_array[Soduko::grid[row][i]-1] == 0){
                GLOBAL_DATA::boxes_status[i] = Soduko::grid[row][i];
            }
        }
        pthread_exit(&invalid_position[row]);
    }

    void * RegionChecker(void * args){
        unsigned test_array[9] = {0};
        Tuple & tuple  = *((Tuple*)args);
        for (unsigned i=tuple.row*3,k=0;k<3;++i,++k){
            for (unsigned j=tuple.col*3,l=0;l<3;++j,++l){
                if (Soduko::grid[i][j] < 1 || Soduko::grid[i][j] > 9){
                    invalid_position[(tuple.row*3)+tuple.col+18] = Tuple(i,j);
                }
                else if (++test_array[Soduko::grid[i][j]-1] != 1){
                    invalid_position[(tuple.row*3)+tuple.col+18] = Tuple(i,j);
                }
            }
        }
        for (unsigned i=tuple.row*3,k=0;k<3;++i,++k){
            for (unsigned j=tuple.col*3,l=0;l<3;++j,++l){
                if (test_array[Soduko::grid[i][j]-1] == 2){
                    sem_wait(&GLOBAL_DATA::novux);
                    ++GLOBAL_DATA::error_count[i][j];
                    sem_post(&GLOBAL_DATA::novux);
                }
                else if (test_array[Soduko::grid[i][j]-1] == 0){
                    GLOBAL_DATA::boxes_status[i+18] = Soduko::grid[i][j];
                }
            }
        }

        pthread_exit(&invalid_position[(tuple.row*3)+tuple.col+18]);
    }

    void * FixInvalidEntry(void * args){

    }
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

bool* JoinThreads(){
    static bool error_type[2] = {false};
    for (short i=0;i<9;++i){
        void * status1,*status2,*status3;
        pthread_join(ThreadStructs::row_threads[i],&status1);
        Tuple & received_position1 = (*((Tuple*)status1));
        if (received_position1.row != -1 && received_position1.col != -1){
            if (!Utility::CheckIfExists(GLOBAL_DATA::invalid_boxes,received_position1))
                GLOBAL_DATA::invalid_boxes.push_back(received_position1);
            if (Soduko::grid[received_position1.row][received_position1.col] > 9 || Soduko::grid[received_position1.row][received_position1.col] < 1)
                error_type[0] = true;
            else
                error_type[1] = true;
            PRINT_RED ("Thread ID: ") 
                      << ThreadStructs::row_threads[i] 
                      << " calculated INVALID --> Row[" << i << "]"
                      << std::endl
                      << "(Row,Column) = (" << received_position1.row << "," <<received_position1.col << ") "
                      << "Value = " << Soduko::grid[received_position1.row][received_position1.col] 
                      << std::endl;
            pthread_cancel(ThreadStructs::row_threads[i]);
        }
        pthread_join(ThreadStructs::column_threads[i],&status2);
        Tuple & received_position2 = (*((Tuple*)status2));
        if (received_position2.row != -1 && received_position2.col != -1){
            if (!Utility::CheckIfExists(GLOBAL_DATA::invalid_boxes,received_position2))
                GLOBAL_DATA::invalid_boxes.push_back(received_position2);
            if (Soduko::grid[received_position2.row][received_position2.col] > 9 || Soduko::grid[received_position2.row][received_position2.col] < 1)
                error_type[0] = true;
            else
                error_type[1] = true;
            PRINT_RED ("Thread ID: ") 
                      << ThreadStructs::column_threads[i] 
                      << " calculated INVALID --> Column[" << i << "]"
                      << std::endl
                      << "(Row,Column) = (" << received_position2.row << "," <<received_position2.col << ") "
                      << "Value = " << Soduko::grid[received_position2.row][received_position2.col] 
                      << std::endl;
            pthread_cancel(ThreadStructs::column_threads[i]);
        }
        pthread_join(ThreadStructs::region_threads[i],&status3);
        Tuple & received_position3 = (*((Tuple*)status3));
        if (received_position3.row != -1 && received_position3.col != -1){
            if (!Utility::CheckIfExists(GLOBAL_DATA::invalid_boxes,received_position3))
                GLOBAL_DATA::invalid_boxes.push_back(received_position3);
            if (Soduko::grid[received_position3.row][received_position3.col] > 9 || Soduko::grid[received_position3.row][received_position3.col] < 1)
                error_type[0] = true;
            else
                error_type[1] = true;
            PRINT_RED ("Thread ID: ") 
                      << ThreadStructs::region_threads[i] 
                      << " calculated INVALID -- > Box[" << i << "]"
                      << std::endl
                      << "(Row,Column) = (" << received_position3.row << "," <<received_position3.col << ") "
                      << "Value = " << Soduko::grid[received_position3.row][received_position3.col] 
                      << std::endl;
            pthread_cancel(ThreadStructs::region_threads[i]);
        }
    }
    return error_type;
}

#endif