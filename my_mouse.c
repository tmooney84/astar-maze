#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAX_LENGTH 1000

void malloc_error(){
    fprintf(stderr, "Memory Allocation Error\n");
    exit(1);
}

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

//pStack push pop peek empty
typedef struct pNode{
    Pair *pair;
    struct pNode *next;
}pNode;

typedef struct pStack {
    pNode *step[MAX_LENGTH * MAX_LENGTH];
    int highest_used;
}pStack;

//node wrapper
pNode *new_pNode(Pair *pair){
    pNode *n = malloc(sizeof(pNode));
    if(!n){
        malloc_error();
    }
    n->pair = pair;

}

//free node
void free_pNode(pNode * n){
    free(n->pair);
    n->pair = NULL;
    free(n);
    n = NULL;
    return;
}

//new stack
pStack *new_pStack(void){
    pStack *s = malloc(sizeof(pStack));
    if(!s){
        malloc_error();
    }
    s->highest_used = -1;

    return s;
}

//push to stack
void push_pStack(pStack *s, Pair *p){
   pNode *n = malloc(sizeof(pNode));
   if(!n){
    malloc_error();
   }
   s->highest_used++;
   s->step[s->highest_used] = n;
   return; 
}

//pop from stack
Pair *pop_pStack(pStack *s){
    pNode *top = s->step[s->highest_used];
    s->highest_used++;
    Pair *top_coor = top->pair; 
    free(top);
    return top_coor;
}

//is empty
bool is_empty_pStack(pStack *s){
    return s->highest_used == -1;
}

/*****************pPair Set Logic *****************/


//for inputs of ROW and COL from input instead of const
static int ROW = 0;
static int COL = 0;

//constructor for Pair *
Pair *make_pair(int row, int col){
    Pair *p = malloc(sizeof(Pair));
        if(!p){
            prinf("malloc error\n");
            exit(1);
        }
    
    p->y = row;
    p->x = col;
}

