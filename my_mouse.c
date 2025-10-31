#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//#define ROW 10
//#define COL 10

#define MAX_LENGTH 1000
#define HEAD_MAX 1000

static int ROW; 
static int COL; 

void malloc_error()
{
    fprintf(stderr, "Memory Allocation Error\n");
    exit(1);
}

typedef struct Pair
{
    int y;
    int x;
} Pair;

typedef struct pPair
{
    int f;
    Pair pair;
} pPair;

typedef struct Cell
{
    int parent_i, parent_j;
    int f, g, h;
} Cell;

// pStack push pop peek empty
typedef struct pNode
{
    Pair *pair;
    struct pNode *next;
} pNode;

typedef struct pStack
{
    pNode *step[MAX_LENGTH * MAX_LENGTH];
    int highest_used;
} pStack;

// node wrapper
pNode *new_pNode(Pair *pair)
{
    pNode *n = malloc(sizeof(pNode));
    if (!n)
    {
        malloc_error();
    }
    n->pair = pair;

    return n;
}

// free node
void free_pNode(pNode *n)
{
    free(n->pair);
    n->pair = NULL;
    free(n);
    n = NULL;
    return;
}
/***************pStack Logic******************************/
// new stack
pStack *new_pStack(void)
{
    pStack *s = malloc(sizeof(pStack));
    if (!s)
    {
        malloc_error();
    }
    s->highest_used = -1;

    return s;
}

// is empty stack
bool is_empty_pStack(pStack *s)
{
    return s->highest_used == -1 ? true : false;
}

// push to stack
void push_pStack(pStack *s, Pair *p)
{
    if (s->highest_used >= MAX_LENGTH * MAX_LENGTH - 1)
    {
        fprintf(stderr, "Error: Stack overflow\n");
        return;
    }

    pNode *n = new_pNode(p);
    s->highest_used++;
    s->step[s->highest_used] = n;
    return;
}

// pop from stack
Pair *pop_pStack(pStack *s)
{
    if (is_empty_pStack(s))
    {
        fprintf(stderr, "Error: Attempt to pop from empty stack\n");
        return NULL;
    }
    pNode *top = s->step[s->highest_used];
    Pair *top_coor = top->pair;
    s->highest_used--;
    free(top);
    return top_coor;
}

/*****************pPair Set Logic *****************/
typedef struct pPair_set
{
    pPair *list[MAX_LENGTH * MAX_LENGTH];
    int start_idx;
    int num_elements;
} pPair_set;

// insert at location
void insert_pPair_at(pPair_set *set, pPair *p, int start_idx, int end_idx)
{
    pPair *temp = set->list[start_idx];
    if (end_idx < (int)(sizeof(set->list) / sizeof(set->list[0]) - 1))
    {
        while (start_idx <= end_idx + 1)
        {
            set->list[start_idx] = p;
            p = temp;
            temp = set->list[++start_idx];
        }
        set->num_elements++;
        return;
    }
    else
        perror("insert_pPair_at() unable to"
               "insert element due to exceeding array size.\n");
}

void insert_pPair_set(pPair_set *set, pPair *p)
{
    if (set->num_elements >= MAX_LENGTH)
    {
        printf("Open list overflow!\n");
        return;
    }

    int i = set->num_elements - 1;

    // Shift all elements with higher f (or same f but higher y/x) up one position
    while (i >= 0 && (set->list[i]->f > p->f ||
                      (set->list[i]->f == p->f && set->list[i]->pair.y > p->pair.y) ||
                      (set->list[i]->f == p->f && set->list[i]->pair.y == p->pair.y &&
                       set->list[i]->pair.x > p->pair.x)))
    {
        set->list[i + 1] = set->list[i];
        i--;
    }

    // Insert new element in its sorted position
    set->list[i + 1] = p;
    set->num_elements++;
    return;
}

