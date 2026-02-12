#include <iostream>
#include <thread>
#include <exception>
#include <stdexcept>
#include <functional> // std::ref를 사용하기 위함

using namespace std;

// 1. 실제 작업을 수행하며 예외를 던질 수 있는 함수
void doSomeWork() {
	for (int i = 0; i < 5; ++i) {
		cout << i << endl;
	}
	cout << "Thread throwing a runtime_error exception..." << endl;
	throw runtime_error("Exception from thread");
}

// 2. 예외를 잡아 exception_ptr에 저장하는 스레드 함수
void threadFunc(exception_ptr& err) {
	try {
		doSomeWork();
	}
	catch (...) {
		cout << "Thread caught exception, returning exception..." << endl;
		// 현재 발생한 익셉션을 가리키는 포인터를 err에 저장
		err = current_exception();
	}
}

// 3. 스레드를 생성하고 실행을 관리하는 함수
void doWorkInThread() {
	exception_ptr error;

	// c가 아닌 error 객체를 참조로 전달하기 위해 ref() 사용
	thread t{ threadFunc, ref(error) };

	t.join(); // 스레드가 종료될 때까지 대기

	// 스레드 종료 후 에러 객체가 비어있지 않은지 검사
	if (error) {
		cout << "Main thread received exception, rethrowing it..." << endl;
		rethrow_exception(error); // 받은 익셉션을 다시 던짐
	}
	else {
		cout << "Main thread did not receive any exception." << endl;
	}
}

int main() {
	try {
		doWorkInThread();
	}
	catch (const exception& e) {
		// 메인 스레드에서 최종적으로 예외를 처리
		cout << "Main function caught: '" << e.what() << "'" << endl;
	}

	return 0;
}
