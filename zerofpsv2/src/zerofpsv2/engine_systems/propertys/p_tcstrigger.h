#ifndef _P_TCSTRIGGER_H_
#define _P_TCSTRIGGER_H_
  
#include "../../engine/property.h"
#include <vector>
#include "../engine_systems_x.h"
 

using namespace std;

class Tcs;
class P_Tcs;



enum TriggerType
{
	eSPHERE	=0,
	eBOX		=1,
};


class ENGINE_SYSTEMS_API P_TcsTrigger : public Property
{
	private:
		Tcs*			m_pkTcs;
		ZSSRender*	m_pkRender;
		
		int		m_iTriggerType;
		int		m_iTriggerID;
		
		Vector3	m_kBoxSize;
		float		m_fRadius;
		
		vector<PropertyValues> GetPropertyValues();
		
	public:
		
		P_TcsTrigger();
		~P_TcsTrigger();		
		
		void Init();
		void Update();
		
		void Enable();
		void Disable();

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		void Trigger(P_Tcs* pkTcs);

		
		int GetTriggerID()												{	return m_iTriggerID;	};
		Vector3 GetBoxSize()												{	return m_kBoxSize;	};
		

		
		friend class Tcs;
};



Property* Create_P_TcsTrigger();
void ENGINE_SYSTEMS_API Register_PTcsTrigger(ZSSZeroFps* pkZeroFps);


#endif







