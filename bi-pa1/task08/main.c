#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LIST_BY_YEAR 0
#define LIST_BY_TYPE 1

#define TYPE_MAX  100
#define SETUP_MAX 100

typedef struct TEngine
{
    struct TEngine * m_Next;
    struct TEngine * m_Prev;
    int              m_Year;
    char             m_Type  [ TYPE_MAX ];
    int              m_Setup [ SETUP_MAX ];
} TENGINE;

typedef struct TArchive
{
    struct TArchive * m_Next;
    struct TArchive * m_Prev;
    TENGINE         * m_Engines;
} TARCHIVE;
//======================================================================================================================
TENGINE * createEngine ( const char * type, int year )
{
    TENGINE * res = ( TENGINE * ) malloc ( sizeof  ( *res ) );
    res -> m_Next = NULL;
    res -> m_Prev = NULL;
    res -> m_Year = year;
    strncpy ( res -> m_Type, type, sizeof ( res -> m_Type ) );
    for ( int i = 0; i < SETUP_MAX; i ++ )
        res -> m_Setup[i] = 0;
    return res;
}
//======================================================================================================================
#endif /* __PROGTEST__ */
//======================================================================================================================
TARCHIVE * createArchive ( TARCHIVE * prev, TARCHIVE * next, TENGINE * engine )
{
    TARCHIVE * res = ( TARCHIVE * ) malloc ( sizeof ( TARCHIVE ) );
    res->m_Prev = prev;
    res->m_Next = next;
    res->m_Engines = engine;
    return res;
}
//======================================================================================================================
/**
 * Function searches in archives for proper engine type.
 * If the type is found, it stores engine to the proper place in the proper m_Engines branch.
 * If the type is not found, it appends new archive to the end of the archive and stores the engine there.
 * @param list
 * @param engine
 * @return
 */