void update_pPair_set(pPair_set *set, int old_f, pPair *p)
{
    int start_update_idx = -1;
    int end_update_idx = -1;

    int end = (set->start_idx + set->num_elements - 1) % (sizeof(set->list) / sizeof(set->list[0]));
    for (int i = set->start_idx; i <= end; i++)
    {
        if (set->list[i]->f == p->f)
        {
            while (set->list[i]->f == p->f && set->list[i]->pair.y > p->pair.y)
            {
                i++;
                while (set->list[i]->f == p->f && set->list[i]->pair.y == p->pair.y && set->list[i]->pair.x > p->pair.x)
                {
                    i++;
                }
            }
            start_update_idx = i;

            // end_update_idx: find the old pPair to mark as end of copying
            while (i < end && (set->list[i]->f != old_f || set->list[i]->pair.y != p->pair.y || set->list[i]->pair.x != p->pair.x))
            {
                i++;
            }
            end_update_idx = i;
            if (end_update_idx == start_update_idx)
            {
                fprintf(stderr, "update_pPair_set");
                exit(1);
            }

            insert_pPair_at(set, p, start_update_idx, end_update_idx - 1);
            return;
        }
        fprintf(stderr, "update_pPair_set error");
        exit(1);
    }
}

bool is_empty_pPair_set(pPair_set *set)
{
    return (set->num_elements == 0);
}

// constructor for Pair *
Pair *make_pair(int row, int col)
{
    Pair *p = malloc(sizeof(Pair));
    if (!p)
    {
        printf("malloc error\n");
        exit(1);
    }

    p->y = row;
    p->x = col;

    return p;
}

// constructor for pPair *
pPair *make_p_pair(int f, int row, int col)
{
    pPair *pp = malloc(sizeof(pPair));
    if (!pp)
    {
        printf("malloc error\n");
        exit(1);
    }

    pp->f = f;
    pp->pair.y = row;
    pp->pair.x = col;

    return pp;
}

bool is_valid(int row, int col)
{
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

bool is_unblocked(int **grid, int row, int col)
{
    if (grid[row][col] == 1)
        return true;
    else
        return false;
}

bool is_destination(int row, int col, Pair dst)
{
    if (row == dst.y && col == dst.x)
        return true;
    else
        return false;
}

int calc_hvalue(int row, int col, Pair dst)
{
    return abs(row - dst.y) + abs(col - dst.x);
}

void trace_path(FILE *fp, Cell cell_details[][COL], int **grid, Pair dst)
{
    int row = dst.y;
    int col = dst.x;

    pStack *steps = new_pStack();

    while (!(cell_details[row][col].parent_i == row && cell_details[row][col].parent_j == col))
    {
        Pair *p1 = make_pair(row, col);
        push_pStack(steps, p1);
        int temp_row = cell_details[row][col].parent_i;
        int temp_col = cell_details[row][col].parent_j;
        row = temp_row;
        col = temp_col;
    }
    Pair *p2 = make_pair(row, col);
    push_pStack(steps, p2);

    int num_steps = steps->highest_used + 1;
    int counter = num_steps;
    while (!is_empty_pStack(steps))
    {
        Pair *top_pair = pop_pStack(steps);
     //   printf("-> (%d,%d) ", top_pair->y, top_pair->x);
        // 7 is a placeholder
        if(counter == num_steps)
            grid[top_pair->y][top_pair->x] = 9;
        else if(counter == 1)
            grid[top_pair->y][top_pair->x] = 2;
        else
            grid[top_pair->y][top_pair->x] = 7;
        
        counter--;
        free(top_pair);
    }

    fseek(fp, 0, SEEK_SET);
   
    //format to original map 
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(grid[i][j] == 9)
                fprintf(fp, "1");
            else if(grid[i][j] == 2)
               fprintf(fp, "2"); 
            else if(grid[i][j] == 0)
                fprintf(fp, "*");
            else if(grid[i][j] == 7)
                fprintf(fp, "o");
            else
                fprintf(fp, " ");
        }
        if(i < ROW - 1){
            fprintf(fp, "\n");
        }
    }

    //-2 to account for the start and end
    fprintf(fp, "\n%d STEPS!\n", num_steps - 2);

    free(steps);
    return;
}

