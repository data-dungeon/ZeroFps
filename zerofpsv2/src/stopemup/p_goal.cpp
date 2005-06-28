#include "p_goal.h"
#include "stopemup.h"
#include "../zerofpsv2/engine/entitymanager.h"
#include "walker.h"


P_Goal::P_Goal()
{
	m_pkStopEmUp = (StopEmUp*)(g_ZFObjSys.GetObjectPtr("Application"));

	strcpy(m_acName,"P_Goal");
	m_iType=0;
	m_iSide=0;
	m_bNetwork = false;
	m_iVersion = 1;
	

	m_iLives = 20;	
}

void P_Goal::Touch(int iID)
{
	static int iLast = -1;
	
	//hax
	if(iID == iLast)
		return;		
	iLast = iID;

	
	if(Entity* pkEnt = m_pkEntityManager->GetEntityByID(iID))
	{
		//collide with other walker
		if(pkEnt->GetType() == "walker.lua")
		{
			m_pkStopEmUp->RemoveLife();
			
			if(P_Walker* pkWalker = (P_Walker*)pkEnt->GetProperty("P_Walker"))
			{
				pkWalker->Damage(9999999,-1);
			}
		}
	}		
}

Property* Create_P_Goal()
{
	return new P_Goal;
}







