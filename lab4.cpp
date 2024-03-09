/*
Cameron Hancock
02/29/2024
Questions from lab 4
*/
#include <iostream>
#include <array>
#include <iterator>
#include <functional>

// Question 1
template <class iterator, class function, class predicate>
void for_each_if(iterator begin, iterator end, function f, predicate p) {
    for (;begin != end; begin++) {
        if (p(*begin)) {
            f(*begin);
        }
    }
}
void f(int n) {
    std::cout << n << "\n";
}

// Question 2
template <class anyType>
bool is_sorted(anyType *begin, anyType *end) {
    anyType last = *begin;
    begin++;
    for (;begin != end; begin++) {
        if (last > *begin) {
            return false;
        }
        last = *begin;
    }
    return true;
}

// Question 3
template <class anyType>
anyType range(anyType *begin, anyType *end, anyType n) {
    for(;begin != end; begin++) {
        *begin = n++;
    }
    return n;
}

int main() {
    // Question 1
    std::array<int, 4> a = {1,2,3,4};
    for_each_if(a.begin(), a.end(), f, [](int x){return x%2==0;});

    // Question 2
    std::array<int, 4> b = {1,3,2,4};
    std::cout << is_sorted(a.begin(), a.end()) << std::endl;
    std::cout << is_sorted(b.begin(), b.end()) << std::endl;

    // Question 3
    std::array<int, 4> c;
    int n = range(c.begin(), c.end(), 1);
    std::array<int, 2> d;
    range(d.begin(), d.end(), n);
    std::copy(c.begin(), c.end(), std::ostream_iterator<int>(std::cout, " ")); // 1 2 3 4
    std::cout << std::endl;
    std::copy(d.begin(), d.end(), std::ostream_iterator<int>(std::cout, " ")); // 5 6
}