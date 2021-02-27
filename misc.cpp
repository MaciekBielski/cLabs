#include <iostream>
#include <fstream>

// #include "include/xpcc/register.hpp"

using namespace std;


int main(int argc, char **argv)
{
    const char *fpath = "/tmp/foo/testfile.log\n------------";

    while (*fpath++) {
        std::cout.put(*fpath);
    }


    // /* Moving semantics */
    // cout<<"\n_____ lvalue assignment _____"<<endl;
    // auto m = Bleh{1};   // this is initialization and elided moving/copying
    // auto n = m;         // this is copying
    // cout<< "\trhs was copied test: " << m.i <<endl;
    // cout<<"\n_____ rvalue assignment _____"<<endl;
    // auto o = move(m);
    // cout<< "\trhs was moved and should no longer be used: " << m.i <<endl;
    // o = move(n);
    // cout<< "\trhs was assigned-moved: " << m.i <<endl;
    // cout<<endl<<endl;

    return 0;
}
