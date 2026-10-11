#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>
#include <string>

// Abstract base class Shape demonstrating Polymorphism
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};

class Circle : public Shape {
private:
    double r_;
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159 * r_ * r_; }
    std::string name() const override { return "Circle"; }
};

class Rectangle : public Shape {
private:
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    double area() const override { return w_ * h_; }
    std::string name() const override { return "Rectangle"; }
};

int main() {
    int n;
    if (!(std::cin >> n)) return 0;

    std::vector<std::unique_ptr<Shape>> shapes;
    for (int i = 0; i < n; ++i) {
        std::string type;
        std::cin >> type;
        if (type == "circle") {
            double r;
            std::cin >> r;
            shapes.push_back(std::make_unique<Circle>(r));
        } else if (type == "rectangle") {
            double w, h;
            std::cin >> w >> h;
            shapes.push_back(std::make_unique<Rectangle>(w, h));
        }
    }

    std::cout << std::fixed << std::setprecision(2);
    for (const auto& s : shapes) {
        std::cout << s->name() << ": " << s->area() << "\n";
    }
    return 0;
}
