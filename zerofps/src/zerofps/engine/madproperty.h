#ifndef _MADPROPERTY_H_
#define _MADPROPERTY_H_

#include "mad_modell.h"
#include "zerofps.h"
#include "property.h"
#include <string>
using namespace std;

#define NO_ANIMATION_ID	-1


class ENGINE_API MadProperty : public Property, public Mad_Modell {
	public:
		MadProperty();
		MadProperty(Mad_Core* pkModell);
 
		Frustum* m_pkFrustum;
		ZeroFps* m_pkZeroFps;
		string m_kMadFile;


//		void SetBase(Mad_Core* pkModell);
		void SetBase(const char* acName);
		void Update();
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);
	
	//	private:
/*	
		

		void SetBase(Core* pkModell);
		void SetBase(const char* acName);
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

		
		void Save(ZFMemPackage* pkPackage);
		void Load(ZFMemPackage* pkPackage);*/

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



