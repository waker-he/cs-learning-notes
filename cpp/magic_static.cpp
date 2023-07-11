#include <iostream>

int bar2() {
    std::cout << "init function static!\n";
    return 3;
}

int bar() {
    static int i = bar2();
    return 3;
}

int foo() {
    std::cout << "init class inline static!\n";
    return 100;
}

struct S {
    inline static int i = foo();
};

int main() {
    std::cout << "start main!\n";
    S s1;
    std::cout << S::i << '\n';
    std::cout << bar();
}