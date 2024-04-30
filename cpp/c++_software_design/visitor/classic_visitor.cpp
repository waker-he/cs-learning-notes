// ---- <ShapeVisitor.h> -------------------------------------

class Circle;
class Square;

class ShapeVisitor {
public:
    virtual ~ShapeVisitor() = default;

    virtual void visit(Circle const&) const = 0;
    virtual void visit(Square const&) const = 0;
};

// ---- <Shape.h> -------------------------------------

#include <ShapeVisitor.h>

class Shape {
public:
    Shape() = default;
    virtual ~Shape() = default;
    virtual void accept(ShapeVisitor const&) const = 0;
    // cannot use deducing this here,
    // as we need runtime polymorphism (call through base class ref/ptr)
};

// ---- <Circle.h> -------------------------------------

#include <Shape.h>

class Circle : public Shape {
public:
    explicit Circle(double radius)
        : radius_(radius) {}
    double radius() const { return radius_; }
    virtual void accept(ShapeVisitor const& v) const { v.visit(*this); }
private:
    double radius_;
};


// ---------------------------------------------------------
// Similarly: <Square.h>
// ---------------------------------------------------------



// ---- <Draw.h> -------------------------------------

#include <ShapeVisitor.h>

class Draw : public ShapeVisitor {
public:
    void visit(Circle const& circle) const override;
    void visit(Square const& circle) const override;
};
// implemented in <Draw.cpp>

// ---- <Main.cpp> -------------------------------------

#include <vector>
#include <memory>
#include <Circle.h>
#include <Square.h>
#include <Draw.h>

void drawAllShapes(std::vector<std::unique_ptr<Shape>> const& shapes) {
    for (auto const& shape : shapes) {
        shape->accept(Draw{});
    }
}

int main() {
    using Shapes = std::vector<std::unique_ptr<Shape>>;

    Shapes shapes;
    shapes.emplace_back(std::make_unique<Circle>(2.3));
    shapes.emplace_back(std::make_unique<Square>(1.2));

    drawAllShapes(shapes);
}
