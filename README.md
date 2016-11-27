Notes on modern C++


Rules
--------------------------------------------------------------------------------
1.  Avoid automatic default constructors, at least use `= default`,
2.  Make methods `const` whenever possible,
3.  Constructors may be `explicit`, for blocked implicit conversions,
4.  Container class should have initializer-list constructor,
5.  Class with virtual function should have virtual destructor,
6.  Virtual function should be marked `override` to force the compiler check if
    prototype matches (or if parent version is marked `final`)
7.  Functions returning a pointer should return `unique_ptr` or `shared_ptr`
8.  Designing a class always consider if and how it can be copied, for
    move-only types delete copy constructor and use universal assignment
    operator,
9.  RAII (Resource Acquisition Is Initialization):
    - constructor acquires resource, destructor releases,
    - each resource has an owner in a scope and is released at the end,
    - for (passing) owners and only for them use owning pointer;
      `make_unique<X>` or `make_shared<T>`, otherwise use classic pointers
    - ownership can be moved to other scope with move semantics or "smart
      pointers",
    - raw pointer is non-owner, no naked `new` or `delete` in application
      code,
    - don't mix owners and non-owners, eg. by pushing them to the same
      container,
    - raw pointer don't escape to enclosing scope (returns, parameters)
    - after moving out the resource object it should no longer be used in the
      same scope (this is not checked by the compiler?), this is why it should
      be pointed by `unique_ptr`,
    - resource handler by default should be `move-only` type,
11. Move semantics for rvalue references:
    - Return objects by value if they support moving, `return x;` is implicitly
      turned into `return move(x);`,
    - Moving constructor has to secure subsequent destruction of `rhs`,
    - Use 'construct & swap' for one universal assignment operator,
    - After `auto y = move(x)`, you should no longer use `x`,
12. Base class in a hierarchy should not be copied by default, use explicit
    `virtual clone` function for that,
13. Code in a declarative way
14. Passing/returning by value:
    - Return by value by default (elided by compiler),
    - Pass by value in the constructor,
    - By default pass by `const T&`, for justified optimization pass by `T&&
      noexcept and move at the end`,


Miscs
--------------------------------------------------------------------------------

* `enum class`
  Enums are not treated as integers but distinct types, more type-safety. Use
  it instead of classical enums, alternatively put the enum in a separate
  namespace to avoid `static_cast` the enum to int.

* `constexpr float pi = 3.14`
  Known at the compile time. For constants initialized on runtime and never
  changed use `const`.

* Special containers:
  - `pair` and `tuple` are heterogenious
  - `array`, `vector` and `tuple` are contiguously allocated
  - `bitset` and `vectov<bool>` keep bits and access them via proxy objects,

* `array`
  Better than built-in array, has `constexpr` size defined at compile time and
  can be on stack, no constructor or allocator, efficient for moving. Can be
  seen as tuple of elements of the same type.

* `array_view<T>`
  Run-time check when using raw array `T tab[123]`, `array_view<T>{tab}` will
  make `tab.size()` for scope-check,

* `bitset` (not bit-field)
  - Have few interesting ways of initialization, eg. from part of basic_string,
  - `vector<bool>` is similar but has an allocator and changeable size,


* `not_null`
  Run-time check when a pointer cannot be nullptr: `not_null<T*> ptr`,

* `bitset`, `make_pair()`, `make_tuple()`
    
* `using distance_t = double`
  Type aliasing, instead of `typedef`'s, useful for binding templates:

        template<typename Value>
        using String_map = Map<string,Value>;

* auto
  - use it for local variable initialization by default, it reduces refactoring
    eg. when changing constness, it would imply moving or copying (fallback)
    but compiler does good job with 'returned type elision',
  - without elision moving or copying it can be expensive!!
  - if you want to stick to a certain type put it on the right side, it will
    also be elided, also for cast-initializations:
        `auto a = {x}`          // track x regardless of its type
        `auto f = widget{}`     // stick to a specific type
        `auto pb = unique_ptr<Parent>{ make_unique<Child>() };
                            // declarative way of telling what is going on
  - the only exception for `auto` initialization (with assummed elision) is for
    non-movable and non-copyable types - then declare the type on the left
    `T t{}`
  - auto foo() -> int`
    Trailing return type syntax, even this is optional but then function has to
    be defined before it is used,

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

* alignent
  - get:
    auto align = alignof(obj);
  - set:
    alignas(Foo) tab[1024];

