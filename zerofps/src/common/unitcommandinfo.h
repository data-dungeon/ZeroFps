// unitcommandinfo.h: interface for the unitcommandinfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNITCOMMANDINFO_H__B7E45861_6E60_45D4_AD4C_E4783386312E__INCLUDED_)
#define AFX_UNITCOMMANDINFO_H__B7E45861_6E60_45D4_AD4C_E4783386312E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct UnitCommandInfo  
{
	char m_acCommandName[64];
	char m_acComments[256];
	bool m_bNeedTarget;
	bool m_bNeedDestination;
	int  m_iIconIndex;
};

#endif // !defined(AFX_UNITCOMMANDINFO_H__B7E45861_6E60_45D4_AD4C_E4783386312E__INCLUDED_)
