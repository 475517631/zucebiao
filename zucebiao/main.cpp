// zucebiao.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include<Windows.h>
#include<io.h>
#include<direct.h>
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
int length = 256;
int Case;
bool status;
string desktop;
HKEY  hKey = NULL;
LPCWSTR strSubKey; LPCWSTR strValueName;
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


wstring string2wstring(string str)
{
	wstring result;
	//获取缓冲区大小，并申请空间，缓冲区大小按字符计算  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //添加字符串结尾  
									//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;
	return result;
}

std::string TCHAR2STRING(TCHAR* str)//tchar转string 返回第一个字符
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
	strstr = strstr[0];
	return strstr;
}


int  dir(string path)
{
	long hFile = 0;  int nu = 0;
	struct _finddata_t fileInfo;
	string pathName, excName;
	// \\*代表要遍历所有的类型
	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1)
	{
		return 0;
	}
	do
	{
		nu++;
		//判断文件的属性是文件夹还是文件
		//cout << fileInfo.name << (fileInfo.attrib&_A_SUBDIR ? "[folder]" : "[file]") << endl;
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return nu;

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
	return strstr;
}
bool OpenRegKey(HKEY& hRetKey)
{
	LPCWSTR sw;
	if (mark == 1)
	{
	if (_mark == 64)
	{
	sw = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{0D9BCA0A-76FF-4FAC-A9ED-CA05B66C27B7}_is1");//小班课注册表地址64位
	}else if (_mark==32)
	{
	sw = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{0D9BCA0A-76FF-4FAC-A9ED-CA05B66C27B7}_is1");//小班课注册表地址32位
	}
	}else if (mark == 2)
	{
		if (_mark == 64)
		{
			sw = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{2F0F058C-D340-4520-948E-7639602467EF}_is1");//三分屏注册表地址64位
		}
		else if (_mark == 32)
		{
			sw = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{2F0F058C-D340-4520-948E-7639602467EF}_is1");//三分屏注册表地址32位
		}
	}
	else if (mark == 3)
	{
		if (_mark == 64)
		{
			sw = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{6E49F7DF-907C-4A7B-B240-199A3396CA3A}_is1");//小灶课注册表地址64位
		}
		else if (_mark == 32)
		{
			sw = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{6E49F7DF-907C-4A7B-B240-199A3396CA3A}_is1");//小灶课注册表地址32位
		}
	}
	else if (mark == 4)
	{
		if (_mark == 64)
		{
			sw = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{A843C0F2-287C-4F7E-B1D2-85FC3CDE3515}_is1");//公立校注册表地址64位
		}
		else if (_mark == 32)
		{
			sw = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{A843C0F2-287C-4F7E-B1D2-85FC3CDE3515}_is1");//公立校注册表地址32位
		}
	}
	else if (mark == 5)
	{
		if (_mark == 64)
		{
			sw = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B724361C-7994-4D17-9E05-67BA819DD32A}_is1");//辅导端注册表地址64位
		}
		else if (_mark == 32)
		{
			sw = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B724361C-7994-4D17-9E05-67BA819DD32A}_is1");//辅导端注册表地址32位
		}
	}
	else if (mark == 6)
	{
		if (_mark == 64)
		{
			sw = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F1E94093-BFBE-40FF-826F-79A348111AD4}_is1");//学生端注册表地址64位
		}
		else if (_mark == 32)
		{
			sw = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F1E94093-BFBE-40FF-826F-79A348111AD4}_is1");//学生端注册表地址32位
		}
	}

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

