#ifndef _ENGINE_LINKTOJOINT_H_
#define _ENGINE_LINKTOJOINT_H_

#include "../mad/mad_modell.h"
#include "../../engine/zerofps.h"
#include "../../engine/property.h"
#include <string>
#include "../engine_systems_x.h"
using namespace std;

/**	\brief	Property to link a object to a MAD.Joint of the parent

	This Property links the entity to a joint of it's parent.
*/
class ENGINE_SYSTEMS_API P_LinkToJoint : public Property
{
	private:
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );

		string	m_strToJoint;
		
	public:

		P_LinkToJoint();
		~P_LinkToJoint();
		void Init();

		void CloneOf(Property* pkProperty) { };
		void Update();
		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage);

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		void SetJoint(const char* czJoint);

};

Property* Create_LinkToJoint();



#endif