#include <stdio.h>
#include <stdlib.h>

/*
 * This program computes water height in communicating vessels.
 * Idea of the algorithm: we create an array to simulate the altitude. Every field in the array contains information
 * about the altitude of the upmost "point" in the field, capacity of the field and total capacity of all fields below
 * including the field we are working with. Thus, we can effectively compute the altitude of certain amount of water.
 */

#define ARR_MAX 200000

typedef struct
{
    int alt;    /**< Altitude of the bottom of the tank. */
    int height; /**< Height of the tank. */
    int width;  /**< Width of the tank. */
    int depth;  /**< Depth of the tank. */
}TTANK;

typedef struct
{
    int      alt;        /**< Altitude of this meter of space. */
    int      volume;     /**< Volume of this meter of altitude. */
    long int tot_volume; /**< Total volume of this meter and all meters below. */
}TALTVOLUME;

typedef struct
{
    TALTVOLUME * arr;      /**< Array of altitudes. */
    int          arr_size; /**< Size of the array of altitudes. */
    int          min_alt;  /**< Minimal altitude of the array. */
    int          max_alt;  /**< Maximal altitude of the array. */
}TARRAY;

//======================================================================================================================
/**
 * Creates and initialises an array of tanks.
 * @return array of tanks.
 */
TARRAY initialiseTanks ( void )
{
    TARRAY tanks;
    tanks.arr      = NULL;
    tanks.arr_size = 0;
    tanks.min_alt  = 0;
    tanks.max_alt  = 0;
    return tanks;
}
//======================================================================================================================
/**
 * Binary search searching for certain altitude in the array of altitudes.
 * @param[in] altitudes array of altitudes
 * @param[in] alt altitude to be searched for in the array
 * @return index of alt in the array, -1=value not in array
 */
int binarySearch ( TARRAY * altitudes, const int alt )
{
    int lo  = 0;
    int hi  = altitudes->arr_size;
    int mid;

    while ( lo < hi )
    {
        mid = (lo+hi)/2;
        if ( altitudes->arr[mid].alt == alt )
            return mid;
        if ( altitudes->arr[mid].alt < alt )
            lo = mid + 1;
        else
            hi = mid;
    }
    return -1;
}
//======================================================================================================================
/**
 * Modified lower bound. Function performs binary search to find the first occurrence of the number. If the number
 * is not present in the array, it returns the last occurrence of the closest lower value.
 * @param[in] altitudes array of altitudes
 * @param[in] tot_volume volume to be searched for in the array
 * @param[in,out] precise indicates whether the precise tot_volume has been found or not
 * @return index of tot_volume in the array, or the index of the last occurrence of the closest lower value
 */
int lowerBound ( TARRAY * altitudes, const int * tot_volume, int * precise )
{
    int lo  = 0;
    int hi  = altitudes->arr_size;
    int mid = (lo+hi)/2;
    int res = -1;

    /** If the volume of water is less than total volume of the first meter of the array, returns the first meter.*/
    if ( *tot_volume < altitudes->arr[0].tot_volume )
        return 0;

    while ( lo < hi )
    {
        if ( altitudes->arr[mid].tot_volume == *tot_volume )
        {
            *precise = 1;
            res = mid;
            hi = mid;
        }
        if ( altitudes->arr[mid].tot_volume < *tot_volume )
            lo = mid + 1;
        else
            hi = mid;
        mid = (lo+hi)/2;
    }

    /** Lower bound hasn't found the precise value so it will return the closest lower value. */
    if ( res == -1 )
    {
        *precise = 0;
        res = mid -1;
    }
    return res;
}
//======================================================================================================================
/**
 * Creates the first part of the array.
 * This function slightly optimises the creation of the array, because it creates the array precisely in the range
 * of altitudes we want to cover. If we used pushFront or pushBack, maximal altitude or minimal altitude would be 0
 * which may be ineffective if we want to cover, for example, the range from -1000 to -900 metres.
 * @param[in,out] altitudes array of altitudes being created
 * @param[in] min_alt_new new minimum altitude of the array
 * @param[in] max_alt_new new maximum altitude of the array
 */
