#include <iostream>
#include <thread>

using namespace std;

class Request
{
public:
	Request(int id) : m_id{ id } {}

	void process() {
		cout << "Processing request " << m_id << endl;
	}

private:
	int m_id;
};

int main()
{
	Request req{ 100 };

	// 멤버 함수 포인터 + 객체(또는 객체 포인터/참조)를 함께 전달
	thread t{ &Request::process, &req };   // req.process()를 스레드에서 실행

	t.join();
	return 0;
}
