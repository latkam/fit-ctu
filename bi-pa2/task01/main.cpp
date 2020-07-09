#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
using namespace std;
#endif /* __PROGTEST__ */

#define CHECK_PREFIX if ( ! vec[pos] || vec[pos+1] ) m_Con = false;
#define CHECK_LETTRE if ( ! CheckLettre ( nmb ) ) m_Con = false;

class CHuffman
{
public:
                  CHuffman          ( const char * src, const char * dst );
    bool          DecompressFile    ( void );
    bool          CompressFile      ( void );

private:
    class CHufTree
    {
    public:
        struct TNode
        {
            uint32_t m_V;
            TNode *  m_P;
            TNode *  m_L;
            TNode *  m_R;
            bool     m_I; // inner node

                  TNode             ( uint32_t v, TNode * p, TNode * l, TNode * r, bool i );
        };
                  CHufTree          ( void );
                 ~CHufTree          ( void );
        TNode *   CreateNode        ( uint32_t val, TNode * p, bool i );
        bool      ConstructTree     ( vector<bool> & vec, int & pos );
        bool      GetLeafValue      ( uint32_t & val, vector<bool> & vec, int vec_size, int & pos );
        bool      CheckLettre       ( uint32_t val );
        bool      WriteLettre       ( ofstream & outFile, uint32_t val );

    private:
        TNode *   m_Root;
        bool      m_Con;

        void      ConstructTree     ( TNode ** ptr, TNode * parent, vector<bool> & vec, int & pos );
        void      DeleteTree        ( TNode * ptr );
        int       UTFCategory       ( uint32_t val );
        int       UTFCategoryDetail ( uint32_t val );
    };

    CHufTree      m_Tree;
    const char *  m_Src;
    const char *  m_Dst;
    vector<bool>  m_SrcV;
    int           m_SrcSize;
    int           m_SrcVSize;
    int           m_SrcVPos;
    int           m_CompChSize;

    bool          ConvertToStr      ( ifstream & inFile );
    bool          Decrypt           ( ofstream & outFile );
};
//======================================================================================================================
CHuffman::CHufTree::TNode::TNode ( uint32_t v, TNode * p, TNode * l, TNode * r, bool i ) :
        m_V ( v ),
        m_P ( p ),
        m_L ( l ),
        m_R ( r ),
        m_I ( i ) { }
//======================================================================================================================
CHuffman::CHufTree::CHufTree ( void ) :
        m_Root ( nullptr ),
        m_Con  ( true ) { }
