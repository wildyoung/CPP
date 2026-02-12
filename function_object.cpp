#include <iostream>
#include <thread>
#include <functional> // std::ref

using namespace std;

class Counter
{
public:
	Counter(int id, int numIterations)
		: m_id(id), m_numIterations(numIterations) {}

	void operator()() const
	{
		for (int i{ 0 }; i < m_numIterations; ++i) {
			cout << "Counter " << m_id << " has value " << i << endl;
		}
	}

private:
	int m_id;
	int m_numIterations;
};

int main()
{
	// 1) thread 생성 시 임시 객체를 바로 넘기는 방법 (가장 흔함)
	thread t1{ Counter{1, 20} };

	// 2) 미리 객체를 만들어두고 thread에 전달하는 방법
	Counter c{ 2, 12 };

	// 주의: 이 방식은 기본적으로 'c를 복사해서' 스레드가 사용함
	thread t2{ c };

	// (참고) c 자체를 "복사 없이" 스레드에서 쓰고 싶으면 ref로 참조 전달
	// thread t2{ std::ref(c) };

	t1.join();
	t2.join();

	return 0;
}