TARCHIVE * AddEngineByType ( TARCHIVE * list, TENGINE * engine )
{
    TARCHIVE * ptr_A = list;
    TENGINE  * ptr_E = NULL;

    if ( ! list )
    {
        list = createArchive ( NULL, NULL, engine );
        return list;
    }

    while ( ptr_A )
    {
        if ( ptr_A->m_Engines
             && ! strncmp ( ptr_A->m_Engines->m_Type, engine->m_Type, sizeof ( ptr_A->m_Engines->m_Type ) ) )
        {
            ptr_E = ptr_A->m_Engines;
            while ( ptr_E )
            {
                if ( ptr_E->m_Year > engine->m_Year )
                {
                    if ( ! ptr_E->m_Prev ) // previous engine doesn't exist
                    {
                        TENGINE * ptr_Next = ptr_E;
                        ptr_A->m_Engines = engine;
                        engine->m_Next = ptr_Next;
                        ptr_Next->m_Prev = engine;
                        return list;
                    }
                    else // both surrounding engines exist
                    {
                        TENGINE * ptr_Prev = ptr_E->m_Prev, * ptr_Next = ptr_E;
                        ptr_Prev->m_Next = engine;
                        engine->m_Prev = ptr_Prev;
                        engine->m_Next = ptr_Next;
                        ptr_Next->m_Prev = engine;
                        return list;
                    }
                }
                if ( ! ptr_E->m_Next ) // next engine doesn't exist
                {
                    ptr_E->m_Next = engine;
                    engine->m_Prev = ptr_E;
                    return list;
                }
                ptr_E = ptr_E->m_Next;
            }
        }
        if ( ptr_A->m_Engines // creates new archive for desired type
             && strncmp ( ptr_A->m_Engines->m_Type, engine->m_Type, sizeof ( ptr_A->m_Engines->m_Type ) ) > 0 )
        {
            if ( ! ptr_A->m_Prev ) // previous archive doesn't exist
            {
                list = createArchive ( NULL, ptr_A, engine );
                ptr_A->m_Prev = list;
                return list;
            }
            else // both surrounding archives exists
            {
                TARCHIVE * ptr_Prev = ptr_A->m_Prev;
                TARCHIVE * ptr_Next = ptr_A;
                TARCHIVE * new_archive = createArchive ( ptr_Prev, ptr_Next, engine );
                ptr_Prev->m_Next = new_archive;
                ptr_Next->m_Prev = new_archive;
                return list;
            }
        }
        if ( ! ptr_A->m_Next ) // next archive doesn't exist
        {
            ptr_A->m_Next = createArchive ( ptr_A, NULL, engine );
            return list;
        }
        ptr_A = ptr_A->m_Next;
    }
    return list;
}
//======================================================================================================================
TARCHIVE * AddEngineByYear ( TARCHIVE * list, TENGINE * engine )
{
    TARCHIVE * ptr_A = list;
    TENGINE  * ptr_E = NULL;

    if ( ! list )
    {
        list = createArchive ( NULL, NULL, engine );
        return list;
    }

    while ( ptr_A )
    {
        if ( ptr_A->m_Engines && ptr_A->m_Engines->m_Year == engine->m_Year )
        {
            ptr_E = ptr_A->m_Engines;
            while ( ptr_E )
            {
                if ( strncmp ( ptr_E->m_Type, engine->m_Type, sizeof ( ptr_E->m_Type ) ) > 0 )
                {
                    if ( ! ptr_E->m_Prev ) // previous engine doesn't exist
                    {
                        TENGINE * ptr_Next = ptr_E;
                        ptr_A->m_Engines = engine;
                        engine->m_Next = ptr_Next;
                        ptr_Next->m_Prev = engine;
                        return list;
                    }
                    else // both surrounding engines exist
                    {
                        TENGINE * ptr_Prev = ptr_E->m_Prev, * ptr_Next = ptr_E;
                        ptr_Prev->m_Next = engine;
                        engine->m_Prev = ptr_Prev;
                        engine->m_Next = ptr_Next;
                        ptr_Next->m_Prev = engine;
                        return list;
                    }
                }
                if ( ! ptr_E->m_Next ) // next engine doesn't exist
                {
                    ptr_E->m_Next = engine;
                    engine->m_Prev = ptr_E;
                    return list;
                }
                ptr_E = ptr_E->m_Next;
            }
        }
        if ( ptr_A->m_Engines && ptr_A->m_Engines->m_Year > engine->m_Year) // creates new archive for desired year
        {
            if ( ! ptr_A->m_Prev ) // previous archive doesn't exist
            {
                list = createArchive ( NULL, ptr_A, engine );
                ptr_A->m_Prev = list;
                return list;
            }
            else // both surrounding archives exists
            {
                TARCHIVE * ptr_Prev = ptr_A->m_Prev;
                TARCHIVE * ptr_Next = ptr_A;
                TARCHIVE * new_archive = createArchive ( ptr_Prev, ptr_Next, engine );
                ptr_Prev->m_Next = new_archive;
                ptr_Next->m_Prev = new_archive;
                return list;
            }
        }
        if ( ! ptr_A->m_Next ) // next archive doesn't exist
        {
            ptr_A->m_Next = createArchive ( ptr_A, NULL, engine );
            return list;
        }
        ptr_A = ptr_A->m_Next;
    }
    return list;
}
//======================================================================================================================
TARCHIVE * AddEngine ( TARCHIVE * list, int listBy, TENGINE * engine )
{
    switch ( listBy )
    {
        case LIST_BY_TYPE:
            return AddEngineByType ( list, engine );
        case LIST_BY_YEAR:
            return AddEngineByYear ( list, engine );
    }
    return NULL;
}
//======================================================================================================================
void DelArchive ( TARCHIVE * list )
{
    TARCHIVE * ptr_A = list, * disconnected_A = NULL;
    TENGINE  * disconnected_E = NULL;

    while ( ptr_A )
    {
        while ( ptr_A->m_Engines )
        {
            disconnected_E = ptr_A->m_Engines;
            ptr_A->m_Engines = ptr_A->m_Engines->m_Next;
            if ( ptr_A->m_Engines )
                ptr_A->m_Engines->m_Prev = NULL;
            disconnected_E->m_Next = NULL;
            free ( disconnected_E );
        }
        disconnected_A = ptr_A;
        ptr_A = ptr_A->m_Next;
        if ( ptr_A )
            ptr_A->m_Prev = NULL;
        disconnected_A->m_Next = NULL;
        free ( disconnected_A );
        disconnected_A = NULL;
    }
}
//======================================================================================================================
TARCHIVE * ReorderArchiveByType ( TARCHIVE * list )
{
    TARCHIVE * res = NULL;

    TARCHIVE * ptr_A = list;
    TENGINE * disconnected = NULL;

    while ( ptr_A )
    {
        while ( ptr_A->m_Engines )
        {
            disconnected = ptr_A->m_Engines;
            ptr_A->m_Engines = ptr_A->m_Engines->m_Next;
            if ( ptr_A->m_Engines )
                ptr_A->m_Engines->m_Prev = NULL;
            disconnected->m_Next = NULL;
            res = AddEngineByType ( res, disconnected );
        }
        ptr_A = ptr_A->m_Next;
    }

    DelArchive ( list );

    return res;
}
//======================================================================================================================
TARCHIVE * ReorderArchiveByYear ( TARCHIVE * list )
{
    TARCHIVE * res = NULL;

    TARCHIVE * ptr_A = list;
    TENGINE * disconnected = NULL;

    while ( ptr_A )
    {
        while ( ptr_A->m_Engines )
        {
            disconnected = ptr_A->m_Engines;
            ptr_A->m_Engines = ptr_A->m_Engines->m_Next;
            if ( ptr_A->m_Engines )
                ptr_A->m_Engines->m_Prev = NULL;
            disconnected->m_Next = NULL;
            res = AddEngineByYear ( res, disconnected );
        }
        ptr_A = ptr_A->m_Next;
    }

    DelArchive ( list );

    return res;
}
//======================================================================================================================
TARCHIVE * ReorderArchive ( TARCHIVE * list, int listBy )
{
    switch ( listBy )
    {
        case LIST_BY_TYPE:
            return ReorderArchiveByType ( list );
        case LIST_BY_YEAR:
            return ReorderArchiveByYear ( list );
    }
    return NULL;
}
//======================================================================================================================
#ifndef __PROGTEST__
//======================================================================================================================
int main ( int argc, char * argv [] )
{

    return 0;
}
#endif /* __PROGTEST__ */
