#ifndef _ENGINE_LIGHTPROPERTY_H_
#define _ENGINE_LIGHTPROPERTY_H_

#include "../../engine/property.h"

using namespace std;

class ENGINE_API LightProperty: public Property {
	private:
		LightSource*	m_pkLightSource;
		Vector3* pkPos;
		Vector3* pkRot;

		Light*	m_pkLight;
		

		vector<PropertyValues> GetPropertyValues();
		
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
	 
		void TurnOn();
		void TurnOff();

	public:
		LightProperty();
		~LightProperty();
		void CloneOf(Property* pkProperty) { }

		void Update();
		void PackTo(NetPacket* pkNetPacket) ;
		void PackFrom(NetPacket* pkNetPacket) ;
		void Init();
		inline void SetPos(Vector3 kPos){*m_pkLightSource->kPos=kPos;};	
		inline void SetRot(Vector3 kRot){*m_pkLightSource->kRot=kRot;};	
		inline void SetDiffuse(Vector4 kDiffuse){m_pkLightSource->kDiffuse=kDiffuse;};
		inline void SetAmbient(Vector4 kAmbient){m_pkLightSource->kAmbient=kAmbient;};
		inline void SetSpecular(Vector4 kSpecular){m_pkLightSource->kSpecular=kSpecular;};		
		inline void SetType(int iType) {m_pkLightSource->iType=iType;};
		inline void SetPriority(int iPriority) {m_pkLightSource->iPriority=iPriority;};
		inline void SetCutoff(float fCutoff) {m_pkLightSource->fCutoff=fCutoff;};
		inline void SetExp(float fExp) {m_pkLightSource->fExp=fExp;}; 
		inline void SetConst_Atten(float fConst_Atten) {m_pkLightSource->fConst_Atten=fConst_Atten;};
		inline void SetLinear_Atten(float fLinear_Atten) {m_pkLightSource->fLinear_Atten=fLinear_Atten;};
		inline void SetQuadratic_Atten(float fQuadratic_Atten) {fQuadratic_Atten=fQuadratic_Atten;};	

		void	HandleGameMessage(GameMessage& Msg);

};

Property* Create_LightProperty();

#endif



