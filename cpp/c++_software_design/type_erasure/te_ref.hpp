#include <memory>

class ShapeConstRef {
public:
    // Though it is ShapeConstRef, we takes shape by non-const ref to avoid
    // accepting rvalue argument as our reference do not extend lifetime
    // of temporaries like built-in const-ref does.
    // If we has a const Shape and wants to pass in, we can perform const_cast,
    // it is a valid use of const_cast.
    template <class ShapeT, class DrawStrategy>
    ShapeConstRef(ShapeT& shape, DrawStrategy& drawer)
        : shape_{std::addressof(shape)}, drawer_{std::addressof(drawer)}
        , draw_{ [](void const* shapeBytes, void const* drawerBytes) {
            (*static_cast<DrawStrategy const*>(drawerBytes))
            (*static_cast<ShapeT const*>(shapeBytes));
        }}
    {}

    // Rule of 0

    void draw() const { draw_(shape_, drawer_); }
private:
    void const* shape_;
    void const* drawer_;
    void (*draw_)(void const*, void const*);
    // can add more func ptrs or defining NonOwningShapeModel to handle
    // conversion between ShapeConstRef and Shape
};
