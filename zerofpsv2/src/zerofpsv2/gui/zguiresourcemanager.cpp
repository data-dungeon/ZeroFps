// zguiresourcemanager.cpp: implementation of the ZGuiResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "zguiresourcemanager.h"
#include "../basic/zguiskin.h"
#include "../basic/zguifont.h"
#include "../basic/zfassert.h"
#include "zguiwindow.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ZGuiResourceManager::ZGuiResourceManager() : ZFSubSystem("ZGuiResourceManager")
{

}

ZGuiResourceManager::~ZGuiResourceManager()
{

}

ZGuiWnd* ZGuiResourceManager::Wnd(string strName)
{
	map<string, ZGuiWnd* >::iterator it;
	it = m_kWindows.find(strName);
	if(it != m_kWindows.end())
		return it->second;
	return NULL;
}

ZGuiSkin* ZGuiResourceManager::Skin(string strName)
{
	map<string, ZGuiSkin* >::iterator it;
	it = m_kSkins.find(strName);
	if(it != m_kSkins.end())
		return (*it).second;
	return NULL;
}

ZGuiFont* ZGuiResourceManager::Font(string strName)
{
	map<string, ZGuiFont* >::iterator it;
	it = m_kFonts.find(strName);
	if(it != m_kFonts.end())
		return it->second;
	return NULL;
}

ZGuiWnd* ZGuiResourceManager::Add(string strName, ZGuiWnd* pkNewWnd)
{
	ZGuiWnd* pkExisting = Wnd(strName);
	if( pkExisting == NULL) 
	{
		m_kWindows.insert( map<string, ZGuiWnd*>::value_type(strName, pkNewWnd));
		return Wnd(strName);
	}

	return pkExisting;
}

ZGuiSkin* ZGuiResourceManager::Add(string strName, ZGuiSkin* pkNewSkin)
{
	ZGuiSkin* pkExisting = Skin(strName);
	if( pkExisting != NULL)
		return pkExisting;
	else
	{
		m_kSkins.insert( map<string, ZGuiSkin*>::value_type(strName, pkNewSkin));
		return Skin(strName);
	}
}

ZGuiFont* ZGuiResourceManager::Add(string strName, ZGuiFont* pkNewFont)
{
	ZGuiFont* pkExisting = Font(strName);
	if( pkExisting == NULL) 
	{
		m_kFonts.insert( map<string, ZGuiFont*>::value_type(strName, pkNewFont));
		return Font(strName);
	}

	return pkExisting;
}

bool ZGuiResourceManager::RemoveWnd(string strName)
{
	map<string, ZGuiWnd*>::iterator it;
	it = m_kWindows.find(strName);
	if(it != m_kWindows.end())
	{
		m_kWindows.erase(it);
		return true;
	}

	return false;
}

bool ZGuiResourceManager::RemoveSkin(string strName)
{
	map<string, ZGuiSkin*>::iterator it;
	it = m_kSkins.find(strName);
	if(it != m_kSkins.end())
	{
		m_kSkins.erase(it);
		return true;
	}

	return false;
}

bool ZGuiResourceManager::RemoveFont(string strName)
{
	map<string, ZGuiFont*>::iterator it;
	it = m_kFonts.find(strName);
	if(it != m_kFonts.end())
	{
		m_kFonts.erase(it);
		return true;
	}

	return false;
}

bool ZGuiResourceManager::StartUp()	{ return true; }
bool ZGuiResourceManager::ShutDown() { return true; }
bool ZGuiResourceManager::IsValid()	{ return true; }












