#include "mad.h"
#include "../oglload/zfpsgl.h"

float fGameTime;

void SetGameTime(void)
{
	fGameTime = SDL_GetTicks() / 1000.0;
}

void MadInstans::SetBase(Core* pkModell)
{
	pkCore = pkModell;
}

void MadInstans::PlayAnimation(int iAnimNum, float fStartTime)
{
	SetGameTime();

	iActiveAnimation = iAnimNum;
	fCurrentTime = fStartTime;
	fLastUpdate = fGameTime; 
	cout << "iActiveAnimation " << iActiveAnimation << endl;

}

void MadInstans::UpdateAnimation(void)
{
	SetGameTime();

	float fDelta = fGameTime - fLastUpdate;
	fCurrentTime += fDelta;

	float fAnimLength = pkCore->GetAnimationLengthInS(iActiveAnimation);
	if(fCurrentTime >= fAnimLength)
		fCurrentTime -= fAnimLength;
	fLastUpdate = fGameTime; 
}

void MadInstans::Draw(void)
{
	int iNumOfFrame = pkCore->GetAnimationTimeInFrames(iActiveAnimation);
	int iFrame = fCurrentTime / 0.1;

//	cout << " iFrame" << iFrame;
//	cout << " fCurrentTime" << fCurrentTime;

	pkCore->SetFrameI(iFrame);
	pkCore->draw();
}

/*	iActiveKeyFrame++;
	if(iActiveKeyFrame >= akAnimation[iAnim].KeyFrame.size())
		iActiveKeyFrame = 0;

//	cout << akAnimation[iAnim].KeyFrame[iActiveKeyFrame].iVertexFrame;
	iActiveFrame = akAnimation[iAnim].KeyFrame[iActiveKeyFrame].iVertexFrame;

	cout << iActiveKeyFrame << "/" << iActiveFrame << endl;
*/

/*class MadInstans
{
	Core*
	public:
	void SetBase(Core*);	// Väljer vilken bas modell denna instans är.

	void PlayAnimation(int iAnimNum, float fStartTime);
	
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
};*/


