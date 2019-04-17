#include <iostream>

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
    void Print2DArray(type ** array,unsigned dim_1,unsigned dim_2){
        for (unsigned i=0;i<dim_1;++i){
            for (unsigned j=0;j<dim_2;++j){
                std::cout << array[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }  
}

#endif