void findAllFile(const char * path, const char * format,string mupath) //搜索.dmp文件
{
	//cout << path << endl;
	char newpath[200];
	strcpy(newpath, path);
	strcat(newpath, "\\*.*");    // 在目录后面加上"\\*.*"进行第一次搜索
	int handle;
	_finddata_t findData;
	handle = _findfirst(newpath, &findData);
	if (handle == -1)        // 检查是否成功
		return;
	while (_findnext(handle, &findData) == 0) {
		if (findData.attrib & _A_SUBDIR) {
			if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
				continue;
			strcpy(newpath, path);
			strcat(newpath, "\\");
			strcat(newpath, findData.name);
			findAllFile(newpath, format,mupath);
		}
		else {
			if (strstr(findData.name, format)) {   //判断是不是dmp文件
				string st = path;
				st.append("\\");
				st.append(findData.name);
				//cout << st << endl;
				CString str1 = st.c_str();
				string ss = mupath;
				ss.append("\\");
				ss.append(findData.name);
				CString str2 =ss.c_str();
				//cout << ss << endl;
				CopyFile(str1,str2,FALSE);
			//	cout << findData.name << "\t" << path << "\t" << findData.size << " bytes.\n";
			}
		}
	}
	_findclose(handle);    // 关闭搜索句柄
}

void listFiles(string dir) {
	//在目录后面加上"\\*.*"进行第一次搜索
	string newDir = dir + "\\*.*";
	//用于查找的句柄
	intptr_t handle;
	struct _finddata_t fileinfo;
	//第一次查找
	handle = _findfirst(newDir.c_str(), &fileinfo);

	if (handle == -1) {
		cout << "无文件" << endl;
		system("pause");
		return;
	}

	do
	{
		if (fileinfo.attrib & _A_SUBDIR) {//如果为文件夹，加上文件夹路径，再次遍历
			if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0)
				continue;

			// 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
			newDir = dir + "\\" + fileinfo.name;
			listFiles(newDir.c_str());//先遍历删除文件夹下的文件，再删除空的文件夹
			//cout << newDir.c_str() << endl;
			if (_rmdir(newDir.c_str()) == 0) {//删除空文件夹
				//cout << "delete empty dir success" << endl;
			}
			else {
				//cout << "delete empty dir error" << endl;
			}
		}
		else {
			string file_path = dir + "\\" + fileinfo.name;
		//	cout << file_path.c_str() << endl;
			if (remove(file_path.c_str()) == 0) {//删除文件
				//cout << "delete file success" << endl;
			}
			else {
				//cout << "delete file error" << endl;
			}
		}
	} while (!_findnext(handle, &fileinfo));

	_findclose(handle);
	return;
}

void logpath(string apppath, string mupath)//第一个参数是表示读取log位置 第二个参数是文件生成文件名
{
	wstring mcpath = GetLocalAppdataPath();
	string minclaspath = wstring2string(mcpath);
	apppath.c_str();
	minclaspath.append(apppath);
	zip_un smzip;
	string strZipPath_un, strZipPath = minclaspath;
	//cout << strZipPath << endl;
	strZipPath_un = strZipPath + ".zip";
	//cout << strZipPath_un << endl;
	//name();
	if (dir(strZipPath) < 3)
	{
		//cout << "log该目录为空" << endl;
		return;
	}
	smzip.Zip_PackFiles(strZipPath);
	string fname = "";
	fname.append(desktop);
	fname = fname + "\\journalFolder";
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	CString str1 = strZipPath_un.c_str();
	//cout << strZipPath_un << endl;
	fname.append(mupath);
	//cout << fname << endl;
	CString str2 = fname.c_str();
	CopyFile(str1, str2, FALSE);
	//cout << strZipPath_un << endl;
	remove(strZipPath_un.c_str());
	return;
}
void test1()//小班课
{
	string s1 = "\\zytheater\\log";
	string s2 = "\\smallclasslog.zip";
	mark = 1;
	logpath(s1, s2);
	hKey = NULL;
	if (_mark == 64)
	{
		strSubKey = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{0D9BCA0A-76FF-4FAC-A9ED-CA05B66C27B7}_is1");
		strValueName = _T("InstallLocation");
	}
	else if (_mark == 32)
	{
		strSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{0D9BCA0A-76FF-4FAC-A9ED-CA05B66C27B7}_is1");
		strValueName = _T("InstallLocation");
	}
	status = QueryRegKey(strSubKey, strValueName, strValue1, length);
	string ss = ""; string fri = "";
	ss.append(strValue1);
	fri = ss;
	fri.append("\Application\\");
	string en = "\\User Data\\version.ini";
	ss.append(en); 
	ifstream ifs; string ass;
	ifs.open(ss); vector<string>svec;
	while (ifs)
	{
		getline(ifs, ass);
		if (ass[0] == 'l')
		{
			break;
		}
	}
	string ver= "";
	for (int i = 0; i < ass.size(); i++)
	{
		if (ass[i] >= '0'&&ass[i] <= '9')
		{ 
			int _le = ass.size();
			ver=ass.substr(i,_le);
			break;
		}
	}
	string fname = "";;
	fname.append(desktop);
	fname = fname + "\\journalFolder\\smallclasscrash";
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	findAllFile(fri.c_str(), ".dmp",fname);
	wstring s = string2wstring(fname);
	RemoveDirectory(s.c_str());
	return;
}

