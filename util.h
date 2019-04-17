#ifndef _UTIL_HEAD
#define _UTIL_HEAD

namespace Utility{
    template <class type>
    bool CheckArray(type*array, unsigned size){
        for (unsigned i=0;i<size;++i)
            if (!array[i])
                return 0;
        return 1;
    }    
}

#endif