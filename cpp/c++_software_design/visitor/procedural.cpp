// ---- <Shape.h> -------------------------------------

enum class ShapeType { circle, square };
class Shape {
protected:
    explicit Shape (ShapeType type) : type_(type) {}
public:
    virtual ~Shape() = default;
    ShapeType getType() const { return type_; }
private:
    ShapeType type_;
};

// ---- <Circle.h> -------------------------------------

#include <Shape.h>

class Circle : public Shape {
public:
    explicit Circle(double radius)
        : Shape(ShapeType::circle), radius_(radius) {}
    double radius() const { return radius_; }
private:
    double radius_;
};

// ---- <DrawCircle.h> -------------------------------------
class Circle;
void draw(Circle const&);

// ---- <DrawCircle.cpp> -------------------------------------

#include <DrawCircle.h>
#include <Circle.h>
#include /* some graphics library */

void draw(Circle const& c) { /*...*/ }

// ---------------------------------------------------------
// Similarly: <Square.h>, <DrawSquare.h>, <DrawSquare.cpp> 
// ---------------------------------------------------------

// ---- <Main.cpp> -------------------------------------

#include <memory>
#include <vector>
#include <DrawCicle.h>
#include <DrawSquare.h>

void drawAllShapes(std::vector<std::unique_ptr<Shape>> const& shapes) {
    for (auto const& shape : shapes) {
        switch (shape->getType())
        {
        case ShapeType::circle:
            draw(static_cast<Circle const&>(*shape));
            break;
        case ShapeType::square:
            draw(static_cast<Square const&>(*shape));
            break;
        }
    }
}

int main() {
    using Shapes = std::vector<std::unique_ptr<Shape>>;

    Shapes shapes;
    shapes.emplace_back(std::make_unique<Circle>(2.3));
    shapes.emplace_back(std::make_unique<Square>(1.2));

    drawAllShapes(shapes);
}
