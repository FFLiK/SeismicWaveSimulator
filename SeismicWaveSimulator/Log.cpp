#include "Log.h"
#include <iostream>
#include <Config.h>

void Log::PrintDebugLog(string class_name, string function_name, string log) {
#if Debug	
	cout << "[Debug LOG] " + class_name + "::" + function_name + "(...)" + " - " + log << endl;
#endif
}

void Log::PrintSystemLog(string log) {
	cout << "[System Log] " << log << endl;
}
