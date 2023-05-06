#include <FileIO.h>
#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
#include <cstdio>
#include <Config.h>
#include <Log.h>
#include <atlstr.h>
#include <shlobj_core.h>
using namespace std;

std::string FileIO::Location(FileMode f, std::string name, std::string pos) {
	std::string loc;
	loc = name;

	if (pos != "") {
		loc = pos + "\\" + loc;
	}

	switch (f) {
	case RES:
		loc = "res\\" + loc;
		break;
	case DAT:
		loc = "dat\\" + loc;
		break;
	case EDIT:
		loc = "edit\\" + loc;
		break;
	}

	if (!Debug) {
		Log::PrintDebugLog("FileManager", "Location", "loc = " + (std::string)getenv("APPDATA") + "\\" + ProjectFolderName + "\\" + loc);
		return (std::string)getenv("APPDATA") + "\\" + ProjectFolderName + "\\" + loc;
	}
	else {
		Log::PrintDebugLog("FileManager", "Location", "loc = " + loc);
		return loc;
	}
}

Json::Value FileIO::GetJsonFile(std::string location) {
	Log::PrintDebugLog("FileManager", "GetJsonFile", "loc = " + location);
	Json::Value root;
	Json::CharReaderBuilder reader;
	ifstream is(location, ifstream::binary);
	string errorMessage;
	auto bret = Json::parseFromStream(reader, is, &root, &errorMessage);

	if (bret == false) {
		cout << "Error to parse JSON file !!!" << endl;
		cout << "Details : " << errorMessage << endl;
	}

	return root;
}

std::string FileIO::OpenFile(FileType type) {
	HDC hdc;
	PAINTSTRUCT ps;
	OPENFILENAME OFN;
	wchar_t lpstrFile[MAX_PATH] = L"";
	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	switch (type) {
	case CONFIG:
		OFN.lpstrFilter = L"Seismic Wave Configure File\0*.txt\0";
		break;
	}
	OFN.lpstrFile = lpstrFile;
	OFN.hwndOwner = NULL;
	OFN.nMaxFile = 256;
	OFN.nFilterIndex = 1;
	OFN.lpstrFileTitle = NULL;
	OFN.nMaxFileTitle = 0;
	OFN.lpstrInitialDir = NULL;
	OFN.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	string str = "";
	str.clear();
	if (GetOpenFileName(&OFN) != 0)
		str = CW2A(OFN.lpstrFile);
	return str;
}

