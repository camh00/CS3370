#ifndef POOL_H
#define POOL_H

#include <cstddef>
#include <new>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <new>

using std::size_t;
using std::cout;
using std::endl;
using std::byte;

template <typename T>
class Pool
{
	union element {
		byte object[sizeof(T)];
		element* next;
	};
	
	const size_t	_blockSize;
	const bool	_trace;
	
	element*	_freehead;
	element**	_thepool;
	
	int		_num_blocks;
	
	void expand();		// Add more blocks
	
	void link(element*);
	
public:

	Pool(size_t blockSize = 5, bool traceFlag = true) :
	_blockSize (blockSize), _trace (traceFlag),
	_freehead (nullptr),
	_thepool (nullptr),
	_num_blocks (0)
{}
   ~Pool();
   void* allocate();       // Get a pointer inside a pre-allocated block for a new object
   void deallocate(void*); // Free an object's slot (push the address on the "free list")
   void profile() const;   // Print the addresses on the free list.
};


// Add the mamber function definitions here
#endif