void initialiseAltitudes ( TARRAY * altitudes, const int min_alt_new, const int max_alt_new )
{
    int delta = abs (max_alt_new-min_alt_new)+1;
    altitudes->arr_size = delta;
    altitudes->min_alt  = min_alt_new;
    altitudes->max_alt  = max_alt_new;

    altitudes->arr = ( TALTVOLUME * ) malloc ( delta * sizeof ( *altitudes->arr ) );

    for ( int i = 0, alt = min_alt_new; i < delta; i++, alt++ )
    {
        altitudes->arr[i].alt        = alt;
        altitudes->arr[i].volume     = 0;
        altitudes->arr[i].tot_volume = 0;
    }
}
//======================================================================================================================
/**
 * Extends the array of altitudes to cover lower altitude than before. It creates and initialises a new array of desired
 * size, leaves space for new values and copies the old ones.
 * @param[in,out] altitudes array of altitudes to be extended
 * @param[in] min_alt_new new minimum altitude of the array
 */
void pushFront ( TARRAY * altitudes, const int min_alt_new )
{
    int delta = abs (altitudes->min_alt-min_alt_new);
    altitudes->arr_size += delta;
    altitudes->min_alt   = min_alt_new;

    TALTVOLUME * tmp = ( TALTVOLUME * ) malloc ( altitudes->arr_size * sizeof ( *tmp ) );

    for ( int i = 0, alt = min_alt_new; i < delta; i++, alt++ )
    {
        tmp[i].alt        = alt;
        tmp[i].volume     = 0;
        tmp[i].tot_volume = 0;
    }

    for ( int i = delta, j = 0; i < altitudes->arr_size; i++, j++ )
        tmp[i] = altitudes->arr[j];

    free ( altitudes->arr );
    altitudes->arr = tmp;
}
//======================================================================================================================
/**
 * Extends the array of altitudes to cover higher altitude than before. It creates and initialises a new array of
 * desired size, copies old values and leaves space for the new ones.
 * @param[in,out] altitudes array of altitudes to be extended
 * @param[in] max_alt_new new maximum altitude of the array
 */
void pushBack ( TARRAY * altitudes, const int max_alt_new )
{
    int delta = abs (max_alt_new-altitudes->max_alt);
    altitudes->arr_size += delta;

    TALTVOLUME * tmp = ( TALTVOLUME * ) malloc ( altitudes->arr_size * sizeof ( *tmp ) );

    for ( int i = altitudes->arr_size - delta; i < altitudes->arr_size; i++ )
    {
        tmp[i].alt        = ++altitudes->max_alt;
        tmp[i].volume     = 0;
        tmp[i].tot_volume = 0;
    }

    for ( int i = 0, j = 0; j < altitudes->arr_size - delta; i++, j++ )
        tmp[i] = altitudes->arr[j];

    free ( altitudes->arr );
    altitudes->arr = tmp;
}
//======================================================================================================================
/**
 * Stores the information we get from one tank to the array. If the altitude of any part of the tank is
 * not within the already covered range, it extends the array. For every meter of the altitude covered by tank
 * it increases the capacity of the relevant meter of altitude.
 * @param[in,out] altitudes array of altitudes to store the information
 * @param[in] tank tank we are getting information from
 * @param[in] index serial number of the tank being stored subtracted by one
 */
void storeInput ( TARRAY * altitudes, TTANK * tank, const int * index )
{
    int min_index, max_index, space;
    if ( *index == 0 )
        initialiseAltitudes ( altitudes, tank->alt+1, tank->alt + tank->height );

    if ( tank->alt + 1 < altitudes->min_alt )
        pushFront ( altitudes, tank->alt + 1 );

    if ( tank->alt + tank->height > altitudes->max_alt )
        pushBack ( altitudes, tank->alt + tank->height );

    /** Finds which fields in the array will be updated. */
    min_index = binarySearch ( altitudes, tank->alt + 1 );
    max_index = binarySearch ( altitudes, tank->alt + tank->height );

    space = tank->width * tank->depth;

    for ( int i = min_index; i <= max_index; i++ )
        altitudes->arr[i].volume += space;
}
//======================================================================================================================
/**
 * Asks user to enter the number of tanks to be stored and stores each tank. It checks, if the input is in required
 * format. In the end, it iterates through the whole array and for each field it computes total capacity of the field
 * itself and all fields below.
 * @param[in,out] altitudes array of altitudes to store the information
 * @return 1=OK, 0=error
 */
