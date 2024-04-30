// ---- <Shape.h> -------------------------------------

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};

// ---- <Circle.h> -------------------------------------

#include <Shape.h>

class Circle : public Shape {
public:
    explicit Circle(double radius)
        : radius_(radius) {}
    double radius() const { return radius_; }
    void draw() const override;
private:
    double radius_;
};

// ---- <Circle.cpp> -------------------------------------

#include <Circle.h>
#include /* some graphics library */

void Circle::draw() const { /* ... */}


// ---------------------------------------------------------
// Similarly: <Square.h>, <Square.cpp> 
// ---------------------------------------------------------

// ---- <Main.cpp> -------------------------------------

#include <memory>
#include <vector>
#include <Circle.h>
#include <Square.h>

void drawAllShapes(std::vector<std::unique_ptr<Shape>> const& shapes) {
    for (auto const& shape : shapes) {
        shape->draw();
    }
}

int main() {
    using Shapes = std::vector<std::unique_ptr<Shape>>;

    Shapes shapes;
    shapes.emplace_back(std::make_unique<Circle>(2.3));
    shapes.emplace_back(std::make_unique<Square>(1.2));

    drawAllShapes(shapes);
}
