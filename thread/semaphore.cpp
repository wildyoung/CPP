#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

int main() {
    // 동시에 실행 가능한 "슬롯" 3개
    counting_semaphore<3> sem(3);

    vector<jthread> threads;
    threads.reserve(10);

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([i, &sem]() {
            sem.acquire(); // 슬롯 확보(없으면 대기)

            cout << "[T" << i << "] entered (working)\n";
            this_thread::sleep_for(500ms); // 작업 시뮬레이션
            cout << "[T" << i << "] leaving\n";

            sem.release(); // 슬롯 반환
            });
    }

    return 0; // jthread 자동 join
}
