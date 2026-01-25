#include "Test.h"
#include <iostream>

Test::Test() {}
Test::~Test() {}

void Test::SayTestMessage() {
    std::cout << "This is a test message from the Test class!" << std::endl;
}