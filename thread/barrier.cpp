#include <iostream>
#include <thread>
#include <vector>
#include <barrier>
#include <atomic>
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

int main() {
    const int N = 4;
    const int ROUNDS = 5;

    atomic<int> phase{ 0 };

    // phase가 끝날 때마다 1번 실행되는 콜백 (정확히 1개의 스레드가 실행)
    auto on_phase_complete = [&]() noexcept {
        int p = phase.fetch_add(1) + 1;
        cout << "---- phase " << p << " complete (callback) ----\n";
        };

    barrier syncPoint(N, on_phase_complete);

    vector<jthread> threads;
    threads.reserve(N);

    for (int id = 0; id < N; ++id) {
        threads.emplace_back([id, &syncPoint]() {
            for (int r = 1; r <= ROUNDS; ++r) {
                // (1) 라운드별 계산(작업 시간 다르게 해서 동기화가 보이도록)
                this_thread::sleep_for((id + 1) * 50ms);
                cout << "[T" << id << "] round " << r << " computed\n";

                // (2) 모든 스레드가 여기까지 오면 다음 라운드로 같이 진행
                syncPoint.arrive_and_wait();

                // (3) 라운드가 끝난 뒤 다음 라운드 준비
                // (여기부터는 모든 스레드가 "동시에" 시작했다고 보면 됨)
            }
            });
    }

    // jthread라서 자동 join
    return 0;
}
