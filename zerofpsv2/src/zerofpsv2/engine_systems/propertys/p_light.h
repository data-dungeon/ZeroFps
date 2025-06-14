#ifndef _ENGINE_LIGHTPROPERTY_H_
#define _ENGINE_LIGHTPROPERTY_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../../engine/zerofps.h"
#include "../../basic/zfresource.h"

using namespace std;


/// Property to make an object cast light.
class ENGINE_SYSTEMS_API P_Light : public Property 
{
	private:
		ZSSZeroFps*			m_pkZeroFps;
		LightSource*		m_pkLightSource;
		ZSSLight*			m_pkLight;	
		ZSSRender*			m_pkRender;
		ZShaderSystem*		m_pkZShaderSystem;
		
		int				m_iMode;		
		float				m_fTimer;
		Vector3			m_kOffset;
		float				m_fFlareSize;
		
		ZFResourceHandle* m_pkMaterial;
		string				m_strMaterial;
		
		RenderPackage		m_kRenderPackage;
		
		bool HandleSetValue( const string& kValueName ,const string& kValue );		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
	 
		void SetMaterial(const string& strMaterial);
		

		void UpdateLightMode();		
		void DrawFlare();
	public:
		P_Light();
		~P_Light();

		void GetRenderPackages(vector<RenderPackage*>&	kRenderPackages,const RenderState&	kRenderState);
		void Update();
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;
		void Init();
		inline void SetPos(Vector3 kPos)										{m_pkLightSource->kPos=kPos;};	
		inline void SetRot(Vector3 kRot)										{m_pkLightSource->kRot=kRot;};	
		inline void SetDiffuse(Vector4 kDiffuse)							{m_pkLightSource->kDiffuse=kDiffuse;};
		inline void SetAmbient(Vector4 kAmbient)							{m_pkLightSource->kAmbient=kAmbient;};
		inline void SetSpecular(Vector4 kSpecular)						{m_pkLightSource->kSpecular=kSpecular;};		
		inline void SetType(int iType) 										{m_pkLightSource->iType=iType;};
		inline void SetPriority(int iPriority) 							{m_pkLightSource->iPriority=iPriority;};
		inline void SetCutoff(float fCutoff) 								{m_pkLightSource->fCutoff=fCutoff;};
		inline void SetExp(float fExp) 										{m_pkLightSource->fExp=fExp;}; 
		inline void SetConst_Atten(float fConst_Atten) 					{m_pkLightSource->fConst_Atten=fConst_Atten;};
		inline void SetLinear_Atten(float fLinear_Atten) 				{m_pkLightSource->fLinear_Atten=fLinear_Atten;};
		inline void SetQuadratic_Atten(float fQuadratic_Atten) 		{fQuadratic_Atten=fQuadratic_Atten;};	

		inline Vector4 GetDiffuse()	{return m_pkLightSource->kDiffuse;};
		inline Vector4 GetAmbient()	{return m_pkLightSource->kAmbient;};		
		inline Vector3 GetRot()			{return m_pkLightSource->kRot;};				
		
		void TurnOn();
		void TurnOff();

		void	OnEvent(GameMessage& Msg);

};

Property* Create_LightProperty();

#endif



