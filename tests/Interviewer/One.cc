/**
 *
 * 请谈谈 C++ 中的值类别（value categories）：C++17 之后标准把表达式分为 glvalue / rvalue，再细分为
 * lvalue、xvalue、prvalue。请说明：
 *   1. 这几种值类别各自的定义和典型例子；
 *   2. std::move 和 std::forward 分别在做什么，本质区别是什么；
 *   3. 结合值类别解释：为什么"移动语义"能提高性能，以及 std::move
 * 本身并不移动任何东西这句话怎么理解。
 *
 * glvalue: 广义左值
 * rvalue: 右值
 *
 * lvalue: 左值，有身份，不可移动
 * xvalue: 将亡值，有身份，可移动
 * prvalue: 纯右值，无身份，可移动
 *
 * std::move 无条件的将表达式转成右值，只有标记，并不会做移动操作
 * std::forward 有条件地保持原来的值类别，用于完美转发
 * */
