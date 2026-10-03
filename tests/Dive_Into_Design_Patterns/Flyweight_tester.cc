/**
 *
 * 享元模式
 *
 * 结构模式
 *
 * 一个游戏中，只有三种子弹，它的颜色和速度是固定的，但是它们生成的坐标是随机的
 * 这种情况下，颜色和速度可以在所有子弹中共享
 *
 * */

#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

namespace cvtest::tester {
class Bullet {
private:
  std::string color_{};
  std::int32_t speed_{100};

public:
  Bullet(std::string color, std::int32_t speed) : color_(color), speed_(speed) {}

  void print() const { std::cout << "Bullet " << color_ << " speed " << speed_ << " "; }
};

class Coord {
private:
  int32_t x_{0};
  int32_t y_{0};

public:
  Coord(int32_t x, int32_t y) : x_(x), y_(y) {}

  void print() const { std::cout << "(x, y) = (" << x_ << "," << y_ << ")\n"; }
};

void draw_bullet(const Bullet &bullet, Coord coord) {
  bullet.print();
  coord.print();
}

TEST_F(Tester, DIDP_Flyweight_Tester) {
  Bullet redBullet{"Red", 100};
  Bullet blueBullet("Blue", 200);
  Bullet greenBullet("Green", 50);
  std::vector<Bullet> bullets{redBullet, blueBullet, greenBullet};

  for (int i = 0; i < 200; i++) {
    Coord coord(i, i + 100);
    draw_bullet(bullets[i % 3], coord);
  }
}

} // namespace cvtest::tester
