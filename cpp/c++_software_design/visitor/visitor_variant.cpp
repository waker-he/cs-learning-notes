// ---- <Circle.h> -------------------------------------

class Circle {
public:
    explicit Circle(double radius)
        : radius_(radius) {}
    double radius() const { return radius_; }
private:
    double radius_;
};

// ---- <Square.h> -------------------------------------

class Square {
public:
    explicit Square(double side)
        : side_(side) {}
    double side() const { return side_; }
private:
    double side_;
};

// ---- <Shape.h> -------------------------------------

#include <variant>
#include <Circle.h>
#include <Square.h>

using Shape = std::variant<Circle, Square>;

// ---- <Draw.h> -------------------------------------

#include <Shape.h>

class Draw {
public:
    void operator()(Circle const&) const;
    void operator()(Square const&) const;
};
// implemented in <Draw.cpp>

// ---- <Main.cpp> -----------------

#include <vector>
#include <Shape.h>
#include <Draw.h>

void drawAllShapes(std::vector<Shape> const& shapes) {
    for (auto const& shape : shapes) {
        std::visit(Draw{}, shape);
    }
}

int main() {
    using Shapes = std::vector<Shape>;

    Shapes shapes;
    shapes.emplace_back(Circle{2.3});
    shapes.emplace_back(Square{1.2});

    drawAllShapes(shapes);
}