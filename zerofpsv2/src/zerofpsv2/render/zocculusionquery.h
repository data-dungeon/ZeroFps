#ifndef _ZOCCULUSIONQUERY_H_
#define _ZOCCULUSIONQUERY_H_

#include "render_x.h"
#include "zshadersystem.h"

class RENDER_API ZOcculusionQuery
{
	private:
		GLuint	m_iQueryID;
		bool		m_bIsQuerying;
		
	public:
		ZOcculusionQuery();
		~ZOcculusionQuery();
	
		void Begin();
		void End();		
		bool HaveResult();
		GLuint GetResult();
};

class RENDER_API ZOcculusionTest : ZOcculusionQuery
{
	private:
		ZShaderSystem*		m_pkZShaderSystem;
		bool					m_bHaveTested;
		bool					m_bVisible;

		void DrawOcculusionBox(const Vector3& kMin,const Vector3& kMax);
		void Test(const Vector3& kMin,const Vector3& kMax);
	
	public:
		ZOcculusionTest();	
		bool Visible(const Vector3& kMin,const Vector3& kMax);
		
};

#endif


