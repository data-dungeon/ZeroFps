#ifndef _GUISERVERINFO_H_
#define _GUISERVERINFO_H_

#include <string>
#include <vector>
using namespace std;

class GUIServerInfo
{
public:
	string strServerName, strServerIP, strUserName, strPassword;

	GUIServerInfo() { 
		strServerName = strServerIP = strUserName	= strPassword = ""; 
	};

	GUIServerInfo(string sn, string ip, string un, string pw) {
		strServerName = sn; strServerIP = ip; strUserName = un; strPassword = pw;
	};

	string FullName() { 
		return strServerName + "|" + strServerIP + "|" + strUserName + 
			"|" + strPassword; 
	}

	bool ConvertFromFullName(string strFullName);

	bool GUIServerInfo::operator==(GUIServerInfo cmp) {  
		return(cmp.strServerName == strServerName && cmp.strUserName == strUserName);
	};

	static void Save(vector<GUIServerInfo>& rkList);
	static void Load(vector<GUIServerInfo>& rkLis);
};

#endif // _GUISERVERINFO_H_