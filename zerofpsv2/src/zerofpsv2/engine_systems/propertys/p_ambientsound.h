#ifndef _ENGINE_P_AMBIENTSOUND_H_
#define _ENGINE_P_AMBIENTSOUND_H_

#include "../../engine/property.h"
#include "../engine_systems_x.h"
#include "../../engine/zerofps.h"

using namespace std;

class ENGINE_SYSTEMS_API P_AmbientSound : public Property 
{
	private:

		char* m_szFileName;
		bool m_bLoop;

	public:

		P_AmbientSound();
		~P_AmbientSound();

		void CloneOf(Property* pkProperty);
		void Update();

};

Property* Create_AmbientProperty();


#endif // #ifndef _ENGINE_P_AMBIENTSOUND_H_



