#include "guiserverinfo.h"
#include <iostream.h>


bool GUIServerInfo::ConvertFromFullName(string strFullName)
{
	strServerName = strFullName;
	strServerIP = strFullName;
	strUserName = strFullName;
	strPassword = strFullName;

	int p = 0 , l = strFullName.length();
	
	p = strServerName.find("|"); if(p == string::npos) return false;
	strServerName.erase(p, strServerName.length()-p);

	strServerIP.erase(0, strServerName.length()+1);
	p = strServerIP.find("|"); if(p == string::npos) return false;
	strServerIP.erase(p, strServerIP.length()-p);

	strUserName.erase(0, strServerIP.length()+1+strServerName.length()+1);
	p = strUserName.find("|"); if(p == string::npos) return false;
	strUserName.erase(p, strUserName.length()-p);

	strPassword.erase(0, strServerIP.length()+1+strServerName.length()+1+strUserName.length()+1);

	//	printf("strServerName = %s\n", strServerName.c_str());
	//	printf("strServerIP = %s\n", strServerIP.c_str());
	//	printf("strUserName = %s\n", strUserName.c_str());
	//	printf("strPassword = %s\n", strPassword.c_str());

	return true;
}


void GUIServerInfo::Save(vector<GUIServerInfo>& rkServerAray)
{
	FILE* pkFile = pkFile = fopen("serverlist.txt", "wt");
	if(pkFile)
	{
		for(int i=0; i<rkServerAray.size(); i++)
			fprintf(pkFile, "%s\n", rkServerAray[i].FullName());
	}
}

void GUIServerInfo::Load(vector<GUIServerInfo>& rkServerAray)
{
	FILE* pkFile = fopen("serverlist.txt", "rt");

	if(pkFile)
	{
		rkServerAray.clear();

		char strLine[512];
		while (!feof(pkFile))
		{
			if(fgets(strLine, 512, pkFile))
			{
				if(strlen(strLine) > 1)
				{
					GUIServerInfo info;
					if(info.ConvertFromFullName(strLine))
						rkServerAray.push_back(info);
				}
			}
		}
	}

	if(pkFile)
		fclose(pkFile);
}