#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <deque>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include "progtest_solver.h"
#include "sample_tester.h"
using namespace std;
#endif /* __PROGTEST__ */
//======================================================================================================================
class CMessage
{
public:
                     CMessage ( const uint8_t * payload, size_t payload_len );
    vector <uint8_t> m_Pld;
    size_t           m_PldLen;
};
//======================================================================================================================
class CSentinelHacker
{
  public:
                                               CSentinelHacker ( void );
    static bool                                SeqSolve        ( const vector<uint64_t> & fragments, CBigInt & res );
    void                                       AddTransmitter  ( ATransmitter transmitter );
    void                                       AddReceiver     ( AReceiver receiver );
    void                                       AddFragment     ( uint64_t fragment );
    void                                       Start           ( unsigned thrCount );
    void                                       Stop            ( void );

private:
    static void                                ThrRecFunc      ( CSentinelHacker * instance, AReceiver receiver );
    static void                                ThrWorkFunc     ( CSentinelHacker * instance );
    static void                                ThrTransFunc    ( CSentinelHacker * instance, ATransmitter transmitter );
    static const constexpr uint64_t            MASK_MESSAGE_ID = 0xffffffe000000000;
    vector <AReceiver>                         m_Rec;
    vector <ATransmitter>                      m_Trans;
    vector <thread>                            m_ThrRec;
    vector <thread>                            m_ThrWork;
    vector <thread>                            m_ThrTrans;
    map <uint32_t, vector <uint64_t>>          m_Fragments;
    queue <pair <uint32_t, vector <uint64_t>>> m_QIn;
    queue <pair <uint32_t, CBigInt>>           m_QOut;
    mutex                                      m_MtxMap;
    mutex                                      m_MtxQIn;
    mutex                                      m_MtxQOut;
    condition_variable                         m_CvQIn;
    condition_variable                         m_CvQOut;
    bool                                       m_ThrRecWorking;
    bool                                       m_ThrWorkWorking;
    bool                                       m_Stop;
};
//======================================================================================================================
CMessage::CMessage ( const uint8_t * payload, size_t payload_len )
{
    size_t max_idx = 0;
    size_t res = payload_len % 8;
    if ( res )
        max_idx += 1;
    max_idx += payload_len / 8;
    for ( size_t i = 4; i < max_idx; ++i )
        m_Pld . emplace_back ( payload[i] );
    m_PldLen = payload_len - 32;
}
//======================================================================================================================
CSentinelHacker::CSentinelHacker ( void ) :
    m_ThrRecWorking  ( true ),
    m_ThrWorkWorking ( true ),
    m_Stop           ( false ) { }
