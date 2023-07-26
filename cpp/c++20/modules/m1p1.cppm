export module m1:p1;    // interface module declaration

import :p2;             // import internal partition p2

export struct S {
    int i;
    int square() {
        return sq(i);
    }
};