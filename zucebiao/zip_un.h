//zip_un.h
#pragma once
#include <Windows.h>          //���Windows.h��Ȼ��һ�Ѵ���
#include <string>             //C++ʹ��string���string,��Ҫ���string.h
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
	int index; //������������ı�ʶ
};


