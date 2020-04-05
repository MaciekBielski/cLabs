#include "common.hh"
#include <iostream>

/* For test only, it should be in a namespace anyway */
extern ::std::string global_label;

namespace extern_linkage {
int main()
{
	std::cout <<"Linkage test: " << ::global_label << std::endl;
	return 0;
}
}

int main()
{
	// return enum_class::main();
	// return virt_funs::main();
	return extern_linkage::main();
}
