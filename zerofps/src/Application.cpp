#include "ZeroFps.h"
#include "Application.h"

Application* Application::pkApp;

Application::Application(char* pName,int iWidth,int iHeight,int iDepth) {
	Application::pkApp=this;
	
	m_pTitle=pName;
	m_iWidth=iWidth;
	m_iHeight=iHeight;
	m_iDepth=iDepth;
}
	
	
	
	
	
	