* `thread-local`
  For thread-only objects

* `mutable`
  Mutable member can be defined even in a const object!

* `static_assert()`
    Assertions at compile time, especially usefull in templates

* `initializer_list<T> &il`
    Make your class accepting initializer list (`std::vector t[3] = {1,2,3}`)
  - With uniform initialization it takes precedence over other constructors  
  - This is not initialization_list, it only sounds similar, it uses `=`

* iterators
  The trick is that they can mimick the behaviour of an array element pointer
  and the value is obtained in the same way by dereferencing `*it`,

* `<memory>`
  Provides interesting means of dealing with eg. uninitialized memory,

* `call_once()`
  To call something only once


Smart pointers
--------------------------------------------------------------------------------

Use them when they need to be modified or when dealing with ownership,
otherwise classic pointers and references are still good. Use factories that
produce them.

- `unique_ptr<T,D>`
  unique ownership, cannot be copied, relies on move semantics, named unique
  pointer has to be passed by value with `move()`, underlying object is
  destroyed at EOS by 'deleter' (default is `delete`), `up.get()` vs.
  `up.release()`,

- `shared_ptr<T>`
  shared ownership, rather copied than moved, underlying object destroyed when
  last `shared_ptr` destroyed, it may outlive creating function, it may have
  also allocator, not only deleter,
  - moving retains the refcount, copying increments the refcount,
  - has special castng functions,
  - global or heap allocated shared pointer should be pinned locally
    (refcount++) once before its raw reference will be sink down the
    call-stack, otherwise refcounter might be decremented somewhere else and
    object released:
    * DO NOT DEREFERENCE NON-LOCAL SHARED POINTER,
    * CALLING MEMBER FUNCTION OF A POINTED OBJECT IS ALSO DEREFERENCING,


        template <typename T>
        unique_ptr<T> factory<T>()
        {
            //return unique_ptr<T> { new T{} };
            return make_unique<T>();
        }
        
        void fun()
        {
            auto uPtr  = factory<T>();
            uPtr->doSth();

            //shared_ptr<T> sPtr { new T{"foo"} };  //shortcut below
            auto sPtr = make_shared<T>("foo");

            // implicit delete uPtr;
        }

How to use:
  - sink them by value (that retains refcount) to consume,
  - pass by reference if you want to reseat the pointer,
  - `shared_ptr should ` be passed by const reference in case sometimes it will
    be copied inside (refcount++) but sometimes not, but never reseated,

- `weak_ptr`
  - Used to break loops in data structures managed by `shared_ptr`,
  - Must be converted to `shared_ptr` by `.lock()` function before accessing
    underlying data,
  - May outlive last regular `shared_ptr`, this can be checked with
    `.expired()`


* `mem_fn`
  Turns a method into function object, an algorithm may accept only function,
  lambdas can be alternative

        for_each( foo.begin(); foo.end(); mem_fn( &Foo::method ) );
        // or
        for_each( foo.begin(); foo.end(); []( Foo *f ){ f->method(); } );

* `bind`
  Used for function currying, lambdas are better. Result can be assigned to
  `function<F>`


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


Lambdas
--------------------------------------------------------------------------------

        auto lambdaName = [&]( const string& s ) mutable { return s > outerStr };

* `mutable` if changes captured elements,
* [] - capture list to pass elements from the environment,
  - by default lambda does not modify them,
  - otherwise lambda has to be `mutable`,
* () - parameters list, arguments passed during call,


- `[a, b]`  - capture variables as they are declared,
- `[this]`  - member lambda - other members always passed by reference
- `[&x]`    - only outer x will be captured, by reference,
- `[&x]`    - only outer x will be captured, by reference,
- `[&,y,z]` - capture all by reference,
- `[=x]`    - only outer x will be captured, by value
- `[=,y,z]` - capture all by reference,

* Capturing by value is safer when lambda may outlive the caller (eg. passed to
  another thread),

* Generic lambdas with `auto` arguments (C++14)

    auto do_sum  = [](auto op1, auto op2){ return op1 + op2; };

  can be also inline, `svec` is a vector<string>:

    std::accumulate(svec.begin(), svec.end(), std::string(""),
                             [](auto op1,auto op2){ return op1+op2; } )


Named casts
--------------------------------------------------------------------------------

* Static
  - for reversing implicit conversions (eg. int-to-void or derived-to-parent):

        void *vp = &x;
        int *ip = static_cast<int*>(vp);

