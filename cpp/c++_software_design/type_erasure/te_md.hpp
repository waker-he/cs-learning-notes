#include <memory>

class Shape {
    template <class ShapeT, class DrawStrategy>
    struct OwningModel;
public:
    template <class ShapeT, class DrawStrategy>
    Shape(ShapeT&& shape, DrawStrategy&& drawer) {
        using Model = OwningModel<ShapeT, DrawStrategy>;
        bytes_ = new Model(std::forward<ShapeT>(shape),
                           std::forward<DrawStrategy>(drawer));
        draw_ = [](void* bytes) { 
            auto* const model = static_cast<Model*>(bytes);
            (model->drawer_)(model->shape_);
        };
        clone_ = [](void* bytes) -> void* {
            return new Model(*static_cast<Model*>(bytes));
        };
        move_ = [](void* bytes) -> void* {
            return new Model(std::move(*static_cast<Model*>(bytes)));
        };
        del_ = [](void* bytes) { delete static_cast<Model*>(bytes); };
    }

    Shape(Shape const& other)
        : bytes_(other.clone_(other.bytes_))
        , draw_(other.draw_)
        , clone_(other.clone_)
        , move_(other.move_)
        , del_(other.del_)
    {}

    Shape(Shape&& other)
        : bytes_(other.move_(other.bytes_))
        , draw_(other.draw_)
        , clone_(other.clone_)
        , move_(other.move_)
        , del_(other.del_)
    {}

    Shape& operator=(Shape other) {
        swap(other);
        return *this;
    }

    ~Shape() { del_(bytes_); }

    void swap(Shape& other) {
        using std::swap;
        swap(bytes_, other.bytes_);
        swap(draw_, other.draw_);
        swap(clone_, other.clone_);
        swap(move_, other.move_);
        swap(del_, other.del_);
    }

    void draw() { draw_(bytes_); }
private:
    template <class ShapeT, class DrawStrategy>
    struct OwningModel {
        OwningModel(ShapeT&& shape, DrawStrategy&& drawer_)
            : shape_(std::forward<ShapeT>(shape))
            , drawer_(std::forward<DrawStrategy>(drawer_))
        {}
        ShapeT shape_;
        DrawStrategy drawer_;
    };

    void* bytes_;
    void (*draw_)(void*);
    void* (*clone_)(void*);
    void* (*move_)(void*);
    void (*del_)(void*);
};