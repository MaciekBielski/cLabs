#include <iostream>
#include <string>
#include <array>
#include <functional>
#include <sstream>
#include <typeinfo>
#include <utility>
#include <atomic>

#include "include/xpcc/register.hpp"

using namespace std;

/* This type supports move semantics */
class Bleh {
    public:
        int i = 0;
        Bleh() = delete;
        Bleh(int j) : i{j} { }

        /* rhs still valid after this operation */
        Bleh(const Bleh& lhs) : i{lhs.i} {
            cout <<"\tCopy constructor"<<endl;
        }
        /* rhs destroyed just after this operation */
        Bleh(Bleh &&rhs) noexcept : i{-1} {
            std::swap(i, rhs.i);
            cout << "\tMoving constructor" << endl;
        }

        /* one universal assignment operator */
        Bleh& operator=(Bleh rhs) noexcept {
            std::swap(i, rhs.i);
            rhs.i = -1;             // only for testing that rhs is destructed after
            cout << "\tAssignment" << endl;
            return *this;
        }

        ~Bleh() { cout<< "\t Bleh desctructor"<<endl; }
};


int main()
{
    /* Moving semantics */
    cout<<"\n_____ lvalue assignment _____"<<endl;
    auto m = Bleh{1};   // this is initialization and elided moving/copying
    auto n = m;         // this is copying
    cout<< "\trhs was copied test: " << m.i <<endl;
    cout<<"\n_____ rvalue assignment _____"<<endl;
    auto o = move(m);
    cout<< "\trhs was moved and should no longer be used: " << m.i <<endl;
    o = move(n);
    cout<< "\trhs was assigned-moved: " << m.i <<endl;
    cout<<endl<<endl;

    return 0;
}
