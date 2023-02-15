#include "Analyser.h"

int main() {

	std::string testFile =
		"func test(int x, int y){\n"
		"	var:bool res;"
		"	if(x + y ? 2){"
		"		res = true;"
		"	}"
		"	else{"
		"		res = false;"
		"	}"
		"	return(res);"
		"}"
		"var:int sus = test(2, 3);"
		"";

	auto resp = Analyser::StructuriseFile(testFile);
	auto res = Analyser::TokeniseFile(resp);
	 
	return 0;
}