#ifndef _SODUKO_GRID__
#define _SODUKO_GRID__

namespace Soduko{
    const int dimension = 9;
    int grid[dimension][dimension] =   {{8, 9, 6, 2, 7, 6, 4, 6, 1},
                                        {2, 7, 6, 4, 5, 1, 8, 9, 3},
                                        {4, 5, 1, 8, 9, 3, 2, 7, 6},
                                        {5, 1, 8, 9, 3, 2, 7, 6, 4},
                                        {9, 3, 2, 7, 6, 4, 5, 1, 8},
                                        {7, 6, 4, 5, 1, 8, 9, 3, 2},
                                        {8, 4, 8, 1, 8, 9, 3, 2, 7},
                                        {1, 8, 9, 3, 2, 7, 6, 4, 5},
                                        {3, 2, 7, 6, 4, 5, 1, 8, 9}};
}

#endif