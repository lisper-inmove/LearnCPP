/**
 *
 * 装饰器模式
 * Also known as: Wrapper
 *
 * 结构化设计模式
 *  为对象增加新的功能
 *
 *  接口类型为 A
 *  它有一个子类 A1
 *  装饰器基类D，实现A，持有一个A的子类对象，wrapee
 *  它有两个子类 D1, D2。它们都有 接口A的函数实现
 *    对于某一个函数，先执行指令，然后调用 wrapee 的对应函数
 *
 *  a = A1();
 *  a = D1(a);
 *  a = D2(a);
 *
 *  a.do_something()
 *
 * */
#include "tester.h"
#include <cstddef>
#include <iostream>
#include <memory>

namespace cvtest::tester {

class DataSource {
public:
  virtual ~DataSource() = default;
  virtual void writeData(std::string data) = 0;
  virtual std::string readData() const = 0;
};

class FileDataSource : public DataSource {
private:
  std::string filepath_;
  std::string data_{};

public:
  FileDataSource(std::string &filepath) : filepath_(std::move(filepath)) {};
  void writeData(std::string data) {
    std::cout << "write data `" << data << "` to " << filepath_ << "\n";
    data_ = std::move(data);
  }
  std::string readData() const { return data_; }
};

class DataSourceDecorator : public DataSource {
protected:
  std::shared_ptr<DataSource> wrapee_;

public:
  DataSourceDecorator(std::shared_ptr<DataSource> wrapee) : wrapee_(wrapee) {}
  virtual void writeData(std::string data) = 0;
  virtual std::string readData() const = 0;
};

class EncryptionDataSourceDecorator : public DataSourceDecorator {
private:
  std::string header_ = "Encrypted: ";

public:
  EncryptionDataSourceDecorator(std::shared_ptr<DataSource> wrapee) : DataSourceDecorator(wrapee) {}
  void writeData(std::string data) {
    std::string encryptedData = header_ + data;
    wrapee_->writeData(encryptedData);
  }

  std::string readData() const {
    std::string encryptedData = wrapee_->readData();
    std::size_t size = encryptedData.size() - header_.size();
    std::string data = encryptedData.substr(header_.size(), size);
    return data;
  };
};

class CompressDataSourceDecorator : public DataSourceDecorator {
private:
  std::string header_ = "Compressed: ";

public:
  CompressDataSourceDecorator(std::shared_ptr<DataSource> wrapee) : DataSourceDecorator(wrapee) {}
  void writeData(std::string data) {
    std::string compressedData = header_ + data;
    wrapee_->writeData(compressedData);
  }

  std::string readData() const {
    std::string compressedData = wrapee_->readData();
    std::size_t size = compressedData.size() - header_.size();
    std::string data = compressedData.substr(header_.size(), size);
    return data;
  };
};

TEST_F(Tester, DIDP_Decorator_Tester) {
  std::string value = "Hello World!";
  std::string filepath = "/tmp/tmp.data";

  std::shared_ptr<DataSource> f = std::make_shared<FileDataSource>(filepath);

  f = std::make_shared<EncryptionDataSourceDecorator>(f);
  f = std::make_shared<CompressDataSourceDecorator>(f);

  f->writeData(value);
  std::cout << f->readData() << "\n";
}

} // namespace cvtest::tester
