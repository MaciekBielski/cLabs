Notes on C++11

Miscs
--------------------------------------------------------------------------------

* `enum class`
  Enums are not treated as integers but distinct types, more type-safety. Use
  it instead of classical enums, alternatively put the enum in a separate
  namespace to avoid `static_cast` the enum to int.

* `constextpr float pi = 3.14`
  Known at the compile time. For constants initialized on runtime and never
  changed use `const`.
    
* `using distance_t = double`
  Type aliasing, instead of `typedef`'s

* `auto foo() -> int`
  Trailing return type syntax, even this is optional but then function has to
  be defined before it is used, otherwise compiler cannot perform type-check.

* Input sanitizing
  Handy methods: `cin.fail()`, `cin.clean()`, `cin.ignore()`

* Null-pointer
  `int *p = nullptr`

* Non-failing `new`
  Pointer will be null if the allocation fails.
    int *txt = new (std::nothrow) char[3] { "Init value" };
    delete[] txt;

* Foreach
    for( auto &v : val){}
    for( const auto &v : val){}

* Function pointers
    using FunPtr = std::function<Foo(const Foo &a, const Foo &b)>;

* `decltype(x) val`;
    Determines the type of x at compile time. Eg. in templates, a type can
    depend on the argument template will be instantiated with.

* static_assert()
    Assertions at compile time

* initializer_list<T> &il
    Make your class accepting initializer list (`std::vector t[3] = {1,2,3}`)
  - With uniform initialization it takes precedence over other constructors  
  - This is not initialization_list, it only sounds similar, it uses `=`

* Lambdas

* Generic lambdas (named or unnamed) with `auto` arguments (impossible with
  C++11)

    auto do_sum  = [](auto op1, auto op2){ return op1 + op2; };

  can be also inline, `svec` is a vector<string>:

    std::accumulate(svec.begin(), svec.end(), std::string(""),
                             [](auto op1,auto op2){ return op1+op2; } )

Overloaded functions
--------------------------------------------------------------------------------

Matching preference:
1. Exact match
2. Match by promotion to larger type
3. Match by standard conversion - all considered equal
    - numeric-to-numeric
    - enum-to-numeric
    - zero to pointer type
    - pointer to void pointer
4. Match by user-defined conversion - all considered equal

* default params (rightmost) don't count for methods matching

### Casts

* void-to-int
    void *vp = &x;
    int *ip = static_cast<int*>(vp);

* fun-to-void
    void funFoo(){}
    cout << reinterpret_cast<void*>(funFoo);


Classes
--------------------------------------------------------------------------------

### Rules
* avoid automatic default constructors
* make methods const whenever possible
* constructors may be explicit, except for implicit conversions
* destructors should always be virtual

### Constructors
* non-static members should get assigned with default values inside class body,
  but still default constructor has to be defined
* construction order: initalization list, contained objects, current object
* const members have to be set from initialization list

    Foo():
        memArr{1,2,3}
    { /*constructor body*/ }

* copy-constructor vs. assignment operator
  The difference is important e.g. in case of dealing with objects that have
  heap-allocated memory.
  - Copy constructor initializes an object, it is used:
    1. at initialization from another object
    2. when passing/returning to/from a function BY VALUE
    3. can be made private to prevent copying
  - Assignment operator replaces existing object
    - returns `*this`
    - Self assignment needs to be detected!

        if (this == &input) { return *this; }

* constructor elision:
  Foo(Foo(3,1)) can be optimized by the compilerto Foo(3,1)

* constructor delegation: specific constructor call more generic one, only in
  its initalization list!!! (C++11)

* any method can be disabled by 'delete' to prevent conversion from some
  specific type

    Foo(char) = delete;

  - or allow only specific type:

    class Foo
    {
        Foo(long long); // Can create Foo() with a long long
        template<typename T> Foo(T) = delete;
            //But can't create it with
            //anything else that potentially could be casted to long long
    };

### Misc
* chaining objects

    Foo& fun{ /* body */; return *this }

* const object can call only 'const' methods, declared as below:
    
    int fun() const;

* static members are associated with class, not instances, they have to be
  explicitly defined outside of the class.
  - only members of type `static const int` can be initialized directly inside
    the class body
  - if private, can be accessed by static methods (or normal methods but this
    requires an instance)
  - static methods can ONLY access static members

  * friend functions and classes are declared inside the class that grants an
  access, with friend class, all its methods have access to the granting class

### Operators overloading
* by function (may need to be friend)
  necessary if l-operand is not user defined

    Foo operator+(const Foo &l, const Foo &r){}     //sum operator

