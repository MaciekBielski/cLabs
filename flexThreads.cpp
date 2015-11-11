/* Perhaps the most peculiar C++ code I've ever written
 *
 * Program shows how to connect C pthread_create library with C++ code. An
 * address of a thread function needs to be passed to pthread_create.
 * The address of member function cannot just be passed, it's not C. Member
 * function can be poined only by special pointer- a translation vector between
 * the beginning of the class and a member function address in memory. The
 * pointer has to be set on 'definition' of the class, not an instance. It is
 * later used with a particular instance.
 *
 * It could be created a static thread function and passed to pthread_create
 * together with a particular instance of a class as an parameter but this
 * approach is limited. If the class needs to be entered from many places and
 * should spawn a thread, it could be done by a static function. Moreover, it 
 * has to be known which function is about to be run at a compile time.
 *
 * Instead, there is one STATIC function threadMaker as a thread function. It
 * is provided with ThreadData structure which includes parameters for a given
 * thread. A member function - threadFunProvider which returns pointer to
 * another member function, drawing it upon it's internal structure. Inside it
 * there is a class instance pointer which supplies 'this' and pointer to
 * member function which uses that instance.
 *
 * Simple function 'workOnArray' is run in a loop many times. It was for taking
 * measurements of such operation, times had been collected. This is extracted
 * from a program that operated on two cameras.
 */

#include <iostream>
#include <pthread.h>
#include <string>
#include <sys/time.h>

using namespace std;

class Example
{
    public:
        Example()
        { 
            for(int k=0; k<10; t[k++]='x'); 
            t[10]='\0';
        };
        char t[11];
        string s;
        void workOnArray(int side);
        void callingFun(itimerval *it);
        /* threadFunProvider returns a 'translation vector' to a function that gets int
         * and returns void. "()" has higher priority than "*".
         * damn c++
         */
        void (Example::*threadFunProvider())(int){ 
            /* could return pointer to different function, of the same type
             * depending on calling instance for example. */
            return &Example::workOnArray;
        }
        /* common function for the whole class, proxy runner */
        static void* threadMaker(void* opaque);
    private:
        pthread_mutex_t mutex;
};

struct ThreadData
{
    int arg;
    Example* e;
    void (Example::*providedFun)(int);
    itimerval *it;
};

void* Example::threadMaker(void* opaque)
{
    /* run at the beginning of a thread */
    ThreadData *td = static_cast<ThreadData*>(opaque); 
    //setitimer(ITIMER_PROF,td->it,NULL); 
    /* pointer to an instance of a class */
    Example *e = td->e;
    //and pointer to a function inside of that class
    void (Example::* properFun)(int) = (td->providedFun);
    //invokation
    (e->*properFun)(td->arg);
    return NULL;
}

void Example::workOnArray(int side)
{
    int start = side==0 ? 0:1;
    char sign = side==0 ? 'l':'r';
    pthread_mutex_lock(&mutex);
    for(size_t i=start; i<10; i+=2)
        t[i] = sign;
    pthread_mutex_unlock(&mutex);
    return;
}

void Example::callingFun(itimerval *it)
{
    /* spawns two threads, and waits till they will be done.
     * Not in charge of performing any specific logic or choosing the function */
    ThreadData td[2] = {
        {0, this, this->threadFunProvider(), it},
        {1, this, this->threadFunProvider(), it}
        //{0, this, &Example::workOnArray,it},
        //{1, this, &Example::workOnArray,it}
    };
    pthread_mutex_init(&this->mutex, NULL);
    pthread_t tid[2];
    pthread_mutex_lock(&this->mutex);
    for(size_t j=0; j<2; j++){
        pthread_create(&tid[j], NULL, &Example::threadMaker, static_cast<void*>(td+j));
    }
    pthread_mutex_unlock(&this->mutex);
    //wait till both threads will finish
    for(size_t j=0; j<2; j++)
        pthread_join(tid[j], NULL);
    pthread_mutex_destroy(&this->mutex);
    return;
}

int main()
{
    Example arrayTest;
    itimerval it;
    for(size_t i=0; i<100000; i++)
    {
        arrayTest.callingFun(&it);
        if(0==i%10000)
            std::cout<<"."<<std::endl;
    }
    std::cout<<"done: "<<arrayTest.t<<std::endl;
    return 0;
}
