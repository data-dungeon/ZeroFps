//#include <windows.h>
#include <iostream>
#include <cstdio>
#include <SDL/SDL.h>
#include <GL/GLee.h>
#include <vector>

using namespace std;

void Log_DriverInfo()
{
	vector<string>	astrExt;

	cout<<"Gl Version: "<<(char*)glGetString(GL_VERSION)<<endl;
	cout<<"Gl Vendor:  "<<(char*)glGetString(GL_VENDOR)<<endl;
	cout<<"Gl Render:  "<<(char*)glGetString(GL_RENDERER)<<endl;
	cout<<"Gl Extensions:" << endl;

	int i = 0;
	char szExtName[256];
	unsigned char* pcExt1 = const_cast<unsigned char*>(glGetString(GL_EXTENSIONS));
	
	if(!pcExt1)
	{		
		cout<<"didt get any extensions"<<endl;
		exit(1);
	}
	
	char* pcExt = (char*)pcExt1;
	while(sscanf(pcExt, "%s", szExtName) != EOF) 
	{
		//cout<<" Ext["<<i<<"]:"<<szExtName<<endl;
		astrExt.push_back( string(szExtName) );
		i++;
		pcExt += strlen(szExtName) + 1;
	}

	for(int i=0; i<astrExt.size(); i++)
		cout<<" Ext["<<i<<"]:"<<astrExt[i]<<endl;
}

int main(int argc, char *argv[])
{
	int m_iWidth = 640;
	int m_iHeight = 480;
	int m_iDepth = 16;

	if(SDL_Init(SDL_OPENGL | SDL_INIT_NOPARACHUTE )<0)
	{
		cout << "Error initiating SDL." << endl;
		exit(1);
	}	
	
	SDL_Surface* m_pkScreen= SDL_SetVideoMode(m_iWidth,m_iHeight,m_iDepth,SDL_OPENGL);
	glViewport(0, 0,m_iWidth,m_iHeight);	

	Log_DriverInfo();

	return 0;
}
