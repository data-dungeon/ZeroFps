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


#endif


