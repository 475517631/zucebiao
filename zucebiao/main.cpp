// zucebiao.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include<Windows.h>
#include<io.h>
#include<atlstr.h>
#include<direct.h>
#include"zip_un.h"
#include<string.h>
#include<string>
#include<vector>
#include <iostream>
#include <assert.h>
#include "windows.h"
#include "conio.h"
#include "stdio.h"
#include<locale.h>
#include<tchar.h>
#include<iostream>
#include <Shlobj.h>
#include<fstream>
#include<winbase.h>
using namespace std;
int mark;//区分三分屏or小班课
int _mark;//区分系统32位还是64位  
int length=256; 
int Case;
bool status;	
string desktop;
HKEY  hKey = NULL;
LPCWSTR strSubKey; LPCWSTR strValueName;	
ifstream ifs;//流文件
char strValue1[256];//收集三分屏的安装地址
char strValue[256];//收取小班课的安装地址
TCHAR  path[MAX_PATH];//存系统盘地址
void wcharTochar(const wchar_t *wchar, char *chr, int length)
{
	WideCharToMultiByte(CP_ACP, 0, wchar, -1,
		chr, length, NULL, NULL);
}

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;
BOOL IsWow64()//判断系统位数 直接调用即可
{
	BOOL bIsWow64 = FALSE;
	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function
	//and GetProcAddress to get a pointer to the function if available.
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			//handle error
		}
	}
	return bIsWow64;
}


wstring GetLocalAppdataPath()
{
	wchar_t m_lpszDefaultDir[MAX_PATH] = { 0 };
	wchar_t szDocument[MAX_PATH] = { 0 };

	LPITEMIDLIST pidl = NULL;
	SHGetSpecialFolderLocation(NULL, CSIDL_LOCAL_APPDATA, &pidl);
	if (pidl && SHGetPathFromIDList(pidl, szDocument))
	{
		GetShortPathName(szDocument, m_lpszDefaultDir, _MAX_PATH);
	}
	return m_lpszDefaultDir;
}


string wstring2string(wstring wstr)
{
	string result;
	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}


std::string TCHAR2STRING(TCHAR* str)
{
	std::string strstr;
	try
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

		char* chRtn = new char[iLen * sizeof(char)];

		WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

		strstr = chRtn;
	}
	catch (std::exception e)
	{
	}
	//cout << "one" << endl;
//	cout << strstr << endl;
	strstr = strstr[0];
	return strstr;
}

std::string TCHARSTRING(TCHAR* str)
{
	std::string strstr;
	try
	{
		int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

		char* chRtn = new char[iLen * sizeof(char)];

		WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

		strstr = chRtn;
	}
	catch (std::exception e)
	{
	}
	//cout << "one" << endl;
	//cout << strstr << endl;
	return strstr;
}
bool OpenRegKey(HKEY& hRetKey)
{
	LPCWSTR sw;
	/*if (mark == 1)
	{
		if (_mark == 64)
		{
			sw = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{0D9BCA0A-76FF-4FAC-A9ED-CA05B66C27B7}_is1");//小班课注册表地址64位
		}else if (_mark==32)
		{
			sw = _T("SOFTWARE\\WOW3232Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{0D9BCA0A-76FF-4FAC-A9ED-CA05B66C27B7}_is1");//小班课注册表地址32位
		}
	}*/
	 if (mark == 2)
	{
		if (_mark == 64)
		{
			sw = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{2F0F058C-D340-4520-948E-7639602467EF}_is1");//三分屏注册表地址64位
		}else if (_mark == 32)
		{
			sw = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{2F0F058C-D340-4520-948E-7639602467EF}_is1");//三分屏注册表地址32位
		}
	}
	//wprintf(L"SW is %s\n", sw);
	if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, sw, &hRetKey))
	{
		return true;
	}
	printf("OpenRegKey return is false!\n");
	return false;
}

