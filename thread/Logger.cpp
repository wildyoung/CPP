#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <format>

using namespace std;
using namespace std::chrono_literals;

class Logger {
public:
    explicit Logger(const string& filename = "log.txt")
        : m_exit(false), m_logfile(filename, ios::out | ios::app)
    {
        if (!m_logfile.is_open()) {
            throw runtime_error("Failed to open log file.");
        }

        // 백그라운드 스레드 시작
        m_worker = thread(&Logger::processEntries, this);
    }

    // 복사 금지(뮤텍스/스레드/파일 핸들 보유)
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // 이동도 단순화를 위해 금지(원하면 구현 가능)
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    ~Logger() {
        {
            lock_guard<mutex> lock(m_mutex);
            m_exit = true;
        }
        m_cond.notify_all();

        if (m_worker.joinable())
            m_worker.join();
    }

    void log(string entry) {
        {
            lock_guard<mutex> lock(m_mutex);
            m_queue.push(move(entry));
        }
        // 로그가 들어왔으니 백그라운드 스레드 깨우기
        m_cond.notify_one();
    }

private:
    void processEntries() {
        queue<string> localQueue;

        while (true) {
            {
                unique_lock<mutex> lock(m_mutex);

                //  조건 기반 wait: (큐가 비어있고 종료도 아니면) 계속 대기
                m_cond.wait(lock, [&] {
                    return m_exit || !m_queue.empty();
                    });

                // 종료 요청 + 남은 로그 없음 → 종료
                if (m_exit && m_queue.empty())
                    break;

                //  락 점유 최소화: 통째로 swap 후 락 해제
                localQueue.swap(m_queue);
            }

            // 락 밖에서(다른 스레드 log() 방해 없이) 파일에 기록
            while (!localQueue.empty()) {
                m_logfile << localQueue.front() << "\n";
                localQueue.pop();
            }

            // 필요하면 flush(성능 vs 안정성 트레이드오프)
            m_logfile.flush();
        }
    }

private:
    bool m_exit;
    ofstream m_logfile;

    mutex m_mutex;
    condition_variable m_cond;
    queue<string> m_queue;

    thread m_worker;
};

// ----------------- 사용 예제 -----------------
void logSomeMessages(int id, Logger& logger) {
    for (int i = 0; i < 10; ++i) {
        logger.log(format("Log entry {} from thread {}", i, id));
        this_thread::sleep_for(10ms);
    }
}

int main() {
    try {
        Logger logger("log.txt");

        vector<thread> threads;
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back(logSomeMessages, i, ref(logger));
        }

        for (auto& t : threads) t.join();

        // main 종료 시 logger 소멸자에서 백그라운드 스레드 안전 종료 + 남은 로그 처리
        cout << "All worker threads finished. Check log.txt\n";
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }
}
