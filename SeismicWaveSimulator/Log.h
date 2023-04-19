#pragma once
#include <string>
using namespace std;

class Log {
public:
	static void PrintDebugLog(string class_name, string function_name, string log = "");
	static void PrintSystemLog(string log = "");
};

