#include "../hpp/thread.hpp"

using namespace GLOBAL_DATA;

int main(int argc,char*argv[]){
    sem_init(&GLOBAL_DATA::novux,0,1);
    pthread_mutex_init(&GLOBAL_DATA::mutex,NULL);
    CreateThreads();
    bool * error_type;
    error_type =  JoinThreads();
    std::cout << "---------------------\n";
    Utility::Print2DArray(Soduko::grid,9,GLOBAL_DATA::invalid_boxes);
    std::cout << "---------------------\n";
    std::cout << "Invalid Indexes = ";
    (!invalid_boxes.size())?(PRINT_GREEN(invalid_boxes.size())):(PRINT_RED(invalid_boxes.size())) << std::endl;
    std::cout << "---------------------\n";
    if (error_type[0] && error_type[1]){
        std::cout << "Error was because of digits greater than 9 or less than 1 AND because of invalid placement\n";
        pthread_create(&ThreadStructs::fix,NULL,ThreadFunctions::FixInvalidEntry,NULL);
    }
    else if (error_type[0] && !error_type[1]){
        std::cout << "Error was because of digits greater than 9 or less than 1\n";
        pthread_create(&ThreadStructs::fix,NULL,ThreadFunctions::FixInvalidEntry,NULL);
    }
    else if (!error_type[0] && error_type[1]){
        std::cout << "Error was because of invalid placement\n";
        pthread_create(&ThreadStructs::fix,NULL,ThreadFunctions::FixInvalidEntry,NULL);
    }
    else{
         std::cout << "The grid was perfectly fine\n";
    }
    pthread_exit(NULL);
}
