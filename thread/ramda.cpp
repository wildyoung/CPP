#include <iostream>
#include <thread>

using namespace std;

int main()
{
	int id1{ 1 };
	int id2{ 2 };

	int num1{ 5 };
	int num2{ 4 };

	thread t1{ [id1, num1]() {
		for (int i{ 0 }; i < num1; ++i) {
			cout << "Counter " << id1 << " has value " << i << endl;
		}
	} };

	thread t2{ [id2, num2]() {
		for (int i{ 0 }; i < num2; ++i) {
			cout << "Counter " << id2 << " has value " << i << endl;
		}
	} };

	t1.join();
	t2.join();

	/*
	 auto task = [](int id, int numIterations) {
        for (int i{ 0 }; i < numIterations; ++i) {
            cout << "Counter " << id << " has value " << i << endl;
        }
    };

    thread t1{ task, 1, 5 };
    thread t2{ task, 2, 4 };

    t1.join();
    t2.join();
	
	*/

	return 0;
}
