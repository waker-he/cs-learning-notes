module;         // start module unit with global module fragment

// in global module fragment, put in any preprocessor commands
// these are only visible within this module unit
#include <iostream>
#include <format>

module m1;      // implementation unit of module m1

// #include is no longer support after module declaration
// can still use #define and #ifdef

namespace m1 {
void foo(int i) {
    std::cout << std::format("foo: {}\n", i);
}
}

void bar(int i) {
    std::cout << std::format("bar: {}\n", i);
}