#include <memory>

class Shape {
    struct Concept;
    template <class, class> struct OwningModel;
public:
    template <class ShapeT, class DrawStrategy>
    Shape(ShapeT&& shape, DrawStrategy&& drawer) 
        : pimpl_{std::make_unique<OwningModel<ShapeT, DrawStrategy>>(
            std::forward<ShapeT>(shape), std::forward<DrawStrategy>(drawer)
        )}
        {}
    
    Shape(Shape const& other) : pimpl_(other.pimpl_->clone()) {}
    // Here move operation is not noexcept, which can make relocate operation
    // expensive, we can mark it as noexcept if ignoring std::bad_alloc
    Shape(Shape&& other) : pimpl_(other.pimpl_->move()) {}

    // We allows Shape to change the actual type it refers to,
    // so that we are using copy-swap idiom here, which costs extra dynamic
    // allocation if the types are same and we just want to call the underlying
    // type's assign operator
    // Assignment is a binary operation, thanks to copy-swap idiom, it is
    // still trivial to implement.
    Shape& operator=(Shape other) {
        pimpl_.swap(other.pimpl_);
        return *this;
    }
    ~Shape() = default;

    void draw() const { pimpl_->draw(); }
private:
    std::unique_ptr<Concept> pimpl_;   // Bridge to impl details

    // External Polymorphism
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw() const = 0;

        // Prototype
        virtual std::unique_ptr<Concept> clone() const = 0;
        virtual std::unique_ptr<Concept> move() = 0;
    };

    template <class ShapeT, class DrawStrategy>
    struct OwningModel : Concept {
        explicit OwningModel(ShapeT&& shape, DrawStrategy&& drawer)
            : shape_{std::forward<ShapeT>(shape)}
            , drawer{std::forward<DrawStrategy>(drawer)}
        {}
        
        void draw() const override { drawer_(shape_); }

        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<OwningModel>(*this);
        }

        std::unique_ptr<Concept> move() override {
            return std::make_unique<OwningModel>(std::move(*this));
        }
    private:
        ShapeT shape_;
        DrawStrategy drawer_;
    };
};
