/**
 *
 * 代理模式
 * 代理类与被代理的类实现相同的接口，客户端持有代理调用实际被代理类的功能
 *
 * */
#include "tester.h"
#include <iostream>
#include <map>
#include <memory>

namespace cvtest::tester {
class DBInterface {
public:
  DBInterface() = default;
  virtual ~DBInterface() = default;
  virtual std::string getValue(std::string key) = 0;
  virtual void setValue(std::string key, std::string value) = 0;
};

class FirstDB : public DBInterface {
public:
  std::string getValue(std::string key) {
    std::cout << "Get value in FirstDB \n";
    return db_.at(key);
  }
  void setValue(std::string key, std::string value) {
    std::cout << "Set value in FirstDB\n";
    db_.insert({key, value});
  }

private:
  std::map<std::string, std::string> db_;
};

class SecondDB : public DBInterface {
public:
  std::string getValue(std::string key) {
    std::cout << "Get value in SecondDB \n";
    return db_.at(key);
  }
  void setValue(std::string key, std::string value) {
    std::cout << "Set value in SecondDB\n";
    db_.insert({key, value});
  }

private:
  std::map<std::string, std::string> db_;
};

class DBProxy : public DBInterface {
public:
  DBProxy(std::unique_ptr<DBInterface> db) : db_(std::move(db)) {}

  std::string getValue(std::string key) { return db_->getValue(key); }
  void setValue(std::string key, std::string value) { db_->setValue(key, value); }

private:
  std::unique_ptr<DBInterface> db_;
};

TEST_F(Tester, DIDP_Proxy_Tester) {
  // DBProxy proxy(std::make_unique<FirstDB>());
  DBProxy proxy(std::make_unique<SecondDB>());

  proxy.setValue("name", "inmove");
  proxy.getValue("name");
}
} // namespace cvtest::tester
