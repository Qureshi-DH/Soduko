#include "../hpp/thread.hpp"

using namespace GLOBAL_DATA;

int main(int argc,char*argv[]){
    sem_init(&GLOBAL_DATA::novux,0,1);
    CreateThreads();
    bool status = JoinThreads();
    std::cout << "---------------------\n";
    Utility::Print2DArray(Soduko::grid,9,GLOBAL_DATA::invalid_boxes);
    std::cout << "---------------------\n";
    std::cout << "Validity = ";
    status?(PRINT_GREEN("Valid")):(PRINT_RED("Invalid")) << std::endl;
    pthread_exit(NULL);
}
