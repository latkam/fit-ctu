#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <math.h>

typedef struct
{
    double frequency;
    int    text_size;
    char * text;
}TPHRASE;
//======================================================================================================================
void initialisePhrases ( TPHRASE * arr, const int arr_size )
{
    for ( int i = 0; i < arr_size; i++ )
    {
        arr[i].frequency = 0;
        arr[i].text_size = 0;
        arr[i].text      = NULL;
    }
}
//======================================================================================================================
void freePhrases ( TPHRASE * arr, const int arr_size )
{
    for ( int i = 0; i < arr_size; i++ )
        free ( arr[i].text );
    free ( arr );
}
//======================================================================================================================
void reallocatePhrases ( TPHRASE ** arr, int * arr_size )
{
    TPHRASE * tmp = ( TPHRASE * ) malloc ( 2 * *arr_size * sizeof ( TPHRASE ) );
    if ( ! tmp )
    {
        freePhrases ( *arr, *arr_size);
        return;
    }

    initialisePhrases ( tmp, 2 * *arr_size );

    for ( int i = 0; i < *arr_size; i++ )
    {
        tmp[i].frequency = arr[0][i].frequency;
        tmp[i].text_size = arr[0][i].text_size;
        tmp[i].text      = arr[0][i].text;
        arr[0][i].text = NULL;
    }

    freePhrases ( *arr, *arr_size );

    *arr_size *= 2;
    *arr = tmp;
}
//======================================================================================================================
int comparePhrases ( const void * a, const void * b )
{
    TPHRASE phrase1 = *( const TPHRASE * ) a;
    TPHRASE phrase2 = *( const TPHRASE * ) b;

    if ( fabs ( phrase1.frequency - phrase2.frequency ) < 100 * DBL_EPSILON * phrase1.frequency * phrase1.frequency )
        return 0;
    else if ( phrase1.frequency > phrase2.frequency )
        return -1;
    return 1;

}
//======================================================================================================================
void toLower ( char * text, int size )
{
    for (int i = 0; i < size; i++ )
        if (text[i] >= 65 && text[i] <= 90 )
            text[i] += 32;
}
//======================================================================================================================
void searchInPhrases ( TPHRASE * arr, const int arr_size, char * query, const int query_size )
{
    int ok = 0, found_size = 10, found_index = 0;

    int * found = ( int * ) malloc ( found_size * sizeof ( int ) );
    for ( int i = 0; i < found_size; i++ )
        found[i] = -1;

    char * tmp = NULL;
    for ( int i = 0; i < arr_size; i++ ) /* pro vsechny zname fraze */
    {
        tmp = ( char * ) malloc ( arr[i].text_size * sizeof ( char ) );
        for ( int x = 0; x < arr[i].text_size; x++ )
            tmp[x] = arr[i].text[x];

        toLower ( tmp, arr[i].text_size );

        for ( int j = 0; j <= arr[i].text_size-query_size; j++ )
        {
            ok = 0;
            for ( int k = 0; k < query_size-1; k++ )
            {
                if ( tmp[j+k] != query[k] )
                {
                    ok = 0;
                    break;
                }
                else
                    ok = 1;
            }
            if ( ok )
            {
                found[found_index++] = i;
                if ( found_index == found_size )
                {
                    int * found_tmp = ( int * ) malloc ( 2 * found_size * sizeof ( int ) );
                    int y = 0;
                    for ( ; y < found_size; y++ )
                        found_tmp[y] = found[y];
                    found_size *= 2;
                    for ( ; y < found_size; y++ )
                        found_tmp[y] = -1;
                    free ( found );
                    found = found_tmp;
                }
                break;
            }
        }
        free ( tmp );
    }

    printf ( "Nalezeno: %d\n", found_index );
    for ( int i = 0; i < 50; i++ )
    {
        if ( found[i] == -1 )
            break;
        printf ( "> %s\n", arr[found[i]].text );
    }
    free ( found );
}
//======================================================================================================================
int parseInput ( TPHRASE * phrase, char ** buffer_phrase, size_t input_size )
{
    char colon = 0;
    int first_part_index = 0;
    if ( feof ( stdin ) )
        return 0;
    if (sscanf (*buffer_phrase, "%lf%c%n", & phrase->frequency, & colon, &first_part_index ) != 2 || colon != ':' )
        return 0;

    phrase->text_size = input_size - first_part_index;
    phrase->text = ( char * ) calloc ( phrase->text_size, sizeof ( char ) );

    sscanf(buffer_phrase[0] + first_part_index, "%[^\n]s", phrase->text );

    return 1;
}
//======================================================================================================================
int getPhrases ( TPHRASE ** arr, int * arr_size, int * arr_index )
{
    size_t input_size = 0;
    char * buffer_phrase = NULL;
    printf ( "Casto hledane fraze:\n" );
    while ( 1 )
    {
        if ( *arr_index == *arr_size - 1 )
            reallocatePhrases ( arr, arr_size );

        input_size = getline ( & buffer_phrase, & input_size, stdin );

        if ( input_size == 1 && ! strncmp (buffer_phrase, "\n", 1 ) )
            break;

        if ( ! parseInput ( & arr[0][*arr_index], & buffer_phrase, input_size ) )
        {
            free(buffer_phrase);
            return 0;
        }
        *arr_index += 1;
    }
    free ( buffer_phrase );
    if ( *arr_index == 0 )
        return 0;
    return 1;
}
//======================================================================================================================
void getQueries ( TPHRASE * arr, const int arr_index )
{
    char * buffer_query = NULL;
    size_t query_size = 0;
    printf ( "Hledani:\n" );
    while ( 1 )
    {
        query_size = getline ( & buffer_query, & query_size, stdin );
        if ( feof ( stdin ) )
            break;

        toLower ( buffer_query, query_size );

        searchInPhrases ( arr, arr_index, buffer_query, query_size );
    }
    free ( buffer_query );
}
//======================================================================================================================
int main ( void )
{
    int arr_size = 1, arr_index = 0;
    TPHRASE * arr = ( TPHRASE * ) malloc ( arr_size * sizeof ( TPHRASE ) );
    initialisePhrases ( arr, arr_size );
    if ( ! getPhrases( &arr, &arr_size, &arr_index ) )
    {
        printf ( "Nespravny vstup.\n" );
        freePhrases ( arr, arr_size );
        return 1;
    }
    qsort ( arr, arr_index, sizeof ( TPHRASE ), comparePhrases );
    getQueries ( arr, arr_index );
    freePhrases ( arr, arr_size );
    return 0;
}
