#ifndef _TUPLE__
#define _TUPLE__

struct Tuple{
    int row;
    int col;
    Tuple():row(-1),col(-1){}
    Tuple(int _row,int _col):row(_row),col(_col){}
};

bool operator==(const Tuple& left,const Tuple& right){
    return ((left.row == right.row) && (left.col == right.col));
}

#endif