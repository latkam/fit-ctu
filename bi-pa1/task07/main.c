#include <stdio.h>
#include <stdlib.h>
#include <values.h>

# define ROOT -1
# define LEFT  0
# define RIGHT 1

typedef struct
{
    int min_cuts;
    int l_x;
    int l_y;
    int r_x;
    int r_y;
}TINFO;

int g_Max_area = 0, g_Input_x = 0, g_Input_y = 0, g_Input_max = 0, g_Immersion = 0, g_Lines_size = 4;

int *    g_Lines = NULL;
TINFO ** g_Arr   = NULL;
//======================================================================================================================
void swap ( int * a, int * b )
{
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
}
//======================================================================================================================
void reallocateLines ( void )
{
    int * tmp = ( int * ) calloc ( 2 * g_Lines_size, sizeof ( int ) );

    for ( int i = 0; i < g_Lines_size; i++ )
        tmp[i] = g_Lines[i];

    g_Lines_size *= 2;

    free ( g_Lines );
    g_Lines = tmp;
}
//======================================================================================================================
void allocateArray ( void )
{
    g_Arr = ( TINFO ** ) malloc ( g_Input_max * sizeof ( TINFO * ) );

    for ( int i = 0; i < g_Input_max; i++ )
        g_Arr[i] = ( TINFO * ) malloc ( g_Input_max * sizeof ( TINFO ) );

    for ( int i = 0; i < g_Input_max; i++ )
        for ( int j = 0; j < g_Input_max; j++ )
        {
            g_Arr[i][j].min_cuts = -1;
            g_Arr[i][j].l_x = g_Arr[i][j].l_y = g_Arr[i][j].r_x = g_Arr[i][j].r_y = 0;
        }
}
//======================================================================================================================
void freeArray ( void )
{
    for ( int i = 0; i < g_Input_max; i++ )
        free ( g_Arr[i] );

    free ( g_Arr );
    g_Arr = NULL;
}
//======================================================================================================================
int getInput ( void )
{
    printf ( "Velikost:\n" );
    if ( scanf ( "%d %d", & g_Input_x, & g_Input_y ) != 2
         || g_Input_x <= 0
         || g_Input_y <= 0 )
    {
        printf ( "Nespravny vstup.\n" );
        return 0;
    }

    printf ( "Maximalni plocha:\n" );
    if ( scanf ( "%d", & g_Max_area ) != 1
         || g_Max_area <= 0 )
    {
        printf ( "Nespravny vstup.\n" );
        return 0;
    }
    return 1;
}
//======================================================================================================================
void printTree ( int x, int y, int parent_x, int parent_y, int side )
{
    g_Immersion++;
    switch ( side )
    {
        case ROOT:
        {
            g_Immersion = 0;
            printf ( "[%d,%d]\n", x, y );
            break;
        }
        case LEFT:
        {
            if ( g_Immersion == g_Lines_size )
                reallocateLines ( );

            g_Lines[g_Immersion] = 1;

            for ( int i = 1; i < g_Immersion; i++ )
            {
                if ( g_Lines[i] )
                    printf ( "| " );
                else
                    printf ( "  " );
            }
            if ( x != parent_x && y != parent_y )
            {
                swap ( & x, & y );
                printf ( "+-[%d,%d]\n", x, y );
            }

            else
                printf ( "+-[%d,%d]\n", x, y );
            break;
        }
        case RIGHT:
        {
            g_Lines[g_Immersion] = 0;
            for ( int i = 1; i < g_Immersion; i++ )
            {
                if ( g_Lines[i] )
                    printf ( "| " );
                else
                    printf ( "  " );
            }

            if ( x != parent_x && y != parent_y )
            {
                swap ( & x, & y );
                printf("\\-[%d,%d]\n", x, y );
            }
            else
                printf ( "\\-[%d,%d]\n", x, y );
            break;
        }
    }


    if ( g_Arr[x-1][y-1].l_x && g_Arr[x-1][y-1].l_y && g_Arr[x-1][y-1].r_x && g_Arr[x-1][y-1].r_y )
    {
        printTree ( g_Arr[x-1][y-1].l_x, g_Arr[x-1][y-1].l_y, x, y, LEFT );
        printTree ( g_Arr[x-1][y-1].r_x, g_Arr[x-1][y-1].r_y, x, y, RIGHT );
    }

    g_Immersion--;
}
//======================================================================================================================
int computeCuts ( int x, int y )
{
    int min_cuts = INT_MAX, tmp = 0, i = 0, j = 0;

    if ( x * y <= g_Max_area
         && 1.0 * x/y <= 2   + 100 * DBL_EPSILON * x * x
         && 1.0 * x/y >= 0.5 - 100 * DBL_EPSILON * x * x )
        return 0;

    if ( g_Arr[x-1][y-1].min_cuts != -1 && g_Arr[x-1][y-1].min_cuts == g_Arr[y-1][x-1].min_cuts )
        return g_Arr[x-1][y-1].min_cuts;


    for ( i = 1; i <= x/2; i++ )
    {
        tmp = 1;
        tmp += computeCuts ( i, y );
        tmp += computeCuts ( x-i, y );

        if ( tmp < min_cuts )
        {
            min_cuts = tmp;

            if ( ( x == g_Input_max && y == g_Input_max ) || x == y )
            {
                g_Arr[x-1][y-1].min_cuts = min_cuts;
                g_Arr[x-1][y-1].l_x      = i;
                g_Arr[x-1][y-1].l_y      = y;
                g_Arr[x-1][y-1].r_x      = x-i;
                g_Arr[x-1][y-1].r_y      = y;
            }
            else
            {
                g_Arr[x-1][y-1].min_cuts = g_Arr[y-1][x-1].min_cuts = min_cuts;
                g_Arr[x-1][y-1].l_x      = g_Arr[y-1][x-1].r_x      = i;
                g_Arr[x-1][y-1].l_y      = g_Arr[y-1][x-1].r_y      = y;
                g_Arr[x-1][y-1].r_x      = g_Arr[y-1][x-1].l_x      = x-i;
                g_Arr[x-1][y-1].r_y      = g_Arr[y-1][x-1].l_y      = y;
            }
        }
    }

    if ( x != y )
    {
        for ( j = 1; j <= y/2; j++ )
        {
            tmp = 1;
            tmp += computeCuts ( x, j );
            tmp += computeCuts ( x, y-j );
            if ( tmp < min_cuts )
            {
                min_cuts = tmp;
                if ( ( x == g_Input_max && y == g_Input_max ) || x == y )
                {
                    g_Arr[x-1][y-1].min_cuts = min_cuts;
                    g_Arr[x-1][y-1].l_x      = x;
                    g_Arr[x-1][y-1].l_y      = j;
                    g_Arr[x-1][y-1].r_x      = x;
                    g_Arr[x-1][y-1].r_y      = y-j;
                }
                else
                {
                    g_Arr[x-1][y-1].min_cuts = g_Arr[y-1][x-1].min_cuts = min_cuts;
                    g_Arr[x-1][y-1].l_x      = g_Arr[y-1][x-1].r_x      = x;
                    g_Arr[x-1][y-1].l_y      = g_Arr[y-1][x-1].r_y      = j;
                    g_Arr[x-1][y-1].r_x      = g_Arr[y-1][x-1].l_x      = x;
                    g_Arr[x-1][y-1].r_y      = g_Arr[y-1][x-1].l_y      = y-j;
                }
            }
        }
    }

    if ( min_cuts == INT_MAX )
        min_cuts = 0;
    return min_cuts;
}
//======================================================================================================================
int main ( void )
{
    if ( ! getInput ( ) )
        return 1;

    g_Input_max = ( g_Input_x > g_Input_y ) ? g_Input_x : g_Input_y;

    allocateArray ( );
    g_Lines = ( int * ) calloc ( g_Lines_size, sizeof ( int ) );

    int nmb_cuts = computeCuts ( g_Input_x, g_Input_y );
    if ( nmb_cuts == 0 )
    {
        printf ( "Rezu: 0\n" );
        printf ( "[%d,%d]\n", g_Input_x, g_Input_y );
    }
    else
    {
        printf ("Rezu: %d\n", nmb_cuts );
        printTree ( g_Input_x, g_Input_y, g_Input_x, g_Input_y, ROOT );
    }
    freeArray ( );
    free ( g_Lines );
    return 0;
}