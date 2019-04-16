#ifndef _TUPLE__
#define _TUPLE__

struct tuple{
    int row;
    int col;
    tuple():row(0),col(0){}
    tuple(int _row,int _col):row(_row),col(_col){}
};

#endif