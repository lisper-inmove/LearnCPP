/**
 *
 * 适配器模式
 *
 * */
#include "tester.h"
#include <iostream>
#include <memory>

namespace cvtest::tester {

/**
 * Target 客户端期望的接口
 * */
class Target {
public:
  virtual ~Target() = default;
  virtual void request(const std::string &data) const = 0;
};

/**
 *
 * 已存在的类，但是它的接口并不符合客户端的期望
 * */
class Adaptee {
public:
  void specificRequest(const std::string &data) const {
    std::cout << "Adaptee specific request " << data << "\n";
  }
};

/**
 * 对象适配，组合 Adaptee
 * */
class ObjectAdapter : public Target {
public:
  explicit ObjectAdapter(std::unique_ptr<Adaptee> adaptee) : adaptee_(std::move(adaptee)) {}
  void request(const std::string &data) const override {
    adaptee_->specificRequest("Object Adapter " + data);
  }

public:
  std::unique_ptr<Adaptee> adaptee_;
};

/**
 * 类适配器, 多重继承
 * */
class ClassAdapter : public Target, private Adaptee {
public:
  void request(const std::string &data) const override {
    specificRequest("Class Adapter " + data);
  };
};

void client(const Target &target) { target.request("Hello"); }

TEST_F(Tester, DIDP_Adapter_Tester) {
  ObjectAdapter objectAdapter(std::make_unique<Adaptee>());
  client(objectAdapter);

  ClassAdapter classAdapter;
  client(classAdapter);

  std::unique_ptr<Target> target = std::make_unique<ObjectAdapter>(std::make_unique<Adaptee>());
  target->request("World");
}

} // namespace cvtest::tester
