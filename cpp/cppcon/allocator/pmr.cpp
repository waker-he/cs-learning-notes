#include <memory_resource>
#include <cstddef>
#include <vector>
#include <array>
#include <iostream>

using namespace std;

class test_resource : public pmr::memory_resource
{
public:

    explicit test_resource(pmr::memory_resource* parent_ =
                            pmr::get_default_resource())
        : parent(parent_) {
    }

protected:
    [[nodiscard]] void *do_allocate(size_t bytes, size_t alignment) override {
        bytes_allocated += bytes;
        bytes_outstanding += bytes;
        cout << "allocated " << bytes_allocated << " bytes, ";
        cout << "oustanding bytes: " << bytes_outstanding << '\n';
        return parent->allocate(bytes, alignment);
    }

    void do_deallocate(void* p, size_t bytes, size_t alignment) override {
        bytes_deallocated += bytes;
        bytes_outstanding -= bytes;
        cout << "deallocated " << bytes_deallocated << " bytes, ";
        cout << "oustanding bytes: " << bytes_outstanding << '\n';
        parent->deallocate(p, bytes, alignment);
    }

    bool do_is_equal(const pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

private:
    pmr::memory_resource *parent;
    size_t bytes_deallocated = 0;
    size_t bytes_allocated = 0;
    size_t bytes_outstanding = 0;
};


int main() {
    array<int, 1000> buf;
    pmr::monotonic_buffer_resource pool{buf.data(), buf.size()};
    test_resource pool2{&pool};
    pmr::vector<pmr::string> vec(&pool2);

    for (int i = 0; i < 10; i++) {
        vec.emplace_back("just a non-SSO string");
    }
}