//!!! may need src and dst to be pointers???
void a_star_search(FILE *fp, int **grid, Pair *src, Pair *dst)
{
    if (is_valid(src->y, src->x) == false)
    {
        printf("Source is invalid\n");
        return;
    }

    // Either source or destination is blocked
    if (is_unblocked(grid, src->y, src->x) == false || is_unblocked(grid, dst->y, dst->x) == false)
    {
        printf("Source or destintation is blocked\n");
        return;
    }

    // If destination is same as source cell
    if (is_destination(src->y, src->x, *dst) == true)
    {
        printf("We are already at the destination\n");
        return;
    }

    bool closed_list[ROW][COL];
    memset(closed_list, 0, sizeof(closed_list));

    Cell cell_details[ROW][COL];

    int i, j;

    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL; j++)
        {
            cell_details[i][j].f = __INT_MAX__;
            cell_details[i][j].g = __INT_MAX__;
            cell_details[i][j].h = __INT_MAX__;
            cell_details[i][j].parent_i = -1;
            cell_details[i][j].parent_j = -1;
        }
    }

    // Initializing parameters of starting node
    i = src->y;
    j = src->x;
    cell_details[i][j].f = 0.0;
    cell_details[i][j].g = 0.0;
    cell_details[i][j].h = 0.0;
    cell_details[i][j].parent_i = i;
    cell_details[i][j].parent_j = j;

    pPair_set *open_list = malloc(sizeof(pPair_set));
    if (!open_list)
    {
        malloc_error();
    }
    open_list->start_idx = -1;
    open_list->num_elements = 0;

    insert_pPair_set(open_list, make_p_pair(0, i, j));

    bool found_dst = false;

    while (open_list->num_elements > 0)
    {
        pPair *pp = open_list->list[0];

        for (int k = 0; k < open_list->num_elements - 1; k++)
            open_list->list[k] = open_list->list[k + 1];
        open_list->num_elements--;

        if (open_list->start_idx >= MAX_LENGTH * MAX_LENGTH)
        {
            fprintf(stderr, "open_list exceeds maximum size");
            exit(1);
        }

        i = pp->pair.y;
        j = pp->pair.x;
        closed_list[i][j] = true;

        /*
            Popped Cell (i, j)
            N --> (i - 1, j)
            S --> (i + 1, j)
            E --> (i, j + 1)
            W --> (i, j - 1)
        */

        // to store g, h and f of the 4 successors
        int g_new, h_new, f_new;

        // North Sucessor
        if (is_valid(i - 1, j) == true)
        {
            if (is_destination(i - 1, j, *dst) == true)
            {
                cell_details[i - 1][j].parent_i = i;
                cell_details[i - 1][j].parent_j = j;
                //printf("The destination cell is found\n");
                trace_path(fp, cell_details, grid, *dst);
                found_dst = true;
                // free pPair after necessary data extracted and open_list
                free(pp);
                pp = NULL;
                free(open_list);
                return;
            }
            else if (closed_list[i - 1][j] == false && is_unblocked(grid, i - 1, j) == true)
            {
                g_new = cell_details[i][j].g + 1;
                h_new = calc_hvalue(i - 1, j, *dst);
                f_new = g_new + h_new;

                //***“Ignore closed” optimization (common for uniform grids)
                if (cell_details[i - 1][j].f == __INT_MAX__)
                {
                    pPair *pp = make_p_pair(f_new, i - 1, j);
                    insert_pPair_set(open_list, pp);

                    cell_details[i - 1][j].f = f_new;
                    cell_details[i - 1][j].g = g_new;
                    cell_details[i - 1][j].h = h_new;
                    cell_details[i - 1][j].parent_i = i;
                    cell_details[i - 1][j].parent_j = j;
                }
                else if (cell_details[i - 1][j].f > f_new)
                {
                    pPair *pp = make_p_pair(f_new, i - 1, j);
                    update_pPair_set(open_list, cell_details[i - 1][j].f, pp);

                    cell_details[i - 1][j].f = f_new;
                    cell_details[i - 1][j].g = g_new;
                    cell_details[i - 1][j].h = h_new;
                    cell_details[i - 1][j].parent_i = i;
                    cell_details[i - 1][j].parent_j = j;
                }
            }
        }

        // South Successor
        if (is_valid(i + 1, j) == true)
        {
            if (is_destination(i + 1, j, *dst) == true)
            {
                cell_details[i + 1][j].parent_i = i;
                cell_details[i + 1][j].parent_j = j;
                //printf("The destination cell is found\n");
                trace_path(fp, cell_details, grid, *dst);
                found_dst = true;
                free(pp);
                pp = NULL;
                free(open_list);
                return;
            }
            else if (closed_list[i + 1][j] == false && is_unblocked(grid, i + 1, j) == true)
            {
                g_new = cell_details[i][j].g + 1;
                h_new = calc_hvalue(i + 1, j, *dst);
                f_new = g_new + h_new;

                if (cell_details[i + 1][j].f == __INT_MAX__)
                {
                    pPair *pp = make_p_pair(f_new, i + 1, j);
                    insert_pPair_set(open_list, pp);

                    cell_details[i + 1][j].f = f_new;
                    cell_details[i + 1][j].g = g_new;
                    cell_details[i + 1][j].h = h_new;
                    cell_details[i + 1][j].parent_i = i;
                    cell_details[i + 1][j].parent_j = j;
                }
                else if (cell_details[i + 1][j].f > f_new)
                {
                    pPair *pp = make_p_pair(f_new, i + 1, j);
                    update_pPair_set(open_list, cell_details[i + 1][j].f, pp);

                    cell_details[i + 1][j].f = f_new;
                    cell_details[i + 1][j].g = g_new;
                    cell_details[i + 1][j].h = h_new;
                    cell_details[i + 1][j].parent_i = i;
                    cell_details[i + 1][j].parent_j = j;
                }
            }
        }

        // East Successor
        if (is_valid(i, j + 1) == true)
        {
            if (is_destination(i, j + 1, *dst) == true)
            {
                cell_details[i][j + 1].parent_i = i;
                cell_details[i][j + 1].parent_j = j;
                //printf("The destination cell is found\n");
                trace_path(fp, cell_details, grid, *dst);
                found_dst = true;
                // free pPair after necessary data extracted and open_list
                free(pp);
                pp = NULL;
                free(open_list);
                return;
            }
            // if successor is closed list or if it is blocked, ignore
            // else do this
            else if (closed_list[i][j + 1] == false && is_unblocked(grid, i, j + 1) == true)
            {
                g_new = cell_details[i][j].g + 1;
                h_new = calc_hvalue(i, j + 1, *dst);
                f_new = g_new + h_new;

                if (cell_details[i][j + 1].f == __INT_MAX__)
                {
                    pPair *pp = make_p_pair(f_new, i, j + 1);
                    insert_pPair_set(open_list, pp);

                    cell_details[i][j + 1].f = f_new;
                    cell_details[i][j + 1].g = g_new;
                    cell_details[i][j + 1].h = h_new;
                    cell_details[i][j + 1].parent_i = i;
                    cell_details[i][j + 1].parent_j = j;
                }
                else if (cell_details[i][j + 1].f > f_new)
                {
                    pPair *pp = make_p_pair(f_new, i, j + 1);
                    update_pPair_set(open_list, cell_details[i][j + 1].f, pp);

                    cell_details[i][j + 1].f = f_new;
                    cell_details[i][j + 1].g = g_new;
                    cell_details[i][j + 1].h = h_new;
                    cell_details[i][j + 1].parent_i = i;
                    cell_details[i][j + 1].parent_j = j;
                }
            }
        }

        // West Successor
        if (is_valid(i, j - 1) == true)
        {
            if (is_destination(i, j - 1, *dst) == true)
            {
                cell_details[i][j - 1].parent_i = i;
                cell_details[i][j - 1].parent_j = j;
                //printf("The destination cell is found\n");
                trace_path(fp, cell_details, grid, *dst);
                found_dst = true;
                free(pp);
                pp = NULL;
                free(open_list);
                return;
            }
            else if (closed_list[i][j - 1] == false && is_unblocked(grid, i, j - 1) == true)
            {
                g_new = cell_details[i][j].g + 1;
                h_new = calc_hvalue(i, j - 1, *dst);
                f_new = g_new + h_new;

                if (cell_details[i][j - 1].f == __INT_MAX__)
                {
                    pPair *pp = make_p_pair(f_new, i, j - 1);
                    insert_pPair_set(open_list, pp);

                    cell_details[i][j - 1].f = f_new;
                    cell_details[i][j - 1].g = g_new;
                    cell_details[i][j - 1].h = h_new;
                    cell_details[i][j - 1].parent_i = i;
                    cell_details[i][j - 1].parent_j = j;
                }
                else if (cell_details[i][j - 1].f > f_new)
                {
                    pPair *pp = make_p_pair(f_new, i, j - 1);
                    update_pPair_set(open_list, cell_details[i][j - 1].f, pp);

                    cell_details[i][j - 1].f = f_new;
                    cell_details[i][j - 1].g = g_new;
                    cell_details[i][j - 1].h = h_new;
                    cell_details[i][j - 1].parent_i = i;
                    cell_details[i][j - 1].parent_j = j;
                }
            }
        }
    }
    if (found_dst == false)
    {
        printf("Failed to find the Destination Cell\n");

        free(open_list);
        return;
    }

    free(open_list);
}

