// aibase.h: interface for the aibase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AIBASE_H__4CC8C070_719D_4D80_83F2_FB920119C51E__INCLUDED_)
#define AFX_AIBASE_H__4CC8C070_719D_4D80_83F2_FB920119C51E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AIBase  
{
public:
	//AIBase();
	//virtual ~AIBase();
	virtual AIBase* UpdateAI() =0;
	virtual AIBase* RunUnitCommand(int iCommandID, int iXDestinaton, int iYDestinaton, int iTarget) =0;
	
};

#endif // !defined(AFX_AIBASE_H__4CC8C070_719D_4D80_83F2_FB920119C51E__INCLUDED_)
