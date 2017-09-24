/*
// Copyright(c) 2002-2005 Anatoliy Kuznetsov(anatoliy_kuznetsov at yahoo.com)

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation 
files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
OTHER DEALINGS IN THE SOFTWARE.

For more information please visit:  http://bmagic.sourceforge.net

*/

#ifndef BMFUNC__H__INCLUDED__
#define BMFUNC__H__INCLUDED__

#include <memory.h>


#ifdef _MSC_VER
# pragma warning( disable: 4146 )
#endif

namespace bm
{


/*!
    @brief Structure with statistical information about bitset's memory 
            allocation details. 
    @ingroup bvector
*/
struct bv_statistics
{
    /// Number of bit blocks.
    unsigned bit_blocks; 
    /// Number of GAP blocks.
    unsigned gap_blocks;  
    /// Estimated maximum of memory required for serialization.
    unsigned max_serialize_mem;
    /// Memory used by bitvector including temp and service blocks
    unsigned  memory_used;
    /// Array of all GAP block lengths in the bvector.
    gap_word_t   gap_length[bm::set_total_blocks];
    /// GAP lengths used by bvector
    gap_word_t  gap_levels[bm::gap_levels];



    /// cound bit block
    void add_bit_block()
    {
        ++bit_blocks;
        unsigned mem_used = sizeof(bm::word_t) * bm::set_block_size;
        memory_used += mem_used;
        max_serialize_mem += mem_used;
    }