// for inputs of ROW and COL from input instead of const
// static int ROW = 0;
// static int COL = 0;

   void print_grid(int **grid, Pair *src, Pair *dst){
      // ✅ Print verification
    printf("Grid (%dx%d):\n", ROW, COL);
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }

    if (src && dst) {
        printf("\nSource: (%d, %d)\n", src->x, src->y);
        printf("Destination: (%d, %d)\n", dst->x, dst->y);
    } else {
        printf("\nError: Missing source or destination.\n");
    }
   }


int main(int argc, char **argv)
{
    if(argc != 2){
        perror("MAP ERROR");
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r+");
    if(!fp){
        perror("Error opening file");
        return 1;
    }

    char header[HEAD_MAX];

    if(!fgets(header, sizeof(header),fp)){
        fprintf(stderr, "MAP ERROR\n");
        fclose(fp);
        return 1;
    }

    sscanf(header, "%dx%d", &ROW, &COL);

    int **grid = malloc(ROW * sizeof(int *));
    for(int i = 0; i < ROW; i++){
        grid[i] = malloc(COL * sizeof(int));
    }

    char line[HEAD_MAX];
    int row = 0;
    Pair *src = NULL;
    Pair *dst = NULL;

    while(fgets(line, sizeof(line), fp) && row < ROW){
        for(int col = 0; col < COL; col++){
            char c = line[col];

            if(c == '*'){
                grid[row][col] = 0;
            } else if(c == ' '){
           // } else if(c == ' ' || c == '\n' || c == '\r'){
                grid[row][col] = 1;
            } else if(c == '1'){
                grid[row][col] = 1;
                src = make_pair(row, col);
            } else if (c == '2'){
                grid[row][col] = 1;
                dst = make_pair(row, col);
            } 
            else{
            
            }
        }
        row++;
    }
    
    a_star_search(fp, grid, src, dst);

    fclose(fp);


  //  printf("Before: \n");
  //  print_grid(grid, src, dst);

    // int grid[10][10] = {
    //     {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    //     {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    //     {0, 1, 0, 1, 1, 0, 0, 1, 1, 0},
    //     {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    //     {0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
    //     {0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    //     {0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    //     {0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    //     {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    //     {0, 0, 0, 1, 0, 0, 0, 0, 0, 0}};
    
   
   // printf("After: \n");
   // print_grid(grid, src, dst);

    //free allocations
    for (int i = 0; i < ROW; i++) free(grid[i]);
    free(grid);
    grid = NULL;
    free(src);
    src = NULL;
    free(dst);
    dst = NULL;
    
    return 0;
}

/***************************************************************** */
/*Initial Grid*/
// ROW = 9;
// COL = 10;

// vvv 13 Steps
// int grid[ROW][COL] = {{1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
//                       {1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
//                       {1, 1, 1, 0, 1, 1, 0, 1, 0, 1},
//                       {0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
//                       {1, 1, 1, 0, 1, 1, 1, 0, 1, 0},
//                       {1, 0, 1, 1, 1, 1, 0, 1, 0, 0},
//                       {1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
//                       {1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
//                       {1, 1, 1, 0, 0, 0, 1, 0, 0, 1}};

/**************************************************************** */

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
*  o    **-e 01.map
10x10* o12$
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
// first pass through to build map representation and make find the
// start + end points and barriers

// once built