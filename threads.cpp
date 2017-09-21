#include <iostream>
#include <thread>
#include <chrono>
#include <array>

using namespace std;

void thFun(thread &shift)
{
    for (int i = 0; i < 10; ++i) {
        cout << "Id: "<< this_thread::get_id() <<
            " possible threads: " << thread::hardware_concurrency() << "\n";
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main()
{
    auto trds = array<thread,2>{};
    trds[1] = move( thread{thFun, ref(trds[0])} );
}
