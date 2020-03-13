#include "common.hh"
#include <iostream>
#include <memory>

namespace virt_funs
{

class shape {
public:
	shape() = default;
	virtual ~shape() { std::cout<< this->c <<std::endl; };
	virtual void repr() const { std::cout << this->name << std::endl; };
private:
	char c{'S'};
	std::string name{"shape"};
};

class circle : public shape {
public:
	circle() = default;
	~circle() { std::cout<< this->c <<std::endl; };
	void repr() const override final { std::cout << this->name << std::endl; };
private:
	char c{'C'};
	std::string name{R"--("(circle)")--"};
};

class wheel : public circle {
public:
	wheel() = default;
	~wheel() { std::cout<< this->c <<std::endl; };
private:
	char c{'W'};
	std::string name{R"--(wheel "))--"};
};

std::unique_ptr<wheel> prod_wheel()
{
	return std::make_unique<wheel>();
}

int main()
{
	std::unique_ptr<shape> s{new wheel{}};
	s->repr();

	return 0;
}
} /* namespace virt_funs */