void test2()//三分屏处理
{
	mark = 2; int flag = 0;
	hKey = NULL;
	if (_mark == 64)
	{
		strSubKey = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{2F0F058C-D340-4520-948E-7639602467EF}_is1");//读取三分屏64位
		strValueName = _T("InstallLocation");
	}
	else if (_mark == 32)
	{
		strSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{2F0F058C-D340-4520-948E-7639602467EF}_is1");//读取三分屏32位
		strValueName = _T("InstallLocation");
	}
	status = QueryRegKey(strSubKey, strValueName, strValue1, length);
	//cout << "status数值";
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
	ss.append(_en);
	string ass; ifstream ifs;
	ifs.open(ss);vector<string>svec;
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
	if (dir(ss)<3)
	{
		flag = 1;
		//name();
		//cout << "log目录为空" << endl;
	}
	if (flag == 0)
	{
		zip_un z;
		string strZipPath_un, strZipPath = ss;
		strZipPath_un = strZipPath + ".zip";
		//name();
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
		remove(strZipPath_un.c_str());
	}
	ss = ""; string fri = "";
	ss.append(strValue1);
	fri = ss;
	string fname = "";;
	fname.append(desktop);
	fname = fname + "\\journalFolder\\zyLecturecrash";
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	findAllFile(fri.c_str(), ".dmp", fname);
	wstring s = string2wstring(fname);
	RemoveDirectory(s.c_str());
   	return;
}
void test3()//小灶课
{
	string s1 = "\\zystove\\log";
	string s2 = "\\ztstovelog.zip";
	logpath(s1, s2);
	mark = 3;
	hKey = NULL;
	if (_mark == 64)
	{
		strSubKey = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{6E49F7DF-907C-4A7B-B240-199A3396CA3A}_is1");
		strValueName = _T("InstallLocation");
	}
	else if (_mark == 32)
	{
		strSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{6E49F7DF-907C-4A7B-B240-199A3396CA3A}_is1");
		strValueName = _T("InstallLocation");
	}
	status = QueryRegKey(strSubKey, strValueName, strValue1, length);
	string ss = ""; string fri = "";
	ss.append(strValue1);
	fri = ss;
	string fname = "";;
	fname.append(desktop);
	fname = fname + "\\journalFolder\\zystovecrash";
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	findAllFile(fri.c_str(), ".dmp", fname);
	wstring s = string2wstring(fname);
	RemoveDirectory(s.c_str());
	return;
}

void test4()//公立校
{
	string s1 = "\\zyschool\\log";
	string s2 = "\\zyschoollog.zip";
	logpath(s1, s2);
	mark = 4;
	hKey = NULL;
	if (_mark == 64)
	{
		strSubKey = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{A843C0F2-287C-4F7E-B1D2-85FC3CDE3515}_is1");
		strValueName = _T("InstallLocation");
	}
	else if (_mark == 32)
	{
		strSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{A843C0F2-287C-4F7E-B1D2-85FC3CDE3515}_is1");
		strValueName = _T("InstallLocation");
	}
	status = QueryRegKey(strSubKey, strValueName, strValue1, length);
	string ss = ""; string fri = "";
	ss.append(strValue1);
	fri = ss;
	string fname = "";;
	fname.append(desktop);
	fname = fname + "\\journalFolder\\zyschoolcrash";
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	findAllFile(fri.c_str(), ".dmp", fname);
	wstring s = string2wstring(fname);
	RemoveDirectory(s.c_str());
	return;
}

