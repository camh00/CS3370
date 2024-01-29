#ifndef CIRCBUF_H
#define CIRCBUF_H

#include <iostream>
#include <string>
#include <cstddef>
using std::string;

class CircBuf {
	const size_t CHUNK { 8 };
	char* buffer;
	char* temp_buffer;
	unsigned int read_index;
	unsigned int write_index;
	size_t buffer_capacity;
	size_t buffer_size;
public:
	CircBuf(size_t reserve = 0) {
		buffer_capacity = 0;
		if (reserve == 0) {
			buffer_capacity = 0;
		}
		else {
			buffer_capacity = (reserve/CHUNK)*CHUNK+CHUNK;
		}
		// creates buffer according to required size
		buffer = new char[buffer_capacity];
		buffer_size = 0;
		read_index = write_index = 0;
	}
	~CircBuf(){
	}
	// return size or capacity
	size_t	size(){
		return buffer_size;
	}
	size_t	capacity(){
		return buffer_capacity;
	}
	// if the buffer is to small, increase the capacity by one CHUNK
	void grow() {
		size_t temp_buffer_size = 0;
		size_t temp_buffer_capacity = buffer_capacity+CHUNK;
		temp_buffer = new char[temp_buffer_capacity];
		// store contents of buffer in a temporary buffer
		if (buffer_size != 0) {
			if (read_index > write_index) {
				for (unsigned int x = read_index; x < buffer_capacity; x++) {
					*(temp_buffer+temp_buffer_size) = *(buffer+x);
					temp_buffer_size++;
				}
				for (unsigned int x = 0; x < write_index; x++) {
					*(temp_buffer+temp_buffer_size) = *(buffer+x);
					temp_buffer_size++;
				}
			}
			else {
				for (unsigned int x = read_index; x <= write_index; x++) {
					*(temp_buffer+temp_buffer_size) = *(buffer+x);
					temp_buffer_size++;
				}
			}
		}
		// delete old buffer and create a new one, copying in the contents from tne old one
		delete[] CircBuf::buffer;
		buffer = new char[temp_buffer_capacity];
		for (unsigned int x = 0; x < temp_buffer_size; x++) {
			*(buffer+x) = *(temp_buffer+x);
		}
		delete[] temp_buffer;
		buffer_capacity = temp_buffer_capacity;
		buffer_size = temp_buffer_size;
		read_index = 0;
		write_index = buffer_size;
	}
	
	// insert one character
	void insert(char insert_char) {
		//if at the end of the buffer, loop back around
		if (write_index == buffer_capacity+1 && read_index > 0) {
			write_index = 0;
		}
		// grow the buffer if out of space
		else if ((write_index == buffer_capacity+1 && read_index == 0) || write_index == read_index) {
			grow();
		}
		// insert char and update index
		*(buffer+write_index) = insert_char;
		write_index++;
		buffer_size++;
	}
	
	// insert a amount of characters from a given char*
	void insert (const char* insert_chars, size_t sz) {
		// grow the buffer if out of space
		while (buffer_size+sz > buffer_capacity) {
			grow();
		}
		// insert char and update index
		for (unsigned int x = 0; x < sz; x++) {
			//if at the end of the buffer, loop back around
			if (write_index >= buffer_capacity) {
				write_index = 0;
				*(buffer+write_index) = *(insert_chars+x);
				write_index++;
				buffer_size++;
			}
			else {
				*(buffer+write_index) = *(insert_chars+x);
				write_index++;
				buffer_size++;
			}
		}
	}

	// insert a given string
	void insert(const string& insert_string) {
		// grow the buffer if out of space
		while ((write_index+insert_string.length() > buffer_capacity && read_index < insert_string.length()-(buffer_capacity-write_index)) || (write_index < read_index && write_index+insert_string.length() >= read_index)) {
			grow();
		}
		// insert char and update index
		for (unsigned int x = 0; x < insert_string.length(); x++){
			//if at the end of the buffer, loop back around
			if (write_index >= buffer_capacity) {
				write_index = 0;
				*(buffer+write_index) = insert_string[x];
				write_index++;
				buffer_size++;
			}
			else {
				*(buffer+write_index) = insert_string[x];
				write_index++;
				buffer_size++;
			}
		}
	}
	// return current char 
	char get() {
		char output = '\0';
		if (buffer_size > 0) {
			if (read_index < write_index) {
				output += *(buffer+read_index);
				read_index++;
				buffer_size--;
			}
			else if (read_index > buffer_capacity && read_index < buffer_capacity) {
				output += *(buffer+read_index);
				read_index++;
				buffer_size--;
			}
			// wrap around if read_index is at the end of the buffer
			else if (read_index > buffer_capacity && read_index >= buffer_capacity) {
				read_index = 0;
				output += *(buffer+read_index);
				read_index++;
				buffer_size--;
			}
		}
		return output;
	}
	// return a given amount of chars from buffer as a string
	std::string	get(size_t amount) {
		std::string output = "";
		// if the amount requested is greater than buffer_size, return all chars
		if ((read_index < write_index && read_index+amount >= write_index) || (read_index+amount > buffer_capacity && read_index+amount-buffer_capacity >= write_index)) {
			if (read_index < write_index) {
				for (unsigned int x = read_index; x < write_index; x++) {
					output += *(buffer+read_index);
					read_index++;
					buffer_size--;
				}
			}
			else if (read_index > write_index) {
				for (unsigned int x = read_index; x < buffer_capacity; x++) {
					output += *(buffer+read_index);
					read_index++;
					buffer_size--;
				}
				read_index = 0;
				for (unsigned int x = read_index; x < write_index; x++) {
					output += *(buffer+read_index);
					read_index++;
					buffer_size--;
				}
			}
		}
		// return the amount of chars requested
		else {
			if (read_index < write_index) {
				size_t new_read_index = read_index+amount;
				for (unsigned int x = read_index; x < new_read_index; x++) {
					output += *(buffer+read_index);
					read_index++;
					buffer_size--;
				}
			}
			// return requested amount of chars if buffer is full
			else if (read_index == write_index && buffer_size > 0) {
				if (read_index+amount >= buffer_capacity) {
					for (unsigned int x = read_index; x < buffer_capacity; x++) {
						output += *(buffer+read_index);
						amount--;
						read_index++;
						buffer_size--;
					}
					read_index = 0;
					for (unsigned int x = read_index; x <= amount; x++) {
						output += *(buffer+read_index);
						read_index++;
						buffer_size--;
					}
				}
				else {
					size_t new_read_index = read_index+amount;
					for (unsigned int x = read_index; x < new_read_index; x++) {
						output += *(buffer+read_index);
						read_index++;
						buffer_size--;
					}
				}
			}
			// return chars and loop around if read is greater than write
			else if (read_index > write_index) {
				if (read_index+amount >= buffer_capacity) {
					for (unsigned int x = read_index; x < buffer_capacity; x++) {
						output += *(buffer+read_index);
						read_index++;
						amount--;
						buffer_size--;
					}
					read_index = 0;
					for (unsigned int x = 0; x <= amount; x++) {
						output += *(buffer+read_index);
						read_index++;
						buffer_size--;
					}
				}
				else {
					for (unsigned int x; x < amount; x++) {
						output += *(buffer+read_index);
						read_index++;
						buffer_size--;
					}
				}
			}
		}
		return output;
	}

