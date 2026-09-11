#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
#include <memory>
using namespace std::chrono_literals;

/**
 * 原型模式
 * 原型模式是一种创建型模式。它的作用是使得你能在不依赖某一个类的情况下复制一个对象
 * 一个支持clone的对象，被称做原型
 * */

namespace cvtest::tester {
class Cloneable {
public:
  Cloneable() = default;
  virtual ~Cloneable() = default;
  virtual std::unique_ptr<Cloneable> clone() const = 0;
};

class Car : public Cloneable {
private:
  std::string color_{"Red"};
  int32_t height_{100};
  int32_t width_{100};

public:
  Car(std::string color, int32_t height, int32_t width)
      : color_(color), height_(height), width_(width) {};
  virtual ~Car() = default;
  std::unique_ptr<Cloneable> clone() const { return std::make_unique<Car>(*this); }
};

TEST_F(Tester, DIDP_Cloneable_Tester) {
  Car c = Car("Red", 100, 100);
  auto c2 = c.clone();
  Car *p = dynamic_cast<Car *>(c2.get());
  ASSERT_NE(p, nullptr);
}

template <typename Derived> class Cloneable2 {
public:
  virtual ~Cloneable2() = default;
  std::unique_ptr<Derived> clone() const {
    return std::make_unique<Derived>(static_cast<const Derived &>(*this));
  }
};

// CRTP 泛型clone
class Car2 : public Cloneable2<Car2> {
private:
  std::string color_{"Red"};
  int32_t height_{100};
  int32_t width_{100};

public:
  Car2(std::string color, int32_t height, int32_t width)
      : color_(color), height_(height), width_(width) {};
  virtual ~Car2() = default;
  std::unique_ptr<Cloneable2> clone() const { return std::make_unique<Car2>(*this); }
};
TEST_F(Tester, DIDP_Cloneable2_Tester) {
  Car2 c = Car2("Red", 100, 100);
  auto c2 = c.clone();
  Car2 *p = dynamic_cast<Car2 *>(c2.get());
  ASSERT_NE(p, nullptr);
}

} // namespace cvtest::tester
