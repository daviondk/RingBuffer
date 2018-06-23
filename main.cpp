#include <iostream>
#include "RingBuffer.h"

int main() {
    //Tester tester;
    //tester.startListen();

    RingBuffer<int> buf(5);
    buf.push_front(5);
    buf.push_front(3);
    buf.push_front(4);
    std::cout << *std::next(buf.rbegin(), 2);

    //for (const auto e : buf) {printf("%d ", e);}
    return 0;
}