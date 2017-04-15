# fast_mutex

目前支持x86/x64指令集，gcc/msvc编译器

测试代码：

```
#include <stdio.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <iostream>
#include "../../mutex.hpp"

br::mutex m;
int num;

std::mutex sm;
std::shared_mutex ssm;

int tmp;

const int num_threads = 12;
const int num_times = 1000000;

bool stop_read = false;

void test1()
{
    std::vector<std::thread> threads;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(std::thread([]() {
            for (int i = 0; i < num_times; i++) {
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


void test10()
{
    stop_read = false;
    std::vector<std::thread> read_threads;
    for (int i = 0; i < num_threads; i++) {
        read_threads.emplace_back(std::thread([]() {
            while (!stop_read) {
                br::locker lock(m, true);
                tmp = num;
            }
        }));
    }

    std::vector<std::thread> threads;
    
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(std::thread([]() {
            for (int i = 0; i < num_times; i++) {
                br::locker lock(m);
                num += 1;
            }
        }));
    }

    for (auto& t : threads) {
        t.join();
    }
    stop_read = true;

    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << num << "\tcost: " << duration.count() << "\t"
        << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
        << std::endl;

    for (auto& t : read_threads) {
        t.join();
    }
}


void test2()
{
    std::vector<std::thread> threads;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(std::thread([]() {
            for (int i = 0; i < num_times; i++) {
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

void test20()
{
    stop_read = false;
    std::vector<std::thread> read_threads;
    for (int i = 0; i < num_threads; i++) {
        read_threads.emplace_back(std::thread([]() {
            while (!stop_read) {
                std::shared_lock<std::shared_mutex> lock(ssm);
                tmp = num;
            }
        }));
    }

    std::vector<std::thread> threads;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(std::thread([]() {
            for (int i = 0; i < num_times; i++) {
                std::unique_lock<std::shared_mutex> lock(ssm);
                num += 1;
            }
        }));
    }

    for (auto& t : threads) {
        t.join();
    }
    stop_read = true;
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << num << "\tcost: " << duration.count() << "\t"
        << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
        << std::endl;


    for (auto& t : read_threads) {
        t.join();
    }
}

void test3()
{
    std::vector<std::thread> threads;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(std::thread([]() {
            for (int i = 0; i < num_times; i++) {
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

void test30()
{
    stop_read = false;
    std::vector<std::thread> read_threads;
    for (int i = 0; i < num_threads; i++) {
        read_threads.emplace_back(std::thread([]() {
            while (!stop_read) {
                std::shared_lock<std::shared_mutex> lock(ssm);
                tmp = num;
            }
        }));
    }

    std::vector<std::thread> threads;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(std::thread([]() {
            while (!stop_read) {
                std::lock_guard<std::shared_mutex> lock(ssm);
                num += 1;
            }
        }));
    }

    for (auto& t : threads) {
        t.join();
    }
    stop_read = true;
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << num << "\tcost: " << duration.count() << "\t"
        << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
        << std::endl;

    for (auto& t : read_threads) {
        t.join();
    }
}

int main()
{
    std::cout << "=============test unique lock..." << std::endl;
    test1();
    test2();
    test3();

    std::cout << "=============test read/write lock..." << std::endl;
    test10();
    test20();
    test30();
}

```

输出：

x86 Release -o2
```
=============test unique lock...
12000000        cost: 335019    0.335019
24000000        cost: 870049    0.870049
36000000        cost: 868049    0.868049
=============test read/write lock...
48000000        cost: 2179124   2.17912
60000000        cost: 1174067   1.17407
...             ∞               ∞

```

x64 Release -o2
```
=============test unique lock...
12000000        cost: 340019    0.340019
24000000        cost: 1705097   1.7051
36000000        cost: 1497085   1.49708
=============test read/write lock...
48000000        cost: 2297131   2.29713
60000000        cost: 5167295   5.1673
...             ∞               ∞
```