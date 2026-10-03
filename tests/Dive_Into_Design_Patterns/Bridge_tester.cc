/**
 *
 * 桥接模式
 *  将抽象部分与实现分离。通过组合代替继承
 *
 * */
#include "tester.h"
#include <iostream>
#include <memory>

namespace cvtest::tester {
class Color {
public:
  virtual ~Color() = default;
  virtual void fill() const = 0;
};

class Red : public Color {
public:
  void fill() const override { std::cout << "Red \n"; }
};

class Blue : public Color {
public:
  void fill() const override { std::cout << "Blue \n"; }
};

class Shape {
protected:
  std::shared_ptr<Color> color_;

public:
  Shape(std::shared_ptr<Color> color) : color_(color) {}
  virtual ~Shape() = default;
  virtual void draw() const = 0;

  void setColor(std::shared_ptr<Color> color) { color_ = std::move(color); }
};

class Circle : public Shape {
private:
  double radius_;

public:
  Circle(double radius, std::shared_ptr<Color> color) : Shape(std::move(color)), radius_(radius) {}
  void draw() const override {
    std::cout << "Draw a circle, radius =  " << radius_ << " fill coller is ";
    color_->fill();
    std::cout << "\n";
  }
};

class Rectangle : public Shape {
private:
  double width_;
  double height_;

public:
  Rectangle(double width, double height, std::shared_ptr<Color> color)
      : Shape(std::move(color)), width_(width), height_(height) {}
  void draw() const override {
    std::cout << "Draw a rectangle, width = " << width_ << " height = " << height_
              << " fill color is ";
    color_->fill();
    std::cout << "\n";
  }
};

TEST_F(Tester, DIDP_Bridge_Tester) {
  auto red = std::make_shared<Red>();
  Circle redCircle = Circle(10, red);
  Rectangle redRectangle = Rectangle(10, 10, red);

  auto blue = std::make_shared<Blue>();
  Circle blueCircle = Circle(10, blue);
  Rectangle blueRectangle = Rectangle(10, 10, blue);

  redCircle.draw();
  blueCircle.draw();
  redRectangle.draw();
  blueRectangle.draw();
}

} // namespace cvtest::tester
