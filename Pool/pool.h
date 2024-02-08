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
	int		_num_cells;
	
	void expand();		// Add more blocks
	
	void link(element*);
	
public:

	Pool(size_t blockSize = 5, bool traceFlag = true) :
	_blockSize (blockSize), _trace (traceFlag),
	_freehead (nullptr),
	_thepool (new element*[blockSize]),
	_num_blocks (0), _num_cells (0)
	{
		for (int i = 0; i < blockSize; i++) {
			_thepool[i] = nullptr;
		}
		cout << "Initializing a pool with element size " << sizeof(T) << " and block size " << blockSize << endl;
	}
	~Pool() {
		for (int i = 0; i < _num_blocks; i++) {
			delete[] _thepool[i];
		}
		delete[] _thepool;
	}
	void* allocate() {       // Get a pointer inside a pre-allocated block for a new object
		if (!_freehead) {
			expand();
		}
		element* allocated = _freehead;
		_freehead = _freehead -> next;
		_num_cells++;
		return reinterpret_cast<void*>(allocated);
	}
	void deallocate(void* object_slot) { // Free an object's slot (push the address on the "free list")
		element* freed_slot = reinterpret_cast<element*>(object_slot);
		freed_slot -> next = _freehead;
		_freehead = freed_slot;
	}
	void profile() const {   // Print the addresses on the free list.
		cout << "Live Cells: " << _num_cells << ", Free Cells: " << (_num_blocks*_blockSize)-_num_cells << endl;
		element* current = _freehead;
		cout << "Free List: " << endl;
		while (current != nullptr) {
			cout << reinterpret_cast<void*>(current) << endl;
			current = current ->next;
		}
		cout << endl;
	}
};


// Add the mamber function definitions here

template <typename T>
void Pool<T>::expand() {		// Add more blocks
	cout << "expanding pool..." << endl;
	element* newBlock = new element[_blockSize];
	cout << "Linking cells starting at " << &newBlock[0] << endl;
	for (size_t i = 0; i < _blockSize - 1; i++) {
		link(&newBlock[i]);
	}
	link(&newBlock[_blockSize - 1]);
	_thepool[_num_blocks] = newBlock;
	_num_blocks++;
}
	
template <typename T>
void Pool<T>::link(element* object) {
	
	object -> next = _freehead;
	_freehead = object;
	cout << "Cell allocated at " << object << endl;
}

#endif