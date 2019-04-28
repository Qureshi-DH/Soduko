#ifndef _SODUKO_GRID__
#define _SODUKO_GRID__

namespace Soduko{
    static const int dimension = 9;
    static const int grid[dimension][dimension] = { {8,3,5,4,1,6,0,2,7},
                                                    {2,9,6,8,5,7,4,3,1},
                                                    {4,1,7,2,9,3,6,5,8},
                                                    {5,6,9,6,3,4,7,8,2},
                                                    {1,2,3,6,7,8,5,4,9},
                                                    {7,4,8,5,2,9,1,6,3},
                                                    {6,5,2,7,8,1,3,9,4},
                                                    {9,8,1,3,4,5,2,7,6},
                                                    {3,7,5,9,6,2,8,1,4} };
}

#endif