int getInput ( TARRAY * altitudes )
{
    int nmb_tanks;
    TTANK tank;

    printf ( "Zadejte pocet nadrzi:\n" );
    if (scanf ( "%d", & nmb_tanks ) != 1
        || nmb_tanks <= 0
        || nmb_tanks > ARR_MAX )
    {
        printf ( "Nespravny vstup.\n" );
        return 0;
    }

    printf ( "Zadejte parametry nadrzi:\n" );
    for ( int i = 0; i < nmb_tanks; i++ )
    {
        if ( scanf ( "%d %d %d %d", & tank.alt, & tank.height, & tank.width, & tank.depth ) != 4
             || tank.height <= 0
             || tank.width  <= 0
             || tank.depth  <= 0 )
        {
            printf ( "Nespravny vstup.\n" );
            free ( altitudes->arr );
            return 0;
        }
        storeInput ( altitudes, & tank, & i );
    }

    if ( altitudes->arr_size > 0 )
        altitudes->arr[0].tot_volume = altitudes->arr[0].volume;

    for ( int i = 1; i < altitudes->arr_size; i++ )
        altitudes->arr[i].tot_volume += altitudes->arr[i-1].tot_volume + altitudes->arr[i].volume;
    return 1;
}
//======================================================================================================================
/**
 * Computes which altitude will reach water poured into communicating vessels. In most cases, the computation
 * is very simple, however, there is one slightly more complicated case: if the volume is not equal to any total
 * volume in the array and the volume is not less than the volume of the first field and also not more than
 * the total volume of the last field, it computes the altitude of the closest lower volume and for the upmost
 * field able to store larger volume, it computes the height of the rest of the liquid. This value is added to the
 * altitude computed before.
 * @param[in] altitudes array of altitudes used to compute the final altitude
 * @param[in] volume amount of water poured into communicating vessels
 */
void computeAltitude ( TARRAY * altitudes, int * volume )
{
    int precise = 0, index1 = 0, index2 = 0;
    double result = 0;
    if ( *volume == 0 )
    {
        printf ( "Prazdne.\n" );
        return;
    }

    if ( *volume > altitudes->arr[altitudes->arr_size - 1].tot_volume )
    {
        printf ( "Pretece.\n" );
        return;
    }

    if ( *volume < altitudes->arr[0].tot_volume )
    {
        printf ( "h = %.6lf\n", (altitudes->arr[0].alt-1)+1.0 * *volume/altitudes->arr[0].volume );
        return;
    }

    index1 = lowerBound ( altitudes, volume, & precise );

    if ( precise )
        printf ( "h = %.6lf\n", ( double ) altitudes->arr[index1].alt );
    else
    {
        for ( index2 = index1 + 1; index2 < altitudes->arr_size; index2++ )
            if ( altitudes->arr[index2].volume != 0 )
                break;
        result = altitudes->arr[index2 - 1].alt;
        result += 1.0 *(*volume-altitudes->arr[index2-1].tot_volume)
                  /(altitudes->arr[index2].tot_volume-altitudes->arr[index2-1].tot_volume);
        printf ( "h = %.6lf\n", result );
    }
}
//======================================================================================================================
int main ( void )
{
    TARRAY tanks = initialiseTanks ();
    int volume;

    if ( ! getInput ( & tanks ) )
        return 1;

    printf ( "Zadejte objem vody:\n" );
    while ( scanf ( "%d", & volume ) == 1 && volume >= 0 )
        computeAltitude ( & tanks, & volume );

    free ( tanks.arr );

    if ( ! feof ( stdin ) )
    {
        printf ( "Nespravny vstup.\n" );
        return 1;
    }
    return 0;
}