	// Return a string with all the characters in buffer, and shrinks the buffer to zero
	string flush() {
		std::string output = "";
			// return the chars as a string
			if (read_index < write_index) {
				for (unsigned int x = read_index; x < write_index; x++) {
					output += *(buffer+read_index);
					read_index++;
					buffer_size--;
				}
			}
			else if (read_index > write_index) {
				for (unsigned int x = read_index; x < buffer_capacity; x++) {
					output += *(buffer+read_index);
					read_index++;
					buffer_size--;
				}
				read_index = 0;
				for (unsigned int x = read_index; x < write_index; x++) {
					output += *(buffer+read_index);
					read_index++;
					buffer_size--;
				}
			}
		// shrink the buffer to 0
		delete[] CircBuf::buffer;
		buffer = new char[0];
		CircBuf::buffer_capacity = 0;
		return output;
	}

	// return the current state of the buffer with '-' for empty spots
	string examine() {
		std::string current_buffer = "[";
		if (read_index == write_index) {
			if (buffer_size == 1) {
				for (unsigned int x = 0; x < read_index; x++) {
					current_buffer += '-';
				}
				current_buffer += *(buffer+read_index);
				for (unsigned int x = read_index+1; x < buffer_capacity; x++) {
					current_buffer += '-';
				}
			}
			else if (buffer_size > 1) {
				for (unsigned int x = 0; x < buffer_capacity; x++) {
					current_buffer += *(buffer+x);
				}
			}
		}
		if (read_index < write_index && read_index > 0) {
			for (unsigned int x = 0; x < read_index; x++) {
					current_buffer += '-';
				}
		}
		else if (write_index < read_index && write_index > 0) {
			for (unsigned int x = 0; x < write_index; x++) {
				current_buffer += *(buffer+x);
			}
		}
		if (read_index < write_index) {
			for (unsigned int x = read_index; x < write_index; x++) {
				current_buffer += *(buffer+x);
			}
		}
		else if (read_index > write_index) {
			for (unsigned int x = write_index; x < read_index; x++) {
				current_buffer += '-';
			}
		}
		if (read_index < write_index && write_index < buffer_capacity) {
			for (unsigned int x = write_index; x < buffer_capacity; x++) {
				current_buffer += '-';
			}
		}
		else if (read_index > write_index && read_index < buffer_capacity) {
			for (unsigned int x = read_index; x < buffer_capacity; x++) {
				current_buffer += *(buffer+x);
			}
		}
		current_buffer += "]";
		return current_buffer;
	}

	// Reduce the unused space in the buffer.
	void shrink() {
		size_t temp_buffer_size = 0;
		// find the smallest size needed
		size_t temp_buffer_capacity = (buffer_size/CHUNK)*CHUNK+CHUNK;
		// create a temp_buffer and store all chars in buffer
		temp_buffer = new char[temp_buffer_capacity];
		if (buffer_size == 0) {
			delete[] CircBuf::buffer;
			buffer = new char[0];
			CircBuf::buffer_capacity = 0;
		}
		else {
			if (read_index > write_index) {
				for (unsigned int x = read_index; x < buffer_capacity; x++) {
					*(temp_buffer+temp_buffer_size) = *(buffer+x);
					temp_buffer_size++;
				}
				for (unsigned int x = 0; x <= write_index; x++) {
					*(temp_buffer+temp_buffer_size) = *(buffer+x);
					temp_buffer_size++;
				}
			}
			else {
				for (unsigned int x = read_index; x < write_index; x++) {
					*(temp_buffer+temp_buffer_size) = *(buffer+x);
					temp_buffer_size++;
				}
			}
			// create a new buffer and copy chars from temp_buffer to buffer
			delete[] CircBuf::buffer;
			buffer = new char[temp_buffer_capacity];
			for (unsigned int x = 0; x < temp_buffer_size; x++) {
				*(buffer+x) = *(temp_buffer+x);
			}
			delete[] temp_buffer;
			buffer_capacity = temp_buffer_capacity;
			buffer_size = temp_buffer_size;
			read_index = 0;
			write_index = temp_buffer_size;
		}
	}
};
#endif
