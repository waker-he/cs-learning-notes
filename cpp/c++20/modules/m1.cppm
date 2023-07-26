export module m1;

export import :p1;  // import and export interface partition p1

// name of namespace does not conflict with module name
export namespace m1 {
    void foo(int i);
}

export void bar(int i);

