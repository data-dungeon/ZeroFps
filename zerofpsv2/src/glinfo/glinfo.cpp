#include <GL/gl.h>
#include <iostream>
#include <cstdio>
#include <SDL/SDL.h>

using namespace std;

int main()
{
	int m_iWidth = 640;
	int m_iHeight = 480;
	int m_iDepth = 16;

	if(SDL_Init(SDL_OPENGL | SDL_INIT_NOPARACHUTE )<0)
	{
		cout<<"error initiating sdl"<<endl;
		exit(1);
	}	
	
	SDL_Surface* m_pkScreen= SDL_SetVideoMode(m_iWidth,m_iHeight,m_iDepth,SDL_OPENGL);


	glViewport(0, 0,m_iWidth,m_iHeight);	
	
	

	cout<<"Gl Version: "<<(char*)glGetString(GL_VERSION)<<endl;;
	cout<<"Gl Vendor:  "<<(char*)glGetString(GL_VENDOR)<<endl;
	cout<<"Gl Render:  "<<(char*)glGetString(GL_RENDERER)<<endl;
	cout<<"Gl Extensions:";
	
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
		cout<<" Ext["<<i<<"]:"<<szExtName<<endl;
		i++;
		pcExt += strlen(szExtName) + 1;
	}
}
