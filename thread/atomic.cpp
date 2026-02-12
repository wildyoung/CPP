#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

using namespace std;

// --------------------------------------------------
// 1. 데이터 경쟁 예제 (문제 발생)
// --------------------------------------------------
void race_condition_demo() {
    cout << "\n===== [1] Race Condition Demo =====\n";

    int counter = 0;

    auto work = [&]() {
        for (int i = 0; i < 100000; ++i) {
            ++counter;  // 데이터 경쟁 발생
        }
        };

    vector<thread> threads;
    for (int i = 0; i < 8; ++i)
        threads.emplace_back(work);

    for (auto& t : threads)
        t.join();

    cout << "Expected: 800000\n";
    cout << "Actual  : " << counter << "\n"; // 보통 800000이 안 나옴
}

// --------------------------------------------------
// 2. atomic 사용 (해결)
// --------------------------------------------------
void atomic_demo() {
    cout << "\n===== [2] Atomic Demo =====\n";

    atomic<int> counter{ 0 };

    auto work = [&]() {
        for (int i = 0; i < 100000; ++i) {
            ++counter;   // 원자적 증가
        }
        };

    vector<thread> threads;
    for (int i = 0; i < 8; ++i)
        threads.emplace_back(work);

    for (auto& t : threads)
        t.join();

    cout << "Expected: 800000\n";
    cout << "Actual  : " << counter.load() << "\n"; // 항상 800000
}

// --------------------------------------------------
// 3. fetch_add 예제
// --------------------------------------------------
void fetch_add_demo() {
    cout << "\n===== [3] fetch_add Demo =====\n";

    atomic<int> value{ 10 };

    cout << "Initial value: " << value << "\n";

    int before = value.fetch_add(4);  // 이전 값 반환

    cout << "Fetched value (before add): " << before << "\n";
    cout << "Value after add: " << value.load() << "\n";
}

// --------------------------------------------------
// 4. compare_exchange (CAS) 예제
// --------------------------------------------------
void compare_exchange_demo() {
    cout << "\n===== [4] compare_exchange Demo =====\n";

    atomic<int> x{ 0 };

    int expected = 0;
    int desired = 42;

    if (x.compare_exchange_strong(expected, desired)) {
        cout << "Success! x = " << x.load() << "\n";
    }
    else {
        cout << "Failed! expected updated to " << expected << "\n";
    }
}

// --------------------------------------------------
// 5. atomic wait / notify (C++20)
// --------------------------------------------------
void wait_notify_demo() {
    cout << "\n===== [5] wait / notify Demo =====\n";

    atomic<int> value{ 0 };

    thread worker([&]() {
        cout << "Worker: waiting...\n";
        value.wait(0);  // 값이 0일 동안 대기
        cout << "Worker: woke up! value = " << value.load() << "\n";
        });

    this_thread::sleep_for(chrono::seconds(2));

    cout << "Main: set value = 1\n";
    value.store(1);
    value.notify_all();   // 대기 중인 스레드 깨움

    worker.join();
}

// --------------------------------------------------
int main() {

    race_condition_demo();
    atomic_demo();
    fetch_add_demo();
    compare_exchange_demo();
    wait_notify_demo();

    return 0;
}
