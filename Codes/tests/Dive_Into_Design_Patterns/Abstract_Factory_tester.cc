/**
 *
 * 抽象工厂
 *
 * */
#include "tester.h"
#include <iostream>
#include <memory>
namespace cvtest::tester {

class Chair {
public:
  virtual ~Chair() = default;
  virtual bool hasLegs() = 0;
  virtual bool sitOn() = 0;
};

class VictorianChair : public Chair {
public:
  bool hasLegs() {
    std::cout << "VictorianChair has legs \n";
    return true;
  };

  bool sitOn() {
    std::cout << "VictorianChair sit on \n";
    return true;
  }
};

class ModernChair : public Chair {
public:
  bool hasLegs() {
    std::cout << "ModernChair has no legs \n";
    return false;
  };

  bool sitOn() {
    std::cout << "ModernChair sit on \n";
    return false;
  }
};

class Table {
public:
  virtual ~Table() = default;
  virtual std::string shape() = 0;
};

class VictorianTable : public Table {
public:
  std::string shape() {
    std::cout << "Shape of Victorian table is square \n";
    return "square";
  }
};

class ModernTable : public Table {
public:
  std::string shape() {
    std::cout << "Shape of Modern table is circle \n";
    return "circle";
  }
};

class FurnitureFactory {
public:
  virtual ~FurnitureFactory() = default;
  virtual std::unique_ptr<Chair> createChair() = 0;
  virtual std::unique_ptr<Table> createTable() = 0;
};

class VictorianFurnitureFactory : public FurnitureFactory {
public:
  std::unique_ptr<Chair> createChair() override { return std::make_unique<VictorianChair>(); }
  std::unique_ptr<Table> createTable() override { return std::make_unique<VictorianTable>(); };
};

class ModernFurnitureFactory : public FurnitureFactory {
public:
  std::unique_ptr<Chair> createChair() override { return std::make_unique<ModernChair>(); }
  std::unique_ptr<Table> createTable() override { return std::make_unique<ModernTable>(); };
};

void createFurnitures(std::unique_ptr<FurnitureFactory> factory) {
  auto chair = factory->createChair();
  auto table = factory->createTable();
  chair->sitOn();
  chair->hasLegs();
  table->shape();
}

TEST_F(Tester, DIDP_AbstractFactoryTester) {
  // 使用维多利亚工厂
  std::unique_ptr<FurnitureFactory> vFactory = std::make_unique<VictorianFurnitureFactory>();
  // auto vChair = vFactory->createChair();
  // auto vTable = vFactory->createTable();
  // EXPECT_TRUE(vChair->hasLegs());
  // EXPECT_TRUE(vChair->sitOn());
  createFurnitures(std::move(vFactory));

  // // 使用现代工厂
  std::unique_ptr<FurnitureFactory> mFactory = std::make_unique<ModernFurnitureFactory>();
  // auto mChair = mFactory->createChair();
  // auto mTable = mFactory->createTable();
  // EXPECT_FALSE(mChair->hasLegs());
  // EXPECT_FALSE(mChair->sitOn());
  createFurnitures(std::move(mFactory));
}
} // namespace cvtest::tester
