/**
 *
 * 创建者模式
 *
 * */
#include "tester.h"
#include <iostream>
#include <memory>
namespace cvtest::tester {
class Computer {
public:
  void setCPU(const std::string &cpu) { cpu_ = cpu; }
  void setRAM(const std::string &ram) { ram_ = ram; }
  void setHDD(const std::string &hdd) { hdd_ = hdd; }
  void setGPU(const std::string &gpu) { gpu_ = gpu; }

  void display() {
    std::cout << "Computer: \n"
              << "\t CPU: " << cpu_ << "\n"
              << "\t RAM: " << ram_ << "\n"
              << "\t HDD: " << hdd_ << "\n"
              << "\t GPU: " << gpu_ << "\n";
  }

private:
  std::string cpu_;
  std::string ram_;
  std::string hdd_;
  std::string gpu_;
};

class ComputerBuilder {
public:
  virtual ~ComputerBuilder() = default;
  virtual void buildCPU() = 0;
  virtual void buildRAM() = 0;
  virtual void buildHDD() = 0;
  virtual void buildGPU() = 0;

  virtual std::unique_ptr<Computer> getResult() = 0;
};

class GamingComputerBuilder : public ComputerBuilder {
public:
  GamingComputerBuilder() { computer_ = std::make_unique<Computer>(); }

  void buildCPU() override { computer_->setCPU("intel core i9-13900k"); }
  void buildRAM() override { computer_->setRAM("32GB DDR5 6000HZ"); }
  void buildHDD() override { computer_->setHDD("1TB NVMe SSD"); }
  void buildGPU() override { computer_->setGPU("NVIDIA RTX 4090"); }

  std::unique_ptr<Computer> getResult() override { return std::move(computer_); }

private:
  std::unique_ptr<Computer> computer_;
};

class OfficeComputerBuilder : public ComputerBuilder {
public:
  OfficeComputerBuilder() { computer_ = std::make_unique<Computer>(); }

  void buildCPU() override { computer_->setCPU("Inter core i5-13400"); }
  void buildRAM() override { computer_->setRAM("16GB DDR4 3000HZ"); }
  void buildHDD() override { computer_->setHDD("512GB SATA SSD"); }
  void buildGPU() override { computer_->setGPU("Intel UHD"); }

  std::unique_ptr<Computer> getResult() override { return std::move(computer_); }

private:
  std::unique_ptr<Computer> computer_;
};

class ComputerDirector {
public:
  void setBuilder(std::unique_ptr<ComputerBuilder> builder) { builder_ = std::move(builder); }
  void construct() {
    if (builder_) {
      builder_->buildCPU();
      builder_->buildRAM();
      builder_->buildHDD();
      builder_->buildGPU();
    }
  }
  std::unique_ptr<Computer> getComputer() {
    if (builder_) {
      return builder_->getResult();
    }
    return nullptr;
  }

private:
  std::unique_ptr<ComputerBuilder> builder_;
};

TEST_F(Tester, DIDP_Builder_Tester) {
  ComputerDirector director;
  director.setBuilder(std::make_unique<GamingComputerBuilder>());
  director.construct();
  auto gamingPC = director.getComputer();
  if (gamingPC) {
    gamingPC->display();
  }
}

} // namespace cvtest::tester
