/**
 *
 * 组合模式
 * Also known as: Object Tree
 *
 * 结构化设计模式
 * 用于组合树型结构的元素对象
 *
 * 比如目录树，订单
 *
 * */
#include "tester.h"
#include <iostream>
#include <memory>

namespace cvtest::tester {
/**
 * 所有元素都要遵守的规则
 * */
class Component {
public:
  virtual ~Component() = default;
  virtual void print(std::ostream &os, int depth = 0) const = 0;
  virtual std::uintmax_t size() const = 0;
  virtual const std::string &name() const = 0;
};

// Leaf
class File final : public Component {
public:
  File(std::string name, std::uintmax_t bytes) : name_(std::move(name)), bytes_(bytes) {}

  void print(std::ostream &os, int depth) const override {
    os << std::string(depth * 2, ' ') << "- " << name_ << " (" << bytes_ << " bytes)\n";
  }

  std::uintmax_t size() const override { return bytes_; }
  const std::string &name() const override { return name_; }

private:
  std::string name_;
  std::uintmax_t bytes_;
};

class Directory final : public Component {
public:
  explicit Directory(std::string name) : name_(std::move(name)) {}

  Directory &add(std::unique_ptr<Component> child) {
    children_.push_back(std::move(child));
    return *this;
  }

  bool remove(const Component *target) {
    auto it =
        std::find_if(children_.begin(), children_.end(),
                     [target](const std::unique_ptr<Component> &p) { return p.get() == target; });
    if (it == children_.end())
      return false;
    children_.erase(it);
    return true;
  }

  std::size_t childCount() const { return children_.size(); }

  void print(std::ostream &os, int depth) const override {
    os << std::string(depth * 2, ' ') << "+ " << name_ << "/ " << "[" << size() << " bytes]\n";
    for (const auto &child : children_) {
      child->print(os, depth + 1);
    }
  }

  std::uintmax_t size() const override {
    std::uintmax_t total = 0;
    for (const auto &child : children_) {
      total += child->size();
    }
    return total;
  }

  const std::string &name() const override { return name_; }

private:
  std::string name_;
  std::vector<std::unique_ptr<Component>> children_;
};

static void render(const Component &node) { node.print(std::cout, 0); }

TEST_F(Tester, DIDP_Composite_Tester) {
  auto root = std::make_unique<Directory>("Root");
  auto src = std::make_unique<Directory>("src");
  src->add(std::make_unique<File>("Main.cpp", 2048))
      .add(std::make_unique<File>("Utils.cpp", 1024))
      .add(std::make_unique<File>("Utils.h", 512));
  root->add(std::move(src));
  render(*root);
}

} // namespace cvtest::tester
