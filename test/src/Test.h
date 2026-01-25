#ifndef TEST_H
#define TEST_H

#include <iostream>

class Test {
public:
    Test();
    ~Test();

    void SayTestMessage();
    void SayHello() { std::cout << "Hello from Test class!" << std::endl; }
};

#endif