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
    pthread_t row_threads[9],column_threads[9],region_threads[9],fix;
}

namespace GLOBAL_DATA{
    sem_t novux;
    pthread_mutex_t mutex;
    int boxes_status[27] = {0};
    std::vector <Tuple> invalid_boxes;
    int error_count[Soduko::dimension][Soduko::dimension] = {{0}};
    pthread_t correction_threads[270];
    unsigned correction_threads_count = 0;
}

namespace ThreadFunctions{
    Tuple invalid_position[27];
    bool error_present[3];
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
            if (test_array[Soduko::grid[i][column]-1] > 1){
                pthread_mutex_lock(&GLOBAL_DATA::mutex);
                ++GLOBAL_DATA::error_count[i][column];
                pthread_mutex_unlock(&GLOBAL_DATA::mutex);
            }
            if (test_array[i] == 0){
                GLOBAL_DATA::boxes_status[column+9] = i+1;
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
            if (test_array[Soduko::grid[row][i]-1] > 1){
                pthread_mutex_lock(&GLOBAL_DATA::mutex);
                ++GLOBAL_DATA::error_count[row][i];
                pthread_mutex_unlock(&GLOBAL_DATA::mutex);
            }
            if (test_array[i] == 0){
                GLOBAL_DATA::boxes_status[row] = i+1;
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
                if (test_array[Soduko::grid[i][j]-1] > 1){
                    pthread_mutex_lock(&GLOBAL_DATA::mutex);
                    ++GLOBAL_DATA::error_count[i][j];
                    pthread_mutex_unlock(&GLOBAL_DATA::mutex);
                }
                if (test_array[(k*3)+l] == 0){
                    using namespace Utility;
                    GLOBAL_DATA::boxes_status[IndexToBox(Tuple(i,j))+18] = (k*3)+l+1;
                }
            }
        }
        pthread_exit(&invalid_position[(tuple.row*3)+tuple.col+18]);
    }

    void * RowVerifier(void * args){
        unsigned test_array[9] = {0};
        unsigned & row = *((unsigned*)args);
        for (unsigned i=0;i<9;++i){
            if (Soduko::grid[row][i] > 9 || Soduko::grid[row][i] < 1){
                error_present[0] = true;
            }
            else if (++test_array[Soduko::grid[row][i]-1] != 1){
                error_present[0] = true;
            }
        }
        pthread_exit(&error_present[0]);
    }

    void * ColumnVerifier(void * args){
        unsigned test_array[9] = {0};
        unsigned & col = *((unsigned*)args);
        for (unsigned i=0;i<9;++i){
            if (Soduko::grid[i][col] > 9 || Soduko::grid[i][col] < 1){
                error_present[1] = true;
            }
            else if (++test_array[Soduko::grid[i][col]-1] != 1){
                error_present[1] = true;
            }
        }
        pthread_exit(&error_present[1]);
    }

    void * RegionVerifier(void * args){
        unsigned test_array[9] = {0};
        Tuple & tuple  = *((Tuple*)args);
        for (unsigned i=tuple.row*3,k=0;k<3;++i,++k){
            for (unsigned j=tuple.col*3,l=0;l<3;++j,++l){
                if (Soduko::grid[i][j] < 1 || Soduko::grid[i][j] > 9){
                    error_present[2] = true;
                }
                else if (++test_array[Soduko::grid[i][j]-1] != 1){
                    error_present[2] = true;
                }
            }
        }
        pthread_exit(&error_present[2]);
    }

    bool Validate(Tuple position){
        Tuple auxiliary = Utility::IndexToStartingIndex(position);
        auxiliary.row /= 3;
        auxiliary.col /= 3;

        error_present[0] = error_present[1] = error_present[2] = false;
        sem_wait(&GLOBAL_DATA::novux);

        pthread_create(&GLOBAL_DATA::correction_threads[GLOBAL_DATA::correction_threads_count++],NULL,ThreadFunctions::RowVerifier,&position.row);
        pthread_create(&GLOBAL_DATA::correction_threads[GLOBAL_DATA::correction_threads_count++],NULL,ThreadFunctions::ColumnVerifier,&position.col);
        pthread_create(&GLOBAL_DATA::correction_threads[GLOBAL_DATA::correction_threads_count++],NULL,ThreadFunctions::RegionVerifier,&auxiliary);
  
        void *status1,*status2,*status3;
        int local_count = GLOBAL_DATA::correction_threads_count;
        sem_post(&GLOBAL_DATA::novux);

        pthread_join(GLOBAL_DATA::correction_threads[local_count-3],&status1);
        pthread_join(GLOBAL_DATA::correction_threads[local_count-2],&status2);
        pthread_join(GLOBAL_DATA::correction_threads[local_count-1],&status3);

        return ((*(bool*)status1 == 0) && (*(bool*)status2 == 0)) || 
               ((*(bool*)status3 == 0) && (*(bool*)status2 == 0)) ||
               ((*(bool*)status1 == 0) && (*(bool*)status3 == 0)) ;
    }

    void * FixInvalidEntry(void * args){
        for (short i=0;i<9;++i){
            for (short j=0;j<9;++j){
                if (GLOBAL_DATA::error_count[i][j] == 3){
                    if (!Validate(Tuple(i,j))){
                        Soduko::grid[i][j] = GLOBAL_DATA::boxes_status[i];
                        GLOBAL_DATA::error_count[i][j] = 0;
                        if (!Validate(Tuple(i,j))){
                            GLOBAL_DATA::error_count[i][j] = 0;
                            Soduko::grid[i][j] = GLOBAL_DATA::boxes_status[j+9];
                        }
                        if (!Validate(Tuple(i,j))){
                            GLOBAL_DATA::error_count[i][j] = 0;
                            Soduko::grid[i][j] = GLOBAL_DATA::boxes_status[Utility::IndexToBox(Tuple(i,j))+18];
                        }
                    }
                }
            }
        }

        for (short i=0;i<9;++i){
            for (short j=0;j<9;++j){
                if (GLOBAL_DATA::error_count[i][j] == 2){
                    if (!Validate(Tuple(i,j))){
                        Soduko::grid[i][j] = GLOBAL_DATA::boxes_status[i];
                        GLOBAL_DATA::error_count[i][j] = 0;
                        if (!Validate(Tuple(i,j))){
                            GLOBAL_DATA::error_count[i][j] = 0;
                            Soduko::grid[i][j] = GLOBAL_DATA::boxes_status[j+9];
                        }
                        if (!Validate(Tuple(i,j))){
                            GLOBAL_DATA::error_count[i][j] = 0;
                            Soduko::grid[i][j] = GLOBAL_DATA::boxes_status[Utility::IndexToBox(Tuple(i,j))+18];
                        }
                    }
                }
            }
        }
        std::cout << "---------------------\n";
        GLOBAL_DATA::invalid_boxes.clear();
        Utility::Print2DArray(Soduko::grid,Soduko::dimension,GLOBAL_DATA::invalid_boxes);
        std::cout << "---------------------\n";
        PRINT_RED ("Main Fixing Thread ID: ")  << ThreadStructs::fix << std::endl;
        std::cout << "---------------------\n";
        PRINT_GREEN ("Number of threads created against all invalid indexes: ") << GLOBAL_DATA::correction_threads_count << std::endl;
        std::cout << "---------------------\n";
        for (short i=0;i<9;++i){
            PRINT_GREEN("Thread ID: ") << ThreadStructs::row_threads[i] << " created against row: " << i << std::endl;
            PRINT_GREEN("Thread ID: ") << ThreadStructs::column_threads[i] << " created against column: " << i << std::endl;
            PRINT_GREEN("Thread ID: ") << ThreadStructs::region_threads[i] << " created against region: " << i << std::endl;
        }
        std::cout << "---------------------\n";
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