    /// count gap block
    void add_gap_block(unsigned capacity, unsigned length)
    {
        ++gap_blocks;
        unsigned mem_used = capacity * sizeof(gap_word_t);
        memory_used += mem_used;
        max_serialize_mem += length * sizeof(gap_word_t);
    }
};


/*! @defgroup gapfunc GAP functions
 *  GAP functions implement different opereations on GAP compressed blocks
 *  and serve as a minimal building blocks.
 *  @ingroup bmagic
 *
 */

/*! @defgroup bitfunc BIT functions
 *  Bit functions implement different opereations on bit blocks
 *  and serve as a minimal building blocks.
 *  @ingroup bmagic
 */


/*! @brief Default GAP lengths table.
    @ingroup gapfunc
*/
template<bool T> struct gap_len_table
{
    static const gap_word_t _len[bm::gap_levels];
};

template<bool T>
const gap_word_t gap_len_table<T>::_len[bm::gap_levels] = 
                { 128, 256, 512, bm::gap_max_buff_len }; 


/*! @brief Alternative GAP lengths table. 
    Good for for memory saver mode and very sparse bitsets.

    @ingroup gapfunc
*/
template<bool T> struct gap_len_table_min
{
    static const gap_word_t _len[bm::gap_levels];
};

template<bool T>
const gap_word_t gap_len_table_min<T>::_len[bm::gap_levels] = 
                                { 32, 96, 128, 512 }; 


//---------------------------------------------------------------------

/** Structure to aid in counting bits
    table contains count of bits in 0-255 diapason of numbers

   @ingroup bitfunc
*/
template<bool T> struct bit_count_table 
{
  static const unsigned char _count[256];
};

template<bool T>
const unsigned char bit_count_table<T>::_count[256] = {
    0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

//---------------------------------------------------------------------

/** Structure keeps all-left/right ON bits masks. 
    @ingroup bitfunc 
*/
template<bool T> struct block_set_table
{
    static const unsigned _left[32];
    static const unsigned _right[32];
};

template<bool T>
const unsigned block_set_table<T>::_left[32] = {
    0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff, 0x1ff, 0x3ff, 0x7ff,
    0xfff, 0x1fff, 0x3fff, 0x7fff, 0xffff, 0x1ffff, 0x3ffff, 0x7ffff,
    0xfffff, 0x1fffff, 0x3fffff, 0x7fffff, 0xffffff, 0x1ffffff, 0x3ffffff,
    0x7ffffff, 0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};

template<bool T>
const unsigned block_set_table<T>::_right[32] = {
    0xffffffff, 0xfffffffe, 0xfffffffc, 0xfffffff8, 0xfffffff0,
    0xffffffe0, 0xffffffc0, 0xffffff80, 0xffffff00, 0xfffffe00,
    0xfffffc00, 0xfffff800, 0xfffff000, 0xffffe000, 0xffffc000,
    0xffff8000, 0xffff0000, 0xfffe0000, 0xfffc0000, 0xfff80000,
    0xfff00000, 0xffe00000, 0xffc00000, 0xff800000, 0xff000000,
    0xfe000000, 0xfc000000, 0xf8000000, 0xf0000000, 0xe0000000,
    0xc0000000, 0x80000000
};



/** Structure keeps index of first ON bit for every byte.  
    @ingroup bitfunc 
*/
template<bool T> struct first_bit_table
{
    static const char _idx[256];
};

template<bool T>
const char first_bit_table<T>::_idx[256] = {
    -1,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,
    1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1, 0,2,0,1,0,3,0,
    1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,
    0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,
    2,0,1,0,3,0,1,0,2,0,1,0,7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,
    0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,
    1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,
    0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,
    3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0 
};


/*! 
	Define calculates number of 1 bits in 32-bit word.
    @ingroup bitfunc 
*/

#define BM_INCWORD_BITCOUNT(cnt, w) cnt += \
    bm::bit_count_table<true>::_count[(unsigned char)(w)] + \
    bm::bit_count_table<true>::_count[(unsigned char)((w) >> 8)] + \
    bm::bit_count_table<true>::_count[(unsigned char)((w) >> 16)] + \
    bm::bit_count_table<true>::_count[(unsigned char)((w) >> 24)];

/*!
    Returns bit count
    @ingroup bitfunc 
*/
BMFORCEINLINE
bm::id_t word_bitcount(bm::id_t w)
{
    return
    bm::bit_count_table<true>::_count[(unsigned char)(w)] + 
    bm::bit_count_table<true>::_count[(unsigned char)((w) >> 8)] + 
    bm::bit_count_table<true>::_count[(unsigned char)((w) >> 16)] + 
    bm::bit_count_table<true>::_count[(unsigned char)((w) >> 24)];
}

#ifdef BM64OPT
/*! 
	Function calculates number of 1 bits in 64-bit word.
    @ingroup bitfunc 
*/
inline bm::id_t word_bitcount64(bm::id64_t w)
{
    w = (w & 0x5555555555555555LU) + (w >> 1 & 0x5555555555555555LU);
    w = (w & 0x3333333333333333LU) + (w >> 2 & 0x3333333333333333LU);
    w = w + (w >> 4) & 0x0F0F0F0F0F0F0F0FLU;
    w = w + (w >> 8);
    w = w + (w >> 16);
    w = w + (w >> 32) & 0x0000007F;
    return (bm::id_t)w;
}
#endif


//---------------------------------------------------------------------


bm::id_t bit_block_any_range(const bm::word_t* block,
                             bm::word_t left,
                             bm::word_t right);

bm::id_t bit_block_calc_count_range(const bm::word_t* block,
                                    bm::word_t left,
                                    bm::word_t right);

//---------------------------------------------------------------------

/**
    Nomenclature of set operations
*/
enum set_operation
{
    set_AND         = 0,
    set_OR          = 1,
    set_SUB         = 2,
    set_XOR         = 3,
    set_ASSIGN      = 4,
    set_COUNT       = 5,
    set_COUNT_AND   = 6,
    set_COUNT_XOR   = 7,
    set_COUNT_OR    = 8,
    set_COUNT_SUB_AB= 9,
    set_COUNT_SUB_BA= 10,
    set_COUNT_A     = 11,
    set_COUNT_B     = 12,

    set_END
};

/// Returns true if set operation is constant (bitcount)
inline
bool is_const_set_operation(set_operation op)
{
    return (int(op) >= int(set_COUNT));
}

/**
    Bit operations enumeration.
*/
enum operation
{
    BM_AND = set_AND,
    BM_OR  = set_OR,
    BM_SUB = set_SUB,
    BM_XOR = set_XOR
};

/**
    Convert set operation to operation
*/
inline
bm::operation setop2op(bm::set_operation op)
{
    BM_ASSERT(op == set_AND || 
              op == set_OR  || 
              op == set_SUB || 
              op == set_XOR);
    return (bm::operation) op;
}

//---------------------------------------------------------------------

/** 
    Structure carries pointer on bit block with all bits 1
    @ingroup bitfunc 
*/
template<bool T> struct all_set
{
    struct all_set_block
    {
        bm::word_t _p[bm::set_block_size];

        all_set_block()
        {
            ::memset(_p, 0xFF, sizeof(_p));
        }
    };

    static all_set_block  _block;
};


template<bool T> typename all_set<T>::all_set_block all_set<T>::_block;

/// XOR swap two scalar variables
template<typename W> 
void xor_swap(W& x, W& y) 
{
    BM_ASSERT(&x != &y);
    x ^= y;
    y ^= x;
    x ^= y;
}


//---------------------------------------------------------------------

/*! 
   \brief Lexicographical comparison of two words as bit strings.
   Auxiliary implementation for testing and reference purposes.
   \param buf1 - First word.
   \param buf2 - Second word.
   \return  <0 - less, =0 - equal,  >0 - greater.

   @ingroup bitfunc 
*/
template<typename T> int wordcmp0(T w1, T w2)
{
    while (w1 != w2)
    {
        int res = (w1 & 1) - (w2 & 1);
        if (res != 0) return res;
        w1 >>= 1;
        w2 >>= 1;
    }
    return 0;
}


/*! 
   \brief Lexicographical comparison of two words as bit strings.
   Auxiliary implementation for testing and reference purposes.
   \param buf1 - First word.
   \param buf2 - Second word.
   \return  <0 - less, =0 - equal,  >0 - greater.

   @ingroup bitfunc 
*/
/*
template<typename T> int wordcmp(T w1, T w2)
{
    T diff = w1 ^ w2;
    return diff ? ((w1 & diff & (diff ^ (diff - 1)))? 1 : -1) : 0; 
}
*/

template<typename T> int wordcmp(T a, T b)
{
    T diff = a ^ b;
    return diff? ( (a & diff & -diff)? 1 : -1 ) : 0;
}


// Low bit extraction
// x & (x ^ (x-1))


/**
    Internal structure. Copyright information.
*/
template<bool T> struct _copyright
{
    static const char _p[];
};

template<bool T> const char _copyright<T>::_p[] = 
    "BitMagic Library. v.3.5.0 (c) 2002-2007 Anatoliy Kuznetsov.";


/*! 
   \brief Byte orders recognized by the library.
*/
enum ByteOrder
{
	BigEndian    = 0,
	LittleEndian = 1
};


/**
    Internal structure. Different global settings.
*/
template<bool T> struct globals
{
    struct bo
    {
        ByteOrder  _byte_order;

        bo()
        {
            unsigned x;
	        unsigned char *s = (unsigned char *)&x;
            s[0] = 1;
            s[1] = 2;
            s[2] = 3;
            s[3] = 4;

            if(x == 0x04030201) 
            {
                _byte_order = LittleEndian;
                return;
            }

            if(x == 0x01020304) 
            {
                _byte_order = BigEndian;
                return;
            }

            BM_ASSERT(0); // "Invalid Byte Order\n"
	        _byte_order = LittleEndian;
        }
    };

    static bo  _bo;

    static ByteOrder byte_order() { return _bo._byte_order; }

};

template<bool T> typename globals<T>::bo globals<T>::_bo;






/*
   \brief Binary search for the block where bit = pos located.
   \param buf - GAP buffer pointer.
   \param pos - index of the element.
   \param is_set - output. GAP value (0 or 1). 
   \return GAP index.
   @ingroup gapfunc
*/
template<typename T> 
unsigned gap_bfind(const T* buf, unsigned pos, unsigned* is_set)
{
    BM_ASSERT(pos < bm::gap_max_bits);
	*is_set = (*buf) & 1;

	register unsigned start = 1;
	register unsigned end = 1 + ((*buf) >> 3);

	while ( start != end )
	{
		unsigned curr = (start + end) >> 1;
		if ( buf[curr] < pos )
			start = curr + 1;
		else
			end = curr;
	}
	*is_set ^= ((start-1) & 1);
	return start; 
}


/*!
   \brief Tests if bit = pos is true.
   \param buf - GAP buffer pointer.
   \param pos - index of the element.
   \return true if position is in "1" gap
   @ingroup gapfunc
*/
template<typename T> unsigned gap_test(const T* buf, unsigned pos)
{
    BM_ASSERT(pos < bm::gap_max_bits);

	unsigned start = 1;
    unsigned end = 1 + ((*buf) >> 3);

    if (end - start < 10)
    {
        unsigned sv = *buf & 1;
        unsigned sv1= sv ^ 1;
        if (buf[1] >= pos) return sv;
        if (buf[2] >= pos) return sv1;
        if (buf[3] >= pos) return sv;
        if (buf[4] >= pos) return sv1;
        if (buf[5] >= pos) return sv;
        if (buf[6] >= pos) return sv1;
        if (buf[7] >= pos) return sv;
        if (buf[8] >= pos) return sv1;
        if (buf[9] >= pos) return sv;
        BM_ASSERT(0);
    }
    else
	while ( start != end )
	{
		unsigned curr = (start + end) >> 1;
		if ( buf[curr] < pos )
			start = curr + 1;
		else
			end = curr;
	}
	return ((*buf) & 1) ^ ((--start) & 1); 
}


/*! For each non-zero block executes supplied function.
*/
template<class T, class F> 
void for_each_nzblock(T*** root, unsigned size1, unsigned size2, 
                      F& f)
{
    unsigned block_idx = 0;
    for (unsigned i = 0; i < size1; ++i)
    {
        T** blk_blk = root[i];

        if (!blk_blk) 
        {
            f.on_empty_top(i);
            block_idx += size2;
            continue;
        }

        unsigned non_empty_top = 0;
        for (unsigned j = 0;j < size2; ++j, ++block_idx)
        {
            if (blk_blk[j]) 
            {
                f(blk_blk[j], block_idx);
                // re-check (blk_blk[j]): could be a mutation
                non_empty_top += (blk_blk[j] != 0);
            }
            else
            {
                f.on_empty_block(block_idx);
            }
        } // for j
        if (non_empty_top == 0)
        {
            f.on_empty_top(i);
        }
    }  // for i
}

/*! For each non-zero block executes supplied function-predicate.
    Function returns if function-predicate returns true
*/
template<class T, class F> 
bool for_each_nzblock_if(T*** root, unsigned size1, unsigned size2, F& f)
{
    unsigned block_idx = 0;
    for (unsigned i = 0; i < size1; ++i)
    {
        T** blk_blk = root[i];

        if (!blk_blk) 
        {
            block_idx += bm::set_array_size;
            continue;
        }

        for (unsigned j = 0;j < size2; ++j, ++block_idx)
        {
            if (blk_blk[j]) 
                if (f(blk_blk[j], block_idx)) return true;
        }
    }
    return false;
}

/*! For each block executes supplied function.
*/
template<class T, class F> 
void for_each_block(T*** root, unsigned size1, unsigned size2, F& f)
{
    unsigned block_idx = 0;

    for (unsigned i = 0; i < size1; ++i)
    {
        T** blk_blk = root[i];

        if (blk_blk)
        {
            for (unsigned j = 0;j < size2; ++j, ++block_idx)
            {
                f(blk_blk[j], block_idx);
            }
        }
        else
        {
            for (unsigned j = 0;j < size2; ++j, ++block_idx)
            {
                f(0, block_idx);
            }
        }
    }  
}



/*! Special BM optimized analog of STL for_each
*/
template<class T, class F> F bmfor_each(T first, T last, F f)
{
    do
    {
        f(*first);
        ++first;
    } while (first < last);
    return f;
}

/*! Computes SUM of all elements of the sequence
*/
template<class T> T sum_arr(T* first, T* last)
{
    T sum = 0;
    while (first < last)
    {
        sum += *first;
        ++first;
    }
    return sum;
}


/*! 
   \brief Calculates number of bits ON in GAP buffer.
   \param buf - GAP buffer pointer.
   \return Number of non-zero bits.
   @ingroup gapfunc
*/
template<typename T> unsigned gap_bit_count(const T* buf) 
{
    register const T* pcurr = buf;
    register const T* pend = pcurr + (*pcurr >> 3);

    register unsigned bits_counter = 0;
    ++pcurr;

    if (*buf & 1)
    {
        bits_counter += *pcurr + 1;
        ++pcurr;
    }
    ++pcurr;  // set GAP to 1

    while (pcurr <= pend)
    {
        bits_counter += *pcurr - *(pcurr-1);
        pcurr += 2; // jump to the next positive GAP
    } 

    return bits_counter;
}

/*!
   \brief Counts 1 bits in GAP buffer in the closed [left, right] diapason.
   \param buf - GAP buffer pointer.
   \param left - leftmost bit index to start from
   \param right- rightmost bit index
   \return Number of non-zero bits.
*/
template<typename T>
unsigned gap_bit_count_range(const T* buf, T left, T right)
{
    BM_ASSERT(left <= right);
    
    const T* pcurr = buf;
    const T* pend = pcurr + (*pcurr >> 3);
    
    unsigned bits_counter = 0;
    unsigned is_set;
    unsigned start_pos = gap_bfind(buf, left, &is_set);

    pcurr = buf + start_pos;
    if (right <= *pcurr) // we are in the target block right now
    {
        if (is_set)
            bits_counter = (right - left + 1);
        return bits_counter;
    }
    if (is_set)
        bits_counter += *pcurr - left + 1;

    unsigned prev_gap = *pcurr++;
    is_set ^= 1;
    while (right > *pcurr)
    {
        if (is_set)
            bits_counter += *pcurr - prev_gap;
        if (pcurr == pend) 
            return bits_counter;
        prev_gap = *pcurr++;
        is_set ^= 1;
    }
    if (is_set)
        bits_counter += right - prev_gap;

    return bits_counter;
}



/*! 
   \brief Lexicographical comparison of GAP buffers.
   \param buf1 - First GAP buffer pointer.
   \param buf2 - Second GAP buffer pointer.
   \return  <0 - less, =0 - equal,  >0 - greater.

   @ingroup gapfunc
*/
template<typename T> int gapcmp(const T* buf1, const T* buf2)
{
    const T* pcurr1 = buf1;
    const T* pend1 = pcurr1 + (*pcurr1 >> 3);
    unsigned bitval1 = *buf1 & 1;
    ++pcurr1;

    const T* pcurr2 = buf2;
    unsigned bitval2 = *buf2 & 1;
    ++pcurr2;

    while (pcurr1 <= pend1)
    {
        if (*pcurr1 == *pcurr2)
        {
            if (bitval1 != bitval2)
            {
                return (bitval1) ? 1 : -1;
            }
        }
        else
        {
            if (bitval1 == bitval2)
            {
                if (bitval1)
                {
                    return (*pcurr1 < *pcurr2) ? -1 : 1;
                }
                else
                {
                    return (*pcurr1 < *pcurr2) ? 1 : -1;
                }
            }
            else
            {
                return (bitval1) ? 1 : -1;
            }
        }

        ++pcurr1; ++pcurr2;

        bitval1 ^= 1;
        bitval2 ^= 1;
    }

    return 0;
}


/*!
   \brief Abstract operation for GAP buffers. 
          Receives functor F as a template argument
   \param dest - destination memory buffer.
   \param vect1 - operand 1 GAP encoded buffer.
   \param vect1_mask - XOR mask for starting bitflag for vector1 
   can be 0 or 1 (1 inverts the vector)
   \param vect2 - operand 2 GAP encoded buffer.
   \param vect2_mask - same as vect1_mask
   \param f - operation functor.
   \note Internal function.

   @ingroup gapfunc
*/
template<typename T, class F> 
void gap_buff_op(T*         BMRESTRICT dest, 
                 const T*   BMRESTRICT vect1,
                 unsigned   vect1_mask, 
                 const T*   BMRESTRICT vect2,
                 unsigned   vect2_mask, 
                 F f)
{
    register const T*  cur1 = vect1;
    register const T*  cur2 = vect2;

    unsigned bitval1 = (*cur1++ & 1) ^ vect1_mask;
    unsigned bitval2 = (*cur2++ & 1) ^ vect2_mask;
    
    unsigned bitval = f(bitval1, bitval2);
    unsigned bitval_prev = bitval;

    register T* res = dest; 
    *res = bitval;
    ++res;

    while (1)
    {
        bitval = f(bitval1, bitval2);

        // Check if GAP value changes and we need to 
        // start the next one.
        if (bitval != bitval_prev)
        {
            ++res;
            bitval_prev = bitval;
        }

        if (*cur1 < *cur2)
        {
            *res = *cur1;
            ++cur1;
            bitval1 ^= 1;
        }
        else // >=
        {
            *res = *cur2;
            if (*cur2 < *cur1)
            {
                bitval2 ^= 1;                
            }
            else  // equal
            {
                if (*cur2 == (bm::gap_max_bits - 1))
                {
                    break;
                }

                ++cur1;
                bitval1 ^= 1;
                bitval2 ^= 1;
            }
            ++cur2;
        }

    } // while

    unsigned dlen = (unsigned)(res - dest);
    *dest = (*dest & 7) + (dlen << 3);

}

/*!
   \brief Abstract distance test operation for GAP buffers. 
          Receives functor F as a template argument
   \param vect1 - operand 1 GAP encoded buffer.
   \param vect1_mask - XOR mask for starting bitflag for vector1 
                       can be 0 or 1 (1 inverts the vector)
   \param vect2 - operand 2 GAP encoded buffer.
   \param vect2_mask - same as vect1_mask
   \param f - operation functor.
   \note Internal function.
   \return non zero value if operation result returns any 1 bit 

   @ingroup gapfunc
*/
template<typename T, class F> 
unsigned gap_buff_any_op(const T*   BMRESTRICT vect1,
                         unsigned              vect1_mask, 
                         const T*   BMRESTRICT vect2,
                         unsigned              vect2_mask, 
                         F                     f)
{
    register const T*  cur1 = vect1;
    register const T*  cur2 = vect2;

    unsigned bitval1 = (*cur1++ & 1) ^ vect1_mask;
    unsigned bitval2 = (*cur2++ & 1) ^ vect2_mask;
    
    unsigned bitval = f(bitval1, bitval2);
    if (bitval)
        return bitval;
    unsigned bitval_prev = bitval;

    while (1)
    {
        bitval = f(bitval1, bitval2);
        if (bitval)
            return bitval;

        if (bitval != bitval_prev)
            bitval_prev = bitval;

        if (*cur1 < *cur2)
        {
            ++cur1;
            bitval1 ^= 1;
        }
        else // >=
        {
            if (*cur2 < *cur1)
            {
                bitval2 ^= 1;                
            }
            else  // equal
            {
                if (*cur2 == (bm::gap_max_bits - 1))
                {
                    break;
                }

                ++cur1;
                bitval1 ^= 1;
                bitval2 ^= 1;
            }
            ++cur2;
        }

    } // while

    return 0;
}



/*!
   \brief Abstract distance(similarity) operation for GAP buffers. 
          Receives functor F as a template argument
   \param vect1 - operand 1 GAP encoded buffer.
   \param vect2 - operand 2 GAP encoded buffer.
   \param f - operation functor.
   \note Internal function.

   @ingroup gapfunc
*/
/*
template<typename T, class F> 
unsigned gap_buff_count_op(const T*  vect1, const T*  vect2, F f)
{
    register const T* cur1 = vect1;
    register const T* cur2 = vect2;

    unsigned bitval1 = (*cur1++ & 1);
    unsigned bitval2 = (*cur2++ & 1);
    unsigned bitval = f(bitval1, bitval2);
    unsigned bitval_prev = bitval;

    unsigned count = 0;
    T res;
    T res_prev;

    while (1)
    {
        bitval = f(bitval1, bitval2);

        // Check if GAP value changes and we need to 
        // start the next one.
        if (bitval != bitval_prev)
        {
            bitval_prev = bitval;
        }

        if (*cur1 < *cur2)
        {
            if (bitval)
                count += *cur1; 
            ++cur1;
            bitval1 ^= 1;
        }
        else // >=
        {
            if (bitval)
                count += *cur2; 
            if (*cur2 < *cur1)
            {
                bitval2 ^= 1;                
            }
            else  // equal
            {
                if (*cur2 == (bm::gap_max_bits - 1))
                {
                    break;
                }

                ++cur1;
                bitval1 ^= 1;
                bitval2 ^= 1;
            }
            ++cur2;
        }

    } // while

    return count;
}
*/


/*!
   \brief Sets or clears bit in the GAP buffer.

   \param val - new bit value
   \param buf - GAP buffer.
   \param pos - Index of bit to set.
   \param is_set - (OUT) flag if bit was actually set.

   \return New GAP buffer length. 

   @ingroup gapfunc
*/
template<typename T> unsigned gap_set_value(unsigned val, 
                                            T* BMRESTRICT buf, 
                                            unsigned pos, 
                                            unsigned* BMRESTRICT is_set)
{
    BM_ASSERT(pos < bm::gap_max_bits);
    unsigned curr = gap_bfind(buf, pos, is_set);

    register T end = (*buf >> 3);
	if (*is_set == val)
	{
		*is_set = 0;
		return end;
	}
    *is_set = 1;

    register T* pcurr = buf + curr;
    register T* pprev = pcurr - 1;
    register T* pend = buf + end;

    // Special case, first bit GAP operation. There is no platform beside it.
    // initial flag must be inverted.
    if (pos == 0)
    {
        *buf ^= 1;
        if ( buf[1] ) // We need to insert a 1 bit platform here.
        {
            ::memmove(&buf[2], &buf[1], (end - 1) * sizeof(gap_word_t));
            buf[1] = 0;
            ++end;
        }
        else // Only 1 bit in the GAP. We need to delete the first GAP.
        {
            pprev = buf + 1;
            pcurr = pprev + 1;
            do
            {
                *pprev++ = *pcurr++;
            } while (pcurr < pend);
            --end;
        }
    }
    else if (curr > 1 && ((unsigned)(*pprev))+1 == pos) // Left border bit
	{
 	   ++(*pprev);
	   if (*pprev == *pcurr)  // Curr. GAP to be merged with prev.GAP.
	   {
            --end;
            if (pcurr != pend) // GAP merge: 2 GAPS to be deleted 
            {
                --end;
                ++pcurr;
                do
                {
                    *pprev++ = *pcurr++;
                } while (pcurr < pend);
            }
	   }    
    }
	else if (*pcurr == pos) // Rightmost bit in the GAP. Border goes left.
	{
		--(*pcurr);       
		if (pcurr == pend)
        {
		   ++end;
        }
	}
	else  // Worst case we need to split current block.
	{
        ::memmove(pcurr+2, pcurr,(end - curr + 1)*sizeof(T));
        *pcurr++ = pos - 1;
        *pcurr = pos;
		end+=2;
	}

    // Set correct length word.
    *buf = (*buf & 7) + (end << 3);

    buf[end] = bm::gap_max_bits - 1;
    return end;
}

//------------------------------------------------------------------------

/**
    \brief Searches for the next 1 bit in the GAP block
    \param buf - GAP buffer
    \param nbit - bit index to start checking from.
    \param prev - returns previously checked value

    @ingroup gapfunc
*/
template<typename T> int gap_find_in_block(const T* buf, 
                                           unsigned nbit, 
                                           bm::id_t* prev)
{
    BM_ASSERT(nbit < bm::gap_max_bits);

    unsigned bitval;
    unsigned gap_idx = bm::gap_bfind(buf, nbit, &bitval);

    if (bitval) // positive block.
    {
       return 1;
    }

    register unsigned val = buf[gap_idx] + 1;
    *prev += val - nbit;
 
    return (val != bm::gap_max_bits);  // no bug here.
}



/*! 
   \brief Sets bits to 1 in the bitblock.
   \param dest - Bitset buffer.
   \param bitpos - Offset of the start bit.
   \param bitcount - number of bits to set.

   @ingroup bitfunc
*/  
inline void or_bit_block(unsigned* dest, 
                         unsigned bitpos, 
                         unsigned bitcount)
{
    unsigned nbit  = unsigned(bitpos & bm::set_block_mask); 
    unsigned nword = unsigned(nbit >> bm::set_word_shift); 
    nbit &= bm::set_word_mask;

    bm::word_t* word = dest + nword;

    if (bitcount == 1)  // special case (only 1 bit to set)
    {
        *word |= unsigned(1 << nbit);
        return;
    }

    if (nbit) // starting position is not aligned
    {
        unsigned right_margin = nbit + bitcount;

        // here we checking if we setting bits only in the current
        // word. Example: 00111000000000000000000000000000 (32 bits word)

        if (right_margin < 32) 
        {
            unsigned mask = 
                block_set_table<true>::_right[nbit] & 
                block_set_table<true>::_left[right_margin-1];
            *word |= mask;
            return; // we are done
        }
        else
        {
            *word |= block_set_table<true>::_right[nbit];
            bitcount -= 32 - nbit;
        }
        ++word;
    }

    // now we are word aligned, lets find out how many words we 
    // can now turn ON using loop

    for ( ;bitcount >= 32; bitcount -= 32) 
    {
        *word++ = 0xffffffff;
    }

    if (bitcount) 
    {
        *word |= block_set_table<true>::_left[bitcount-1];
    }
}


/*! 
   \brief SUB (AND NOT) bit interval to 1 in the bitblock.
   \param dest - Bitset buffer.
   \param bitpos - Offset of the start bit.
   \param bitcount - number of bits to set.

   @ingroup bitfunc
*/  
inline void sub_bit_block(unsigned* dest, 
                          unsigned bitpos, 
                          unsigned bitcount)
{
    unsigned nbit  = unsigned(bitpos & bm::set_block_mask); 
    unsigned nword = unsigned(nbit >> bm::set_word_shift); 
    nbit &= bm::set_word_mask;

    bm::word_t* word = dest + nword;

    if (bitcount == 1)  // special case (only 1 bit to set)
    {
        *word &= ~unsigned(1 << nbit);
        return;
    }

    if (nbit) // starting position is not aligned
    {
        unsigned right_margin = nbit + bitcount;

        // here we checking if we setting bits only in the current
        // word. Example: 00111000000000000000000000000000 (32 bits word)

        if (right_margin < 32) 
        {
            unsigned mask = 
                block_set_table<true>::_right[nbit] & 
                block_set_table<true>::_left[right_margin-1];
            *word &= ~mask;
            return; // we are done
        }
        else
        {
            *word &= ~block_set_table<true>::_right[nbit];
            bitcount -= 32 - nbit;
        }
        ++word;
    }

    // now we are word aligned, lets find out how many words we 
    // can now turn ON using loop

    for ( ;bitcount >= 32; bitcount -= 32) 
    {
        *word++ = 0;
    }

    if (bitcount) 
    {
        *word &= ~block_set_table<true>::_left[bitcount-1];
    }
}


/*! 
   \brief XOR bit interval to 1 in the bitblock.
   \param dest - Bitset buffer.
   \param bitpos - Offset of the start bit.
   \param bitcount - number of bits to set.

   @ingroup bitfunc
*/  
inline void xor_bit_block(unsigned* dest, 
                          unsigned bitpos, 
                          unsigned bitcount)
{
    unsigned nbit  = unsigned(bitpos & bm::set_block_mask); 
    unsigned nword = unsigned(nbit >> bm::set_word_shift); 
    nbit &= bm::set_word_mask;

    bm::word_t* word = dest + nword;

    if (bitcount == 1)  // special case (only 1 bit to set)
    {
        *word ^= unsigned(1 << nbit);
        return;
    }

    if (nbit) // starting position is not aligned
    {
        unsigned right_margin = nbit + bitcount;

        // here we checking if we setting bits only in the current
        // word. Example: 00111000000000000000000000000000 (32 bits word)

        if (right_margin < 32) 
        {
            unsigned mask = 
                block_set_table<true>::_right[nbit] & 
                block_set_table<true>::_left[right_margin-1];
            *word ^= mask;
            return; // we are done
        }
        else
        {
            *word ^= block_set_table<true>::_right[nbit];
            bitcount -= 32 - nbit;
        }
        ++word;
    }

    // now we are word aligned, lets find out how many words we 
    // can now turn ON using loop

    for ( ;bitcount >= 32; bitcount -= 32) 
    {
        *word++ ^= 0xffffffff;
    }

    if (bitcount) 
    {
        *word ^= block_set_table<true>::_left[bitcount-1];
    }
}


/*!
   \brief SUB (AND NOT) GAP block to bitblock.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc
*/
template<typename T> 
void gap_sub_to_bitset(unsigned* dest, const T*  buf)
{
    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    if (*buf & 1)  // Starts with 1
    {
        sub_bit_block(dest, 0, *pcurr + 1);
        ++pcurr;
    }
    ++pcurr; // now we are in GAP "1" again

    while (pcurr <= pend)
    {
        unsigned bitpos = *(pcurr-1) + 1;
        BM_ASSERT(*pcurr > *(pcurr-1));
        unsigned gap_len = *pcurr - *(pcurr-1);
        sub_bit_block(dest, bitpos, gap_len);
        pcurr += 2;
    }
}


/*!
   \brief XOR GAP block to bitblock.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc
*/
template<typename T> 
void gap_xor_to_bitset(unsigned* dest, const T*  buf)
{
    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    if (*buf & 1)  // Starts with 1
    {
        xor_bit_block(dest, 0, *pcurr + 1);
        ++pcurr;
    }
    ++pcurr; // now we are in GAP "1" again

    while (pcurr <= pend)
    {
        unsigned bitpos = *(pcurr-1) + 1;
        BM_ASSERT(*pcurr > *(pcurr-1));
        unsigned gap_len = *pcurr - *(pcurr-1);
        xor_bit_block(dest, bitpos, gap_len);
        pcurr += 2;
    }
}


/*!
   \brief Adds(OR) GAP block to bitblock.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc
*/
template<typename T> 
void gap_add_to_bitset(unsigned* dest, const T*  buf)
{
    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    if (*buf & 1)  // Starts with 1
    {
        or_bit_block(dest, 0, *pcurr + 1);
        ++pcurr;
    }
    ++pcurr; // now we are in GAP "1" again

    while (pcurr <= pend)
    {
        unsigned bitpos = *(pcurr-1) + 1;
        BM_ASSERT(*pcurr > *(pcurr-1));
        unsigned gap_len = *pcurr - *(pcurr-1);
        or_bit_block(dest, bitpos, gap_len);
        pcurr += 2;
    }
}


/*!
   \brief ANDs GAP block to bitblock.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc
*/
template<typename T> 
void gap_and_to_bitset(unsigned* dest, const T*  buf)
{
    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    if (! (*buf & 1) )  // Starts with 0 
    {
        // Instead of AND we can SUB 0 gaps here 
        sub_bit_block(dest, 0, *pcurr + 1);
        ++pcurr;
    }
    ++pcurr; // now we are in GAP "0" again

    while (pcurr <= pend)
    {
        unsigned bitpos = *(pcurr-1) + 1;
        BM_ASSERT(*pcurr > *(pcurr-1));
        unsigned gap_len = *pcurr - *(pcurr-1);
        sub_bit_block(dest, bitpos, gap_len);
        pcurr += 2;
    }
}


/*!
   \brief Compute bitcount of bit block AND masked by GAP block.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc bitfunc
*/
template<typename T> 
bm::id_t gap_bitset_and_count(const unsigned* block, const T*  buf)
{
    BM_ASSERT(block);

    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    bm::id_t count = 0;

    if (*buf & 1)  // Starts with 1
    {
        count += bit_block_calc_count_range(block, 0, *pcurr);
        ++pcurr;
    }
    ++pcurr; // now we are in GAP "1" again

    while (pcurr <= pend)
    {
        bm::id_t c = bit_block_calc_count_range(block, *(pcurr-1)+1, *pcurr);

        count += c;
        pcurr += 2;
    }
    return count;
}


/*!
   \brief Bitcount test of bit block AND masked by GAP block.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc bitfunc
*/
template<typename T> 
bm::id_t gap_bitset_and_any(const unsigned* block, const T*  buf)
{
    BM_ASSERT(block);

    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    bm::id_t count = 0;
    if (*buf & 1)  // Starts with 1
    {
        count += bit_block_any_range(block, 0, *pcurr);
        if (count)
            return count;
        ++pcurr;
    }
    ++pcurr; // now we are in GAP "1" again

    while (pcurr <= pend)
    {
        bm::id_t c = bit_block_any_range(block, *(pcurr-1)+1, *pcurr);
        count += c;
        if (count)
            break;
        pcurr += 2;
    }
    return count;
}



/*!
   \brief Compute bitcount of bit block SUB masked by GAP block.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc bitfunc
*/
template<typename T> 
bm::id_t gap_bitset_sub_count(const unsigned* block, const T*  buf)
{
    BM_ASSERT(block);

    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    bm::id_t count = 0;

    if (!(*buf & 1))  // Starts with 0
    {
        count += bit_block_calc_count_range(block, 0, *pcurr);
        ++pcurr;
    }
    ++pcurr; // now we are in GAP "0" again

    for (;pcurr <= pend; pcurr+=2)
    {
        count += bit_block_calc_count_range(block, *(pcurr-1)+1, *pcurr);
    }
    return count;
}


/*!
   \brief Compute bitcount test of bit block SUB masked by GAP block.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc bitfunc
*/
template<typename T> 
bm::id_t gap_bitset_sub_any(const unsigned* block, const T*  buf)
{
    BM_ASSERT(block);

    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    bm::id_t count = 0;

    if (!(*buf & 1))  // Starts with 0
    {
        count += bit_block_any_range(block, 0, *pcurr);
        if (count)
            return count;
        ++pcurr;
    }
    ++pcurr; // now we are in GAP "0" again

    for (;pcurr <= pend; pcurr+=2)
    {
        count += bit_block_any_range(block, *(pcurr-1)+1, *pcurr);
        if (count)
            return count;
    }
    return count;
}



/*!
   \brief Compute bitcount of bit block XOR masked by GAP block.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc bitfunc
*/
template<typename T> 
bm::id_t gap_bitset_xor_count(const unsigned* block, const T*  buf)
{
    BM_ASSERT(block);

    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    unsigned bitval = *buf & 1;
    
    register bm::id_t count = bit_block_calc_count_range(block, 0, *pcurr);
    if (bitval)
    {
        count = *pcurr + 1 - count;
    }
    
    for (bitval^=1, ++pcurr; pcurr <= pend; bitval^=1, ++pcurr)
    {
        T prev = *(pcurr-1)+1;
        bm::id_t c = bit_block_calc_count_range(block, prev, *pcurr);
        
        if (bitval) // 1 gap; means Result = Total_Bits - BitCount;
        {
            c = (*pcurr - prev + 1) - c;
        }
        
        count += c;
    }
    return count;
}

/*!
   \brief Compute bitcount test of bit block XOR masked by GAP block.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc bitfunc
*/
template<typename T> 
bm::id_t gap_bitset_xor_any(const unsigned* block, const T*  buf)
{
    BM_ASSERT(block);

    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    unsigned bitval = *buf & 1;
    
    register bm::id_t count = bit_block_any_range(block, 0, *pcurr);
    if (bitval)
    {
        count = *pcurr + 1 - count;
    }
    
    for (bitval^=1, ++pcurr; pcurr <= pend; bitval^=1, ++pcurr)
    {
        T prev = *(pcurr-1)+1;
        bm::id_t c = bit_block_any_range(block, prev, *pcurr);
        
        if (bitval) // 1 gap; means Result = Total_Bits - BitCount;
        {
            c = (*pcurr - prev + 1) - c;
        }
        
        count += c;
        if (count)
            return count;
    }
    return count;
}



/*!
   \brief Compute bitcount of bit block OR masked by GAP block.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc bitfunc
*/
template<typename T> 
bm::id_t gap_bitset_or_count(const unsigned* block, const T*  buf)
{
    BM_ASSERT(block);

    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    unsigned bitval = *buf & 1;
    
    register bm::id_t count;
    if (bitval)
    {
        count = *pcurr + 1;
    } 
    else
    {
        count = bit_block_calc_count_range(block, 0, *pcurr);
    }
    
    for (bitval^=1, ++pcurr; pcurr <= pend; bitval^=1, ++pcurr)
    {
        T prev = *(pcurr-1)+1;
        bm::id_t c;
        
        if (bitval)
        {
            c = (*pcurr - prev + 1);
        }
        else
        {
            c = bit_block_calc_count_range(block, prev, *pcurr);
        }
        
        count += c;
    }
    return count;
}

/*!
   \brief Compute bitcount test of bit block OR masked by GAP block.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc bitfunc
*/
template<typename T> 
bm::id_t gap_bitset_or_any(const unsigned* block, const T*  buf)
{
    BM_ASSERT(block);

    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    unsigned bitval = *buf & 1;
    
    register bm::id_t count;
    if (bitval)
    {
        count = *pcurr + 1;
    } 
    else
    {
        count = bit_block_any_range(block, 0, *pcurr);
    }
    
    for (bitval^=1, ++pcurr; pcurr <= pend; bitval^=1, ++pcurr)
    {
        T prev = *(pcurr-1)+1;
        bm::id_t c;
        
        if (bitval)
        {
            c = (*pcurr - prev + 1);
        }
        else
        {
            c = bit_block_any_range(block, prev, *pcurr);
        }        
        count += c;
        if (count)
            return count;
    }
    return count;
}



/*!
   \brief Bitblock memset operation. 

   \param dst - destination block.
   \param value - value to set.

   @ingroup bitfunc
*/
inline 
void bit_block_set(bm::word_t* BMRESTRICT dst, bm::word_t value)
{
//#ifdef BMVECTOPT
//    VECT_SET_BLOCK(dst, dst + bm::set_block_size, value);
//#else
    ::memset(dst, value, bm::set_block_size * sizeof(bm::word_t));
//#endif
}


/*!
   \brief GAP block to bitblock conversion.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.

   @ingroup gapfunc
*/
template<typename T> 
void gap_convert_to_bitset(unsigned* dest, const T*  buf)
{
    bit_block_set(dest, 0);
    gap_add_to_bitset(dest, buf);
}


/*!
   \brief GAP block to bitblock conversion.
   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.
   \param dest_size - length of the destination buffer.

   @ingroup gapfunc
*/
template<typename T> 
void gap_convert_to_bitset(unsigned* dest, const T*  buf,  unsigned  dest_len)
{
   ::memset(dest, 0, dest_len * sizeof(unsigned));
    gap_add_to_bitset(dest, buf);
}



/*!
   \brief Smart GAP block to bitblock conversion.

    Checks if GAP block is ALL-ZERO or ALL-ON. In those cases returns 
    pointer on special static bitblocks.

   \param dest - bitblock buffer pointer.
   \param buf  - GAP buffer pointer.
   \param set_max - max possible bitset length

   @ingroup gapfunc
*/
template<typename T> 
        unsigned* gap_convert_to_bitset_smart(unsigned* dest,
                                              const T* buf, 
                                              id_t set_max)
{
    if (buf[1] == set_max - 1)
    {
        return (buf[0] & 1) ? FULL_BLOCK_ADDR : 0;
    }

    gap_convert_to_bitset(dest, buf);
    return dest;
}


/*!
   \brief Calculates sum of all words in GAP block. (For debugging purposes)
   \note For debugging and testing ONLY.
   \param buf - GAP buffer pointer.
   \return Sum of all words.

   @ingroup gapfunc
*/
template<typename T> unsigned gap_control_sum(const T* buf)
{
    unsigned end = *buf >> 3;

    register const T* pcurr = buf;    
    register const T* pend = pcurr + (*pcurr >> 3);
    ++pcurr;

    if (*buf & 1)  // Starts with 1
    {
        ++pcurr;
    }
    ++pcurr; // now we are in GAP "1" again

    while (pcurr <= pend)
    {
        BM_ASSERT(*pcurr > *(pcurr-1));
        pcurr += 2;
    }
    return buf[end];

}


/*! 
   \brief Sets all bits to 0 or 1 (GAP)
   \param buf - GAP buffer pointer.
   \param set_max - max possible bitset length

   @ingroup gapfunc
*/
template<class T> void gap_set_all(T* buf, 
                                        unsigned set_max,
                                        unsigned value)
{
    BM_ASSERT(value == 0 || value == 1);
    *buf = (*buf & 6u) + (1u << 3) + value;
    *(++buf) = set_max - 1;
}


/*!
    \brief Init gap block so it has block in it (can be whole block)
    \param buf  - GAP buffer pointer.
    \param from - one block start
    \param to   - one block end
    \param value - (block value)1 or 0
    \param set_max - max possible bitset length
    
   @ingroup gapfunc
*/
template<class T> 
void gap_init_range_block(T*       buf,
                          unsigned from,
                          unsigned to,
                          unsigned value,
                          unsigned set_max)
{
    BM_ASSERT(value == 0 || value == 1);

    unsigned gap_len;
    if (from == 0)
    {
        if (to == set_max - 1)
        {
            gap_set_all(buf, set_max, value);
        }
        else
        {
            gap_len = 2;
            buf[1] = to;
            buf[2] = set_max - 1;
            buf[0] =  (*buf & 6u) + (gap_len << 3) + value;
        }
        return;
    }
    // from != 0

    value = !value;
    if (to == set_max - 1)
    {
        gap_len = 2;
        buf[1] = from - 1;
        buf[2] = set_max - 1;
    }
    else
    {
        gap_len = 3;
        buf[1] = from - 1;
        buf[2] = to;
        buf[3] = set_max - 1;
    }
    buf[0] =  (*buf & 6u) + (gap_len << 3) + value;
}


/*! 
   \brief Inverts all bits in the GAP buffer.
   \param buf - GAP buffer pointer.

   @ingroup gapfunc
*/
template<typename T> void gap_invert(T* buf)
{ 
    *buf ^= 1;
}

/*! 
   \brief Temporary inverts all bits in the GAP buffer.
   
   In this function const-ness of the buffer means nothing.
   Calling this function again restores the status of the buffer.

   \param buf - GAP buffer pointer. (Buffer IS changed) 

   @ingroup gapfunc
*/
/*
template<typename T> void gap_temp_invert(const T* buf)
{
    T* buftmp = const_cast<T*>(buf);
    *buftmp ^= 1;
}
*/

/*!
   \brief Checks if GAP block is all-zero.
   \param buf - GAP buffer pointer.
   \param set_max - max possible bitset length
   \returns true if all-zero.

   @ingroup gapfunc
*/
template<typename T> 
             bool gap_is_all_zero(const T* buf, unsigned set_max)
{
    return (((*buf & 1)==0)  && (*(++buf) == set_max - 1));
}

/*!
   \brief Checks if GAP block is all-one.
   \param buf - GAP buffer pointer.
   \param set_max - max possible bitset length
   \returns true if all-one.

   @ingroup gapfunc
*/
template<typename T> 
         bool gap_is_all_one(const T* buf, unsigned set_max)
{
    return ((*buf & 1)  && (*(++buf) == set_max - 1));
}

/*!
   \brief Returs GAP block length.
   \param buf - GAP buffer pointer.
   \returns GAP block length.

   @ingroup gapfunc
*/
template<typename T> unsigned gap_length(const T* buf)
{
    return (*buf >> 3) + 1;
}


/*!
   \brief Returs GAP block capacity.
   \param buf - GAP buffer pointer.
   \returns GAP block capacity.

   @ingroup gapfunc
*/
template<typename T> 
unsigned gap_capacity(const T* buf, const T* glevel_len)
{
    return glevel_len[(*buf >> 1) & 3];
}


/*!
   \brief Returs GAP block capacity limit.
   \param buf - GAP buffer pointer.
   \param glevel_len - GAP lengths table (gap_len_table)
   \returns GAP block limit.

   @ingroup gapfunc
*/
template<typename T> 
unsigned gap_limit(const T* buf, const T* glevel_len)
{
    return glevel_len[(*buf >> 1) & 3]-4;
}


/*!
   \brief Returs GAP blocks capacity level.
   \param buf - GAP buffer pointer.
   \returns GAP block capacity level.

   @ingroup gapfunc
*/
template<typename T> unsigned gap_level(const T* buf)
{
    return (*buf >> 1) & 3;
}


/*!
   \brief Sets GAP block capacity level.
   \param buf - GAP buffer pointer.
   \param level new GAP block capacity level.

   @ingroup gapfunc
*/
template<typename T> void set_gap_level(T* buf, 
                                        unsigned level)
{
    BM_ASSERT(level < bm::gap_levels);
    *buf = ((level & 3) << 1) | (*buf & 1) | (*buf & ~7); 
}


/*!
   \brief Calculates GAP block capacity level.
   \param len - GAP buffer length.
   \param glevel_len - GAP lengths table
   \return GAP block capacity level. 
            -1 if block does not fit any level.
   @ingroup gapfunc
*/
template<typename T>
inline int gap_calc_level(int len, const T* glevel_len)
{
    if (len <= (glevel_len[0]-4)) return 0;
    if (len <= (glevel_len[1]-4)) return 1;
    if (len <= (glevel_len[2]-4)) return 2;
    if (len <= (glevel_len[3]-4)) return 3;

    BM_ASSERT(bm::gap_levels == 4);
    return -1;
}

/*! @brief Returns number of free elements in GAP block array. 
    Difference between GAP block capacity on this level and actual GAP length.
    
    @param buf - GAP buffer pointer
    @parma glevel_len - GAP lengths table
    
    @return Number of free GAP elements
    @ingroup gapfunc
*/
template<typename T>
inline unsigned gap_free_elements(const T* buf, const T* glevel_len)
{
    unsigned len = gap_length(buf);
    unsigned capacity = gap_capacity(buf, glevel_len);
    return capacity - len;
}


/*! 
   \brief Lexicographical comparison of BIT buffers.
   \param buf1 - First buffer pointer.
   \param buf2 - Second buffer pointer.
   \param len - Buffer length in elements (T).
   \return  <0 - less, =0 - equal,  >0 - greater.

   @ingroup bitfunc 
*/
template<typename T> 
int bitcmp(const T* buf1, const T* buf2, unsigned len)
{
    BM_ASSERT(len);

    const T* pend1 = buf1 + len; 
    do
    {
        T w1 = *buf1++;
        T w2 = *buf2++;
        T diff = w1 ^ w2;
    
        if (diff)
        { 
            return (w1 & diff & -diff) ? 1 : -1;
        }

    } while (buf1 < pend1);

    return 0;
}


/*! 
   \brief Converts bit block to GAP. 
   \param dest - Destinatio GAP buffer.
   \param src - Source bitblock buffer.
   \param bits - Number of bits to convert.
   \param dest_len - length of the dest. buffer.
   \return  New ength of GAP block or 0 if conversion failed 
   (insufficicent space).

   @ingroup gapfunc
*/
template<typename T> 
    unsigned bit_convert_to_gap(T* BMRESTRICT dest, 
                                const unsigned* BMRESTRICT src, 
                                bm::id_t bits, 
                                unsigned dest_len)
{
    register T* BMRESTRICT pcurr = dest;
    T* BMRESTRICT end = dest + dest_len; 
    register int bitval = (*src) & 1;
    *pcurr |= bitval;

    ++pcurr;
    *pcurr = 0;
    register unsigned bit_idx = 0;
    register int bitval_next;

    unsigned val = *src;

    do
    {
        // We can fast pace if *src == 0 or *src = 0xffffffff

        while (val == 0 || val == 0xffffffff)
        {
           bitval_next = val ? 1 : 0;
           if (bitval != bitval_next)
           {
               *pcurr++ = bit_idx-1; 
               BM_ASSERT((pcurr-1) == (dest+1) || *(pcurr-1) > *(pcurr-2));
               if (pcurr >= end)
               {
                   return 0; // OUT of memory
               }
               bitval = bitval_next;
           }
           bit_idx += sizeof(*src) * 8;
           if (bit_idx >= bits)
           {
               goto complete;
           }
           ++src;
           val = *src;
        }


        register unsigned mask = 1;
        while (mask)
        {
            // Now plain bitshifting. Optimization wanted.

            bitval_next = val & mask ? 1 : 0;
            if (bitval != bitval_next)
            {
                *pcurr++ = bit_idx-1;
                BM_ASSERT((pcurr-1) == (dest+1) || *(pcurr-1) > *(pcurr-2));
                bitval = bitval_next;
                if (pcurr >= end)
                {
                    return 0; // OUT of memory
                }
            }

            mask <<= 1;
            ++bit_idx;

        } // while mask

        if (bit_idx >= bits)
        {
            goto complete;
        }

        ++src;
        val = *src;

    } while(1);

complete:
    *pcurr = bit_idx-1;
    unsigned len = (unsigned)(pcurr - dest);
    *dest = (*dest & 7) + (len << 3);
    return len;
}


/*!
   \brief Convert gap block into array of ints corresponding to 1 bits
   @ingroup gapfunc
*/
template<typename D, typename T>
D gap_convert_to_arr(D* BMRESTRICT       dest, 
                     const T* BMRESTRICT buf,
                     unsigned            dest_len)
{
    register const T* BMRESTRICT pcurr = buf;
    register const T* pend = pcurr + (*pcurr >> 3);

    D* BMRESTRICT dest_curr = dest;
    ++pcurr;

    if (*buf & 1)
    {
        if (unsigned(*pcurr + 1) >= dest_len)
            return 0; // insufficient space
        dest_len -= *pcurr;
        T to = *pcurr;
        for (T i = 0; ;++i) 
        {
            *dest_curr++ = i;
            if (i == to) break;
        }
        ++pcurr;
    }
    ++pcurr;  // set GAP to 1

    while (pcurr <= pend)
    {
        unsigned pending = *pcurr - *(pcurr-1);
        if (pending >= dest_len)
            return 0;
        dest_len -= pending;
        T from = *(pcurr-1)+1;
        T to = *pcurr;
        for (T i = from; ;++i) 
        {
            *dest_curr++ = i;
            if (i == to) break;
        }
        pcurr += 2; // jump to the next positive GAP
    }
    return (D) (dest_curr - dest);
}

/*!
    \brief Convert bit block into an array of ints corresponding to 1 bits
    @ingroup bitfunc 
*/
template<typename T> T bit_convert_to_arr(T* BMRESTRICT dest, 
                                          const unsigned* BMRESTRICT src, 
                                          bm::id_t bits, 
                                          unsigned dest_len)
{
    register T* BMRESTRICT pcurr = dest;
    T* BMRESTRICT end = dest + dest_len; 
    register unsigned bit_idx = 0;

    do
    {
        register unsigned val = *src;
        // We can skip if *src == 0 

        while (val == 0)
        {
            bit_idx += sizeof(*src) * 8;
            if (bit_idx >= bits)
            {
               return (T)(pcurr - dest);
            }
            val = *(++src);
        }

        if (pcurr + sizeof(val)*8 > end) // insufficient space
        {
            return 0;
        }

        for (int i = 0; i < 32; i+=4)
        {
            if (val & 1)
                *pcurr++ = bit_idx;
            val >>= 1; ++bit_idx;
            if (val & 1)
                *pcurr++ = bit_idx;
            val >>= 1; ++bit_idx;
            if (val & 1)
                *pcurr++ = bit_idx;
            val >>= 1; ++bit_idx;
            if (val & 1)
                *pcurr++ = bit_idx;
            val >>= 1; ++bit_idx;
        }
        if (bits <= bit_idx)
            break;

        val = *(++src);

    } while (1);

    return (T)(pcurr - dest);
}



/*! 
    @brief Bitcount for bit string
    
	Function calculates number of 1 bits in the given array of words.
    Make sure the addresses are aligned.

    @ingroup bitfunc 
*/
inline 
bm::id_t bit_block_calc_count(const bm::word_t* block, 
							  const bm::word_t* block_end)
{
    BM_ASSERT(block < block_end);
	bm::id_t count = 0;

#ifdef BM64OPT

    // 64-bit optimized algorithm.

    const bm::id64_t* b1 = (bm::id64_t*) block;
    const bm::id64_t* b2 = (bm::id64_t*) block_end;

    bm::id64_t  acc = *b1++;  // accumulator (sparse vectors optimization)

    do
    {
        bm::id64_t in = *b1++;
        bm::id64_t acc_prev = acc;
        acc |= in;

        if (acc_prev &= in)  // counting bits in accumulator
        {
            acc = (acc & 0x5555555555555555LU) + (acc >> 1 & 0x5555555555555555LU);
            acc = (acc & 0x3333333333333333LU) + (acc >> 2 & 0x3333333333333333LU);
            acc = acc + (acc >> 4) & 0x0F0F0F0F0F0F0F0FLU;
            acc = acc + (acc >> 8);
            acc = acc + (acc >> 16);
            acc = acc + (acc >> 32) & 0x0000007F;
            count += (unsigned)acc;

            acc = acc_prev;
        }
    } while (b1 < b2);
    count += word_bitcount64(acc);  // count-in remaining accumulator 

#else
    // For 32 bit code the fastest method is
    // to use bitcount table for each byte in the block.
    // As optimization for sparse bitsets used bits accumulator
    // to collect ON bits using bitwise OR. 
    bm::word_t  acc = *block++;
    do
    {
        bm::word_t in = *block++;
        bm::word_t acc_prev = acc;
        acc |= in;
        if (acc_prev &= in)  // accumulator miss: counting bits
        {
            BM_INCWORD_BITCOUNT(count, acc);
            acc = acc_prev;
        }
    } while (block < block_end);

    BM_INCWORD_BITCOUNT(count, acc); // count-in remaining accumulator 

#endif
	
    return count;
}

/*!
    Function calculates number of times when bit value changed 
    (1-0 or 0-1).
    
    For 001 result is 2
        010 - 3
        011 - 2
        111 - 1
    
    @ingroup bitfunc 
*/

inline 
bm::id_t bit_count_change(bm::word_t w)
{
    unsigned count = 1;
    w ^= (w >> 1);

    BM_INCWORD_BITCOUNT(count, w);
    count -= (w >> ((sizeof(w) * 8) - 1));
    return count;
}


/*!
    Function calculates number of times when bit value changed 
    (1-0 or 0-1) in the bit block.
        
    @ingroup bitfunc 
*/
inline 
bm::id_t bit_block_calc_count_change(const bm::word_t* block, 
							         const bm::word_t* block_end)
{
    BM_ASSERT(block < block_end);
    bm::id_t count = 1;
    
#ifdef BM64OPT

    // 64-bit optimized algorithm.

    const bm::id64_t* b1 = (bm::id64_t*) block;
    const bm::id64_t* b2 = (bm::id64_t*) block_end;

    bm::id64_t w, w0, w_prev, w_l;
    w = w0 = *b1;
    const int w_shift = sizeof(w) * 8 - 1;
    w ^= (w >> 1);
    count += word_bitcount64(w);
    count -= (w_prev = (w0 >> w_shift)); // negative value correction
    
    for (++b1 ;b1 < b2; ++b1)
    {
        w = w0 = *b1;
        ++count;
        
        if (!w)
        {
            count -= !w_prev;
            w_prev = 0;
        }
        else
        {
            w ^= (w >> 1);
            count += word_bitcount64(w);
            
            w_l = w0 & 1;
            count -= (w0 >> w_shift);  // negative value correction
            count -= !(w_prev ^ w_l);  // word border correction
            
            w_prev = (w0 >> w_shift);
        }
    } // for

#else
    
    bm::word_t  w, w0, w_prev, w_l; 
    
    w = w0 = *block;
    const int w_shift = sizeof(w) * 8 - 1;    
    w ^= (w >> 1);
    BM_INCWORD_BITCOUNT(count, w);
    count -= (w_prev = (w0 >> w_shift)); // negative value correction

    for (++block ;block < block_end; ++block)
    {
        w = w0 = *block;
        ++count;

        if (!w)
        {       
            count -= !w_prev;
            w_prev = 0;
        }
        else
        {
            w ^= (w >> 1);
            BM_INCWORD_BITCOUNT(count, w);
            
            w_l = w0 & 1;
            count -= (w0 >> w_shift);  // negative value correction
            count -= !(w_prev ^ w_l);  // word border correction
            
            w_prev = (w0 >> w_shift);
        }
    } // for
#endif
    return count;
}


/*!
	Function calculates number of 1 bits in the given array of words in
    the range between left anf right bits (borders included)
    Make sure the addresses are aligned.

    @ingroup bitfunc
*/
inline 
bm::id_t bit_block_calc_count_range(const bm::word_t* block,
                                    bm::word_t left,
                                    bm::word_t right)
{
    BM_ASSERT(left <= right);
    
	bm::id_t count = 0;

    unsigned nbit  = left; // unsigned(left & bm::set_block_mask);
    unsigned nword = unsigned(nbit >> bm::set_word_shift);
    nbit &= bm::set_word_mask;

    const bm::word_t* word = block + nword;

    if (left == right)  // special case (only 1 bit to check)
    {
        return (*word >> nbit) & 1;
    }
    unsigned acc;
    unsigned bitcount = right - left + 1;

    if (nbit) // starting position is not aligned
    {
        unsigned right_margin = nbit + (right - left);

        if (right_margin < 32)
        {
            unsigned mask =
                block_set_table<true>::_right[nbit] &
                block_set_table<true>::_left[right_margin];
            acc = *word & mask;
            
            BM_INCWORD_BITCOUNT(count, acc);
            return count;
        }
        else
        {
            acc = *word & block_set_table<true>::_right[nbit];
            BM_INCWORD_BITCOUNT(count, acc);
            bitcount -= 32 - nbit;
        }
        ++word;
    }

    // now when we are word aligned, we can count bits the usual way
    for ( ;bitcount >= 32; bitcount -= 32)
    {
        acc = *word++;
        BM_INCWORD_BITCOUNT(count, acc);
    }

    if (bitcount)  // we have a tail to count
    {
        acc = (*word) & block_set_table<true>::_left[bitcount-1];
        BM_INCWORD_BITCOUNT(count, acc);
    }

    return count;
}


/*!
	Function calculates if there is any number of 1 bits 
    in the given array of words in the range between left anf right bits 
    (borders included). Make sure the addresses are aligned.

    @ingroup bitfunc
*/
inline 
bm::id_t bit_block_any_range(const bm::word_t* block,
                             bm::word_t left,
                             bm::word_t right)
{
    BM_ASSERT(left <= right);
    
    unsigned nbit  = left; // unsigned(left & bm::set_block_mask);
    unsigned nword = unsigned(nbit >> bm::set_word_shift);
    nbit &= bm::set_word_mask;

    const bm::word_t* word = block + nword;

    if (left == right)  // special case (only 1 bit to check)
    {
        return (*word >> nbit) & 1;
    }
    unsigned acc;
    unsigned bitcount = right - left + 1;

    if (nbit) // starting position is not aligned
    {
        unsigned right_margin = nbit + (right - left);
        if (right_margin < 32)
        {
            unsigned mask =
                block_set_table<true>::_right[nbit] &
                block_set_table<true>::_left[right_margin];
            acc = *word & mask;
            return acc;
        }
        else
        {
            acc = *word & block_set_table<true>::_right[nbit];
            if (acc) 
                return acc;
            bitcount -= 32 - nbit;
        }
        ++word;
    }

    // now when we are word aligned, we can check bits the usual way
    for ( ;bitcount >= 32; bitcount -= 32)
    {
        acc = *word++;
        if (acc) 
            return acc;
    }

    if (bitcount)  // we have a tail to count
    {
        acc = (*word) & block_set_table<true>::_left[bitcount-1];
        if (acc) 
            return acc;
    }

    return 0;
}



// ----------------------------------------------------------------------

/*! Function inverts block of bits 
    @ingroup bitfunc 
*/
template<typename T> void bit_invert(T* start, T* end)
{
#ifdef BMVECTOPT
    VECT_INVERT_ARR(start, end);
#else
    do
    {
        start[0] = ~start[0];
        start[1] = ~start[1];
        start[2] = ~start[2];
        start[3] = ~start[3];
        start+=4;
    } while (start < end);
#endif
}

// ----------------------------------------------------------------------

/*! @brief Returns "true" if all bits in the block are 1
    @ingroup bitfunc 
*/
inline bool is_bits_one(const bm::wordop_t* start, 
                        const bm::wordop_t* end)
{
   do
   {
        bm::wordop_t tmp = 
            start[0] & start[1] & start[2] & start[3];
        if (tmp != bm::all_bits_mask) 
            return false;
        start += 4;
   } while (start < end);

   return true;
}

// ----------------------------------------------------------------------


/*! @brief Returns "true" if all bits in the block are 0
    @ingroup bitfunc 
*/
inline bool bit_is_all_zero(const bm::wordop_t* start, 
                            const bm::wordop_t* end)
{
   do
   {
        bm::wordop_t tmp = 
            start[0] | start[1] | start[2] | start[3];
       if (tmp) 
           return false;
       start += 4;
   } while (start < end);

   return true;
}

// ----------------------------------------------------------------------

// GAP blocks manipulation functions:

/*! \brief GAP and functor */
inline unsigned and_op(unsigned v1, unsigned v2)
{
    return v1 & v2;
}


/*! \brief GAP xor functor */
inline unsigned xor_op(unsigned v1, unsigned v2)
{
    return v1 ^ v2;
}


/*!
   \brief GAP AND operation.
   
   Function performs AND logical oparation on gap vectors.
   If possible function put the result into vect1 and returns this
   pointer.  Otherwise result is put into tmp_buf, which should be 
   twice of the vector size.

   \param vect1   - operand 1
   \param vect2   - operand 2
   \param tmp_buf - pointer on temporary buffer
   \return Result pointer (tmp_buf OR vect1)

   @ingroup gapfunc
*/
inline gap_word_t* gap_operation_and(const gap_word_t* BMRESTRICT vect1,
                                     const gap_word_t* BMRESTRICT vect2,
                                     gap_word_t*       BMRESTRICT tmp_buf)
{
    gap_buff_op(tmp_buf, vect1, 0, vect2, 0, and_op);

    return tmp_buf;
}

/*!
   \brief GAP AND operation test.
   
   Function performs AND logical oparation on gap vectors.
   If possible function put the result into vect1 and returns this
   pointer.  Otherwise result is put into tmp_buf, which should be 
   twice of the vector size.

   \param vect1   - operand 1
   \param vect2   - operand 2
   \return non zero value if operation returns any 1 bit

   @ingroup gapfunc
*/
inline unsigned gap_operation_any_and(const gap_word_t* BMRESTRICT vect1,
                                      const gap_word_t* BMRESTRICT vect2)
{
    return gap_buff_any_op(vect1, 0, vect2, 0, and_op);
}



/*!
   \brief GAP XOR operation.
   
   Function performs XOR logical oparation on gap vectors.
   If possible function put the result into vect1 and returns this
   pointer.  Otherwise result is put into tmp_buf, which should be 
   twice of the vector size.

   \param vect1   - operand 1
   \param vect2   - operand 2
   \param tmp_buf - pointer on temporary buffer
   \return Result pointer (tmp_buf)

   @ingroup gapfunc
*/
inline gap_word_t* gap_operation_xor(const gap_word_t*  BMRESTRICT vect1,
                                     const gap_word_t*  BMRESTRICT vect2,
                                     gap_word_t*        BMRESTRICT tmp_buf)
{
    gap_buff_op(tmp_buf, vect1, 0, vect2, 0, xor_op);
    return tmp_buf;
}


/*!
   \brief GAP XOR operation test.
   
   Function performs AND logical oparation on gap vectors.
   If possible function put the result into vect1 and returns this
   pointer.  Otherwise result is put into tmp_buf, which should be 
   twice of the vector size.

   \param vect1   - operand 1
   \param vect2   - operand 2
   \return non zero value if operation returns any 1 bit

   @ingroup gapfunc
*/
inline unsigned gap_operation_any_xor(const gap_word_t* BMRESTRICT vect1,
                                      const gap_word_t* BMRESTRICT vect2)
{
    return gap_buff_any_op(vect1, 0, vect2, 0, xor_op);
}



/*!
   \brief GAP OR operation.
   
   Function performs OR logical oparation on gap vectors.
   If possible function put the result into vect1 and returns this
   pointer.  Otherwise result is put into tmp_buf, which should be 
   twice of the vector size.

   \param vect1   - operand 1
   \param vect2   - operand 2
   \param tmp_buf - pointer on temporary buffer
   \return Result pointer (tmp_buf)

   @ingroup gapfunc
*/
inline gap_word_t* gap_operation_or(const gap_word_t*  BMRESTRICT vect1,
                                    const gap_word_t*  BMRESTRICT vect2,
                                    gap_word_t*        BMRESTRICT tmp_buf)
{
    gap_buff_op(tmp_buf, vect1, 1, vect2, 1, and_op);
    gap_invert(tmp_buf);
    return tmp_buf;
}




/*!
   \brief GAP SUB (AND NOT) operation.
   
   Function performs SUB logical oparation on gap vectors.
   If possible function put the result into vect1 and returns this
   pointer.  Otherwise result is put into tmp_buf, which should be 
   twice of the vector size.

   \param vect1   - operand 1
   \param vect2   - operand 2
   \param tmp_buf - pointer on temporary buffer
   \return Result pointer (tmp_buf)

   @ingroup gapfunc
*/
inline gap_word_t* gap_operation_sub(const gap_word_t*  BMRESTRICT vect1,
                                     const gap_word_t*  BMRESTRICT vect2,
                                     gap_word_t*        BMRESTRICT tmp_buf)
{
    gap_buff_op(tmp_buf, vect1, 0, vect2, 1, and_op);    
    return tmp_buf;
}


/*!
   \brief GAP SUB operation test.
   
   Function performs AND logical oparation on gap vectors.
   If possible function put the result into vect1 and returns this
   pointer.  Otherwise result is put into tmp_buf, which should be 
   twice of the vector size.

   \param vect1   - operand 1
   \param vect2   - operand 2
   \return non zero value if operation returns any 1 bit

   @ingroup gapfunc
*/
inline unsigned gap_operation_any_sub(const gap_word_t* BMRESTRICT vect1,
                                      const gap_word_t* BMRESTRICT vect2)
{
    return gap_buff_any_op(vect1, 0, vect2, 1, and_op);    
}


// ----------------------------------------------------------------------

// BIT blocks manipulation functions:


/*!
   \brief Bitblock copy operation. 

   \param dst - destination block.
   \param src - source block.

   @ingroup bitfunc
*/
inline 
void bit_block_copy(bm::word_t* BMRESTRICT dst, const bm::word_t* BMRESTRICT src)
{
#ifdef BMVECTOPT
    VECT_COPY_BLOCK(dst, src, src + bm::set_block_size);
#else
    ::memcpy(dst, src, bm::set_block_size * sizeof(bm::word_t));
#endif
}


/*!
   \brief Plain bitblock AND operation. 
   Function does not analyse availability of source and destination blocks.

   \param dst - destination block.
   \param src - source block.

   @ingroup bitfunc
*/
inline 
void bit_block_and(bm::word_t* BMRESTRICT dst, const bm::word_t* BMRESTRICT src)
{
#ifdef BMVECTOPT
    VECT_AND_ARR(dst, src, src + bm::set_block_size);
#else
    const bm::wordop_t* BMRESTRICT wrd_ptr = (wordop_t*)src;
    const bm::wordop_t* BMRESTRICT wrd_end = (wordop_t*)(src + bm::set_block_size);
    bm::wordop_t* BMRESTRICT dst_ptr = (wordop_t*)dst;

    do
    {
        dst_ptr[0] &= wrd_ptr[0];
        dst_ptr[1] &= wrd_ptr[1];
        dst_ptr[2] &= wrd_ptr[2];
        dst_ptr[3] &= wrd_ptr[3];

        dst_ptr+=4;
        wrd_ptr+=4;
    } while (wrd_ptr < wrd_end);
#endif
}


/*!
   \brief Function ANDs two bitblocks and computes the bitcount. 
   Function does not analyse availability of source blocks.

   \param src1     - first bit block
   \param src1_end - first bit block end
   \param src2     - second bit block

   @ingroup bitfunc
*/
inline 
unsigned bit_block_and_count(const bm::word_t* src1, 
                             const bm::word_t* src1_end,
                             const bm::word_t* src2)
{
    unsigned count;
#ifdef BMVECTOPT
    count = VECT_BITCOUNT_AND(src1, src1_end, src2);
#else  
    count = 0;  
    do
    {
        BM_INCWORD_BITCOUNT(count, src1[0] & src2[0]);
        BM_INCWORD_BITCOUNT(count, src1[1] & src2[1]);
        BM_INCWORD_BITCOUNT(count, src1[2] & src2[2]);
        BM_INCWORD_BITCOUNT(count, src1[3] & src2[3]);

        src1+=4;
        src2+=4;
    } while (src1 < src1_end);
#endif    
    return count;
}


/*!
   \brief Function ANDs two bitblocks and tests for any bit. 
   Function does not analyse availability of source blocks.

   \param src1     - first bit block
   \param src1_end - first bit block end
   \param src2     - second bit block

   @ingroup bitfunc
*/
inline 
unsigned bit_block_and_any(const bm::word_t* src1, 
                           const bm::word_t* src1_end,
                           const bm::word_t* src2)
{
    unsigned count = 0;
    do
    {
        count = (src1[0] & src2[0]) |
                (src1[1] & src2[1]) |
                (src1[2] & src2[2]) |
                (src1[3] & src2[3]);

        src1+=4; src2+=4;
    } while ((src1 < src1_end) && (count == 0));
    return count;
}




/*!
   \brief Function XORs two bitblocks and computes the bitcount. 
   Function does not analyse availability of source blocks.

   \param src1     - first bit block.
   \param src1_end - first bit block end
   \param src2     - second bit block.

   @ingroup bitfunc
*/
inline 
unsigned bit_block_xor_count(const bm::word_t* BMRESTRICT src1,
                             const bm::word_t* BMRESTRICT src1_end, 
                             const bm::word_t* BMRESTRICT src2)
{
    unsigned count;
#ifdef BMVECTOPT
    count = VECT_BITCOUNT_XOR(src1, src1_end, src2);
#else  
    count = 0;  
    do
    {
        BM_INCWORD_BITCOUNT(count, src1[0] ^ src2[0]);
        BM_INCWORD_BITCOUNT(count, src1[1] ^ src2[1]);
        BM_INCWORD_BITCOUNT(count, src1[2] ^ src2[2]);
        BM_INCWORD_BITCOUNT(count, src1[3] ^ src2[3]);

        src1+=4;
        src2+=4;
    } while (src1 < src1_end);
#endif
    return count;
}


/*!
   \brief Function XORs two bitblocks and and tests for any bit.
   Function does not analyse availability of source blocks.

   \param src1     - first bit block.
   \param src1_end - first bit block end
   \param src2     - second bit block.

   @ingroup bitfunc
*/
inline 
unsigned bit_block_xor_any(const bm::word_t* BMRESTRICT src1,
                             const bm::word_t* BMRESTRICT src1_end, 
                             const bm::word_t* BMRESTRICT src2)
{
    unsigned count = 0;
    do
    {
        count = (src1[0] ^ src2[0]) |
                (src1[1] ^ src2[1]) |
                (src1[2] ^ src2[2]) |
                (src1[3] ^ src2[3]);

        src1+=4; src2+=4;
    } while ((src1 < src1_end) && (count == 0));
    return count;
}




/*!
   \brief Function SUBs two bitblocks and computes the bitcount. 
   Function does not analyse availability of source blocks.

   \param src1     - first bit block.
   \param src1_end - first bit block end
   \param src2     - second bit block.

   @ingroup bitfunc
*/
inline 
unsigned bit_block_sub_count(const bm::word_t* BMRESTRICT src1, 
                             const bm::word_t* BMRESTRICT src1_end, 
                             const bm::word_t* BMRESTRICT src2)
{
    unsigned count;
#ifdef BMVECTOPT
    count = VECT_BITCOUNT_SUB(src1, src1_end, src2);
#else  
    count = 0;  
    do
    {
        BM_INCWORD_BITCOUNT(count, src1[0] & ~src2[0]);
        BM_INCWORD_BITCOUNT(count, src1[1] & ~src2[1]);
        BM_INCWORD_BITCOUNT(count, src1[2] & ~src2[2]);
        BM_INCWORD_BITCOUNT(count, src1[3] & ~src2[3]);

        src1+=4;
        src2+=4;
    } while (src1 < src1_end);
#endif
    return count;
}

/*!
   \brief Function SUBs two bitblocks and and tests for any bit.
   Function does not analyse availability of source blocks.

   \param src1     - first bit block.
   \param src1_end - first bit block end
   \param src2     - second bit block.

   @ingroup bitfunc
*/
inline 
unsigned bit_block_sub_any(const bm::word_t* BMRESTRICT src1,
                             const bm::word_t* BMRESTRICT src1_end, 
                             const bm::word_t* BMRESTRICT src2)
{
    unsigned count = 0;
    do
    {
        count = (src1[0] & ~src2[0]) |
                (src1[1] & ~src2[1]) |
                (src1[2] & ~src2[2]) |
                (src1[3] & ~src2[3]);

        src1+=4; src2+=4;
    } while ((src1 < src1_end) && (count == 0));
    return count;
}



/*!
   \brief Function ORs two bitblocks and computes the bitcount. 
   Function does not analyse availability of source blocks.

   \param src1     - first bit block
   \param src1_end - first block end
   \param src2     - second bit block.

   @ingroup bitfunc
*/
inline 
unsigned bit_block_or_count(const bm::word_t* src1, 
                            const bm::word_t* src1_end,
                            const bm::word_t* src2)
{
    unsigned count;
#ifdef BMVECTOPT
    count = VECT_BITCOUNT_OR(src1, src1_end, src2);
#else  
    count = 0;  
    do
    {
        BM_INCWORD_BITCOUNT(count, src1[0] | src2[0]);
        BM_INCWORD_BITCOUNT(count, src1[1] | src2[1]);
        BM_INCWORD_BITCOUNT(count, src1[2] | src2[2]);
        BM_INCWORD_BITCOUNT(count, src1[3] | src2[3]);

        src1+=4;
        src2+=4;
    } while (src1 < src1_end);
#endif
    return count;
}

/*!
   \brief Function ORs two bitblocks and and tests for any bit.
   Function does not analyse availability of source blocks.

   \param src1     - first bit block.
   \param src1_end - first bit block end
   \param src2     - second bit block.

   @ingroup bitfunc
*/
inline 
unsigned bit_block_or_any(const bm::word_t* BMRESTRICT src1,
                          const bm::word_t* BMRESTRICT src1_end, 
                          const bm::word_t* BMRESTRICT src2)
{
    unsigned count = 0;
    do
    {
        count = (src1[0] | src2[0]) |
                (src1[1] | src2[1]) |
                (src1[2] | src2[2]) |
                (src1[3] | src2[3]);

        src1+=4; src2+=4;
    } while ((src1 < src1_end) && (count == 0));
    return count;
}




/*!
   \brief bitblock AND operation. 

   \param dst - destination block.
   \param src - source block.

   \returns pointer on destination block. 
    If returned value  equal to src means that block mutation requested. 
    NULL is valid return value.

   @ingroup bitfunc
*/
inline bm::word_t* bit_operation_and(bm::word_t* BMRESTRICT dst, 
                                     const bm::word_t* BMRESTRICT src)
{
    BM_ASSERT(dst || src);

    bm::word_t* ret = dst;

    if (IS_VALID_ADDR(dst))  // The destination block already exists
    {

        if (!IS_VALID_ADDR(src))
        {
            if (IS_EMPTY_BLOCK(src))
            {
                //If the source block is zero 
                //just clean the destination block
                return 0;
            }
        }
        else
        {
            // Regular operation AND on the whole block.
            bit_block_and(dst, src);
        }
    }
    else // The destination block does not exist yet
    {
        if(!IS_VALID_ADDR(src))
        {
            if(IS_EMPTY_BLOCK(src)) 
            {
                // The source block is empty.
                // One argument empty - all result is empty.
                return 0;
            }
            // Here we have nothing to do.
            // Src block is all ON, dst block remains as it is
        }
        else // destination block does not exists, src - valid block
        {
            if (IS_FULL_BLOCK(dst))
            {
                return const_cast<bm::word_t*>(src);
            }
            // Nothng to do.
            // Dst block is all ZERO no combination required.
        }
    }

    return ret;
}


/*!
   \brief Performs bitblock AND operation and calculates bitcount of the result. 

   \param src1     - first bit block.
   \param src1_end - first bit block end
   \param src2     - second bit block.

   \returns bitcount value 

   @ingroup bitfunc
*/
inline 
bm::id_t bit_operation_and_count(const bm::word_t* BMRESTRICT src1,
                                 const bm::word_t* BMRESTRICT src1_end,
                                 const bm::word_t* BMRESTRICT src2)
{
    if (IS_EMPTY_BLOCK(src1) || IS_EMPTY_BLOCK(src2))
    {
        return 0;
    }
    return bit_block_and_count(src1, src1_end, src2);
}

/*!
   \brief Performs bitblock AND operation test. 

   \param src1     - first bit block.
   \param src1_end - first bit block end
   \param src2     - second bit block.

   \returns non zero if there is any value 

   @ingroup bitfunc
*/
inline 
bm::id_t bit_operation_and_any(const bm::word_t* BMRESTRICT src1,
                               const bm::word_t* BMRESTRICT src1_end,
                               const bm::word_t* BMRESTRICT src2)
{
    if (IS_EMPTY_BLOCK(src1) || IS_EMPTY_BLOCK(src2))
    {
        return 0;
    }
    return bit_block_and_any(src1, src1_end, src2);
}



/*!
   \brief Performs bitblock SUB operation and calculates bitcount of the result. 

   \param src1      - first bit block.
   \param src1_end  - first bit block end
   \param src2      - second bit block

   \returns bitcount value 

   @ingroup bitfunc
*/
inline 
bm::id_t bit_operation_sub_count(const bm::word_t* BMRESTRICT src1, 
                                 const bm::word_t* BMRESTRICT src1_end,
                                 const bm::word_t* BMRESTRICT src2)
{
    if (IS_EMPTY_BLOCK(src1))
    {
        return 0;
    }
    
    if (IS_EMPTY_BLOCK(src2)) // nothing to diff
    {
        return bit_block_calc_count(src1, src1_end);
    }
    return bit_block_sub_count(src1, src1_end, src2);
}


/*!
   \brief Performs inverted bitblock SUB operation and calculates 
          bitcount of the result. 

   \param src1      - first bit block.
   \param src1_end  - first bit block end
   \param src2      - second bit block

   \returns bitcount value 

   @ingroup bitfunc
*/
inline 
bm::id_t bit_operation_sub_count_inv(const bm::word_t* BMRESTRICT src1, 
                                     const bm::word_t* BMRESTRICT src1_end,
                                     const bm::word_t* BMRESTRICT src2)
{
    id64_t arr_size = src1_end - src1;
    return bit_operation_sub_count(src2, src2+arr_size, src1);
}


/*!
   \brief Performs bitblock test of SUB operation. 

   \param src1      - first bit block.
   \param src1_end  - first bit block end
   \param src2      - second bit block

   \returns non zero value if there are any bits

   @ingroup bitfunc
*/
inline 
bm::id_t bit_operation_sub_any(const bm::word_t* BMRESTRICT src1, 
                               const bm::word_t* BMRESTRICT src1_end,
                               const bm::word_t* BMRESTRICT src2)
{
    if (IS_EMPTY_BLOCK(src1))
    {
        return 0;
    }
    
    if (IS_EMPTY_BLOCK(src2)) // nothing to diff
    {
        return !bit_is_all_zero(src1, src1_end);
    }
    return bit_block_sub_any(src1, src1_end, src2);
}



/*!
   \brief Performs bitblock OR operation and calculates bitcount of the result. 

   \param src1     - first bit block.
   \param src1_end - first bit block end
   \param src2     - second bit block.

   \returns bitcount value 

   @ingroup bitfunc
*/
inline 
bm::id_t bit_operation_or_count(const bm::word_t* BMRESTRICT src1,
                                const bm::word_t* BMRESTRICT src1_end, 
                                const bm::word_t* BMRESTRICT src2)
{
    if (IS_EMPTY_BLOCK(src1))
    {
        if (!IS_EMPTY_BLOCK(src2))
            return bit_block_calc_count(src2, src2 + (src1_end - src1));
        else
            return 0; // both blocks are empty        
    }
    else
    {
        if (IS_EMPTY_BLOCK(src2))
            return bit_block_calc_count(src1, src1_end);
    }

    return bit_block_or_count(src1, src1_end, src2);
}

/*!
   \brief Performs bitblock OR operation test. 

   \param src1     - first bit block.
   \param src1_end - first bit block end
   \param src2     - second bit block.

   \returns non zero value if there are any bits

   @ingroup bitfunc
*/
inline 
bm::id_t bit_operation_or_any(const bm::word_t* BMRESTRICT src1,
                              const bm::word_t* BMRESTRICT src1_end, 
                              const bm::word_t* BMRESTRICT src2)
{
    if (IS_EMPTY_BLOCK(src1))
    {
        if (!IS_EMPTY_BLOCK(src2))
            return !bit_is_all_zero(src2, src2 + (src1_end - src1));
        else
            return 0; // both blocks are empty        
    }
    else
    {
        if (IS_EMPTY_BLOCK(src2))
            return !bit_is_all_zero(src1, src1_end);
    }

    return bit_block_or_any(src1, src1_end, src2);
}



/*!
   \brief Plain bitblock OR operation. 
   Function does not analyse availability of source and destination blocks.

   \param dst - destination block.
   \param src - source block.

   @ingroup bitfunc
*/
inline void bit_block_or(bm::word_t* BMRESTRICT dst, 
                         const bm::word_t* BMRESTRICT src)
{
#ifdef BMVECTOPT
    VECT_OR_ARR(dst, src, src + bm::set_block_size);
#else
    const bm::wordop_t* BMRESTRICT wrd_ptr = (wordop_t*)src;
    const bm::wordop_t* BMRESTRICT wrd_end = (wordop_t*)(src + set_block_size);
    bm::wordop_t* BMRESTRICT dst_ptr = (wordop_t*)dst;

    do
    {
        dst_ptr[0] |= wrd_ptr[0];
        dst_ptr[1] |= wrd_ptr[1];
        dst_ptr[2] |= wrd_ptr[2];
        dst_ptr[3] |= wrd_ptr[3];

        dst_ptr+=4;
        wrd_ptr+=4;

    } while (wrd_ptr < wrd_end);
#endif
}


/*!
   \brief Block OR operation. Makes analysis if block is 0 or FULL. 

   \param dst - destination block.
   \param src - source block.

   \returns pointer on destination block. 
    If returned value  equal to src means that block mutation requested. 
    NULL is valid return value.

   @ingroup bitfunc
*/
inline 
bm::word_t* bit_operation_or(bm::word_t* BMRESTRICT dst, 
                             const bm::word_t* BMRESTRICT src)
{
    BM_ASSERT(dst || src);

    bm::word_t* ret = dst;

    if (IS_VALID_ADDR(dst)) // The destination block already exists
    {
        if (!IS_VALID_ADDR(src))
        {
            if (IS_FULL_BLOCK(src))
            {
                // if the source block is all set 
                // just set the destination block
                ::memset(dst, 0xFF, bm::set_block_size * sizeof(bm::word_t));
            }
        }
        else
        {
            // Regular operation OR on the whole block
            bit_block_or(dst, src);
        }
    }
    else // The destination block does not exist yet
    {
        if (!IS_VALID_ADDR(src))
        {
            if (IS_FULL_BLOCK(src)) 
            {
                // The source block is all set, because dst does not exist
                // we can simply replace it.
                return const_cast<bm::word_t*>(FULL_BLOCK_ADDR);
            }
        }
        else
        {
            if (dst == 0)
            {
                // The only case when we have to allocate the new block:
                // Src is all zero and Dst does not exist
                return const_cast<bm::word_t*>(src);
            }
        }
    }
    return ret;
}

/*!
   \brief Plain bitblock SUB (AND NOT) operation. 
   Function does not analyse availability of source and destination blocks.

   \param dst - destination block.
   \param src - source block.

   @ingroup bitfunc
*/
inline 
void bit_block_sub(bm::word_t* BMRESTRICT dst, 
                   const bm::word_t* BMRESTRICT src)
{
#ifdef BMVECTOPT
    VECT_SUB_ARR(dst, src, src + bm::set_block_size);
#else
    const bm::wordop_t* BMRESTRICT wrd_ptr = (wordop_t*) src;
    const bm::wordop_t* BMRESTRICT wrd_end = 
                     (wordop_t*) (src + bm::set_block_size);
    bm::wordop_t* dst_ptr = (wordop_t*)dst;
    
    // Regular operation AND-NOT on the whole block.
    do
    {
        dst_ptr[0] &= ~wrd_ptr[0];
        dst_ptr[1] &= ~wrd_ptr[1];
        dst_ptr[2] &= ~wrd_ptr[2];
        dst_ptr[3] &= ~wrd_ptr[3];

        dst_ptr+=4;
        wrd_ptr+=4;
    } while (wrd_ptr < wrd_end);
#endif
    
}


/*!
   \brief bitblock SUB operation. 

   \param dst - destination block.
   \param src - source block.

   \returns pointer on destination block. 
    If returned value  equal to src means that block mutation requested. 
    NULL is valid return value.

   @ingroup bitfunc
*/
inline 
bm::word_t* bit_operation_sub(bm::word_t* BMRESTRICT dst, 
                              const bm::word_t* BMRESTRICT src)
{
    BM_ASSERT(dst || src);

    bm::word_t* ret = dst;
    if (IS_VALID_ADDR(dst))  //  The destination block already exists
    {
        if (!IS_VALID_ADDR(src))
        {
            if (IS_FULL_BLOCK(src))
            {
                // If the source block is all set
                // just clean the destination block
                return 0;
            }
        }
        else
        {
            bit_block_sub(dst, src);
        }
    }
    else // The destination block does not exist yet
    {
        if (!IS_VALID_ADDR(src))
        {
            if (IS_FULL_BLOCK(src)) 
            {
                // The source block is full
                return 0;
            }
        }
        else
        {
            if (IS_FULL_BLOCK(dst))
            {
                // The only case when we have to allocate the new block:
                // dst is all set and src exists
                return const_cast<bm::word_t*>(src);                  
            }
        }
    }
    return ret;
}


/*!
   \brief Plain bitblock XOR operation. 
   Function does not analyse availability of source and destination blocks.

   \param dst - destination block.
   \param src - source block.

   @ingroup bitfunc
*/
inline 
void bit_block_xor(bm::word_t* BMRESTRICT dst, 
                   const bm::word_t* BMRESTRICT src)
{
#ifdef BMVECTOPT
    VECT_XOR_ARR(dst, src, src + bm::set_block_size);
#else
    const bm::wordop_t* BMRESTRICT wrd_ptr = (wordop_t*) src;
    const bm::wordop_t* BMRESTRICT wrd_end = 
                            (wordop_t*) (src + bm::set_block_size);
    bm::wordop_t* BMRESTRICT dst_ptr = (wordop_t*)dst;

    // Regular XOR operation on the whole block.
    do
    {
        dst_ptr[0] ^= wrd_ptr[0];
        dst_ptr[1] ^= wrd_ptr[1];
        dst_ptr[2] ^= wrd_ptr[2];
        dst_ptr[3] ^= wrd_ptr[3];

        dst_ptr+=4;
        wrd_ptr+=4;
    } while (wrd_ptr < wrd_end);
#endif
    
}


/*!
   \brief bitblock XOR operation. 

   \param dst - destination block.
   \param src - source block.

   \returns pointer on destination block. 
    If returned value  equal to src means that block mutation requested. 
    NULL is valid return value.

   @ingroup bitfunc
*/
inline 
bm::word_t* bit_operation_xor(bm::word_t* BMRESTRICT dst, 
                              const bm::word_t* BMRESTRICT src)
{
    BM_ASSERT(dst || src);
    if (src == dst) return 0;  // XOR rule  

    bm::word_t* ret = dst;

    if (IS_VALID_ADDR(dst))  //  The destination block already exists
    {           
        if (!src) return dst;
        
        bit_block_xor(dst, src);
    }
    else // The destination block does not exist yet
    {
        if (!src) return dst;      // 1 xor 0 = 1

        // Here we have two cases:
        // if dest block is full or zero if zero we need to copy the source
        // otherwise XOR loop against 0xFF...
        //BM_ASSERT(dst == 0);
        return const_cast<bm::word_t*>(src);  // src is the final result               
    }
    return ret;
}

/*!
   \brief Performs bitblock XOR operation and calculates bitcount of the result. 

   \param src1 - first bit block.
   \param src2 - second bit block.

   \returns bitcount value 

   @ingroup bitfunc
*/
inline 
bm::id_t bit_operation_xor_count(const bm::word_t* BMRESTRICT src1,
                                 const bm::word_t* BMRESTRICT src1_end,
                                 const bm::word_t* BMRESTRICT src2)
{
    if (IS_EMPTY_BLOCK(src1) || IS_EMPTY_BLOCK(src2))
    {
        if (IS_EMPTY_BLOCK(src1) && IS_EMPTY_BLOCK(src2))
            return 0;
        const bm::word_t* block = IS_EMPTY_BLOCK(src1) ? src2 : src1;
        return bit_block_calc_count(block, block + (src1_end - src1));
    }
    return bit_block_xor_count(src1, src1_end, src2);
}

/*!
   \brief Performs bitblock XOR operation test. 

   \param src1 - first bit block.
   \param src2 - second bit block.

   \returns non zero value if there are bits

   @ingroup bitfunc
*/
inline 
bm::id_t bit_operation_xor_any(const bm::word_t* BMRESTRICT src1,
                               const bm::word_t* BMRESTRICT src1_end,
                               const bm::word_t* BMRESTRICT src2)
{
    if (IS_EMPTY_BLOCK(src1) || IS_EMPTY_BLOCK(src2))
    {
        if (IS_EMPTY_BLOCK(src1) && IS_EMPTY_BLOCK(src2))
            return 0;
        const bm::word_t* block = IS_EMPTY_BLOCK(src1) ? src2 : src1;
        return !bit_is_all_zero(block, block + (src1_end - src1));
    }
    return bit_block_xor_any(src1, src1_end, src2);
}


/**
    \brief Inspects bit block for zero words at the head and at the end 

    If there are no head-tail zeroes output parameters 
    head_idx and tail_idx are going to be [0, bm::set_block_size-1].
    If block is all-zero head_idx is -1

    \param data - bit block pointer
    \param head_idx - index of first non-zero word in block
    \param tail_idx - index of the last non-zero word in block

    @ingroup bitfunc
*/
inline
void bit_find_head_tail(const bm::word_t* data, 
                        unsigned*         head_idx,
                        unsigned*         tail_idx)
{
    BM_ASSERT(head_idx && tail_idx);
    *head_idx = (unsigned)-1;
    for (unsigned i = 0; i < bm::set_block_size; ++i) 
    {
        if (data[i]) 
        {
            *head_idx = i; 
            break;
        }
    } // for i
    if (*head_idx == (unsigned)-1) 
    {
        return; // all zero block
    }

    for (unsigned j = bm::set_block_size-1; j >= *head_idx; --j)
    {
        if (data[j]) 
        {
            *tail_idx = j;
            break;
        }
    } // for j

}



/**
    \brief Searches for the next 1 bit in the BIT block
    \param data - BIT buffer
    \param nbit - bit index to start checking from
    \param prev - returns previously checked value

    @ingroup bitfunc
*/
inline 
int bit_find_in_block(const bm::word_t* data, 
                      unsigned nbit, 
                      bm::id_t* prev)
{
    register bm::id_t p = *prev;
    int found = 0;

    for(;;)
    {
        unsigned nword  = nbit >> bm::set_word_shift;
        if (nword >= bm::set_block_size) break;

        register bm::word_t val = data[nword] >> (p & bm::set_word_mask);

        if (val)
        {
            while((val & 1) == 0)
            {
                val >>= 1;
                ++nbit;
                ++p;
            }
            ++found;
            break;
        }
        else
        {
           p    += (bm::set_word_mask + 1) - (nbit & bm::set_word_mask);
           nbit += (bm::set_word_mask + 1) - (nbit & bm::set_word_mask);
        }
    }
    *prev = p;
    return found;
}

/*!
   \brief Unpacks word into list of ON bit indexes
   \param w - value
   \param bits - pointer on the result array 
   \return number of bits in the list

   @ingroup bitfunc
*/
template<typename T,typename B> unsigned bit_list(T w, B* bits)
{
    // Note: 4-bit table method works slower than plain check approach
    unsigned octet = 0;
    B*       bp = bits;
    do
    {
        if (w & 1)   *bp++ = octet + 0;
        if (w & 2)   *bp++ = octet + 1;
        if (w & 4)   *bp++ = octet + 2;
        if (w & 8)   *bp++ = octet + 3;
        if (w & 16)  *bp++ = octet + 4;
        if (w & 32)  *bp++ = octet + 5;
        if (w & 64)  *bp++ = octet + 6;
        if (w & 128) *bp++ = octet + 7;

        w >>= 8;
        octet += 8;
    } while (w);
    return (unsigned)(bp - bits);
}


/*! @brief Calculates memory overhead for number of gap blocks sharing 
           the same memory allocation table (level lengths table).
    @ingroup gapfunc
*/
template<typename T> 
unsigned gap_overhead(const T* length, 
                      const T* length_end, 
                      const T* glevel_len)
{
    BM_ASSERT(length && length_end && glevel_len);

    unsigned overhead = 0;
    for (;length < length_end; ++length)
    {
        unsigned len = *length;
        int level = gap_calc_level(len, glevel_len);
        BM_ASSERT(level >= 0 && level < (int)bm::gap_levels);
        unsigned capacity = glevel_len[level];
        BM_ASSERT(capacity >= len);
        overhead += capacity - len;
    }
    return overhead;
}


/*! @brief Finds optimal gap blocks lengths.
    @param length - first element of GAP lengths array
    @param length_end - end of the GAP lengths array
    @param glevel_len - destination GAP lengths array
    @ingroup gapfunc
*/

template<typename T>
bool improve_gap_levels(const T* length,
                        const T* length_end,
                        T*       glevel_len)
{
    BM_ASSERT(length && length_end && glevel_len);

    size_t lsize = length_end - length;

    BM_ASSERT(lsize);
    
    gap_word_t max_len = 0;
    unsigned i;
    for (i = 0; i < lsize; ++i)
    {
        if (length[i] > max_len)
            max_len = length[i];
    }
    if (max_len < 5 || lsize <= bm::gap_levels)
    {
        glevel_len[0] = max_len + 4;
        for (i = 1; i < bm::gap_levels; ++i)
        {
            glevel_len[i] = bm::gap_max_buff_len;
        }
        return true;
    }

    glevel_len[bm::gap_levels-1] = max_len + 5;

    unsigned min_overhead = gap_overhead(length, length_end, glevel_len);
    bool is_improved = false;
    gap_word_t prev_value = glevel_len[bm::gap_levels-1];
    //
    // main problem solving loop
    //
    for (i = bm::gap_levels-2; ; --i)
    {
        unsigned opt_len = 0;
        unsigned j;
        bool imp_flag = false;
        gap_word_t gap_saved_value = glevel_len[i];
        for (j = 0; j < lsize; ++j)
        {
//            if (length[j]+4 > prev_value)
//                continue;
            
            glevel_len[i] = length[j]+4;
            unsigned ov = gap_overhead(length, length_end, glevel_len);
            if (ov <= min_overhead)
            {
                min_overhead = ov;                
                opt_len = length[j]+4;
                imp_flag = true;
            }
        }
        if (imp_flag) {
            glevel_len[i] = opt_len; // length[opt_idx]+4;
            is_improved = true;
        }
        else 
        {
            glevel_len[i] = gap_saved_value;
        }
        if (i == 0) 
            break;
        prev_value = glevel_len[i];
    }
    // 
    // Remove duplicates
    //

    T val = *glevel_len;
    T* gp = glevel_len;
    T* res = glevel_len;
    for (i = 0; i < bm::gap_levels; ++i)
    {
        if (val != *gp)
        {
            val = *gp;
            *++res = val;
        }
        ++gp;
    }

    // Filling the "unused" part with max. possible value
    while (++res < (glevel_len + bm::gap_levels)) 
    {
        *res = bm::gap_max_buff_len;
    }

    return is_improved;

}



/**
    Bit-block get adapter, takes bitblock and represents it as a 
    get_32() accessor function
    /internal
*/
class bitblock_get_adapter
{
public:
    bitblock_get_adapter(const bm::word_t* bit_block) : b_(bit_block) {}
    
    BMFORCEINLINE
    bm::word_t get_32() { return *b_++; }
private:
    const bm::word_t*  b_;
};


/**
    Bit-block store adapter, takes bitblock and saves results into it
    /internal
*/
class bitblock_store_adapter
{
public:
    bitblock_store_adapter(bm::word_t* bit_block) : b_(bit_block) {}
    BMFORCEINLINE
    void push_back(bm::word_t w) { *b_++ = w; }
private:
    bm::word_t* b_;
};

/**
    Bit-block sum adapter, takes values and sums it
    /internal
*/
class bitblock_sum_adapter
{
public:
    bitblock_sum_adapter() : sum_(0) {}
    BMFORCEINLINE
    void push_back(bm::word_t w) { this->sum_+= w; }
    /// Get accumulated sum
    bm::word_t sum() const { return this->sum_; }
private:
    bm::word_t sum_;
};

/**
    Adapter to get words from a range stream 
    (see range serialized bit-block)
    \internal
*/
template<class DEC> class decoder_range_adapter
{
public: 
    decoder_range_adapter(DEC& dec, unsigned from_idx, unsigned to_idx)
    : decoder_(dec),
      from_(from_idx),
      to_(to_idx),
      cnt_(0)
    {}

    bm::word_t get_32()
    {
        if (cnt_ < from_ || cnt_ > to_)
        {    
            ++cnt_; return 0;
        }
        ++cnt_;
        return decoder_.get_32();
    }

private:
    DEC&     decoder_;
    unsigned from_;
    unsigned to_;
    unsigned cnt_;
};


/*!
    Abstract recombination algorithm for two bit-blocks
    Bit blocks can come as dserialization decoders or bit-streams
*/
template<class It1, class It2, class BinaryOp, class Encoder>
void bit_recomb(It1& it1, It2& it2, 
                BinaryOp& op, 
                Encoder& enc, 
                unsigned block_size = bm::set_block_size)
{
    for (unsigned i = 0; i < block_size; ++i)
    {
        bm::word_t w1 = it1.get_32();
        bm::word_t w2 = it2.get_32();
        bm::word_t w = op(w1, w2);
        enc.push_back( w );
    } // for
}

/// Bit AND functor
template<typename W> struct bit_AND
{
    W operator()(W w1, W w2) { return w1 & w2; }
};

/// Bit OR functor
template<typename W> struct bit_OR
{
    W operator()(W w1, W w2) { return w1 | w2; }
};

/// Bit SUB functor
template<typename W> struct bit_SUB
{
    W operator()(W w1, W w2) { return w1 & ~w2; }
};

/// Bit XOR functor
template<typename W> struct bit_XOR
{
    W operator()(W w1, W w2) { return w1 ^ w2; }
};

/// Bit ASSIGN functor
template<typename W> struct bit_ASSIGN
{
    W operator()(W , W w2) { return w2; }
};

/// Bit COUNT functor
template<typename W> struct bit_COUNT
{
    W operator()(W w1, W w2) 
    {
        w1 = 0;
        BM_INCWORD_BITCOUNT(w1, w2);
        return w1;
    }
};

/// Bit COUNT AND functor
template<typename W> struct bit_COUNT_AND
{
    W operator()(W w1, W w2) 
    {
        W r = 0;
        BM_INCWORD_BITCOUNT(r, w1 & w2);
        return r;
    }
};

/// Bit COUNT XOR functor
template<typename W> struct bit_COUNT_XOR
{
    W operator()(W w1, W w2) 
    {
        W r = 0;
        BM_INCWORD_BITCOUNT(r, w1 ^ w2);
        return r;
    }
};

/// Bit COUNT OR functor
template<typename W> struct bit_COUNT_OR
{
    W operator()(W w1, W w2) 
    {
        W r = 0;
        BM_INCWORD_BITCOUNT(r, w1 | w2);
        return r;
    }
};


/// Bit COUNT SUB AB functor
template<typename W> struct bit_COUNT_SUB_AB
{
    W operator()(W w1, W w2) 
    {
        W r = 0;
        BM_INCWORD_BITCOUNT(r, w1 & (~w2));
        return r;
    }
};

/// Bit SUB BA functor
template<typename W> struct bit_COUNT_SUB_BA
{
    W operator()(W w1, W w2) 
    {
        W r = 0;
        BM_INCWORD_BITCOUNT(r, w2 & (~w1));
        return r;
    }
};

/// Bit COUNT A functor
template<typename W> struct bit_COUNT_A
{
    W operator()(W w1, W ) 
    {
        W r = 0;
        BM_INCWORD_BITCOUNT(r, w1);
        return r;
    }
};

/// Bit COUNT B functor
template<typename W> struct bit_COUNT_B
{
    W operator()(W , W w2) 
    {
        W r = 0;
        BM_INCWORD_BITCOUNT(r, w2);
        return r;
    }
};

typedef 
void (*gap_operation_to_bitset_func_type)(unsigned*, 
                                          const gap_word_t*);

typedef 
gap_word_t* (*gap_operation_func_type)(const gap_word_t*,
                                       const gap_word_t*,
                                       gap_word_t*);

typedef
bm::id_t (*bit_operation_count_func_type)(const bm::word_t* ,
                                          const bm::word_t*, 
                                          const bm::word_t*);


template<bool T> 
struct operation_functions
{
    static 
        gap_operation_to_bitset_func_type gap2bit_table_[bm::set_END];
    static 
        gap_operation_func_type gapop_table_[bm::set_END];
    static
        bit_operation_count_func_type bit_op_count_table_[bm::set_END];

    static
    gap_operation_to_bitset_func_type gap_op_to_bit(unsigned i)
    {
        return gap2bit_table_[i];
    }

    static
    gap_operation_func_type gap_operation(unsigned i)
    {
        return gapop_table_[i];
    }

    static
    bit_operation_count_func_type bit_operation_count(unsigned i)
    {
        return bit_op_count_table_[i];
    }
};

template<bool T>
gap_operation_to_bitset_func_type 
operation_functions<T>::gap2bit_table_[bm::set_END] = {
    &gap_and_to_bitset<bm::gap_word_t>,    // set_AND
    &gap_add_to_bitset<bm::gap_word_t>,    // set_OR
    &gap_sub_to_bitset<bm::gap_word_t>,    // set_SUB
    &gap_xor_to_bitset<bm::gap_word_t>,    // set_XOR
    0
};

template<bool T>
gap_operation_func_type 
operation_functions<T>::gapop_table_[bm::set_END] = {
    &gap_operation_and,    // set_AND
    &gap_operation_or,     // set_OR
    &gap_operation_sub,    // set_SUB
    &gap_operation_xor,    // set_XOR
    0
};


template<bool T>
bit_operation_count_func_type 
operation_functions<T>::bit_op_count_table_[bm::set_END] = {
    0,                            // set_AND
    0,                            // set_OR
    0,                            // set_SUB
    0,                            // set_XOR
    0,                            // set_ASSIGN
    0,                            // set_COUNT
    &bit_operation_and_count,     // set_COUNT_AND
    &bit_operation_xor_count,     // set_COUNT_XOR
    &bit_operation_or_count,      // set_COUNT_OR
    &bit_operation_sub_count,     // set_COUNT_SUB_AB
    &bit_operation_sub_count_inv, // set_COUNT_SUB_BA
    0,                            // set_COUNT_A
    0,                            // set_COUNT_B
};

} // namespace bm

#endif

