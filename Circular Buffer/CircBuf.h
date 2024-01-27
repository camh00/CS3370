//
#ifndef CIRCBUF_H
#define CIRCBUF_H

#include <string>
#include <cstddef>
using std::string;

class CircBuf {
	const size_t CHUNK { 8 };
	char* buffer;
	int read_index;
	int write_index;
	size_t buffer_capacity;
	size_t buffer_size;
	// Insert your private stuff here

	
public:
	CircBuf(size_t reserve = 0) {		// Number of elements you want it to be able to hold to start with.
		if (reserve == 0) {
			size_t buffer_capacity = 0;
		}
		else {
			size_t buffer_capacity = (reserve/CHUNK)+1;
		}
		buffer = new char[buffer_capacity*CHUNK];
		read_index = write_index = 0;
	}
	~CircBuf(){
		delete []buffer;
	}
	size_t	size(){
		return buffer_size;
	}
	size_t	capacity(){
		return buffer_capacity;
	}
	void grow() {
		size_t temp_buffer_size = 0;
		size_t temp_buffer_capacity = buffer_capacity+CHUNK;
		char* temp_buffer = new char[temp_buffer_capacity];
		if (read_index > write_index) {
			for (int x = read_index; x <= buffer_capacity; x++) {
				*(temp_buffer+temp_buffer_size) = *(buffer+x);
				temp_buffer_size++;
			}
			for (int x = 0; x <=write_index; x++) {
				*(temp_buffer+temp_buffer_size) = *(buffer+x);
				temp_buffer_size++;
			}
		}
		else {
			for (int x = read_index; x <= write_index; x++) {
				*(temp_buffer+temp_buffer_size) = *(buffer+x);
				temp_buffer_size++;
			}
		}
		delete []buffer;
		buffer = new char[temp_buffer_capacity];
		for (int x = 0; x < temp_buffer_size; x++) {
			*(buffer+x) = *(temp_buffer+x);
		}
		delete []temp_buffer;
		buffer_capacity = temp_buffer_capacity;
		buffer_size = temp_buffer_size;
	}
	
	void		insert(char insert_char) {
		if (write_index == buffer_capacity+1 && read_index > 0) {
			write_index = 0;
		}
		else if ((write_index == buffer_capacity+1 && read_index == 0) || write_index+1 == read_index) {
			grow();
		}
		*(buffer+write_index) = insert_char;
		write_index++;
	}
	void		insert (const char* insert_chars, size_t sz) {
		if (write_index+sz >= buffer_capacity+1 && read_index > sz) {
			write_index = 0;
		}
		else if ((write_index+sz >= buffer_capacity+1 && read_index <= sz) || (write_index < read_index && write_index+sz >= read_index)) {
			grow();
		}
		for (int x = 0; x < sz; x++){
			*(buffer+write_index) = *insert_chars+x;
			write_index++;
		}
	}
	void		insert(const string& insert_string) {
		if (write_index+insert_string.length() >= buffer_capacity+1 && read_index > insert_string.length()) {
			write_index = 0;
		}
		else if ((write_index+insert_string.length() >= buffer_capacity+1 && read_index <= insert_string.length()) || (write_index < read_index && write_index+insert_string.length() >= read_index)) {
			grow();
		}
		for (int x = 0; x < insert_string.length(); x++){
			*(buffer+write_index) = insert_string[x];
			write_index++;
		}
	}
	char		get();
	string	get(size_t);
	string	flush();	// Returns a string with all the characters, AND shrinks the buffer to zero.	
	string	examine();	
	void		shrink();	// Reduces the unused space in the buffer.
};
#endif
