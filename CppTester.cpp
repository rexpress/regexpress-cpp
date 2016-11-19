#include <iostream>
#include "CppTester.h"

using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[])
{
	json config = json::parse(argv[1]);
	json test_strings = json::parse(argv[2]);

	json result;

	try {
		CppTester tester;
		result = tester.test_regex(config, test_strings);
	} catch (char const* e) {
		result["exception"] = e;
	}

	cout << "##START_RESULT##" << endl;
	cout << result << endl;
	cout << "##END_RESULT##" << endl;
	
	return 0;
}