* by method
  by design necessary for '=', '[]', '()', '->' 

    Foo Foo::operator-() const{}                    //unary operator '-x'

* postfix vs. prefix
  dummy operator to differentiate, non-const
    
    operator++();       //prefix version '++x'
    operator++(int);    //postfix version 'x++' - more expensive

* operator[]:
  WARNING! don't call it on 'Foo*' - this does not work as in C
* operator():
  defines both type and number of arguments, used for functors implementing -
  we can have separate objects of the same function, not possible with global
  function,

### User-defined conversions

1. narrowing
  Uniform initialization prevents narrowing, which should be expressed
  explicitly.

    int i = 0x4141;
    // char c(i); //this would silently compile and perform casting!
    // char c{i}; //error: narrowing non-constant expression, cannot safe-check
    char c{ static_cast<char>(i) };     //correct!

2. casting operators
  have no returned type,

    operator int() { /* body */ }
  //also
    operator Bar() { /* body */ }
  //now possible e.g.:
    int c = static_cast<int>( fooObj );

3. converting constructors:
  - implicit conversion: when a function expects `Foo`, and gets `int`, but
    there is a constructor `Foo(int)`, then it is implicitly converted. This
    can be switched off with `explicit` keyword - constructor will not be used
    for implicit conversions.


Relationships
--------------------------------------------------------------------------------

### Composition
- member belongs to only one parent at a time
- parent responsible for member's creation and cleanup
- member is not aware of a parent - unidirectional relationship
- modeled usually by contained objects

### Aggregation
- member may belong to multiple parents
- member's creation and cleanup is not managed by parent
- member is not aware of a parent - unidirectional relationship
- modeled usually by references/pointers

### Association
- no implied member-parent relationship
- member can be associated to multiple objects at a time
- otherwise there is no relation member-object
- object does not manage member's lifetime
- member may or may not know about the object's existence - uni-/bi-directional
- modeled by any associating information: pointer/idx
- reflective: association with object of the same type

### Dependency
- object uses member's functionality to accomplish some task
- otherwise does not need it
- usually not represented at a class level


Inheritance
-------------------------------------------------------------------------------

- "is-a" relationship, child specifies more generic parent or shows progress
  over time
- constructor can only invode its direct-parent constructor

- public:
  - parent's public members stay public
  - parent's protected members stay protected
  - parent's private members are inaccessible

      class Child : public Parent {
          Child() :
              Parent {111}
          {/* constructor body */}
      };

- private:
  - parent's public and protected members become private - inaccessible via
    child object
  - parent's private members are inaccessible

- protected:
  - parent's public and protected members become protected
  - parent's private members are inaccessible

- child class can change access specifiers to parent's object (only public or
  protected of course) with 'using declaration' or 'delete' for functions

      class Child : private Parent {    //all private, except for...
          public:
              using Parent:x;

              void parentPubFun() = delete;
      };

- friends are not inherited, child objects needs to be casted to parent type to
  use its friend functions,

### Virtual functions
Pointer to parent, obtained from child object, is normally able to see only the
scope of parent class. It makes sense, it is not aware about derived classes -
no polymorphism.

* base functionality
  The solution is `virtual` function, most-derived version will be called.
  Polymorphism works only with reference and pointers.

* covariant return type
  Normally the prototypes of original and override virtual functions have to
  match, with one objection, if return value is pointer or reference to a
  class, the override can return pointer or reference to derived type instead,

* it can be explicitly stopped:
    
    childPtr.Parent::fun();

* override - RECOMMENDED
    Tells explicitly, that a function is supposed to override its own version
    in parent class, prevents inadvertent errors

    class Parent {
        public:
            virtual void B() const;
    };
    class Child : public Parent {
        virtual void B() override;  //compile-time error - different prototype
    };

* final - using previous example
  - function
    
    virtual void B() const final;   //marks function as non-overrideable

    class Parent final {};          //marks class as non-inheritable

### Late/dynamic binding
It is not known at compile time which function will be called. Extra
indirection step required to jump to an address, where the function addres is
stored. Cases:
- function pointers
- virtual functions

Every class, that uses virtual functions, has its own 'vtable', which contains
a single entry for each virtual function that can be called by the object of
this class. The entry is a pointer to most-derived version of a function.

Then, a base class has a `*_vptr` pointer. This pointer is actually inherited
by derived classes which does the trick. It is set during object instantiation
and points to `vtable` of a given class. Thanks to this, with this code:

    Child c;                //_vptr set to vtable_Child
    Parent &p = c;          //_vptr was part of Parent,
                            //still points to vtable_Child

### Abstract functions and class

    virtual void fun() = 0;

