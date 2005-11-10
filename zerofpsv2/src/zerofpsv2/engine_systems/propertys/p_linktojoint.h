#ifndef _ENGINE_LINKTOJOINT_H_
#define _ENGINE_LINKTOJOINT_H_

#include "../mad/mad_modell.h"
#include "../../engine/zerofps.h"
#include "../../engine/property.h"
#include <string>
#include "../engine_systems_x.h"

using namespace std;


/**	\brief	Property to link a object to a MAD.Joint of the parent

	this property is used to attach and entity to a specific joint on a mad model on a specified entity or parent if entity id is -1
	it updates both in render and normal propertys to make sure position is up to date even when object is culled.	
*/
class ENGINE_SYSTEMS_API P_LinkToJoint : public Property
{
	private:
		string			m_strToJoint;																///< joint name to attach to 
		int				m_iLinkEntityID;															///< entity wich to attach to, or -1 if parent is to be used		
		unsigned int	m_iLastFrame;																///< keeps track of when the object was last updated
	
		bool HandleSetValue( const string& kValueName ,const string& kValue );
		
	public:
		P_LinkToJoint();
		~P_LinkToJoint();
		
		void Init();
		void Update();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		void SetJoint(const string& strJoint);
		void SetLinkEntity(int iID);
};

Property* Create_LinkToJoint();



#endif






