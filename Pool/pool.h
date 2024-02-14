/* Cameorn Hancock
02/13/2024
*/
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
	
	size_t		_num_blocks;
	size_t		_free_cells;
	size_t		_live_cells;
	
	void initialize_pool(size_t blockSize); 
	void expand();		// Add more blocks
	
	void link(element*);

	void initialize_pool() { // Initialize the pool
		_thepool = new element*[_blockSize];
	}
	
public:

	// Initialize the pool
	Pool(size_t blockSize = 5, bool traceFlag = true) :
	_blockSize (blockSize), _trace (traceFlag),
	_freehead (nullptr),
	_num_blocks (0), _free_cells (0), _live_cells (0)
	{
		initialize_pool();
		// Set elements in _thepool to nullptr
		for (int i = 0; i < blockSize; i++) {
			_thepool[i] = nullptr;
		}
		cout << "Initializing a pool with element size " << sizeof(_thepool) * 4 << " and block size " << blockSize << endl;
		profile();
	}
	// Delete pool
	~Pool() {
		cout << "deleting " << _num_blocks << " blocks" << endl;
		for (int i = 0; i < _num_blocks; i++) {
			delete[] _thepool[i];
		}
		delete[] _thepool;
	}
	void* allocate() {       // Get a pointer inside a pre-allocated block for a new object
		// If no free space, expand
		if (_freehead == nullptr) {
			expand();
		}
		// Allocate at pointer and move freehead foreward
		element* allocated = _freehead;
		_freehead = _freehead -> next;
		// Update live and free cells count
		_live_cells++;
		_free_cells--;
		cout << "Cell allocated at " << allocated << endl;
		return reinterpret_cast<void*>(allocated);
	}
	void deallocate(void* object_slot) { // Free an object's slot (push the address on the "free list")
		// Deallocate at pointer and add it to freed slots
		element* freed_slot = reinterpret_cast<element*>(object_slot);
		freed_slot -> next = _freehead;
		_freehead = freed_slot;
		// update live and free cells count
		_live_cells--;
		_free_cells++;
		cout << "Cell deallocated at " << freed_slot << endl;
	}
	void profile() const {   // Print the addresses on the free list.
		cout << endl << "Live Cells: " << _live_cells << ", Free Cells: " << _free_cells << endl;
		element* current = _freehead;
		cout << "Free List: " << endl;
		while (current != nullptr) {
			cout << reinterpret_cast<void*>(current) << endl;
			current = current ->next;
		}
		cout << endl;
	}
};

template <typename T>
void Pool<T>::expand() {		// Add more blocks
	cout << "expanding pool..." << endl;
	// Add new block
	element* newBlock = new element[_blockSize];
	cout << "Linking cells starting at " << &newBlock[0] << endl;
	// Link pointers in new block
	for (size_t i = 0; i < _blockSize - 1; i++) {
		link(&newBlock[i]);
	}
	link(&newBlock[_blockSize - 1]);
	_thepool[_num_blocks] = newBlock;
	// update _num_blocks and _free_cells
	_num_blocks++;
	_free_cells += _blockSize;
}
	
template <typename T>
void Pool<T>::link(element* object) { // Link new objects
	object -> next = _freehead;
	_freehead = object;
}

#endif