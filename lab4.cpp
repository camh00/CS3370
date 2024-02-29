#include <iostream>
#include <array>

// Question 1
template <class anyType>
void for_each_if(int* begin, int* end, void f(int n), anyType predicate) {
    for (begin; begin != end; begin++) {
        if (predicate(*begin)) {
            f(*begin);
        }
    }
}
void f(int n) {
    std::cout << n << "\n";
}

// Question 2

int main() {
    // Question 1
    std::array<int, 4> a = {1,2,3,4};
    for_each_if(a.begin(), a.end(), f, [](int x){return x%2==0;});

    // Question 2
}