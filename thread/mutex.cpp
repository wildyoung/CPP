#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

// ----------------------------------------------------
// 1) mutex + lock_guard : 기본 패턴
// ----------------------------------------------------
void demo_lock_guard() {
    cout << "\n===== [1] lock_guard demo =====\n";

    int counter = 0;
    mutex m;

    auto work = [&]() {
        for (int i = 0; i < 100000; ++i) {
            lock_guard<mutex> lock(m); // RAII
            ++counter;
        }
        };

    vector<thread> ts;
    for (int i = 0; i < 4; ++i) ts.emplace_back(work);
    for (auto& t : ts) t.join();

    cout << "counter = " << counter << endl;
}

// ----------------------------------------------------
// 2) timed_mutex + unique_lock : 시간 제한 락
// ----------------------------------------------------
void demo_timed_mutex() {
    cout << "\n===== [2] timed_mutex demo =====\n";

    timed_mutex tm;

    // 스레드 A: 락 잡고 오래 점유
    thread a([&]() {
        tm.lock();
        cout << "[A] lock acquired, holding 1s...\n";
        this_thread::sleep_for(1s);
        tm.unlock();
        cout << "[A] unlocked\n";
        });

    // 스레드 B: 200ms만 기다려보고 못 잡으면 포기
    thread b([&]() {
        this_thread::sleep_for(50ms); // A가 먼저 잡게 살짝 지연
        unique_lock<timed_mutex> lk(tm, defer_lock);

        if (lk.try_lock_for(200ms)) {
            cout << "[B] lock acquired within 200ms\n";
        }
        else {
            cout << "[B] failed to acquire lock within 200ms\n";
        }
        });

    a.join();
    b.join();
}

// ----------------------------------------------------
// 3) shared_mutex : reader/writer 락
//    - 읽기 스레드는 여러 개 동시 가능
//    - 쓰기 스레드는 단독(독점)
// ----------------------------------------------------
shared_mutex rw_m;
int shared_data = 0;

void reader(int id) {
    shared_lock<shared_mutex> lock(rw_m); // 공유 락(읽기 락)
    cout << "[R" << id << "] read shared_data = " << shared_data << endl;
    this_thread::sleep_for(100ms);
}

void writer(int id) {
    unique_lock<shared_mutex> lock(rw_m); // 독점 락(쓰기 락)
    ++shared_data;
    cout << "[W" << id << "] write shared_data = " << shared_data << endl;
    this_thread::sleep_for(200ms);
}

void demo_shared_mutex() {
    cout << "\n===== [3] shared_mutex demo =====\n";

    vector<thread> ts;

    // 읽기 여러 개
    ts.emplace_back(reader, 1);
    ts.emplace_back(reader, 2);

    // 쓰기 하나
    ts.emplace_back(writer, 1);

    // 읽기 추가
    ts.emplace_back(reader, 3);

    for (auto& t : ts) t.join();
}

// ----------------------------------------------------
// 4) scoped_lock : 여러 mutex를 "한 번에" 잠그기
//    - 데드락 방지에 유리
// ----------------------------------------------------
mutex m1, m2;
int x = 0, y = 0;

void demo_scoped_lock() {
    cout << "\n===== [4] scoped_lock demo =====\n";

    auto work = [&](int id) {
        // 두 mutex를 한 번에 잠금
        scoped_lock lock(m1, m2);
        ++x; ++y;
        cout << "[T" << id << "] x=" << x << ", y=" << y << endl;
        };

    thread t1(work, 1);
    thread t2(work, 2);
    t1.join();
    t2.join();
}

// ----------------------------------------------------
// 5) call_once : 초기화 1회 보장
// ----------------------------------------------------
once_flag init_flag;

void initialize_shared_resource() {
    cout << ">>> initialize_shared_resource(): called ONCE\n";
}

void demo_call_once() {
    cout << "\n===== [5] call_once demo =====\n";

    auto work = [](int id) {
        call_once(init_flag, initialize_shared_resource);
        cout << "[T" << id << "] working...\n";
        };

    thread t1(work, 1);
    thread t2(work, 2);
    thread t3(work, 3);

    t1.join(); t2.join(); t3.join();
}

// ----------------------------------------------------
int main() {
    demo_lock_guard();
    demo_timed_mutex();
    demo_shared_mutex();
    demo_scoped_lock();
    demo_call_once();
    return 0;
}
