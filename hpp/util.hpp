#include <iostream>
#include <vector>
#include "tuple.hpp"

#ifndef _UTIL_HEAD
#define _UTIL_HEAD

namespace Utility{

    #define PRINT_GREEN(x) std::cout << "\033[1;32m" << x << "\033[0m"
    #define PRINT_RED(x)   std::cout << "\033[1;31m" << x << "\033[0m"

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
                    PRINT_RED(array[i][j]) << " ";
                }
                else{
                    PRINT_GREEN(array[i][j]) << " ";
                }
            }
            std::cout << std::endl;
        }
    }

    inline Tuple IndexToStartingIndex(Tuple rec){
        return Tuple(rec.row-rec.row%3,rec.col-rec.col%3);
    }

    inline int StartingIndexToBox(Tuple rec){
        return rec.row + rec.col/3;
    }

    inline int IndexToBox(Tuple rec){
        return StartingIndexToBox(IndexToStartingIndex(rec));
    }
}

enum Validity{
    Invalid,Valid
};

#endif