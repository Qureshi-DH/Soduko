#ifndef _SODUKO_GRID__
#define _SODUKO_GRID__

class Soduko{
    static int grid[9][9];
};

#ifdef _GRID_UNINITILIZED

Soduko::grid = {{},{},{},{},{},{},{},{},{}};

#undef _GRID_UNINITIALIZED
#define _GRID_INITILIZED
#endif

#endif