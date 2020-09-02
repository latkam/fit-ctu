#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cmath>
using namespace std;
#endif /* __PROGTEST__ */
//======================================================================================================================
/* (2^6 - 2^32) */
#define BLOCK_LISTS_SIZE 27
#define MIN_EXP 6
#define HEADER_SIZE sizeof ( TBlock )
//======================================================================================================================
template <typename T>
T min_ ( T a, T b )
{
  if ( a < b )
    return a;
  return b;
}
//======================================================================================================================
template <typename T>
T max_ ( T a, T b )
{
  if ( a > b )
    return a;
  return b;
}
//======================================================================================================================
struct TBlock
{
  TBlock   * m_Prev;
  TBlock   * m_Next;
  TBlock   * m_Addr;
  uint32_t   m_Size;
  bool       m_Free;

  bool       IsValid ( void );
};
//----------------------------------------------------------------------------------------------------------------------
bool TBlock::IsValid ( void )
{
  return ! ( m_Addr != this || m_Size == 0 || m_Size % 2 || m_Free );
}
//======================================================================================================================
struct TBlockList
{
  TBlock * m_First;

  void     CleanBlockList    ( void );
};
//----------------------------------------------------------------------------------------------------------------------
void TBlockList::CleanBlockList ( void )
{
  TBlock ** ptr = & m_First;
  while ( *ptr )
  {
    (*ptr) -> m_Free = true;
    (*ptr) -> m_Prev = nullptr;
    (*ptr) -> m_Size = 0;
    ptr = & (*ptr) -> m_Next;
    if ( *ptr && (*ptr) -> m_Prev )
      (*ptr) -> m_Prev -> m_Next = nullptr;
  }
  m_First = nullptr;
}
//======================================================================================================================
class CBuddyAllocator
{
public:
               CBuddyAllocator   ( void );
  void         Init              ( void * memPool, int memSize );
  void       * Alloc             ( int size );
  bool         Free              ( void * block );
  void         Done              ( int * pendingBlk ) const;

private:
  static int   GetIdxFloor       ( uint32_t size );
  static int   GetIdxCeil        ( uint32_t size );
  void         CleanMemPool      ( void );
  TBlock     * FindAndMergeBuddy ( TBlock * blockPtr );

  void       * m_MemPool;
  uint32_t     m_MemPoolSize;
  int          m_BlocksAllocated;
  TBlockList   m_BlockLists[BLOCK_LISTS_SIZE];
};
//----------------------------------------------------------------------------------------------------------------------
CBuddyAllocator::CBuddyAllocator ( void ) :
    m_MemPool         ( nullptr ),
    m_MemPoolSize     ( 0 ),
    m_BlocksAllocated ( 0 ) { }
