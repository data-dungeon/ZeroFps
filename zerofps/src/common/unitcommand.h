// unitcommand.h: interface for the unitcommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNITCOMMAND_H__083E7F5E_EC48_4EFC_83FF_1230E7597E5D__INCLUDED_)
#define AFX_UNITCOMMAND_H__083E7F5E_EC48_4EFC_83FF_1230E7597E5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UnitCommand  
{
public:
	//unitcommand();
	//virtual ~unitcommand();
	char m_acCommandName[64]; 
	int m_iXDestinaton;
	int m_iYDestinaton; 
	int m_iTarget;
	unsigned char m_cPlayerID;
	int m_iUnitID;

};

#endif // !defined(AFX_UNITCOMMAND_H__083E7F5E_EC48_4EFC_83FF_1230E7597E5D__INCLUDED_)