- virtual functions with no body,
- class with abstract function becomes abstract - cannot be instantiated,
- derived class is forced to define a body for this function or it will become
  abstract as well,

### Interface classes
- no specific name
- contains only abstract functions and no members

### Virtual classes - avoid doubled ancestor
Copes with the "diamond problem", when grandparent class copied twice in a
child type and the child object will instantiate two grandparent objects.
USUALLY this is not desired. Solution:

    class GrandParent{}
    class ParentX : virtual public GrandParent{}
    class ParentY : virtual public GrandParent{}
    class Child : public ParentX, public ParentY {
        Child() : GrandParent {555} { /* constructor body */ }
    }

But:
In that case, Child::Child() is responsible for creating GrandParent object,
since ParentX/Y inherit it only virtually and don't instantiate in that case.
However, constructors of ParentX/Y still have calls to GrandParent in theirs
initialization lists because one may wish to instantiate ParentX or ParentY.

Note, that normally a Child constructor can only call a constructor of a direct
parent class - actually, in this case GrandParent becomes a direct parent
class of a Child!

Last mention: ParentX/Y, that virtually inherit GrandParent, also have virtual
tables (like for virtual functions) with pointers to functions of
GrandParent(). So, under the hood, it seems to be the same mechanism.

I/O
-------------------------------------------------------------------------------

* manipulators - put inside the stream
    
    char buf[10];
    cin >> setw(5) >> buf
    cout << hex << 69 << dec << 47 << endl;

* flags - before using the stream

    cout.setf( ios::showpos | ios::uppercase )
    cout << 555 << endl;
    cout.unsetf( ios::uppercase )
    cout.setf( ios::hex, ios::basefield )   //alternative to manipulators

### string streams

    stringstream os;
    os << "foo";
    os.str("");
    os.clear();
    cout << os.str();

Templates
-------------------------------------------------------------------------------

### template functions

    template <typename T1>    //or <class T1>
    T1 max(T1 tX, T1 tY)
    {
        return (tX > tY) ? tX : tY;
    }

### template class

    template <typename T>
    class Foo {
        public:
            T& operator[](int idx);
    };

    template <typename T>    //each templated method needs its own declaration
    T& Foo<T>::operator[](int idx)
    {
        /* function body */
    }

### expression parameter
Usable inside template classes, substituted by a value or pointer, not type.

    template <typename T, int nSz>      //nSz is an expression parameter
    class Buff {
        private:
            T buff[nSz];
    };

usage:
    
    Buff<double, 5> bf;

This is statical creation during template instantiation.

### methods specialization
Template can be specialized per specific type, e.g. specific constructor and
destructor can be created.

    template <>
    Foo<char*>::Foo(char* arg) { /**/ }

    template <>
    Foo<char*>::~Foo() { /**/ }

### type specialization
Customized version of a template will take precedence over generic template

    template <typename T>
    class Buz { /* body */};

specialized per-type:

    template <>         //this is to indicate the template
    class Buz<char*> { /* can be totally different body */ };

Body can be different, however its recommended to keep the public interface the
same.

### explicit instantiation and files structure
Template class and methods cannot be split in two: .hpp + .cpp in a classical
way. For small templates everything can be in one .hpp, but each including file
will create its own copy!

Other solution - explicitly instantiate types that you need in a templates.cpp:

    #include "TempType.hpp"
    #include "TempType.cpp"

    template class Foo<int>;
    template class Foo<double>;

Then, you have only one file that incudes "TempType.cpp" so no code bloat here.
Other users need to include .hpp and link templates.o at the end;


Exceptions
-------------------------------------------------------------------------------

    try{ throw <exception>; }
    catch( <type> ) {    }

    catch(...) { /* catch-all */ }

- try always comes with catch
- throw finishes the execution immediately and goes frame up,
- catch matches only exact type by default
- if an exception has been catched, the function is executed further, after
  `catch{}` block
- exception classes, like std::exception, use `.what()` member function to get
  the info string, this function should be virtual
- exception classes should be catched form specific to general
- catch block should clean proper resources, close files etc.
- destructor should never throw an exception!


STL
-------------------------------------------------------------------------------

- std::unique_ptr<>
  class that holds a pointer and deallocates it when it goes out of scope

- interesting containers: vector, deque, multiset, multimap, priority queue,
- iterator - like a pointer to element with a set of operators overloaded, two
  versions: iterator + const_iterator
- strings: use stringstream objects for converting to/from string
    - c_str()   - raw string with NULL-terminator
    - data()    - raw string without NULL-terminator

### algorithms
- min_element, max_elemenet, find,
- sort - not for lists, they have own .sort() method