//----------------------------------------------------------------------------------------------------------------------
void CBuddyAllocator::Init ( void * memPool, int memSize )
{
  m_MemPool = memPool;
  m_MemPoolSize = memSize;
  m_BlocksAllocated = 0;
  CleanMemPool ( );

  int idx, min = pow ( 2, MIN_EXP );
  uint32_t tmpSize, offset = 0;

  while ( memSize >= min )
  {
    idx = GetIdxFloor ( memSize );
    tmpSize = pow ( 2, idx + MIN_EXP );
    m_BlockLists[idx] . m_First = (TBlock *) ((uint64_t) memPool + offset);
    m_BlockLists[idx] . m_First -> m_Prev = nullptr;
    m_BlockLists[idx] . m_First -> m_Next = nullptr;
    m_BlockLists[idx] . m_First -> m_Addr = m_BlockLists[idx] . m_First;
    m_BlockLists[idx] . m_First -> m_Size = tmpSize;
    m_BlockLists[idx] . m_First -> m_Free = true;
    memSize -= tmpSize;
    offset += tmpSize;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void * CBuddyAllocator::Alloc ( int size )
{
  size += HEADER_SIZE;

  int idxStart, idx;
  bool found = false;

  idxStart = GetIdxCeil ( size );

  /* Block is too large to handle. */
  if ( idxStart >= BLOCK_LISTS_SIZE )
    return nullptr;

  idx = idxStart;
  while ( idx < BLOCK_LISTS_SIZE )
  {
    if ( m_BlockLists[idx] . m_First )
    {
      found = true;
      break;
    }
    ++ idx;
  }

  if ( ! found )
    return nullptr;

  uint32_t sizeSplit = pow ( 2, idx + MIN_EXP ), sizeDesired = pow ( 2, idxStart + MIN_EXP );
  TBlock * blockPtr = m_BlockLists[idx] . m_First, * nextBlockPtr;
  blockPtr -> m_Addr = blockPtr;

  /* Disconnect original block. */
  m_BlockLists[idx] . m_First = m_BlockLists[idx] . m_First -> m_Next;
  if ( blockPtr -> m_Next )
    blockPtr -> m_Next -> m_Prev = blockPtr -> m_Prev;

  while ( sizeSplit != sizeDesired )
  {
    /* Create first half. */
    blockPtr -> m_Prev = blockPtr -> m_Next = nullptr;
    blockPtr -> m_Size /= 2;

    /* Create second half. */
    nextBlockPtr = (TBlock*) ((uint64_t) blockPtr + blockPtr -> m_Size);
    nextBlockPtr -> m_Prev = nextBlockPtr -> m_Next = nullptr;
    nextBlockPtr -> m_Addr = nextBlockPtr;
    nextBlockPtr -> m_Size = blockPtr -> m_Size;
    nextBlockPtr -> m_Free = true;

    /* Connect second half. */
    if ( m_BlockLists[idx - 1] . m_First )
    {
      m_BlockLists[idx - 1] . m_First -> m_Prev = nextBlockPtr;
      nextBlockPtr -> m_Next = m_BlockLists[idx - 1] . m_First;
      m_BlockLists[idx - 1] . m_First = nextBlockPtr;
    }
    else
      m_BlockLists[idx - 1] . m_First = nextBlockPtr;
    -- idx;
    sizeSplit /= 2;
  }
  blockPtr -> m_Prev = blockPtr -> m_Next = nullptr;
  blockPtr -> m_Free = false;
  ++ m_BlocksAllocated;
  return (TBlock *) ((uint64_t)blockPtr + HEADER_SIZE);
}
//----------------------------------------------------------------------------------------------------------------------
bool CBuddyAllocator::Free ( void * block )
{
  block = (void *) ((uint64_t) block - HEADER_SIZE);

  if ( block < m_MemPool
       || block >= (void *) ((uint64_t) m_MemPool + m_MemPoolSize)
       || ! ( (TBlock *) block ) -> IsValid ( ) )
    return false;

  memset ( (void *) ((uint64_t) block + HEADER_SIZE), 0, ((uint64_t)((TBlock *) block ) -> m_Size) - HEADER_SIZE);
  ((TBlock *) block) -> m_Free = true;

  TBlock * merged;
  while ( ( merged = FindAndMergeBuddy ( ( TBlock * ) block ) ) )
    block = (void *) merged;

  /* Connect merged block. */
  int idx = GetIdxCeil ( ( (TBlock *) block ) -> m_Size );
  if ( m_BlockLists[idx] . m_First )
  {
    m_BlockLists[idx] . m_First -> m_Prev = (TBlock *) block;
    ((TBlock *) block) -> m_Next = m_BlockLists[idx] . m_First;
    m_BlockLists[idx] . m_First = (TBlock *) block;
  }
  else
    m_BlockLists[idx] . m_First = (TBlock *) block;
  -- m_BlocksAllocated;
  return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CBuddyAllocator::Done ( int * pendingBlk ) const
{
  *pendingBlk = m_BlocksAllocated;
}
//----------------------------------------------------------------------------------------------------------------------
int CBuddyAllocator::GetIdxFloor ( uint32_t size )
{
  int res = floor ( log2 ( size ) ) - MIN_EXP;
  if ( res < 0 )
    return 0;
  return res;
}
//----------------------------------------------------------------------------------------------------------------------
int CBuddyAllocator::GetIdxCeil ( uint32_t size )
{
  int res = ceil ( log2 ( size ) ) - MIN_EXP;
  if ( res < 0 )
    return 0;
  return res;
}
//----------------------------------------------------------------------------------------------------------------------
void CBuddyAllocator::CleanMemPool ( void )
{
  for ( auto & blockList : m_BlockLists )
    blockList . CleanBlockList ( );
}
//----------------------------------------------------------------------------------------------------------------------
TBlock * CBuddyAllocator::FindAndMergeBuddy ( TBlock * blockPtr )
{
  uint64_t blockAddr      = (uint64_t)blockPtr - (uint64_t) m_MemPool,
           xorOperand     = 1lu << ( GetIdxCeil ( blockPtr -> m_Size ) + MIN_EXP ),
           potentialBuddy = ( blockAddr ^ xorOperand ) + (uint64_t) m_MemPool ;

  TBlock * PBPtr = (TBlock *) potentialBuddy;
  if ( potentialBuddy < (uint64_t) m_MemPool || potentialBuddy >= (uint64_t) m_MemPool + m_MemPoolSize )
    return nullptr;

  if ( ! PBPtr -> m_Free || PBPtr -> m_Addr != PBPtr || PBPtr -> m_Size != blockPtr -> m_Size )
    return nullptr;

  /* Disconnect potential buddy. */
  if ( PBPtr -> m_Prev )
    PBPtr -> m_Prev -> m_Next = PBPtr -> m_Next;
  if ( PBPtr -> m_Next )
    PBPtr -> m_Next -> m_Prev = PBPtr -> m_Prev;

  int idx = GetIdxCeil ( PBPtr -> m_Size );
  if ( PBPtr == m_BlockLists[idx] . m_First )
    m_BlockLists[idx] . m_First = PBPtr -> m_Next;

  PBPtr -> m_Prev = PBPtr -> m_Next = nullptr;

  TBlock * left = min_ ( blockPtr, PBPtr ), * right = max_ ( blockPtr, PBPtr );
  memset ( right, 0, right -> m_Size );

  /* Prepare resulting block. */
  left -> m_Prev = left -> m_Next = nullptr;
  left -> m_Addr = left;
  left -> m_Size *= 2;
  left -> m_Free = true;
  return left;
}
//======================================================================================================================
CBuddyAllocator buddyAllocator;
//======================================================================================================================
void HeapInit ( void * memPool, int memSize )
{
  buddyAllocator . Init ( memPool, memSize );
}
//----------------------------------------------------------------------------------------------------------------------
void * HeapAlloc ( int size )
{
  return buddyAllocator . Alloc ( size );
}
//----------------------------------------------------------------------------------------------------------------------
bool HeapFree ( void * blk )
{
  return buddyAllocator . Free ( blk );
}
//----------------------------------------------------------------------------------------------------------------------
void HeapDone ( int  * pendingBlk )
{
  return buddyAllocator . Done ( pendingBlk );
}
//======================================================================================================================
#ifndef __PROGTEST__
int main ( void )
{
  uint8_t       * p0, *p1, *p2, *p3, *p4;
  int             pendingBlk;
  static uint8_t  memPool[2359296];


  HeapInit ( memPool, 2097152 );
  assert ( ( p0 = (uint8_t*) HeapAlloc ( 512000 ) ) != nullptr );
  memset ( p0, 0, 512000 );
  assert ( ( p1 = (uint8_t*) HeapAlloc ( 511000 ) ) != nullptr );
  memset ( p1, 0, 511000 );
  assert ( ( p2 = (uint8_t*) HeapAlloc ( 26000 ) ) != nullptr );
  memset ( p2, 0, 26000 );
  HeapDone ( &pendingBlk );
  assert ( pendingBlk == 3 );


  HeapInit ( memPool, 2097152 );
  assert ( ( p0 = (uint8_t*) HeapAlloc ( 1000000 ) ) != nullptr );
  memset ( p0, 0, 1000000 );
  assert ( ( p1 = (uint8_t*) HeapAlloc ( 250000 ) ) != nullptr );
  memset ( p1, 0, 250000 );
  assert ( ( p2 = (uint8_t*) HeapAlloc ( 250000 ) ) != nullptr );
  memset ( p2, 0, 250000 );
  assert ( ( p3 = (uint8_t*) HeapAlloc ( 250000 ) ) != nullptr );
  memset ( p3, 0, 250000 );
  assert ( ( p4 = (uint8_t*) HeapAlloc ( 50000 ) ) != nullptr );
  memset ( p4, 0, 50000 );
  assert ( HeapFree ( p2 ) );
  assert ( HeapFree ( p4 ) );
  assert ( HeapFree ( p3 ) );
  assert ( HeapFree ( p1 ) );
  assert ( ( p1 = (uint8_t*) HeapAlloc ( 500000 ) ) != nullptr );
  memset ( p1, 0, 500000 );
  assert ( HeapFree ( p0 ) );
  assert ( HeapFree ( p1 ) );
  HeapDone ( &pendingBlk );
  assert ( pendingBlk == 0 );


  HeapInit ( memPool, 2359296 );
  assert ( ( p0 = (uint8_t*) HeapAlloc ( 1000000 ) ) != nullptr );
  memset ( p0, 0, 1000000 );
  assert ( ( p1 = (uint8_t*) HeapAlloc ( 500000 ) ) != nullptr );
  memset ( p1, 0, 500000 );
  assert ( ( p2 = (uint8_t*) HeapAlloc ( 500000 ) ) != nullptr );
  memset ( p2, 0, 500000 );
  assert ( ( p3 = (uint8_t*) HeapAlloc ( 500000 ) ) == nullptr );
  assert ( HeapFree ( p2 ) );
  assert ( ( p2 = (uint8_t*) HeapAlloc ( 300000 ) ) != nullptr );
  memset ( p2, 0, 300000 );
  assert ( HeapFree ( p0 ) );
  assert ( HeapFree ( p1 ) );
  HeapDone ( &pendingBlk );
  assert ( pendingBlk == 1 );


  HeapInit ( memPool, 2359296 );
  assert ( ( p0 = (uint8_t*) HeapAlloc ( 1000000 ) ) != nullptr );
  memset ( p0, 0, 1000000 );
  assert ( ! HeapFree ( p0 + 1000 ) );
  HeapDone ( &pendingBlk );
  assert ( pendingBlk == 1 );

  return 0;
}
#endif /* __PROGTEST__ */
