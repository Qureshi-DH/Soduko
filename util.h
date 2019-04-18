#include <iostream>
#include <vector>

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
    void Print2DArray(type array[9][9],unsigned dim_1,unsigned dim_2){
        for (unsigned i=0;i<dim_1;++i){
            for (unsigned j=0;j<dim_2;++j){
                std::cout << array[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    template <class type>
    bool CheckIfExists(std::vector<type> & list,type element){
        for (size_t i=0;i<list.size();++i)
            if (list[i] == element)
                return 1;
        return 0;
    }
}

enum Validity{
    Invalid,Valid
};

#endif