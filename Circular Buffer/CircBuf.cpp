/*Cameron Hancock
Last updated: 01/28/2024
I learned a lot about namespaces and pointers in this projece. I found that I struggled the most
with making sure I was keeping withing the bounds of the char* at all times. A lot of the problems
I faced were because I wasn't propperly keeping track of where my read and write indexes were. Once
I figured that out it went a lot more smoothly.*/
#include <iostream>
#include "CircBuf.h"
#include "test.h"

using namespace std;

int main()
{
	CircBuf ab;
	test_(ab.capacity() == 0);
	ab.insert("ONE", 3);
	test_(ab.size() == 3);
	test_(ab.get(2) == "ON");
	test_(ab.size() == 1);
	test_(ab.examine() == "[--E-----]");
	ab.insert("TWO");
	test_(ab.examine() == "[--ETWO--]");
	ab.get();
	ab.insert("THREE");
	test_(ab.examine() == "[REETWOTH]");
	test_(ab.get(3) == "TWO");
	test_(ab.examine() == "[REE---TH]");
	test_(ab.size() == 5);
	test_(ab.capacity() == 8);
	ab.insert ("FOUR", 4);
	test_(ab.size() == 9);
	test_(ab.capacity() == 16);
	test_(ab.examine() == "[THREEFOUR-------]");
	string threefour = ab.get(10);
	test_(threefour == "THREEFOUR");
	test_(ab.size() == 0);
	
	CircBuf cb(1);
	test_(cb.capacity() == 8);
	cb.insert("abcdefgh", 8);
	test_(cb.examine() == "[abcdefgh]");
	test_(cb.size() == 8);
	test_(cb.capacity() == 8);
	test_(cb.get() == 'a');
	test_(cb.examine() == "[-bcdefgh]");
	test_(cb.size() == 7);
	cb.insert("i");
	test_(cb.examine() == "[ibcdefgh]");
	test_(cb.size() == 8);
	string s = cb.get(3);
	test_(s == "bcd");
	test_(cb.get(0) == "");			// Get zero characters
	test_(cb.size() == 5);
	test_(cb.examine() == "[i---efgh]");
	cb.insert ("ABCDEFGH", 6);			// Only put in the first six characters
	test_(cb.size() == 11);
	test_(cb.examine() == "[efghiABCDEF-----]");
	
	s = cb.get(6);
	test_(s == "efghiA");
	test_(cb.size() == 5);
	test_(cb.examine() == "[------BCDEF-----]");
	cb.insert("1234567890");
	test_(cb.size() == 15);
	test_(cb.examine() == "[67890-BCDEF12345]");
	cb.insert("xyz");
	test_(cb.examine() == "[BCDEF1234567890xyz------]");
	test_(cb.get(17) == "BCDEF1234567890xy");
	test_(cb.examine() == "[-----------------z------]");
	cb.shrink();
	test_(cb.examine() == "[z-------]");
	test_(cb.size() == 1);
	test_(cb.capacity() == 8);
	test_(cb.get() == 'z');
	cb.shrink();
	test_(cb.examine() == "[]");
	test_(cb.size()== 0);
	test_(cb.capacity() == 0);
	cb.insert("The Quick Brown Fox Jumped Over the Lazy Poodle");
	test_(cb.size()== 47);
	test_(cb.flush() == "The Quick Brown Fox Jumped Over the Lazy Poodle");
	test_(cb.size()== 0);
	
	CircBuf xb(511);
	test_(xb.size() == 0);
	test_(xb.capacity() == 512);
	
	report_();
	reset_();
}