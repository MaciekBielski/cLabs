/* Exception handling in C++ example */
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sstream>
/*
 * Because of exiting threads by exception and not directly
 * we can assure that all automatic variables on the thread's stack
 * will be destroyed properly
 */

class ThreadExitException
{
    private:
        void *threadReturnValue;
    public:
        ThreadExitException(void* retVal): threadReturnValue(retVal) {}
        void* doThreadExit()
        { 
            pthread_exit(threadReturnValue);
        }
};

class TestObj{
    private:
        unsigned int n;
    public:
        TestObj(const unsigned int n): n(n){ 
            std::ostringstream oss;
            oss<<"Thread: "<<n<<"\n\ttestObject construction"<<std::endl;
            std::cout<<oss.str();
        }
        ~TestObj(){
            std::ostringstream oss;
            oss<<"Thread: "<<n<<"\n\ttestObject destruction"<<std::endl;
            std::cout<<oss.str();
        }
};

void doSomeWork(const unsigned int threadNb, int& retVal){
    //thread number used then by the destructor message
    TestObj test(threadNb);
    std::cout<<std::endl;
    sleep(2.5L);
    //triggering pthread_exit
    //expected return value is passed as an exception constructor argument
    throw ThreadExitException(&retVal);
}

void* threadInit(void *p){
    int *returnValue = (int*)p;
    try{
        doSomeWork(pthread_self(), *returnValue);
    }
    catch(ThreadExitException ex){
        ex.doThreadExit();
    }
    return 0;
}
int main(){
    pthread_t threads[3];
    int retVals[3] {11,22,33};
    int *retTest {0};

    for(int i=0; i<3; i++)
        pthread_create(&threads[i], NULL, &threadInit, &retVals[i]);

    for(int i=0; i<3; i++){
        pthread_join(threads[i], (void**)&retTest);
        std::cout<<"Thread: "<<i<<" returned: "<<(*retTest)<<std::endl;
        std::cout.flush();
    }
    return 0;
}
