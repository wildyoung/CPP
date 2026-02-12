#include <iostream>
#include <thread>

using namespace std;

// 스레드가 실행할 함수
void counter(int id, int numIterations)
{
	for (int i{ 0 }; i < numIterations; ++i) {
		cout << "Counter " << id << " has value " << i << endl;
	}
}
int main()
{
	// 스레드 생성
	thread t1{ counter, 1, 6 };  // counter(1, 6)
	thread t2{ counter, 2, 4 };  // counter(2, 4)

	// 스레드 종료 대기
	t1.join();
	t2.join();

	return 0;
}
