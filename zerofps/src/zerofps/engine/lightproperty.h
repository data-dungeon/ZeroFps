#ifndef _LIGHTPROPERTY_H_
#define _LIGHTPROPERTY_H_

#include "../render/render.pkg"
#include "property.h"
#include "../basic/basic.pkg"

using namespace std;

class ENGINE_API LightProperty: public Property {
	private:
		LightSource*	spot;
		Vector3* pkPos;
		Vector3* pkRot;

		Light*	m_pkLight;
		
		int m_fApa;

		vector<PropertyValues> GetPropertyValues();

	public:
		LightProperty();
		~LightProperty();

		void Update();
		void PackTo(NetPacket* pkNetPacket) ;
		void PackFrom(NetPacket* pkNetPacket) ;

		inline void SetPos(Vector3 kPos){*spot->kPos=kPos;};	
		inline void SetRot(Vector3 kRot){*spot->kRot=kRot;};	
		inline void SetDiffuse(Vector4 kDiffuse){spot->kDiffuse=kDiffuse;};
		inline void SetAmbient(Vector4 kAmbient){spot->kAmbient=kAmbient;};
		inline void SetSpecular(Vector4 kSpecular){spot->kSpecular=kSpecular;};		
		inline void SetType(int iType) {spot->iType=iType;};
		inline void SetPriority(int iPriority) {spot->iPriority=iPriority;};
		inline void SetCutoff(float fCutoff) {spot->fCutoff=fCutoff;};
		inline void SetExp(float fExp) {spot->fExp=fExp;}; 
		inline void SetConst_Atten(float fConst_Atten) {spot->fConst_Atten=fConst_Atten;};
		inline void SetLinear_Atten(float fLinear_Atten) {spot->fLinear_Atten=fLinear_Atten;};
		inline void SetQuadratic_Atten(float fQuadratic_Atten) {fQuadratic_Atten=fQuadratic_Atten;};	
};

Property* Create_LightProperty();

#endif



