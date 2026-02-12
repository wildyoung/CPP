#include <iostream>
#include <thread>

using namespace std;

int k = 0;                // 공유 변수
thread_local int n = 0;   // 스레드 로컬 변수

void threadFunction(int id)
{
	cout << "Thread " << id
		<< ": k=" << k
		<< ", n=" << n << endl;

	++k;
	++n;
}

int main()
{
	thread t1{ threadFunction, 1 };
	thread t2{ threadFunction, 2 };

	t1.join();
	t2.join();

	return 0;
}
