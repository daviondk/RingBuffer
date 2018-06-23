#include <iostream>
#include "RingBuffer.h"

int main() {
    //Tester tester;
    //tester.startListen();

    RingBuffer<int> buf(5);
    const RingBuffer<int> buf1(5);
    buf.back();
    auto x = buf.begin() + 1;
    buf.push_front(5);
    buf.push_front(3);
    buf.push_front(4);
    std::cout << *std::next(buf.rbegin(), 2);

    //for (const auto e : buf) {printf("%d ", e);}
    return 0;
}

//front back const  struct
// + iterator and number
// iterator == iterator / !=
// const []
// include exception
// swap