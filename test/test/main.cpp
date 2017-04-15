#include <stdio.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <iostream>

#ifndef _BR_MUTEX_LOCKER_H_
#define _BR_MUTEX_LOCKER_H_
#include <windows.h>

namespace br {
struct mutex
{
    volatile LONG sem;
    volatile bool askwrite;
    mutex() :sem(0), askwrite(false) {}
};

class locker
{
public:
    locker(mutex& m, bool shared = false) : _m(m), _write(!shared) {
        if (_write) { // big brother
            while (InterlockedCompareExchange(&_m.sem, -1, 0) != 0) { //someone taking my toy
                _m.askwrite = true; // I need the toy right now!!!
                Sleep(1); // wait
            }
            _m.askwrite = false; // your buddies can competing for it now
        } else {
            while (1) {
                if (_m.askwrite) { // the big brother is looking for it, stand by
                    Sleep(1);
                } else if (InterlockedExchangeAdd(&_m.sem, 2) <= -1) { // big brother is enjoying
                    InterlockedExchangeAdd(&_m.sem, -2); // sorry, I'm not mean it, step back
                    Sleep(1);
                } else { // we can share it now
                    break;
                }
            };
        }
    }
    ~locker() {
        if (_write) {
            InterlockedExchangeAdd(&_m.sem, 1);
        } else {
            InterlockedExchangeAdd(&_m.sem, -2);
        }
    }
private:
    mutex& _m;
    bool _write;
};
}

#endif

br::mutex m;
int num;

std::mutex sm;

void test1()
{
    std::vector<std::thread> threads;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < 10; i++) {
        threads.emplace_back(std::thread([]() {
            for (int i = 0; i < 100000; i++) {
                br::locker lock(m);
                num += 1;
            }
        }));
    }
    for (auto& t : threads) {
        t.join();
    }
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << num << "\tcost: " << duration.count() << "\t"
        << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
        << std::endl;

}

void test2()
{
    std::vector<std::thread> threads;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < 10; i++) {
        threads.emplace_back(std::thread([]() {
            for (int i = 0; i < 100000; i++) {
                std::unique_lock<std::mutex> lock(sm);
                num += 1;
            }
        }));
    }
    for (auto& t : threads) {
        t.join();
    }
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << num << "\tcost: " << duration.count() << "\t"
        << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
        << std::endl;
}


void test3()
{
    std::vector<std::thread> threads;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < 10; i++) {
        threads.emplace_back(std::thread([]() {
            for (int i = 0; i < 100000; i++) {
                std::lock_guard<std::mutex> lock(sm);
                num += 1;
            }
        }));
    }
    for (auto& t : threads) {
        t.join();
    }
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << num << "\tcost: " << duration.count() << "\t"
        << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
        << std::endl;
}

int main()
{
    test1();
    test2();
    test3();
}
