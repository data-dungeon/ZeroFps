#ifndef _MADPROPERTY_H_
#define _MADPROPERTY_H_

#include "../render/render.pkg"
#include "property.h"

using namespace std;

class ENGINE_API MadProperty: public Property {
	private:
		Core*	pkCore;

		int		iActiveAnimation;
		float	fCurrentTime;
		float	fLastUpdate;


	public:
		MadProperty(Core* pkModell);

		void Update();
		void SetBase(Core* pkModell);
		void PlayAnimation(int iAnimNum, float fStartTime);
		void UpdateAnimation(void);
		void Draw(void);
};



#endif