//----------------------------------------------------------------------------------------------------------------------
CHuffman::CHufTree::~CHufTree ( void ) { DeleteTree ( m_Root ); }
//----------------------------------------------------------------------------------------------------------------------
CHuffman::CHufTree::TNode * CHuffman::CHufTree::CreateNode ( uint32_t val, TNode * p, bool i )
{
    return new TNode ( val, p, nullptr, nullptr, i );
}
//----------------------------------------------------------------------------------------------------------------------
bool CHuffman::CHufTree::ConstructTree ( vector<bool> & vec, int & pos )
{
    ConstructTree ( & m_Root, nullptr, vec, pos );
    return m_Con;
}
//----------------------------------------------------------------------------------------------------------------------
void CHuffman::CHufTree::ConstructTree ( TNode ** ptr, TNode * parent, vector<bool> & vec, int & pos )
{
    if ( ! m_Con )
        return;
    if ( ! vec[pos++] )
    {
//        cout << "ZERO" << endl;
        (*ptr) = CreateNode ( 0, parent, true );
        ConstructTree ( & (*ptr)->m_L, (*ptr), vec, pos );
        ConstructTree ( & (*ptr)->m_R, (*ptr), vec, pos );
    }
    else
    {
//        cout << "ONE-";
        int pos_tmp = pos;
        int len = vec.size ( );
        uint32_t nmb = 0;
        if ( pos_tmp + 7 < len && ! vec[pos_tmp] ) // ASCII <=> UTF-8 1 byte
        {
//            cout << "ASCII" << endl;
            for ( int i = 7; i >= 0; nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_LETTRE
            (*ptr) = CreateNode ( nmb, parent, false );
//            cout << ( char ) nmb << endl;
        }
        else if ( pos_tmp + 15 < len && vec[pos_tmp] && vec[pos_tmp+1] && ! vec[pos_tmp+2] ) // UTF-8 2 bytes
        {
//            cout << "UTF-8 2 bytes" << endl;
            int i = 15;
            for ( ; i >= 8; nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_PREFIX
            for ( ; i >= 0; nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_LETTRE
            (*ptr) = CreateNode ( nmb, parent, false );
        }
        else if ( pos_tmp + 23 < len && vec[pos_tmp] && vec[pos_tmp+1]
                  && vec[pos_tmp+2] && ! vec[pos_tmp+3] ) // UTF-8 3 bytes
        {
//            cout << "UTF-8 3 bytes" << endl;
            int i = 23;
            for ( ; i >= 16; nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_PREFIX
            for ( ; i >= 8;  nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_PREFIX
            for ( ; i >= 0;  nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_LETTRE
            (*ptr) = CreateNode ( nmb, parent, false );
        }
        else if ( pos_tmp + 31 < len && vec[pos_tmp] && vec[pos_tmp+1]
                  && vec[pos_tmp+2] && vec[pos_tmp+3] && ! vec[pos_tmp+4] ) // UTF-8 4 bytes
        {
//            cout << "UTF-8 4 bytes" << endl;
            int i = 31;
            for ( ; i >= 24; nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_PREFIX
            for ( ; i >= 16; nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_PREFIX
            for ( ; i >= 8;  nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_PREFIX
            for ( ; i >= 0;  nmb += ( vec[pos++] << i ), i-- ) { }
            CHECK_LETTRE
            (*ptr) = CreateNode ( nmb, parent, false );
        }
        else
            m_Con = false;
    }
}
//----------------------------------------------------------------------------------------------------------------------
bool CHuffman::CHufTree::GetLeafValue ( uint32_t & val, vector<bool> & vec, int vec_size, int & pos )
{
    TNode ** ptr = & m_Root;
    while ( (*ptr) && (*ptr)->m_I )
    {
        if ( pos >= vec_size )
            return false;
//        cout << vec[pos] << endl;
        if ( ! vec[pos++] )
            ptr = & (*ptr)->m_L;
        else
            ptr = & (*ptr)->m_R;
    }
    if ( ! (*ptr) )
        return false;
    val = (*ptr)->m_V;
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CHuffman::CHufTree::CheckLettre ( uint32_t val ) // checks values of single bytes
{
    uint32_t m1 = 255, m2 = 65280, m3 = 16711680, m4 = 4278190080;
    uint32_t  b4 = ( val & m4 ) >> 24;
    uint32_t  b3 = ( val & m3 ) >> 16;
    uint32_t  b2 = ( val & m2 ) >>  8;
    uint32_t  b1 =   val & m1;
    switch ( UTFCategoryDetail ( val ) )
    {
        case 0:
            return false;

        case 2:
//            cout << "Case 2. b2 = " << b2 << ", b1 = " << b1 << endl;
            return ( (b2 >= 194 && b2 <= 223) && (b1 >= 128 && b1 <= 191) );

        case 3:
//            cout << "case 3" << endl;
            return ( (b3 == 224) && (b2 >= 160 && b2 <= 191) && (b1 >= 128 && b1 <= 191) );

        case 4:
//            cout << "case 4" << endl;
            return ( (b3 >= 225 && b3 <= 239) && (b2 >= 128 && b2 <= 191) && (b1 >= 128 && b1 <= 191) );

        case 5:
//            cout << "case 5. b4 = " << b4 << ", b3 = " << b3 << ", b2 = " << b2 << ", b1 = " << b1 << endl;
            return ( (b4 == 240) && (b3 >= 144 && b3 <= 191) && (b2 >= 128 && b2 <= 191) && (b1 >= 128 && b1 <= 191) );

        case 6:
//            cout << "case 6" << endl;
            return ( (b4 >= 241 && b4 <= 243) && (b3 >= 128 && b3 <= 191) && (b2 >= 128 && b2 <= 191) && (b1 >= 128 && b1 <= 191) );

        case 7:
//            cout << "case 7" << endl;
            return( (b4 == 244) && (b3 >= 128 && b3 <= 143) && (b2 >= 128 && b2 <= 191) && (b1 >= 128 && b1 <= 191) );
    }
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CHuffman::CHufTree::WriteLettre ( ofstream & outFile, uint32_t val )
{
    uint32_t m1 = 255, m2 = 65280, m3 = 16711680, m4 = 4278190080;
    uint8_t b1 =   val & m1;
    uint8_t b2 = ( val & m2 ) >>  8;
    uint8_t b3 = ( val & m3 ) >> 16;
    uint8_t b4 = ( val & m4 ) >> 24;

    switch ( UTFCategory ( val ) )
    {
        case 0:
            return false;

        case 1:
            outFile.write ( ( const char * ) & b1, sizeof ( uint8_t ) );
            if ( outFile.bad ( ) && ! outFile.eof ( ) )
                return false;
            break;

        case 2:
            outFile.write ( ( const char * ) & b2, sizeof ( uint8_t ) );
            outFile.write ( ( const char * ) & b1, sizeof ( uint8_t ) );
            if ( outFile.bad ( ) && ! outFile.eof ( ) )
                return false;
            break;

        case 3:
            outFile.write ( ( const char * ) & b3, sizeof ( uint8_t ) );
            outFile.write ( ( const char * ) & b2, sizeof ( uint8_t ) );
            outFile.write ( ( const char * ) & b1, sizeof ( uint8_t ) );
            if ( outFile.bad ( ) && ! outFile.eof ( ) )
                return false;
            break;

        case 4:
            outFile.write ( ( const char * ) & b4, sizeof ( uint8_t ) );
            outFile.write ( ( const char * ) & b3, sizeof ( uint8_t ) );
            outFile.write ( ( const char * ) & b2, sizeof ( uint8_t ) );
            outFile.write ( ( const char * ) & b1, sizeof ( uint8_t ) );
            if ( outFile.bad ( ) && ! outFile.eof ( ) )
                return false;
    }
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CHuffman::CHufTree::DeleteTree ( CHuffman::CHufTree::TNode * ptr )
{
    if ( ! ptr )
        return;
    if ( ptr->m_L )
        DeleteTree ( ptr->m_L );
    if ( ptr->m_R )
        DeleteTree ( ptr->m_R );
    delete ptr;
}
//----------------------------------------------------------------------------------------------------------------------
int CHuffman::CHufTree::UTFCategory ( uint32_t val ) // checks how many bytes UTF-8 consists of
{
    uint32_t m1 = 255, m2 = 65535, m3 = 16777215;
    if ( val == ( val & m1 ) )
        return 1;
    if ( val == ( val & m2 ) )
        return 2;
    if ( val == ( val & m3 ) )
        return 3;
    return 4;
}
//----------------------------------------------------------------------------------------------------------------------
int CHuffman::CHufTree::UTFCategoryDetail ( uint32_t val ) // extracts CODE POINT
{
    uint32_t m21 = 63, m22 = 7936, m32 = 16128, m33 = 983040, m43 = 4128768, m44 = 117440512, b1, b2, b3, b4, res = val;
    switch ( UTFCategory ( val ) )
    {
        case 2:
            b2 = ( val & m22 ) >> 2;
            b1 = ( val & m21 );
            res = b1 | b2;
            break;
        case 3:
            b3 = ( val & m33 ) >> 4;
            b2 = ( val & m32 ) >> 2;
            b1 = ( val & m21 );
            res = b1 | b2 | b3;
            break;
        case 4:
            b4 = ( val & m44 ) >> 6;
            b3 = ( val & m43 ) >> 4;
            b2 = ( val & m32 ) >> 2;
            b1 = ( val & m21 );
            res = b1 | b2 | b3 | b4;
            break;
    }
    if ( res <= 127 )
        return 1;
    if ( res >= 128 && res <= 2047 )
        return 2;
    if ( res >= 2048 && res <= 4095 )
        return 3;
    if ( res >= 4096 && res <= 65535 )
        return 4;
    if ( res >= 65536 && res <= 262143 )
        return 5;
    if ( res >= 262144 && res <= 1048575 )
        return 6;
    if ( res >= 1048576 && res <= 1114111 )
        return 7;
    return 0;
}
//======================================================================================================================
CHuffman::CHuffman ( const char * src, const char * dst ) :
        m_Src        ( src ),
        m_Dst        ( dst ),
        m_CompChSize ( 4096 ) { }

//======================================================================================================================
bool CHuffman::DecompressFile ( void )
{
    ifstream inFile  ( m_Src, ios_base::in  | ios_base::binary | ios_base::ate );
    ofstream outFile ( m_Dst, ios_base::out | ios_base::binary | ios_base::trunc );

    if ( ! inFile.is_open ( ) || ! outFile.is_open ( ) )
        return false;

    m_SrcVPos = 0;

    if ( ! ConvertToStr ( inFile )
         || ! m_Tree.ConstructTree ( m_SrcV, m_SrcVPos )
         || ! Decrypt ( outFile ) )
    {
        inFile.close ( );
        outFile.close ( );
//        cout << "Converting to str or constructing tree or reading chunks or decryption failed." << endl;
        return false;
    }
    inFile.close  ( );
    outFile.close ( );
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CHuffman::CompressFile ( void )
{
    ifstream inFile  ( m_Src, ios_base::in  | ios_base::binary );
    ofstream outFile ( m_Dst, ios_base::out | ios_base::binary | ios_base::trunc );

    if ( ! inFile.is_open ( ) || ! outFile.is_open ( ) )
        return false;

    inFile.close  ( );
    outFile.close ( );
    return false;
}
//----------------------------------------------------------------------------------------------------------------------
bool CHuffman::ConvertToStr ( ifstream & inFile )
{
    char c;
    m_SrcSize = inFile.tellg ( );
    if ( ! m_SrcSize )
        return false;
    inFile.seekg ( ios_base::beg );

    if ( ! inFile.good ( ) && ! inFile.eof ( ) )
        return false;

    for ( int i = 0; i < m_SrcSize; ++i )
    {
        inFile.read ( & c, sizeof ( uint8_t ) );
        if ( ! inFile.good ( ) && ! inFile.eof ( ) )
            return false;
        for ( int j = 7; j >= 0; --j )
            m_SrcV.push_back ( (c >> j) & 1 );
    }
    m_SrcVSize = m_SrcV.size ( );
//    cout << m_SrcVSize / 8 << ", " << m_SrcSize << endl;
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CHuffman::Decrypt ( ofstream & outFile )
{
//    cout << "pos: " << m_SrcVPos << endl;
    uint32_t val;
    while ( m_SrcVPos < m_SrcVSize - 1 && m_SrcV[m_SrcVPos++] )
    {
        for ( int i = 0; i < 4096; ++i )
        {
            if ( ! m_Tree.GetLeafValue ( val, m_SrcV, m_SrcVSize, m_SrcVPos )
                 || ! m_Tree.CheckLettre ( val )
                 || ! m_Tree.WriteLettre ( outFile, val ) )
                return false;
//            cout << "leaf value complete chunk: " << val << endl;
        }
    }

    if ( m_SrcVPos + 11 >= m_SrcVSize )
        return false;
//    cout << "Switched to incomplete chunk." << endl;

    int chunk_size = 0;
    for ( int i = 11; i >= 0; --i )
        chunk_size += ( m_SrcV[m_SrcVPos++] << i );

    for ( int i = 0; i < chunk_size; ++i )
    {
        if ( ! m_Tree.GetLeafValue ( val, m_SrcV, m_SrcVSize, m_SrcVPos )
             || ! m_Tree.CheckLettre ( val )
             || ! m_Tree.WriteLettre ( outFile, val ) )
            return false;
//        cout << "leaf value: " << ( char ) val << endl;
    }
    return true;
}
//======================================================================================================================
bool decompressFile ( const char * inFileName, const char * outFileName )
{
    CHuffman huffman ( inFileName, outFileName );
    return huffman.DecompressFile ( );
}
//----------------------------------------------------------------------------------------------------------------------
bool compressFile ( const char * inFileName, const char * outFileName )
{
    CHuffman huffman ( inFileName, outFileName );
    return huffman.CompressFile ( );
}
//----------------------------------------------------------------------------------------------------------------------
#ifndef __PROGTEST__
bool identicalFiles ( const char * fileName1, const char * fileName2 )
{
    ifstream inFile1, inFile2;
    inFile1.open ( fileName1, ios_base::in | ios_base::ate | ios_base::binary );
    inFile2.open ( fileName2, ios_base::in | ios_base::ate | ios_base::binary );

    if ( ! inFile1.is_open ( ) || ! inFile2.is_open ( ) )
        return false;

    int len1 = 0, len2 = 0;
    uint8_t tmp1, tmp2;
    len1 = inFile1.tellg ( );
    len2 = inFile2.tellg ( );

    if ( len1 != len2 )
        return false;

    inFile1.seekg ( ios_base::beg );
    inFile2.seekg ( ios_base::beg );

    if ( ( ! inFile1.good ( ) && ! inFile1.eof ( ) ) || ( ! inFile2.good ( ) && ! inFile2.eof ( ) ) )
        return false;

    for ( int i = 0; i < len1; ++i )
    {
        inFile1.read ( ( char * ) & tmp1, sizeof ( uint8_t ) );
        inFile2.read ( ( char * ) & tmp2, sizeof ( uint8_t ) );
        if ( tmp1 != tmp2 )
            return false;

        if ( ( ! inFile1.good ( ) && ! inFile1.eof ( ) ) || ( ! inFile2.good ( ) && ! inFile2.eof ( ) ) )
            return false;
    }

    inFile1.close ( );
    inFile2.close ( );
    return true;
}
//======================================================================================================================
int main ( void )
{

    return 0;
}
#endif /* __PROGTEST__ */