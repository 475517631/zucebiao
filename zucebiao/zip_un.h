//zip_un.h
#pragma once
#include <Windows.h>          //添加Windows.h不然会一堆错误
#include <string>             //C++使用string添加string,不要添加string.h
#include <iostream>
#include <tchar.h>
#include <vector>
#include <io.h>
#include "zip.h"
#include "unzip.h"
using namespace std;

class zip_un {
public:
	int Zip_UnPackFiles(string strZipPath);
	void Zip_PackFiles(string path);
	zip_un();

private:
	HZIP hz;
	void browseFile(string inPath);
	WCHAR* toWchar(string strZipPath);
	vector<string> split(const string& str);
	string connectStr(vector<string>& ret);
	string temporaryPath;
	int index; //用于深度搜索的标识
};


