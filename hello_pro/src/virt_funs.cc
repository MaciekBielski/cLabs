#include "common.hh"
#include <iostream>
#include <memory>

static void print(const std::string &s) { std::cout<< s << std::endl; }

namespace virt_funs
{

class shape {
public:
	shape() = default;
	shape(const shape &orig) = delete;
	virtual ~shape() = default;

	shape& operator=(const shape &orig) = delete;

	virtual void repr() const { print(this->name); };
	void set_suffix(std::string&& _s) { this->name += _s; };
protected:
	std::string name{"shape"};
};

class circle : public shape {
public:
	circle() = default;
	// circle(const circle &orig) {
	// 	name = R"-("(copied_circle)")-";
	// 	print(R"([circle]: copying constructor)");
	// };
	// circle(circle &&orig) {
	// 	orig.name = R"-("(empty)")-";
	// 	name = R"-("(moved_circle)")-";
	// 	print(R"([circle]: move constructor)");
	// }
	~circle() = default;

	// circle& operator=(const circle &orig) {
	// 	name = R"-("(copy_assigned_circle)")-";
	// 	print(R"([circle]: copy assignment)");

	// 	return *this;
	// }
	// circle& operator=(circle &&orig) {
	// 	orig.name = R"-("(empty)")-";
	// 	name = R"-("(move_assigned_circle)")-";
	// 	print(R"([circle]: move assignment)");

	// 	return *this;
	// }
};

class wheel : public circle {
public:
	wheel() = default;

	~wheel() = default;
	void repr() const override { print(this->name3); };
protected:
	std::string name3{"wheel"};
};

static void test_moving(shape &&s)
{
	print("--- destroying passed object---");
	s.repr();
}

int main()
{
	wheel w{};
	circle& cr = w;
	cr.repr();


	// auto s_copy = wheel{s};			// copy constructor
	// s_copy.repr();
	// std::cout<<"----\n";

	// auto c = circle {};
	// c.repr();
	// auto c_copy = circle{std::move(c)};	// move constructor
	// // shape& illegal {c};
	// // illegal.set_suffix("BLAH!");
	// c.repr();
	// c_copy.repr();

	// // test_moving(std::move(c));
	return 0;
}
} /* namespace virt_funs */

