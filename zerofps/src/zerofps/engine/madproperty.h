#ifndef _MADPROPERTY_H_
#define _MADPROPERTY_H_

#include "../render/render.pkg"
#include "property.h"

using namespace std;

#define NO_ANIMATION_ID	-1

class ENGINE_API MadProperty: public Property {
	private:
		Core*	pkCore;
		Frustum* m_pkFrustum;
		
		int		iActiveAnimation;
		float	fCurrentTime;
		float	fLastUpdate;

		float	m_fScale;

		int		m_iNextAnimation;
		bool	m_bLoop;

		bool	m_bActive;			// True if animation system is active.

	public:
		MadProperty();
		MadProperty(Core* pkModell);

		void Update();
		void SetBase(Core* pkModell);
		void PlayAnimation(int iAnimNum, float fStartTime);
		void UpdateAnimation(void);
		void Draw(void);


		void	SetNextAnimation(int iAnimNum);
		void	PlayNextAnimations(void);
		int		GetNextAnimation();
		
		int		GetCurrentAnimation();
		void	SetLoopedStatus(bool bLoop);
		bool	IsLooped();

		void	StopAnimation(void);
		void	StartAnimation(void);
	
		void	NextCoreAnimation(void);
		
		void	SetScale(float fScale);

		bool	bFlipFace;

		void	SetReplaceTexture(char* szName);
/*
		// NEED TO WRITE


		float GetAnimationTimeInS(void);
		float GetAnimationTimeInFrames(void);
		float GetAnimationLengthInS(void);
		MadRender* BeginRendering(void);
		void EndRendering(void);
		void SetPlayBackSpeedScale(float fScale);
*/

};

Property* Create_MadProperty();

#endif



