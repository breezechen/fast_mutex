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
