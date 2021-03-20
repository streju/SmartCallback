#include <iostream>
#include <functional>

#include "SmartCallback.hpp"

void f1()
{
    std::cout << "f1" << std::endl;
}

int f2(int c)
{
    return c*10;
}

void f3(SmartCallback<int(int)> cb, unsigned previousRefCounter)
{
    assert(cb.useCount() == ++previousRefCounter);
    std::cout << cb(3) << std::endl;
}

void f4(SmartCallback<void()>& cb, unsigned previousRefCounter)
{
    assert(cb.useCount() == previousRefCounter);
    cb();
}

int main()
{
    SmartCallback<void()> callback = []{f1();};
    assert(callback.useCount() == 1);
    
    auto cb2 = callback;
    assert (cb2.useCount() == 2 && callback.useCount() == 2);

    cb2();

    assert(callback.wasCalled());
    assert(cb2.wasCalled());

    SmartCallback<int(int)> cb3 = std::bind(f2, std::placeholders::_1);
    SmartCallback<int(int)> cb4 = std::bind(f2, std::placeholders::_1);
    cb4(4);

    cb4 = cb3;
    assert(cb4.useCount() == 2 && cb3.useCount() == 2);
    assert(!cb4.wasCalled());

    f3(cb4, cb4.useCount());
    assert(cb4.useCount() == 2 && cb3.useCount() == 2);
    assert(cb4.wasCalled());

    callback = SmartCallback<void()>{[](){}};

    assert(callback.useCount() == 1);
    assert(callback.isValid());
    assert(!callback.wasCalled());
    assert(cb2.useCount() == 1);
    cb2 = std::move(callback); // previous cb2 function ref counter == 0, no error as function was called

    assert(callback.useCount() == 0);
    assert(!callback.isValid());
    assert(cb2.isValid());
    assert(!callback.wasCalled());
    f4(cb2, cb2.useCount());
    // callback(); // assert error "There is no callback!"
    
    auto cb5 = std::move(cb2);
    assert(cb2.useCount() == 0);
    assert(cb5.useCount() == 1);
    assert(!cb2.isValid());
    assert(cb5.isValid());
    assert(cb5.wasCalled());

    // SmartCallback<void()> cb6{nullptr}; // should not compile
}