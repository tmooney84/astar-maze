#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct Pair {
    int y;
    int x;
} Pair;

typedef struct pPair {
    int f; 
    Pair pair;
} pPair;

typedef struct Cell {
    int parent_i, parent_j;
    int f, g, h;
} Cell;

//for inputs of ROW and COL from input instead of const
static int ROW = 0;
static int COL = 0;

//constructor for Pair
void make_pair(Pair *p, int row, int col){
    p->y = row;
    p->x = col;
}

//!!! adding in ROW and COL because they are not globals
bool is_valid(int row, int col){
    return(row >= 0) && (row < ROW) && (col >= 0)
    && (col < COL);
}

//!!! needs to be changed to '*' for grid of chars
bool is_unblocked(int grid[][COL], int row, int col){
    if(grid[row][col] == 1)
        return true;
    else
        return false;
}

bool is_destination(int row, int col, Pair dest){
    if(row == dest.y && col == dest.x)
        return true;
    else
        return false;
}

int calc_hvalue(int row, int col, Pair dest){
    return abs(row - dest.y) + abs(col - dest.x);
}

//!!! will need to add functionality for this to print
//the entire map

void trace_path(Cell cell_details[][COL], Pair dst){
    printf("\nThe Path is ");
    int row = dst.y;
    int col = dst.x;

//need to build with stack
Stack<Pair *> Path;

while (!(cell_details[row][col].parent_i == row
        && cell_details[row][col].parent_j == col)) {
        Pair *p = malloc(sizeof(Pair));
        if(!p){
            prinf("malloc error\n");
            exit();
        }
        Path.push(make_pair(p, row, col));
        int temp_row = cell_details[row][col].parent_i;
        int temp_col = cell_details[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }

    Path.push(make_pair(p, row, col));
    while(!Path.empty()){
        Pair *top = Path.top();
        Path.pop();
        printf("-> (%d,%d) ", p->y, p->x);
        free(top);
    }

    return;
}

//!!! may need src and dst to be pointers???
void a_star_search(int grid[][COL], Pair src, Pair dst){
    if(is_valid(src.y, src.x) == false){
        printf("Source is invalid\n");
        return;
    }

    //Either source or destination is blocked
    if(is_unblocked(grid, src.y, src.x) == false
        || is_unblocked(grid, dst.y, dst.x) ==false){
                    printf("Source or destintation is blocked\n");
                    return;
                }

    //If destination is same as source cell
    if (is_destination(src.y, src.x, dst) == true){
        printf("We are already at the destination\n");
        return;
    }

    bool closedList[ROW][COL];
    memset(closedList, false, sizeof(closedList));
}






int main(void){
    /*Initial Grid*/
    ROW = 9;
    COL = 10;

    int grid[ROW][COL]
        = { { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 },
            { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1 },
            { 1, 1, 1, 0, 1, 1, 0, 1, 0, 1 },
            { 0, 0, 1, 0, 1, 0, 0, 0, 0, 1 },
            { 1, 1, 1, 0, 1, 1, 1, 0, 1, 0 },
            { 1, 0, 1, 1, 1, 1, 0, 1, 0, 0 },
            { 1, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
            { 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 },
            { 1, 1, 1, 0, 0, 0, 1, 0, 0, 1 } };

Pair src = make_pair(8, 0);
Pair dst = make_pair(0,0);

a_star_search(grid, src, dst);

/*
$>cat -e 01.map
10x10* o12$
***1******$
*       **$
* *  **  *$
*        *$
** ** ** *$
*       **$
*      ***$
**       *$
*        *$
***2******$
$>./my_mouse 01.map
10x10* o12
***1******
*  o    **
* *o **  *
* oo     *
**o** ** *
* oo    **
*  o   ***
** o     *
*  o     *
***2******
10 STEPS!
*/


}