bool QueryRegKey(LPCWSTR strSubKey, LPCWSTR strValueName, char *strValue, int length)//传3个参数
{
	DWORD dwType = REG_SZ;//定义数据类型
	DWORD dwLen = MAX_PATH;
	wchar_t data[MAX_PATH];
	HKEY hKey;
	HKEY hSubKey;
	if (OpenRegKey(hKey))
	{
		if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, strSubKey, &hSubKey))
		{
			TCHAR buf[256] = { 0 };

			if (ERROR_SUCCESS == RegQueryValueEx(hSubKey, strValueName, 0, &dwType, (LPBYTE)data, &dwLen))
			{
				wcharTochar(data, strValue, length);
				//wprintf(L"data = %s,len= %d\n", data, strlen((const char *)data));
				RegCloseKey(hKey); //关闭注册表
				return true;
			}
		}
		RegCloseKey(hKey); //关闭注册表
	}

	return false;
}
void name()
{
	switch (Case)
	{
	case 1:cout << "小班课" << endl; break;//小班课
	case 2:cout << "三分屏" << endl; break;//三分屏
	case 3:cout << "小灶课" << endl; break;//小灶课
	case 4:cout << "公立校" << endl; break;//公立校
	case 5:cout << "辅导端" << endl; break;//辅导端
	case 6:cout << "学生端" << endl; break;//学生端
	default:
		break;
	}
	return;
}
void logpath(string apppath,string mupath)//第一个参数是表示读取log位置 第二个参数是文件生成文件名
{
	wstring mcpath = GetLocalAppdataPath();
	string minclaspath = wstring2string(mcpath);
	apppath.c_str();
	minclaspath.append(apppath);
	zip_un smzip;
	string strZipPath_un, strZipPath = minclaspath;

	strZipPath_un = strZipPath + ".zip";
	name();
	smzip.Zip_PackFiles(strZipPath);
	//name();
	//cout << minclaspath << endl;
	//smzip.Zip_UnPackFiles(strZipPath);
	
	/*	name();
		cout << "没有找到该app日志数据请确认是否安装并运行" << endl;
	*/
	//else cout << "no" << endl;
	string zu=TCHAR2STRING(path);
	string fname = "";
	//cout << "desktop" << endl;
	//cout << desktop << endl;

	fname.append(desktop);
	//cout << zu << endl;
	fname = fname+"\\journalFolder";
	//cout << fname << endl;
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	CString str1 = strZipPath_un.c_str();
	fname.append(mupath);
	//cout << fname << endl;
	CString str2 = fname.c_str();
	CopyFile(str1, str2, FALSE);
     
	return;
}
void test1()//小班课
{
	/*wstring mcpath = GetLocalAppdataPath();
	string minclaspath = wstring2string(mcpath);
	string s1="\\zytheater\\log";
	s1.c_str();
	minclaspath.append(s1);
	zip_un smzip;
	string strZipPath_un, strZipPath = minclaspath;
	strZipPath_un = strZipPath + ".zip";
	smzip.Zip_PackFiles(strZipPath);
	string fname;
	fname = "C:\\journalFolder";
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	CString str1 = strZipPath_un.c_str();
	fname.append("\\zytheatherlog.zip");
	//cout << "debug" << endl;
	//cout << fname << endl;
	CString str2 = fname.c_str();
	CopyFile(str1, str2, FALSE);*/
	string s1 = "\\zytheater\\log";
	string s2 = "\\smallclasslog.zip";
	logpath(s1, s2);
	return;
}

