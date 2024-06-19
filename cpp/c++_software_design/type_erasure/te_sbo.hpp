#include <memory>
#include <array>

template <std::size_t BufferCapacity = 32>
class Shape {
    struct Concept;
    template <class, class> struct OwningModel;
public:
    template <class ShapeT, class DrawStrategy>
    Shape(ShapeT&& shape, DrawStrategy&& drawer) {
        using Model = OwningModel<ShapeT, DrawStrategy>;
        if constexpr (sizeof(Model) <= BufferCapacity) {
            pimpl_ = std::construct_at(
                reinterpret_cast<Model*>(buf_.data()),
                std::forward<ShapeT>(shape), std::forward<DrawStrategy>(drawer)
            );
        }
        else {
            pimpl_ = new Model(std::forward<ShapeT>(shape),
                               std::forward<DrawStrategy>(drawer));
        }
    }
    
    Shape(Shape const& other) : pimpl_(other.pimpl_->clone(buf_.data())) {}
    Shape(Shape&& other) : pimpl_(other.pimpl_->move(buf_.data())) {}

    Shape& operator=(Shape other) {
        swap(other);
        return *this;
    }
    ~Shape() {
        pimpl_->del();
    }

    void draw() const { pimpl_->draw(); }
    void swap(Shape& other) {
        std::swap(pimpl_, other.pimpl_);
        std::swap(buf_, other.buf_);
    }
private:
    alignas(std::max_align_t) std::array<std::byte, BufferCapacity> buf_;
    Concept* pimpl_;   // Bridge to impl details

    struct Concept {
        virtual ~Concept() = default;
        virtual void draw() const = 0;

        // takes an additional parameter to construct at buf if possible
        virtual Concept* clone(void* buf) const = 0;
        virtual Concept* move(void* buf) = 0;

        virtual void del() = 0;
    };

    template <class ShapeT, class DrawStrategy>
    struct OwningModel : Concept {
        explicit OwningModel(ShapeT&& shape, DrawStrategy&& drawer)
            : shape_{std::forward<ShapeT>(shape)}
            , drawer{std::forward<DrawStrategy>(drawer)}
        {}
        
        void draw() const override { drawer_(shape_); }

        Concept* clone(void* buf) const override {
            if constexpr (sizeof(OwningModel) <= BufferCapacity) {
                return std::construct_at(
                    reinterpret_cast<OwningModel*>(buf), *this
                );
            }
            else {
                return new OwningModel(*this);
            }
        }

        Concept* move(void* buf) override {
            if constexpr (sizeof(OwningModel) <= BufferCapacity) {
                return std::construct_at(
                    reinterpret_cast<OwningModel*>(buf), std::move(*this)
                );
            }
            else {
                return new OwningModel(std::move(*this));
            }
        }

        void del() override {
            if constexpr (sizeof(OwningModel) <= BufferCapacity) {
                std::destroy_at(this);  // only destroy
            }
            else {
                delete this;    // deallocate and destroy
            }
        } 
    private:
        ShapeT shape_;
        DrawStrategy drawer_;
    };
};