//constructor for pPair *
pPair *make_p_pair(int f, int row, int col){
    pPair *pp = malloc(sizeof(Pair));
        if(!pp){
            prinf("malloc error\n");
            exit(1);
        }
    
    pp->pair.y = row;
    pp->pair.x = col;
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

bool is_destination(int row, int col, Pair dst){
    if(row == dst.y && col == dst.x)
        return true;
    else
        return false;
}

int calc_hvalue(int row, int col, Pair dst){
    return abs(row - dst.y) + abs(col - dst.x);
}

//!!! will need to add functionality for this to print
//the entire map

int trace_path(Cell cell_details[][COL], Pair dst){
    printf("\nThe Path is ");
    int row = dst.y;
    int col = dst.x;

//need to build with stack
pStack *steps = new_pStack();

while (!(cell_details[row][col].parent_i == row
        && cell_details[row][col].parent_j == col)) {
        Pair *p1 = make_pair(row, col);
        push_pStack(steps, p1);
        int temp_row = cell_details[row][col].parent_i;
        int temp_col = cell_details[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }
    Pair *p2 = make_pair(row, col);
    push_pStack(steps, p2);

    //!!! Is this an off by one error?
    int num_steps = steps->highest_used + 1;  

    while(!is_empty_pStack(steps)){
        Pair *top_pair = pop_pStack(steps);
        printf("-> (%d,%d) ", top_pair->y, top_pair->x);
        free(top_pair);
    }

    free(steps);

    //!!!reconstruct map with 'o' at each of the coordinates
    //of the path and maybe print here???

    return;
}

//!!! may need src and dst to be pointers???
void a_star_search(int grid[][COL], Pair *src, Pair *dst){
    if(is_valid(src->y, src->x) == false){
        printf("Source is invalid\n");
        return;
    }

    //Either source or destination is blocked
    if(is_unblocked(grid, src->y, src->x) == false
        || is_unblocked(grid, dst->y, dst->x) ==false){
                    printf("Source or destintation is blocked\n");
                    return;
                }

    //If destination is same as source cell
    if (is_destination(src->y, src->x, *dst) == true){
        printf("We are already at the destination\n");
        return;
    }

    bool closed_list[ROW][COL];
    memset(closed_list, false, sizeof(closed_list));

    Cell cell_details[ROW][COL];
    
    int i, j;

    for(i = 0; i < ROW; i++){
        for(j = 0; j < COL; j++){
            cell_details[i][j].f = __INT_MAX__;
            cell_details[i][j].g = __INT_MAX__;
            cell_details[i][j].h = __INT_MAX__;
            cell_details[i][j].parent_i = -1;
            cell_details[i][j].parent_j = -1;
        }
    }

    //Initializing parameters of starting node
    i = src->y;
    j = src->x;
    cell_details[i][j].f = 0.0;
    cell_details[i][j].g = 0.0;
    cell_details[i][j].h = 0.0;
    cell_details[i][j].parent_i = i;
    cell_details[i][j].parent_j = j;

   //replicate as a hashmap???
   //maybe make same as 1000x1000 grid?
   
   //new_pPair_set(), insert_pPair_set(), begin()/iterator functionality,
   //erase()

    //set<pPair *> open_list;
    static pPair *open_list[MAX_LENGTH * MAX_LENGTH];
    //!!!need to free pPair elements after finished


    open_list.insert(make_p_pair(0, i, j));

    bool found_dst = false;

    while(!open_list.empty()){
        pPair pp = *openList.begin();

        openList.erase(openList.begin());

        i = pp.pair.y;
        j = pp.pair.x;
        closed_list[i][j] = true;
    
        /*
            Popped Cell (i, j)
            N --> (i - 1, j)
            S --> (i + 1, j) 
            E --> (i, j + 1)
            W --> (i, j - 1)  
        */

        //to store g, h and f of the 4 successors
        int g_new, h_new, f_new;

        //North Sucessor
        if(is_valid(i -1, j) == true){
            if(is_destination(i - 1, j, *dst) == true){
                cell_details[i - 1][j].parent_i = i;
                cell_details[i - 1][j].parent_j = j;
                printf("The destination cell is found\n");
                trace_path(cell_details, *dst);
                found_dst = true;
                return;
            }
//if successor is closed list or if it is blocked, ignore
//else do this
            else if(closed_list[i - 1][j] == false
            && is_unblocked(grid, i - 1, j) == true){
            g_new = cell_details[i][j].g + 1;
            h_new = calc_hvalue(i - 1, j, *dst);
            f_new = g_new + h_new;
        
        
//if not on open list then add;; make current sqare parent and record fgh
//-or- if on open list, check if this better with f
            if(cell_details[i - 1][j].f == __INT_MAX__
            || cell_details[i - 1][j].f > f_new){
                open_list.insert(make_p_pair(f_new,i - 1, j));
            
                cell_details[i - 1][j].f = f_new;
                cell_details[i - 1][j].g = g_new;
                cell_details[i - 1][j].h = h_new;
                cell_details[i - 1][j].parent_i = i;
                cell_details[i - 1][j].parent_j = j;
            }
            }
        }
   
    //South Successor
        if(is_valid(i + 1, j) == true){
            if(is_destination(i + 1, j, *dst) == true){
                cell_details[i + 1][j].parent_i = i;
                cell_details[i + 1][j].parent_j = j;
                printf("The destination cell is found\n");
                trace_path(cell_details, *dst);
                found_dst = true;
                return;
            }
//if successor is closed list or if it is blocked, ignore
//else do this
            else if(closed_list[i + 1][j] == false
            && is_unblocked(grid, i + 1, j) == true){
            g_new = cell_details[i][j].g + 1;
            h_new = calc_hvalue(i + 1, j, *dst);
            f_new = g_new + h_new;
        
        
//if not on open list then add;; make current sqare parent and record fgh
//-or- if on open list, check if this better with f
            if(cell_details[i + 1][j].f == __INT_MAX__
            || cell_details[i + 1][j].f > f_new){
                open_list.insert(make_p_pair(f_new,i - 1, j));
            
                cell_details[i + 1][j].f = f_new;
                cell_details[i + 1][j].g = g_new;
                cell_details[i + 1][j].h = h_new;
                cell_details[i + 1][j].parent_i = i;
                cell_details[i + 1][j].parent_j = j;
            }
            }
        }
    //East Successor
    if(is_valid(i, j + 1) == true){
            if(is_destination(i, j + 1, *dst) == true){
                cell_details[i][j + 1].parent_i = i;
                cell_details[i][j + 1].parent_j = j;
                printf("The destination cell is found\n");
                trace_path(cell_details, *dst);
                found_dst = true;
                return;
            }
//if successor is closed list or if it is blocked, ignore
//else do this
            else if(closed_list[i][j + 1] == false
            && is_unblocked(grid, i, j + 1) == true){
            g_new = cell_details[i][j].g + 1;
            h_new = calc_hvalue(i, j + 1, *dst);
            f_new = g_new + h_new;
        
        
//if not on open list then add;; make current sqare parent and record fgh
//-or- if on open list, check if this better with f
            if(cell_details[i][j + 1].f == __INT_MAX__
            || cell_details[i][j + 1].f > f_new){
                open_list.insert(make_p_pair(f_new,i - 1, j));
            
                cell_details[i][j + 1].f = f_new;
                cell_details[i][j + 1].g = g_new;
                cell_details[i][j + 1].h = h_new;
                cell_details[i][j + 1].parent_i = i;
                cell_details[i][j + 1].parent_j = j;
            }
            }
        }
    //West Successor
    if(is_valid(i, j - 1) == true){
            if(is_destination(i, j - 1, *dst) == true){
                cell_details[i][j - 1].parent_i = i;
                cell_details[i][j - 1].parent_j = j;
                printf("The destination cell is found\n");
                trace_path(cell_details, *dst);
                found_dst = true;
                return;
            }
//if successor is closed list or if it is blocked, ignore
//else do this
            else if(closed_list[i][j - 1] == false
            && is_unblocked(grid, i, j - 1) == true){
            g_new = cell_details[i][j].g - 1;
            h_new = calc_hvalue(i, j - 1, *dst);
            f_new = g_new + h_new;
        
        
//if not on open list then add;; make current sqare parent and record fgh
//-or- if on open list, check if this better with f
            if(cell_details[i][j - 1].f == __INT_MAX__
            || cell_details[i][j - 1].f > f_new){
                open_list.insert(make_p_pair(f_new,i - 1, j));
            
                cell_details[i][j - 1].f = f_new;
                cell_details[i][j - 1].g = g_new;
                cell_details[i][j - 1].h = h_new;
                cell_details[i][j - 1].parent_i = i;
                cell_details[i][j - 1].parent_j = j;
                }
            }
        }
    }
    if(found_dst == false){
        printf("Failed to find the Destination Cell\n");

        return;
    }
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

Pair *src = make_pair(8, 0);
Pair *dst = make_pair(0, 0);

a_star_search(grid, src, dst);

free(src);
src = NULL;
free(dst);
dst = NULL;

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

//!!! Will likely have to do readline have to make a 
//first pass through to build map representation and make find the 
//start + end points and barriers

//once built 
}