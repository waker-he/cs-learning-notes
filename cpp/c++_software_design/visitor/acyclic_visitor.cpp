// ---- <AbstractVisitor.h> -------------------------------------

struct AbstractVisitor { virtual ~AbstractVisitor() = default; };

// ---- <Visitor.h> -------------------------------------

template <class T>
struct Visitor {
    virtual ~Visitor() = default;
    virtual void visit(T const&) const = 0;
};

// ---- <Shape.h> -------------------------------------

#include <AbstractVisitor.h>

class Shape {
public:
    Shape() = default;
    virtual ~Shape() = default;
    virtual void accept(AbstractVisitor const&) const = 0;
    // ...
};

// ---- <Circle.h> -------------------------------------

#include <Shape.h>

class Circle : public Shape {
public:
    explicit Circle(double radius)
        : radius_(radius) {}
    double radius() const { return radius_; }
    virtual void accept(AbstractVisitor const& v) const {
        if (auto const* cv = dynamic_cast<Visitor<Circle> const*>(&v))
            cv->visit(*this);
    }
private:
    double radius_;
};


// ---------------------------------------------------------
// Similarly: <Square.h>
// ---------------------------------------------------------

// ---- <Draw.h> -------------------------------------

#include <AbstractVisitor.h>
#include <Visitor.h>

class Draw : AbstractVisitor, Visitor<Circle>, Visitor<Square>
{
public:
    void visit(Circle const& circle) const override;
    void visit(Square const& circle) const override;
};
// implemented in <Draw.cpp>
