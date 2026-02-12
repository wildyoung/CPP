#include <iostream>
#include <thread>
#include <vector>
#include <latch>

using namespace std;

int main() {

    const int N = 5;
    latch startLatch(N);

    vector<thread> threads;

    for (int i = 0; i < N; ++i) {
        threads.emplace_back([i, &startLatch]() {

            cout << "Thread " << i << " initializing...\n";
            this_thread::sleep_for(chrono::milliseconds(100 * i));

            // 준비 완료
            startLatch.count_down();

            // 모든 스레드가 준비될 때까지 기다림
            startLatch.wait();

            cout << "Thread " << i << " started working!\n";
            });
    }

    for (auto& t : threads)
        t.join();

    return 0;
}
