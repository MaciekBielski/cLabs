#include "common.hh"
#include <iostream>

namespace enum_class
{
enum class animal {dog, cat, parrot};

// 1st approach
// static void who_is_it(const animal &a)

// 2nd approach
static void who_is_it(animal &&a)
{
	using namespace std;

	cout << "Here is an animal: " << endl;
	/* Implicit index */
	cout << static_cast<int>(a) << endl;
	switch(a) {
		case animal::dog:
			cout << "DOG" << endl;
			break;
		case animal::cat:
			cout << "CAT" << endl;
			break;
		case animal::parrot:
			cout << "PARROT" << endl;
			break;
	}
}

int main()
{
	// 1st approach
	// animal f {animal::parrot};
	// who_is_it(f);

	// 2nd approach
	who_is_it(animal::parrot);

	return 0;
}

} /* namespace enum_class */

