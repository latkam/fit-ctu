#include <cstdio>
#include <cstdint>
#include <cmath>
#include <bits/move.h>

#define ARR_LENGTH 10000

using namespace std;

class CHolding
{
public:
	     CHolding  ( void );
	    ~CHolding  ( void );
	void Add       ( int chain, unsigned int id, unsigned int revenue );
	bool Remove    ( int chain, unsigned int & id );
	bool Remove    ( unsigned int & id );
	void Merge     ( int dstChain, int srcChain );
	bool Merge     ( void );

private:
	struct TGOODS
	{
		unsigned int id;
		unsigned int revenue;
		bool operator <  ( const TGOODS & other );
		bool operator <= ( const TGOODS & other );
	};

	class CBinaryHeap
	{
	public:
		int      m_max_index;
		         CBinaryHeap    ( void );
		        ~CBinaryHeap    ( void );
		void     HeapReallocate ( void );
		void     HeapInsert     ( TGOODS * goods );
		TGOODS * HeapFindMin    ( void );
		void     HeapExtractMin ( void );
		void	 HeapMerge		( CBinaryHeap & otherHeap );

	private:
		int       m_heap_size;
		TGOODS ** m_heap;
	};
	CBinaryHeap m_arr [ARR_LENGTH];
};
//======================================================================================================================
bool CHolding::TGOODS::operator < ( const TGOODS & other )
{
	return ( ( this->revenue != other.revenue) ? this->revenue < other.revenue : this->id < other.id );
}
//----------------------------------------------------------------------------------------------------------------------
bool CHolding::TGOODS::operator <= ( const TGOODS & other )
{
	return ( ( this->revenue != other.revenue) ? this->revenue <= other.revenue : this->id <= other.id );
}
//======================================================================================================================
CHolding::CBinaryHeap::CBinaryHeap ()
{
	m_heap_size =  0;
	m_max_index = -1;
	m_heap = nullptr;
}
//----------------------------------------------------------------------------------------------------------------------
CHolding::CBinaryHeap::~CBinaryHeap ()
{
	for ( int i = 0; i <= m_max_index; i++ )
		delete m_heap[i];
	delete [] m_heap;
}
//----------------------------------------------------------------------------------------------------------------------
void CHolding::CBinaryHeap::HeapReallocate ( void )
{
	TGOODS ** new_heap = new TGOODS * [2 * m_heap_size];

	for ( int i = 0; i < m_heap_size; i++ )
		new_heap[i] = m_heap[i];

	for ( int i = m_heap_size; i < 2 * m_heap_size; i++ )
		new_heap[i] = nullptr;

	for ( int i = 0; i < m_heap_size; i++ )
		m_heap[i] = nullptr;
	delete [] m_heap;

	m_heap = new_heap;
	m_heap_size *= 2;
}
//----------------------------------------------------------------------------------------------------------------------
void CHolding::CBinaryHeap::HeapInsert ( TGOODS * goods )
{
	/**
	 * For programming purposes, array representing heap is indexed normally, e.g. from 0.
	 * But the heap itself uses different indexing starting from 1, so we can use mathematical operations
	 * for moving in the heap.
	 * Thus, all mathematical operations are performed with the array index increased by 1.
	 */
	int curr_index, parent;
	if ( m_max_index == -1 )
	{
		m_heap_size = 2;
		m_heap = new TGOODS * [m_heap_size];
		for ( int i = 0; i < m_heap_size; i++ )
			m_heap[i] = nullptr;
	}
	if ( m_max_index == m_heap_size -1 )
		HeapReallocate ();

	m_max_index += 1;
	m_heap[m_max_index] = goods;
	curr_index = m_max_index;

	while ( curr_index ) // bubble up
	{
		parent = floor ((curr_index+1)/2.0)-1;
		if ( * m_heap[parent] <= * m_heap[curr_index] )
			break;
		swap (m_heap[parent], m_heap[curr_index] );
		curr_index = parent;
	}
}
//----------------------------------------------------------------------------------------------------------------------
CHolding::TGOODS * CHolding::CBinaryHeap::HeapFindMin ( void )
{
	if ( m_heap != nullptr )
		return m_heap[0];
	return nullptr;
}
//----------------------------------------------------------------------------------------------------------------------
void CHolding::CBinaryHeap::HeapExtractMin ( void )
{
	int curr_index, l_son, r_son, son;
	swap ( m_heap[0], m_heap[m_max_index] );
	delete m_heap[m_max_index];
	m_heap[m_max_index] = nullptr;
	m_max_index -= 1;
	curr_index = 0;
	while ( true ) // bubble down
	{
		l_son = 2*(curr_index+1)-1;
		r_son = 2*(curr_index+1);
		if ( l_son > m_max_index && r_son > m_max_index )
			break;
		else if ( l_son > m_max_index && r_son <= m_max_index )
			son = r_son;
		else if ( l_son <= m_max_index && r_son > m_max_index )
			son = l_son;
		else
			son = ( * m_heap[l_son] < * m_heap[r_son] ) ? l_son : r_son;
		if ( * m_heap[curr_index] <= * m_heap[son] )
			break;
		swap ( m_heap[son], m_heap[curr_index] );
		curr_index = son;
	}
}
//----------------------------------------------------------------------------------------------------------------------
void CHolding::CBinaryHeap::HeapMerge ( CHolding::CBinaryHeap & otherHeap )
{
	int result_size = this->m_max_index + 1 + otherHeap.m_max_index + 1;
	int result_max_index = result_size-1;
	int i, j, curr_index, l_son, r_son, son;
	TGOODS ** result = new TGOODS * [result_size];
	for ( i = 0; i <= otherHeap.m_max_index; i++ )
	{
		result[i] = otherHeap.m_heap[i];
		otherHeap.m_heap[i] = nullptr;
	}

	for ( i = otherHeap.m_max_index+1, j = 0; i <= otherHeap.m_max_index+1+this->m_max_index; i++, j++ )
	{
		result[i] = this->m_heap[j];
		this->m_heap[j] = nullptr;
	}

	delete [] this->m_heap;

	int max_index_to_bubble = (floor(result_size/2.0))-1;
	for ( i = max_index_to_bubble; i >= 0; i-- )
	{
		curr_index = i;
		while ( true ) // bubble down
		{
			l_son = 2*(curr_index+1)-1;
			r_son = 2*(curr_index+1);
			if ( l_son > result_max_index && r_son > result_max_index )
				break;
			else if ( l_son > result_max_index && r_son <= result_max_index )
				son = r_son;
			else if ( l_son <= result_max_index && r_son > result_max_index )
				son = l_son;
			else
				son = ( * result[l_son] < * result[r_son] ) ? l_son : r_son;
			if ( * result[curr_index] <= * result[son] )
				break;
			swap ( result[son], result[curr_index] );
			curr_index = son;
		}
	}
	otherHeap.m_heap_size =  0;
	otherHeap.m_max_index = -1;

	this->m_heap = result;
	this->m_heap_size = result_size;
	this->m_max_index = result_max_index;
}
//======================================================================================================================
CHolding::CHolding ()
{
	for ( auto & i : m_arr )
		i = CBinaryHeap();
}
//----------------------------------------------------------------------------------------------------------------------
CHolding::~CHolding () { }
//----------------------------------------------------------------------------------------------------------------------
void CHolding::Add ( int chain, unsigned int id, unsigned int revenue )
{
	TGOODS * goods = new TGOODS;
	goods->id = id;
	goods->revenue = revenue;
	m_arr[chain-1].HeapInsert ( goods );
}
//----------------------------------------------------------------------------------------------------------------------
bool CHolding::Remove (int chain, unsigned int & id )
{
	if ( m_arr[chain-1].m_max_index == -1 )
		return false;
	id = m_arr[chain-1].HeapFindMin ()->id;
	m_arr[chain-1].HeapExtractMin ();
	return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool CHolding::Remove ( unsigned int & id )
{
	bool empty_arr = true;
	int chain_index = 0;
	TGOODS goods_min_revenue, tmp;
	goods_min_revenue.id = 0;
	goods_min_revenue.revenue = UINT32_MAX;

	for ( int i = 0; i < ARR_LENGTH; i++ )
	{
		if ( m_arr[i].m_max_index == -1 )
			continue;
		empty_arr = false;
		tmp = * m_arr[i].HeapFindMin ();
		if ( tmp.revenue < goods_min_revenue.revenue )
		{
			goods_min_revenue = tmp;
			chain_index = i;
		}
	}

	if ( empty_arr )
		return false;
	id = goods_min_revenue.id;
	m_arr[chain_index].HeapExtractMin ();
	return true;
}
//----------------------------------------------------------------------------------------------------------------------
void CHolding::Merge ( int dstChain, int srcChain )
{
	if ( dstChain == srcChain )
		return;
	m_arr[dstChain].HeapMerge ( m_arr[srcChain] );
}
//----------------------------------------------------------------------------------------------------------------------
bool CHolding::Merge ()
{
	TGOODS min1, min2;
	int i, index1 = -1, index2 = -1;
	for ( i = 0; i < ARR_LENGTH; i++ )
	{
		if ( m_arr[i].m_max_index != -1 )
		{
			if ( index1 == -1 )
			{
				min1 = * m_arr[i].HeapFindMin ();
				index1 = i;
				continue;
			}
			min2 = * m_arr[i].HeapFindMin ();
			index2 = i;
			break;
		}
	}
	if ( index1 == -1 || index2 == -1 )
		return false;
	if ( min2.revenue < min1.revenue )
	{
		swap ( min1, min2 );
		swap ( index1, index2 );
	}

	for ( ; i < ARR_LENGTH; i++ )
	{
		if ( m_arr[i].m_max_index > -1 && m_arr[i].HeapFindMin ()->revenue < min1.revenue )
		{
			min2 = min1;
			index2 = index1;
			min1 = * m_arr[i].HeapFindMin ();
			index1 = i;
		}
		else if ( m_arr[i].m_max_index > -1 && m_arr[i].HeapFindMin()->revenue < min2.revenue )
		{
			min2 = * m_arr[i].HeapFindMin ();
			index2 = i;
		}
	}
	Merge ( index1, index2 );
	return true;
}
//======================================================================================================================
int main ( void )
{

	return 0;
}
