#ifndef _ENGINE_P_CONTAINER_H_
#define _ENGINE_P_CONTAINER_H_

#include "../zerofpsv2/engine/property.h"
#include "mcommon_x.h"
#include <vector>
#include "p_container.h"
#include "p_item.h"

using namespace std;


/**	\brief	Da P_Container
		\ingroup Common
*/
class MCOMMON_API P_Container: public Property 
{
	private:
		EntityManager*		m_pkEntityMan;
	
		int 	m_iSizeX;
		int 	m_iSizeY;
		bool	m_bFirstUpdate;
	
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
		void SetSize();
		
	public:
		MLContainer*	m_pkContainer;
		
		P_Container();		
		~P_Container();
		
		void Init();			
		void Update();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);			

};

MCOMMON_API Property* Create_P_Container();

#endif