void test2()//三分屏处理
{
	mark = 2;
	hKey = NULL;
	if (_mark == 64)
	{
		strSubKey = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{2F0F058C-D340-4520-948E-7639602467EF}_is1");//读取三分屏64位
		strValueName = _T("InstallLocation");
	}
	else if (_mark == 32)
	{
		strSubKey = _T("SOFTWARE\\WOW3232Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{2F0F058C-D340-4520-948E-7639602467EF}_is1");//读取三分屏32位
		strValueName = _T("InstallLocation");
	}
	status = QueryRegKey(strSubKey, strValueName, strValue1, length);
	//cout << "status数值";
	//cout << status << endl;
	if (status != 1)
	{
		cout << "收集不到该软件的日志地址请确认是否安装了此app并成功打开并运行" << endl;
	}
	//cout << status << endl;
	int _len = strlen(strValue1);
	string ss = "";
	string _en = "uninst_config.ini";
	string ap = "\\";
	for (int id = 0; id < _len; id++)
	{
		if (strValue1[id] == '92')
		{
			ss.append(ap);
		}
		else ss.push_back(strValue1[id]);
	}
	ss.append(_en); string ass;
	ifs.open(ss); vector<string>svec;
	while (ifs)
	{
		getline(ifs, ass);
		if (ass[0] == 'l')
		{
			ss = ass;
		}
	}
	for (int i = 1; i < ss.size(); i++)
	{
		if (ss[i - 1] == '=')
		{
			int _le = ss.size();
			ss = ss.substr(i, _le);
		}
	}
	_en = "\\zylive\\log";
	ss.append(_en);
	//cout << ss << endl;
	zip_un z;
	string strZipPath_un, strZipPath = ss;
	strZipPath_un = strZipPath + ".zip";
	name();
	z.Zip_PackFiles(strZipPath);
	for (int i = 0; i < strZipPath_un.size(); i++)
	{
		if (strZipPath_un[i] == '/')
			strZipPath_un[i] = '\\';
	}
	//cout << strZipPath_un << endl;
	string zu = TCHAR2STRING(path);
	string fname = "";
	fname.append(desktop);
	//cout << zu << endl;
	fname = fname + "\\journalFolder";
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	
	CString str1 = strZipPath_un.c_str();
	fname.append("\\zyLecturelog.zip");
	CString str2 = fname.c_str();
	CopyFile(str1, str2, FALSE);
	return;
}
void test3()//小灶课
{
	string s1 = "\\zystove\\log";
	string s2 = "\\ztstovelog.zip";
	logpath(s1, s2);
	return;
}

void test4()//公立校
{    
	string s1 = "\\zyschool\\log";
	string s2 = "\\zyschoollog.zip";
	logpath(s1, s2);
	return;
}

void test5()//辅导端
{
	string s1 = "\\zycounsellor\\logs";
	string s2 = "\\zycounsellorlog.zip";
	logpath(s1, s2);
	return;
}

void test6()//学生端
{
	string s1 = "\\ZYStudent\\ZYLauncherLog";
	string s2 = "\\zyStudentlog.zip";
	logpath(s1, s2);
	return;
}

int main()
{
	GetSystemDirectory(path,MAX_PATH);
	TCHAR path[255];
	SHGetSpecialFolderPath(0, path, CSIDL_DESKTOPDIRECTORY, 0);
	desktop= TCHARSTRING(path);
	if (IsWow64())
	{
		_mark = 64;
	}
	else
	{
		_mark = 32;
	}
	//string result;
	cout << "生成日志存放在桌面journalFolder文件夹下 " << endl;
	cout << "小班课日志为smallclasslog.zip 三分屏日志为zyLecturelog.zip" << endl;
	cout << "小灶课日志为ztstovelog.zip 公立校日志为zyschoollog.zip"<< endl;
	cout << "辅导端的日志为zycounsellorlog.zip 学生端的日志为zystudentlog.zip" << endl;
		for (Case = 1; Case <= 7; Case++)
		{
			if (Case == 7)break;
			switch (Case)
			{
			case 1: test1(); break;//小班课
			case 2: test2(); break;//三分屏
			case 3: test3(); break;//小灶课
			case 4: test4(); break;//公立校
			case 5: test5(); break;//辅导端
			case 6: test6(); break;//学生端
			default:  break;
			}
		}
	//system("pause");
	system("pause");
	return 0;
}