//----------------------------------------------------------------------------------------------------------------------
bool CSentinelHacker::SeqSolve ( const vector <uint64_t> & fragments, CBigInt & res )
{
    vector <CMessage> messages;
    CBigInt tmp = 0;
    bool first = true;
    uint32_t nmb_permutations = FindPermutations ( fragments . data ( ), fragments . size ( ),
                                                   [&] ( const uint8_t * payload, size_t payload_len )
                                                   { messages . emplace_back ( payload, payload_len ); } );

    if ( nmb_permutations )
    {
        first = true; tmp = 0;
        for ( const auto & message : messages )
        {
            tmp = CountExpressions ( message . m_Pld . data ( ), message . m_PldLen );
            if ( tmp . CompareTo ( res ) == 1 || first )
            {
                res = tmp;
                first = false;
            }
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------------------------------------------------
void CSentinelHacker::AddReceiver ( AReceiver receiver ) { m_Rec . emplace_back ( receiver ); }
//----------------------------------------------------------------------------------------------------------------------
void CSentinelHacker::AddTransmitter ( ATransmitter transmitter ) { m_Trans . emplace_back ( transmitter ); }
//----------------------------------------------------------------------------------------------------------------------
void CSentinelHacker::AddFragment ( uint64_t fragment )
{
    uint32_t messageID = ( fragment & MASK_MESSAGE_ID ) >> 37;
    unique_lock lockerMap ( m_MtxMap );
    if ( m_Fragments . find ( messageID ) != m_Fragments . end ( ) )
        m_Fragments[messageID] . emplace_back ( fragment );
    else
    {
        vector <uint64_t> vecTmp = { fragment };
        m_Fragments . insert ( make_pair ( messageID, vecTmp ) );
    }
    lockerMap . unlock ( );

    unique_lock lockerQIn ( m_MtxQIn );
    m_QIn . push ( make_pair ( messageID, m_Fragments[messageID] ) );
    m_CvQIn . notify_one ( );
    lockerQIn . unlock ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CSentinelHacker::Start ( unsigned thrCount )
{
    for ( auto r : m_Rec )
        m_ThrRec . emplace_back( & CSentinelHacker::ThrRecFunc, this,  r );

    for ( unsigned i = 0; i < thrCount; ++i )
        m_ThrWork . emplace_back ( & CSentinelHacker::ThrWorkFunc, this );

    for ( auto t : m_Trans )
        m_ThrTrans . emplace_back ( & CSentinelHacker::ThrTransFunc, this, t );
}
//----------------------------------------------------------------------------------------------------------------------
void CSentinelHacker::Stop ( void )
{
    m_Stop = true;

    for ( auto & thr : m_ThrRec )
        thr . join ( );
    m_ThrRecWorking = false;

    unique_lock lockerQIn ( m_MtxQIn );
    m_CvQIn . notify_all ( );
    lockerQIn . unlock ( );

    for ( auto & thr: m_ThrWork )
        thr . join ( );
    m_ThrWorkWorking = false;

    unique_lock lockerQOut ( m_MtxQOut );
    m_CvQOut . notify_all ( );
    lockerQOut . unlock ( );

    for ( auto & thr : m_ThrTrans )
        thr . join ( );
}
//----------------------------------------------------------------------------------------------------------------------
void CSentinelHacker::ThrRecFunc ( CSentinelHacker * instance, AReceiver receiver )
{
    uint64_t data = 0;
    while ( receiver -> Recv ( data ) )
        instance -> AddFragment ( data );
}
//----------------------------------------------------------------------------------------------------------------------
void CSentinelHacker::ThrWorkFunc ( CSentinelHacker * instance )
{
    pair <uint32_t, vector<uint64_t>> tmpPair;
    vector <CMessage> messages;
    uint32_t nmb_permutations = 0;
    CBigInt tmpCnt = 0, resCnt = 0;
    bool first = true;

    unique_lock lockerQIn  ( instance -> m_MtxQIn );
    lockerQIn  . unlock ( );
    unique_lock lockerQOut ( instance -> m_MtxQOut );
    lockerQOut . unlock ( );
    unique_lock lockerMap  ( instance -> m_MtxMap );
    lockerMap  . unlock ( );

    while ( 1 )
    {
        lockerQIn . lock ( );
        instance -> m_CvQIn . wait ( lockerQIn, [=] ( ) { return ( ! ( instance -> m_QIn . empty ( ) ) || ( instance -> m_Stop && ! ( instance -> m_ThrRecWorking ) ) ); } );
        // if Stop has been called and all Receivers have finished and queue is empty -> return
        if ( instance -> m_Stop && ! ( instance -> m_ThrRecWorking ) && instance -> m_QIn . empty ( ) )
        {
            lockerQIn . unlock ( );
            return;
        }

        // thread has been awaken, no stop has been called or receivers are still working and the queue is not empty -> work
        tmpPair = instance -> m_QIn . front ( );
        instance -> m_QIn . pop ( );
        lockerQIn . unlock ( );

        nmb_permutations = FindPermutations ( tmpPair . second . data ( ), tmpPair . second . size ( ),
                                              [&] ( const uint8_t * payload, size_t payload_len )
                                              { messages . emplace_back ( payload, payload_len ); });
        if ( nmb_permutations )
        {
            first = true; tmpCnt = 0; resCnt = 0;
            for ( const auto & message : messages )
            {
                tmpCnt = CountExpressions ( message . m_Pld . data ( ), message . m_PldLen );
                if ( tmpCnt . CompareTo ( resCnt ) == 1 || first )
                {
                    resCnt = tmpCnt;
                    first = false;
                }
            }

            messages . clear ( );

            lockerQOut . lock ( );
            instance -> m_QOut . push ( make_pair ( tmpPair . first, resCnt ) );
            instance -> m_CvQOut . notify_one ( );
            lockerQOut . unlock ( );

            lockerMap . lock ( );
            instance -> m_Fragments . erase ( tmpPair . first );
            lockerMap . unlock ( );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
void CSentinelHacker::ThrTransFunc ( CSentinelHacker * instance, ATransmitter transmitter )
{
    pair <uint32_t, CBigInt > tmpPair;
    unique_lock lockerQOut ( instance -> m_MtxQOut );
    lockerQOut . unlock ( );

    while ( 1 )
    {
        lockerQOut . lock ( );
        instance -> m_CvQOut . wait ( lockerQOut, [=] ( ) { return ( ! ( instance -> m_QOut . empty ( ) ) || ( instance -> m_Stop && ! ( instance -> m_ThrRecWorking ) && ! ( instance -> m_ThrWorkWorking ) ) ); } );
        if ( instance -> m_Stop && ! ( instance -> m_ThrRecWorking ) && ! ( instance -> m_ThrWorkWorking ) && instance -> m_QOut . empty ( ) )
        {
            lockerQOut . unlock ( );
            break;
        }
        // thread has been awaken, no stop has been called or receivers are still working or workers are still working and the queue is not empty -> work
        tmpPair = instance -> m_QOut . front ( );
        instance -> m_QOut . pop ( );
        lockerQOut . unlock ( );

        transmitter -> Send ( tmpPair . first, tmpPair . second );
    }

    unique_lock lockerMap ( instance -> m_MtxMap );
    while ( ! ( instance -> m_Fragments . empty ( ) ) )
    {
        transmitter -> Incomplete ( instance -> m_Fragments . begin ( ) -> first );
        instance -> m_Fragments . erase ( instance -> m_Fragments . begin ( ) );
    }
    lockerMap . unlock ( );
}
//======================================================================================================================
#ifndef __PROGTEST__
int main ( void )
{
  using namespace std::placeholders;

  for ( const auto & x : g_TestSets )
  {
    CBigInt res;
    assert ( CSentinelHacker::SeqSolve ( x . m_Fragments, res ) );
    assert ( CBigInt ( x . m_Result ) . CompareTo ( res ) == 0 );
  }

  CSentinelHacker test;
  auto            trans = make_shared<CExampleTransmitter> ();
  AReceiver       recv  = make_shared<CExampleReceiver> ( initializer_list<uint64_t> { 0x02230000000c, 0x071e124dabef, 0x02360037680e, 0x071d2f8fe0a1, 0x055500150755 } );
  
  test . AddTransmitter ( trans ); 
  test . AddReceiver ( recv );
  test . Start ( 1 );

  static initializer_list<uint64_t> t1Data = { 0x071f6b8342ab, 0x0738011f538d, 0x0732000129c3, 0x055e6ecfa0f9, 0x02ffaa027451, 0x02280000010b, 0x02fb0b88bc3e };
  thread t1 ( FragmentSender, bind ( &CSentinelHacker::AddFragment, &test, _1 ), t1Data );
  static initializer_list<uint64_t> t2Data = { 0x073700609bbd, 0x055901d61e7b, 0x022a0000032b, 0x016f0000edfb };
  thread t2 ( FragmentSender, bind ( &CSentinelHacker::AddFragment, &test, _1 ), t2Data );
  FragmentSender ( bind ( &CSentinelHacker::AddFragment, &test, _1 ), initializer_list<uint64_t> { 0x017f4cb42a68, 0x02260000000d, 0x072500000025 } );

  t1 . join ();
  t2 . join ();

  test . Stop ();
  assert ( trans -> TotalSent () == 4 );
  assert ( trans -> TotalIncomplete () == 2 );

  return 0;  
}
#endif /* __PROGTEST__ */ 
