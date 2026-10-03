#include "tester.h"
#include "gtest/gtest.h"
#include <iostream>
#include <string>

/**
 * 类和对象
 * */

namespace cvtest::tester {
TEST_F(Tester, ClassTest) {
  class Buffer {
  private:
    int *data_;
    size_t size_;

  public:
    Buffer(size_t s) : size_(s) {
      data_ = new int[size_];
      std::cout << "Constructor allocated " << size_ << " ints" << "\n";
    }

    Buffer(const Buffer &other) : size_(other.size_) {
      /**
       * 拷贝构造函数
       * Buffer b2(b1);
       * */
      data_ = new int[size_];
      std::copy(other.data_, other.data_ + size_, data_);
      std::cout << "Copy constructor: deep copied " << size_ << " ints" << "\n";
    }

    Buffer(Buffer &&other) noexcept : data_(other.data_), size_(other.size_) {
      /**
       * 移动构造函数
       * Buffer b2(std::move(b1));
       * */
      other.data_ = nullptr;
      other.size_ = 0;
      std::cout << "Move constructor: transferred " << size_ << " ints" << "\n";
    }

    Buffer &operator=(const Buffer &other) {
      /**
       * 拷贝赋值运算符
       * b2 = b1;
       * */
      if (this == &other)
        return *this;
      delete[] data_;
      size_ = other.size_;
      if (size_ > 0) {
        data_ = new int[size_];
        std::copy(other.data_, other.data_ + size_, data_);
      } else {
        data_ = nullptr;
      }
      std::cout << "Copy assignment operator called " << "\n";
      return *this;
    }

    Buffer &operator=(Buffer &&other) noexcept {
      /**
       * 移动赋值运算符
       * b2 = std::move(b1);
       * */
      if (this == &other)
        return *this;
      delete[] data_;
      data_ = other.data_;
      size_ = other.size_;
      other.data_ = nullptr;
      other.size_ = 0;
      std::cout << "Move assignment operator called " << "\n";
      return *this;
    }

    ~Buffer() {
      if (data_ != nullptr) {
        delete[] data_;
        std::cout << "Destructor: freed " << size_ << " ints" << "\n";
      }
    }

    void fill(int value) {
      for (size_t i = 0; i < size_; i++) {
        data_[i] = value + i;
      }
    }

    void show() {
      for (size_t i = 0; i < size_; i++) {
        std::cout << "index " << i << " = " << data_[i] << ",";
      }
      std::cout << "\n";
    }
  };

  int size = 5;
  Buffer b1(size);
  b1.fill(10);
  b1.show();

  // 移动构造函数
  Buffer b2(std::move(b1));
  b2.fill(20);
  b2.show();

  // 拷贝构造函数
  Buffer b3 = b2;
  b3.fill(30);
  b3.show();

  Buffer b4(size);
  // 拷贝赋值运算符
  b4 = b3;

  b3.show();
  b4.show();

  // 移动赋值运算符
  b4 = std::move(b3);
}
} // namespace cvtest::tester
