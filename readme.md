# fast_mutex

目前仅支持windows平台，有时间要做成跨平台的。

测试代码：

```

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
```

输出：

x86 Release -o2
```
1000000 cost: 20001     0.020001
2000000 cost: 47002     0.047002
3000000 cost: 47002     0.047002
```

x64 Release -o2
```
1000000 cost: 20001     0.020001
2000000 cost: 142008    0.142008
3000000 cost: 132007    0.132007
```