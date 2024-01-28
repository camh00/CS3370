#ifndef CIRCBUF_H
#define CIRCBUF_H

#include <iostream>
#include <string>
#include <cstddef>
using std::string;

class CircBuf {
	const size_t CHUNK { 8 };
	char* buffer;
	unsigned int read_index;
	unsigned int write_index;
	size_t buffer_capacity;
	size_t buffer_size;
	// Insert your private stuff here

	
public:
	CircBuf(size_t reserve = 0) {		// Number of elements you want it to be able to hold to start with.
		buffer_capacity = 0;
		if (reserve == 0) {
			buffer_capacity = 0;
		}
		else {
			buffer_capacity = (reserve/CHUNK)*CHUNK+CHUNK;
		}
		buffer = new char[buffer_capacity];
		buffer_size = 0;
		read_index = write_index = 0;
	}
	~CircBuf(){
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
		if (buffer_size != 0) {
			if (read_index > write_index) {
				for (unsigned int x = read_index; x <= buffer_capacity; x++) {
					*(temp_buffer+temp_buffer_size) = *(buffer+x);
					temp_buffer_size++;
				}
				for (unsigned int x = 0; x <=write_index; x++) {
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
		delete []buffer;
		buffer = new char[temp_buffer_capacity];
		for (unsigned int x = 0; x < temp_buffer_size; x++) {
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
		else if ((write_index == buffer_capacity+1 && read_index == 0) || write_index == read_index) {
			grow();
		}
		*(buffer+write_index) = insert_char;
		write_index++;
		buffer_size++;
	}
	void		insert (const char* insert_chars, size_t sz) {
		if (write_index+sz > buffer_capacity && read_index >= sz-(buffer_capacity-write_index)) {
			write_index = 0;
		}
		else if ((write_index+sz > buffer_capacity && read_index < sz-(buffer_capacity-write_index)) || (write_index < read_index && write_index+sz >= read_index)) {
			grow();
		}
		for (unsigned int x = 0; x < sz; x++) {
			*(buffer+write_index) = *(insert_chars+x);
			write_index++;
			buffer_size++;
		}
	}
	void		insert(const string& insert_string) {
		if (write_index+insert_string.length() > buffer_capacity && read_index >= insert_string.length()-(buffer_capacity-write_index)) {
			write_index = 0;
		}
		else if ((write_index+insert_string.length() > buffer_capacity && read_index < insert_string.length()-(buffer_capacity-write_index)) || (write_index < read_index && write_index+insert_string.length() >= read_index)) {
			grow();
		}
		for (unsigned int x = 0; x < insert_string.length(); x++){
			*(buffer+write_index) = insert_string[x];
			write_index++;
			buffer_size++;
		}
	}
	char		get() {
		if (read_index != write_index && buffer_size > 0) {
			return *(buffer+read_index);
			read_index++;
			buffer_size--;
		}
		else if (read_index >= buffer_capacity) {
			return *(buffer+read_index);
			read_index = 0;
			buffer_size--;
		}
	}
	std::string	get(size_t amount) {
		std::string output = "";
		if (static_cast<int>(buffer_size)-static_cast<int>(amount) > 0) {
			if ((read_index+amount >= write_index) || (read_index+amount > buffer_capacity && read_index+amount-buffer_capacity >= write_index)) {
				if (read_index < write_index) {
					for (unsigned int x = 0; x < write_index; x++) {
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
					for (unsigned int x = 0; x < write_index; x++) {
						output += *(buffer+read_index);
						read_index++;
						buffer_size--;
					}
				}
			}
			else {
				if (read_index < write_index) {
					int new_read_index = read_index+amount;
					for (int x = 0; x < new_read_index; x++) {
						output += *(buffer+read_index);
						read_index++;
						buffer_size--;
					}
				}
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
		}
		return output;
	}
	string	flush() {	// Returns a string with all the characters, AND shrinks the buffer to zero.
		return get(buffer_size);
		shrink();
	}	
	string	examine() {
		std::string current_buffer = "[";
		if (read_index <= write_index && read_index > 0) {
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
			for (unsigned int x = write_index; x < buffer_capacity; x++) {
				current_buffer += *(buffer+x);
			}
		}
		current_buffer += "]";
		std::cout << current_buffer << std::endl;
		return current_buffer;
	}	
	void		shrink() {	// Reduces the unused space in the buffer.
		size_t temp_buffer_size = buffer_size;
		size_t temp_buffer_capacity = (buffer_size/CHUNK)*CHUNK+CHUNK;
		if (temp_buffer_capacity == 0) {
			delete []buffer;
			char* buffer = new char[0];
		}
		else {
			char* temp_buffer = new char[temp_buffer_capacity];
			if (read_index > write_index) {
			for (unsigned int x = read_index; x <= buffer_capacity; x++) {
				*(temp_buffer+temp_buffer_size) = *(buffer+x);
				temp_buffer_size++;
			}
			for (unsigned int x = 0; x <=write_index; x++) {
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
		delete []buffer;
		buffer = new char[temp_buffer_capacity];
		for (unsigned int x = 0; x < temp_buffer_size; x++) {
			*(buffer+x) = *(temp_buffer+x);
		}
		delete []temp_buffer;
		buffer_capacity = temp_buffer_capacity;
		buffer_size = temp_buffer_size;
		}
	}
};
#endif