* Reinterpret
  - re-interpretting bit patters like long-to-ptr or fun-to-void
    void funFoo(){}

        cout << reinterpret_cast<void*>(funFoo);
        auto mem = reinterpret_cast<Memory*> (0x400000);

* Const
  - for getting rid of const


* Dynamic
  - Uses RTTI, used when conversion correctness cannot be checked by the
    compiler - downcasting and crosscasting,
  - casted type has to be polymorphic (have virtual functions) - pointer to
    `type_info` is stored in `vtbl`
  - Check "is instance", can return `nullptr`, use it if a failure is
    considered valid alternative

        if( auto c = dynamic_cast<Circle *>( shape_ptr ) )
          c.tellRadius();

  - Also other version, but this can throw "std::bad_cast", use it if a failure
    is considered an error

        Circle &c { dynamic_cast<Circle *>( *shape_ptr )};

  - for 'upcasts' it is like simple assignment
    
        Base *basePtr = dynamic_cast<Base*>(drvPtr);
        
  - also `typeId' uses RTTI,


Classes
--------------------------------------------------------------------------------

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

### Initializer-list constructor

        Foo::Foo( std::initializer_list<Bar> lst )
        {
            // lst.size(), lst.begin(), lst.end()...
        }

### Operators overloading
* by function (may need to be friend)
  - should be used for symetric operators,
  - necessary if l-operand is not user defined

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
    // char c( i ); //this would silently compile and perform casting!
    // char c{ i }; //error: narrowing non-constant expression, cannot safe-check
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


Move semantics
--------------------------------------------------------------------------------
* lvalue & rvalue
  - lvalue - everything that can give you an address of memory
  - rvalue - everything that is not lvalue, you cannot assign to it, eg.
    temporary object returned from a function,

* The rvalue refrence refer to rvalues (temporary object). Sth declared as an
  rvalue reference `T&& t` can be used as **rvalue or lvalue** reference.
  - If it has a name then it will be used as lvalue,
  - If a type does not support moving rvalue acts as lvalue,
  - Rvalue reference refers to a temporary object that is never used later,
  Return value is copied to a temporary object and this objects can be bind to
  rvalue reference instead of being copied again to some other object (C++03).

* Rvalues can be bind to `T&&` or `const T&`, for backward compatibility but
  then, if the object is passed by value, we fall back to copying.

* constructors:
  - copy constructor:       
  - move constructor:       T::T( T&& rhs) {};

* move operations - universal implementation:
  - if right-hand side of an assignment is an rvalue it would be more efficient
    to move instead of copying because **rvalue** is destroyed immediately
    after - this destroy has to be secured (e.g. nulling pointers),
  - 'construct & swap' idiom:
    To reduce code duplication for assignment operator use one that takes an
    argument **by value!** This value will be a copy or moved object, depends
    on whether 'rvalue' or 'lvalue' is being assigned. Then swap elements.

        /* standard copy constructor */
        T::T( const T &rhs) {
        };

        /* move constructor, rhs destroyed just after */
        T::T(T &&rhs) noexcept : tabPtr{ nullptr } {
            // swap elements and make sure rhs destruction is safe
            std::swap( tabPtr, rhs.tabPtr );
        };

        /* universal assignment operator */
        T::T& operator=( T byValue ) {
            std::swap( elem, byValue.elem ); 
            return *this;
        }

  - after 'rvalue' assignment 'rhs' should no longer be used!

        auto x = move(y);
        y.foo();            // compiler will not protest(?) but this is bad


Ownership
--------------------------------------------------------------------------------

* move-only types
  Types that own a resource should not be copied, the ownership should only be
  moved.
  - point to resource with `auto res = make_unique<T>( T{} )`,
  - if `T t` is pointed by `unique_ptr` then copying is blocked but when `t` is
    passed to/from the function then it needs additional protection:
    - delete the copy constructor: `T( const T& rhs) = delete;`,
    - implement the universal 'construct & swap' assignment operator,

* any functions that don't move the ownership should use raw pointers and raw
  references,

* resource handling:
  - create resource by `auto res = make_unique<T>();` or `factory()`
  - consume ownership by `void sink( unique_ptr<T> res )`,



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
- constructor can only invoke its direct-parent constructor

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

- abstract function = virtual functions with no body,
- class with abstract function becomes abstract - cannot be instantiated,
- derived class is forced to define a body for this function or it will become
  abstract as well,
- should contain virtual destructor

### Interface classes
- no specific name for this type of classes
- contains only abstract functions and no members
- should contain virtual destructor

### Virtually inherited classes - avoid doubled ancestor
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

        template <typename T>    //each method needs its own declaration
        T& Foo<T>::operator[](int idx)
        {
            /* function body */
        }

### expression parameter
Usable inside template classes, substituted by a value or pointer, not type.

        template <typename T, int nSz>  //nSz is an expression parameter
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

### variadic templates

        void f(){}       recursion floor

        template< typename T>
        void g(T t){ /* do something */ };

        template< typename T, typename... Tail>
        void f(T head, Tail... tail)
        {
            g(head);
            f(tail...);     // at the end it will be f();
        }


### explicit instantiation and files structure
Template class and methods cannot be split in two: .hpp + .cpp in a classical
way. For small templates everything can be in one .hpp, but each including file
will create its own copy!

Other solution - explicitly instantiate types that you need in a templates.cpp:

        #include "TempType.hpp"         // template definition
        #include "TempType.cpp"         // template methods definition

        template class Foo<int>;        //explicitly instantiate template class
        template class Foo<double>;

Then, you have ONLY ONE file that incudes "TempType.cpp" so no code bloat here.
Other users need to include .hpp and link templates.o at the end.

NOTICE: TempType.cpp should not perhaps be used by build script to create '.o'.

### perfect forwarding with move semantics
TODO
http://thbecker.net/articles/rvalue_references/section_08.html


Type functions: meta-programming
-------------------------------------------------------------------------------

Using information retrieved from type objects (vide QEMU TypeInfo) at compile
time.

        constexpr float f_min = numeric_limits<float>::min();

Especially usefull when writing templates. Good place to use
`static_assert<T>`. Examples:

* iterator traits
  The `sort()` function uses iterator but they may be different:
  forward-iterator or random-access-iterator, so `sort()` is wrapped in different
  functions, depending on the iterator type (some additional steps required for
  forward-iterator), differentiated by the iterator tag:

        template <typename Rnd>
        void sort_wrapper(Rnd beg, Rnd end, random_access_iterator_tag)
        { /* body */ }

        template <typename Fwd>
        void sort_wrapper(Fwd beg, Fwd end, forward_access_iterator_tag)
        {
            /*
             * good example, `Fwd` itself has no field 'value_type',
             * but `typename Fwd` does ?
             */
            vector<typename Fwd::value_type> v {beg, end};
        }

        //overloaded sort for generic container
        template <typename Cnt>
        sort( Cnt &cnt )
        {
            /* we need a typename, not the type object */
            usint Iter = typename Cnt::iterator;
            usint IterCat = typename std::iterator_traits<Iter>::iterator_category;

            sort_wrapper( cnt.begin(), cnt.end(), IterCat{} );
        }

    - Nice details: each container records type of contained values that can be
      extracted,



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


Concurrency
-------------------------------------------------------------------------------

- Threads are sharing address space. Process = thread + AS + IPC,
- Stacks are not shared between threads,
- Beware of context binding references in lambdas passed to threads,
- ref{}, crer{} - passes reference to a variadic template,

### Atomic
- Operation on atomic type is not interferred by other threads,
- For types that map to a single built-in type,
- `atomic<T>`, `atomic<T*>`

        auto x = atomic{t};
        t=x.load();
        x.store(t);

* `atomic_flag` - single-bit information, simplest spin-lock

        auto af = atomic_flag{ATOMIC_FLAG_INIT};


### Threads

- constructor is a variadic template so passing a reference has to be done by
  std::ref{x}, otherwise it will be copied,

        auto t = thread {fun, ref(arg)};

- default `id{}` means that thread is not joinable
- `detach()` allows an execution thread outlive its destructor,
- in parallel programs static variables could be replaced by
  `static thread_local pair<const K,V>`


### Mutex

- Acquiring a mutex means taking exclusive ownership,
- defer_lock - defer the moment of lock acquisition
- `.lock()` and `.unlock()` can be called explicitly for a mutex or convenience
  wrappers can handle it,
  - `lock_guard` and `unique_lock` are handles for an object that you can lock,
    first one is a RAII on mutex, second the same plus additional operations,
    eg. may defer acquisition,


        mutex m,mtx1, mtx2, exp_mtx, lck_mtx;

        void tFoo(vector<int> &v) {
            exp_mtx.lock();                     //unlock must be called

            {
                lock_guard<mutex> {lck_mtx}
                // do sth that needs lock
            }

            unique_lock<mutex> lck {m};         // implicit m.lock()
            unique_lock<mutex> defLck1 {mtx1, defer_lock};  // don't lock yet
            unique_lock<mutex> defLck2 {mtx2, defer_lock};  // don't lock yet

            lock(defLck1, defLck2);             // acquire all locks now

            //work, mutex released at the EOS   // implicit m.unlock()
            exp_mtx.unlock();
        }

        int main() {
            vector<int> v1 {1,2,3};

            task t1 {tFoo, ref{v1} };    // explicit reference for templates

            t1.join();
        }

### Condition variables
  - condition variable needs to be called with lock acquired and then it enters
    `wait()` and releases the lock atomically, the lock is re-acquired again
    when `wait()` returns,
  - `queue<T>` is a shared resource that has to be accessed as critical section

    class Msg{ /*...*/ };
    queue<Msg> msgQueue;
    mutex msgMtx;
    condition_variable msgCond;

        void tProducer()
        {
            unique_lock<mutex> lck {msgMtx};
            msgQueue.push( Msg{} );
            msgCond.notify_one();

        }

        void tConsumer()
        {
            unique_lock<mutex> lck {msgMtx};
            while( msgCond.wait(msgMtx) ) {};   // wait returns 0 when woken up
                // lock is re-acquired here
            auto msg1 = msgQueue.front();   // example queue usage
            lck.unlock();                   // also could be implicit perhaps
        }

### Task communication with `<future>`

* `future` and `promise`: transfer value between two tasks without explicit locking
  - producer task puts the value into `promise`
  - consumer task gets the value from `future`
  - `get()` can pass an exception as well

        void tProducer()
        {
            promise<Msg> pxMsg;
            try{
                pxMsg.set_value(pxMsg);
            } catch {
                pxMsg.set_exception( current_exception() );
            }
        }

        void tConsumer()
        {
            future<Msg> fxMsg;
            try{
                auto m = fxMsg.get();               //blocks until message arrives
            } catch { /* handle the error */ }
        }

* `packaged_task`
  - wrapper for a function thread
  - simplifies connecting tasks
  - packaged task is resources owner so it cannot be copied - explicit `move{}`
    for a template


        int funProducer( int &ignored ) { return 123; }      // implicitly put promises

        int main()
        {
            using tProdType = int( int& );          // convenience
            packaged_task<tProdType> ptProd {funProducer};

            future<int> fx { ptProd.get_future(); }

            int x = 888;
            task tProd { move{ptProd}, ref{x} };    // explicit move and ref for templates

            cout << fx.get() << endl;               //blocks here
        }

* `async`
  - Simplest, no resource sharing, just independent work
  - nb of threads decided by async

    int main()
    {
        auto fx = async( funProducer, 888 );
        cout << fx.get() << endl;
    }


Visitor
-------------------------------------------------------------------------------

Main problem: double dispatch implementation, adding funX(), funY() to
hierarchy of classes where also funX() and funY() in one class can be
overloaded depending on argument type.

Solves the problem of applying operations to the hierarchy of classes without
overwriting all of them by using double dispatching trick.

- (hierarchy) operands are derived from `Node` and have `accept()`:
    
        (virtual) void accept(Visitor &) override { v.accept(*this); };

- operations are derived from `Visitor` type that has `accept()` defined for
  each hierarchy type,

        class Visitor {
            public:
                virtual void accept(SubNodeX &xn) = 0;
                virtual void accept(SubNodeY &yn) = 0;
        };

- then, each operation is derived from `Visitor` and this way there are
  different operations for different type of `Node` and adding new operations
  does not require changing Nodes hierarchy,

        class Op1 : public Visitor {
                void accept(SubNodeX &xn) { /* Op1 for SubNodeX */ };
                void accept(SubNodeY &yn) { /* Op1 for SubNodeY */ };
        };

        class Op2 : public Visitor {
                void accept(SubNodeX &xn) { /* Op2 for SubNodeX */ };
                void accept(SubNodeY &yn) { /* Op2 for SubNodeX */ };
        };

Embedded
-------------------------------------------------------------------------------

### ROMmable class
1. no base classes
2. no constructor
3. no private or protected members
4. no virtual functions
5. any contained classes have to be ROMmable as well
6. all member functions logically should be `const`
7. securing `uninitialized` instances creation
  - encapsulate the ROMmable class (inner) in a private segment of outer class
  - create private `static const` instances of inner class, as private members
  - the outer class is not ROMmable

