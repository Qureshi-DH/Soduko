#include <iostream>
#include <vector>
#include "tuple.hpp"

#ifndef _UTIL_HEAD
#define _UTIL_HEAD

namespace Utility{
    template <class type>
    bool CheckArray(type*array, unsigned size){
        for (unsigned i=0;i<size;++i)
            if (array[i] != 1)
                return 0;
        return 1;
    }

    template <class type>
    bool CheckIfExists(std::vector<type> & list,type element){
        for (size_t i=0;i<list.size();++i)
            if (list[i] == element)
                return 1;
        return 0;
    }

    template <class type>
    void Print2DArray(const type array[9][9],unsigned dimension,std::vector<Tuple>& list){
        for (unsigned i=0;i<dimension;++i){
            for (unsigned j=0;j<dimension;++j){
                Tuple auxiliary(i,j);
                if (CheckIfExists(list,auxiliary)){
                    std::cout << "\033[1;31m"
                              <<(array[i][j])
                              << "\033[0m"
                              <<" ";
                }
                else{
                    std::cout << "\033[1;32m"
                              <<(array[i][j])
                              << "\033[0m"
                              <<" ";
                }
            }
            std::cout << std::endl;
        }
    }
}

enum Validity{
    Invalid,Valid
};

#endif