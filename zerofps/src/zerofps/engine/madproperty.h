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

		
/*
		// NEED TO WRITE
		void SetNextAnimation(int iAnimNum);
		void EnableLoopedAnimation(void);
		void DisableLoopedAnimation(void);
		bool IsAnimationLooped(void);
		void PlayNextAnimations(void);
		void StopAnimation(void);
		void StartAnimation(void);
		int GetCurrentAnimation();
		int GetNextAnimation();
		float GetAnimationTimeInS(void);
		float GetAnimationTimeInFrames(void);
		float GetAnimationLengthInS(void);
		MadRender* BeginRendering(void);
		void EndRendering(void);
		void SetPlayBackSpeedScale(float fScale);
*/

};



#endif