void test5()//辅导端
{
	string s1 = "\\zycounsellor\\logs";
	string s2 = "\\zycounsellorlog.zip";
	logpath(s1, s2);
	mark = 5;
	hKey = NULL;
	if (_mark == 64)
	{
		strSubKey = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B724361C-7994-4D17-9E05-67BA819DD32A}_is1");
		strValueName = _T("InstallLocation");
	}
	else if (_mark == 32)
	{
		strSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{B724361C-7994-4D17-9E05-67BA819DD32A}_is1");
		strValueName = _T("InstallLocation");
	}
	status = QueryRegKey(strSubKey, strValueName, strValue1, length);
	string ss = ""; string fri = "";
	ss.append(strValue1);
	fri = ss;
	string fname = "";;
	fname.append(desktop);
	fname = fname + "\\journalFolder\\zycounsellorcrash";
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	findAllFile(fri.c_str(), ".dmp", fname);
	wstring s = string2wstring(fname);
	RemoveDirectory(s.c_str());
	return;
}

void test6()//学生端
{
	string s1 = "\\ZYStudent\\ZYLauncherLog";
	string s2 = "\\zyStudentlog.zip";
	logpath(s1, s2);
	s1 = "\\ZYStudent\\logs";
	s2 = "\\zyStudentlogs.zip";
	logpath(s1, s2);
	mark = 6;
	hKey = NULL;
	if (_mark == 64)
	{
		strSubKey = _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F1E94093-BFBE-40FF-826F-79A348111AD4}_is1");
		strValueName = _T("InstallLocation");
	}
	else if (_mark == 32)
	{
		strSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F1E94093-BFBE-40FF-826F-79A348111AD4}_is1");
		strValueName = _T("InstallLocation");
	}
	status = QueryRegKey(strSubKey, strValueName, strValue1, length);
	string ss = ""; string fri = "";
	ss.append(strValue1);
	fri = ss;
	string fname = "";;
	fname.append(desktop);
	fname = fname + "\\journalFolder\\zystudentcrash";
	if (0 != access(fname.c_str(), 0))
	{
		mkdir(fname.c_str());
	}
	findAllFile(fri.c_str(), ".dmp", fname);
	wstring s = string2wstring(fname);
	RemoveDirectory(s.c_str());
	return;
}

void test7()
{
	zip_un smzip;
	string zz = "";
	zz.append(desktop);
	zz = zz + "\\journalFolder";
	//cout << zz << endl;
	
	smzip.Zip_PackFiles(zz);
	//system("rd C:\Users\\i5-10400F\\Desktop\\journalFolder");
	wstring ss = string2wstring(zz);
	listFiles(zz);
	//cout << zz << endl;
	RemoveDirectory(ss.c_str());
	return;
}

int main()
{
	GetSystemDirectory(path, MAX_PATH);
	TCHAR path[255];
	SHGetSpecialFolderPath(0, path, CSIDL_DESKTOPDIRECTORY, 0);
	desktop = TCHARSTRING(path);
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
	/*cout << "小班课日志为smallclasslog.zip 三分屏日志为zyLecturelog.zip" << endl;
	cout << "小灶课日志为ztstovelog.zip 公立校日志为zyschoollog.zip" << endl;
	cout << "辅导端的日志为zycounsellorlog.zip 学生端的日志为zystudentlog.zip" << endl;*/
	for (Case = 1; Case <= 8; Case++)
	{
		if (Case == 8)break;
		switch (Case)
		{
		case 1: test1(); break;//小班课
		case 2: test2(); break;//三分屏
		case 3: test3(); break;//小灶课
		case 4: test4(); break;//公立校
		case 5: test5(); break;//辅导端
		case 6: test6(); break;//学生端
		case 7: test7(); break;
		default:  break;
		}
	}
	//system("pause");
	system("pause");
	return 0;
}
