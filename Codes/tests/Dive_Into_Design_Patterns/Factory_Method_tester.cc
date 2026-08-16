#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
#include <memory>
using namespace std::chrono_literals;

/**
 * 工厂方法设计模式
 * */

namespace cvtest::tester {

class Transport {
public:
  virtual ~Transport() = default;
  virtual void deliver() const = 0;
};

class Truck : public Transport {
public:
  void deliver() const override { std::cout << "Truck deliver, highway road to delive\n"; }
};

class Ship : public Transport {
public:
  void deliver() const override { std::cout << "Ship deliver, sea road to delive\n"; }
};

class Logistics {
public:
  virtual ~Logistics() = default;
  void planDelivery() const {
    std::unique_ptr<Transport> t = createTransport();
    t->deliver();
  }

  /**
   * 将 createTransport 推迟到子类去实现
   * */
  virtual std::unique_ptr<Transport> createTransport() const = 0;
};

class RoadLogistics : public Logistics {
public:
  std::unique_ptr<Transport> createTransport() const override { return std::make_unique<Truck>(); }
};

class SeaLogistics : public Logistics {
public:
  std::unique_ptr<Transport> createTransport() const override { return std::make_unique<Ship>(); };
};

TEST_F(Tester, DIDP_FactoryMethodTester) {
  // 此处并没有出现 Truck以及Ship
  std::unique_ptr<Logistics> roadLog = std::make_unique<RoadLogistics>();
  std::unique_ptr<Logistics> seaLog = std::make_unique<SeaLogistics>();

  roadLog->planDelivery();
  seaLog->planDelivery();
}
} // namespace cvtest::tester
