#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>

#include "grid.hpp"
#include "util.h"
#include "thread.hpp"

using namespace GLOBAL_DATA;

int main(int argc,char*argv[]){
    sem_init(&GLOBAL_DATA::novux,0,1);
    CreateThreads();
    bool status = JoinThreads();
    Utility::Print2DArray(Soduko::grid,9,GLOBAL_DATA::invalid_boxes);
    std::cout << "Validity = " << (status?("\033[1;32mValid\033[0m"):("\033[1;31mInvalid\033[0m")) << std::endl;
    pthread_exit(NULL);
}