#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

typedef struct
{
    double x;
    double y;
    char * name;
}TFLIGHT;

typedef struct
{
    TFLIGHT f1;
    TFLIGHT f2;
}TDISTANCE;
//======================================================================================================================
void initialiseFlights ( TFLIGHT * flights, const int size )
{
    for ( int i = 0; i < size; i++ )
    {
        flights[i].x = 0;
        flights[i].y = 0;
        flights[i].name = NULL;
    }
}
//======================================================================================================================
void freeFlights ( TFLIGHT * flights, const int size )
{
    for ( int i = 0; i < size; i++ )
        free ( flights[i].name );
    free ( flights );
}
//======================================================================================================================
void reallocateFlights ( TFLIGHT ** flights, int * size )
{
    TFLIGHT * tmp = ( TFLIGHT * ) malloc ( 2 * *size * sizeof ( TFLIGHT ) );
    if ( ! tmp )
    {
        freeFlights ( *flights, *size );
        return;
    }

    initialiseFlights ( tmp, 2 * *size );

    for ( int i = 0; i < *size; i++ )
    {
        tmp[i].x = flights[0][i].x;
        tmp[i].y = flights[0][i].y;
        tmp[i].name = flights[0][i].name;
        flights[0][i].name = NULL;
    }

    freeFlights ( *flights, *size );

    *size *= 2;
    *flights = tmp;
}
//======================================================================================================================
int compareFlights ( const void * a, const void * b )
{
    TFLIGHT flight1 = *( const TFLIGHT * ) a;
    TFLIGHT flight2 = *( const TFLIGHT * ) b;

    if ( fabs ( flight1.x - flight2.x ) < 100 * DBL_EPSILON * flight1.x * flight1.x )
    {
        if ( fabs ( flight1.y - flight2.y) < 100 * DBL_EPSILON * flight1.y * flight1.y )
            return 0;
        else if ( flight1.y < flight2.y )
            return -1;
        else
            return 1;
    }
    else if ( flight1.x < flight2.x )
        return -1;
    else
        return 1;
}
//======================================================================================================================
int parseInput ( char ** buffer, const size_t input_size, TFLIGHT * flight )
{
    int first_read_end;
    char par1, par2;
    if (sscanf ( *buffer, " %c %lf , %lf %c %n", & par1, &flight->x, &flight->y, &par2, &first_read_end ) != 4 || par1 != '[' || par2 != ']' )
        return 0;

    int name_len = input_size - first_read_end + 1;

    flight->name = ( char * ) calloc ( name_len,  sizeof ( char ) );
    sscanf ( buffer[0]+first_read_end, " %[^\n]s", flight->name );
    return 1;
}
//======================================================================================================================
int getInput ( TFLIGHT ** flights, int * flights_index, int * flights_size )
{
    size_t buffer_size = 0, input_size = 0;
    int parse_ok = 1;

    char * buffer = NULL;

    printf ( "Zadejte lety:\n" );
    while ( 1 )
    {
        if ( *flights_index == *flights_size - 1)
            reallocateFlights ( flights, flights_size );

        input_size = getline ( & buffer, & buffer_size, stdin );

        if ( feof ( stdin ) )
            break;
        if ( ! parseInput ( & buffer, input_size, (*flights) + *flights_index ) )
            parse_ok = 0;
        else
            *flights_index += 1;
    }

    if ( *flights_index < 2 )
        parse_ok = 0;
    free ( buffer );
    return parse_ok;
}
//======================================================================================================================
double computeDistance ( TFLIGHT * a, TFLIGHT * b ) { return sqrt (pow((b->x - a->x),2) + pow((b->y - a->y), 2)); }
//======================================================================================================================
double computeSmallestDistance ( TFLIGHT ** flights, int flights_index )
{
    // flights index_stores index, where we actually can't go
    double smallest_distance = DBL_MAX;
    double distance = 0;
    for ( int i = 0; i < flights_index; i++ )
    {
        for ( int j = 0; j < flights_index; j++ )
        {
            if ( j == i || j < i )
                continue;
            distance = computeDistance( (*flights) + i, (*flights) + j );
            if ( distance < smallest_distance )
                smallest_distance = distance;
        }
    }
    return smallest_distance;
}
//======================================================================================================================
void computeDistances ( TFLIGHT ** flights, int flights_index, double smallest_distance )
{
    for ( int i = 0; i < flights_index; i++ )
    {
        for ( int j = i+1; j < flights_index; j++ )
        {
            if ( fabs ( flights[0][j].x - flights[0][i].x ) > smallest_distance + 100 * DBL_EPSILON * smallest_distance * smallest_distance )
                break;

            if ( fabs ( flights[0][i].y - flights[0][j].y ) > smallest_distance + 100 * DBL_EPSILON * smallest_distance * smallest_distance )
                continue;


            if ( fabs ( computeDistance ( (*flights) + i, (*flights) + j ) - smallest_distance ) <=
                 100 * DBL_EPSILON * smallest_distance * smallest_distance )
                printf ( "%s - %s\n", flights[0][i].name, flights[0][j].name );
        }
    }
}
//======================================================================================================================
int main ( void )
{
    int flights_index = 0, flights_size = 2;
    TFLIGHT * flights = ( TFLIGHT * ) malloc ( flights_size * sizeof ( TFLIGHT ) );
    if ( ! flights )
        return 1;
    initialiseFlights ( flights, flights_size );

    if ( ! getInput ( &flights, & flights_index, & flights_size) )
    {
        printf ( "Nespravny vstup.\n" );
        freeFlights ( flights, flights_size );
        return 1;
    }

    qsort( flights, flights_index, sizeof ( TFLIGHT ), compareFlights );


    double smallest_distance = computeSmallestDistance ( & flights, flights_index );
    printf ( "Nejmensi vzdalenost: %.6lf\n", smallest_distance );

    computeDistances ( &flights, flights_index, smallest_distance );

    freeFlights ( flights, flights_size );
    return 0;
}