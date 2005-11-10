// This is a property for easily controlling entities speed, acc., and rotation
// ex.: Bind (KEY_UP, SpdZ, 0.5) will make the entities Speed in Z raise by 0.5 if up arrow is pressed
#ifndef _ENGINE_PCONTROLLER_H_
#define _ENGINE_PCONTROLLER_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"

enum eControlInput {VEL_X, VEL_Y, VEL_Z, ACC_X, ACC_Y, ACC_Z, ROT_X, ROT_Y, ROT_Z};
enum eControlType {eKEYBOARD, eMOUSE, eJOYPAD};

struct sTrigger 
{
	eControlType m_eControlType;
	string m_strKey;
	int m_iInputTrigger;
	float m_fValue;
};

class ENGINE_SYSTEMS_API P_Controller : public Property {

	private:
		bool m_bCameraRotation; // rotates force with camera y-axis

		InputHandle* m_pkInputHandle;

		vector<sTrigger> m_kTriggers;
	public:

		void SetCameraRotation (bool bTrue);
		void AddTrigger (string strKey, int iAxis, float fValue);
		void SpringTrigger (int iAxis, float fValue);

		void Update();

		P_Controller( string kPSType );
		P_Controller();
        ~P_Controller();
		
		bool HandleSetValue( const string& kValueName, const string& kValue );

		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);

		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );
};


namespace SI_PController
{
	int ENGINE_SYSTEMS_API BindKeyLua(lua_State* pkLua);
	int ENGINE_SYSTEMS_API RotWithCamYAxisLua(lua_State* pkLua);
}

Property* Create_P_Controller();
void Register_PController(ZSSZeroFps* pkZeroFps);


#endif





