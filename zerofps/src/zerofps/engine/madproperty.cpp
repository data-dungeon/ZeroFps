#include "madproperty.h"
#include "object.h"

float fGameTime;

void SetGameTime(void)
{

	fGameTime = SDL_GetTicks() / 1000.0;
}


MadProperty::MadProperty(Core* pkModell) {
	strcpy(m_acName,"MadProperty");

	pkCore = pkModell;
	
	PlayAnimation(0, 0.0);
}


void MadProperty::Update() {
	if(!pkCore)
		return;

	UpdateAnimation();

	int iNumOfFrame = pkCore->GetAnimationTimeInFrames(iActiveAnimation);
	int iFrame = fCurrentTime / 0.1;
	pkCore->SetFrameI(pkCore->akAnimation[iActiveAnimation].KeyFrame[0].iVertexFrame + iFrame);
	
	glPushMatrix();
		glTranslatef(m_pkObject->GetPos().x,m_pkObject->GetPos().y,m_pkObject->GetPos().z);
		//glScalef(.02,.02,.02);
		pkCore->draw();
	glPopMatrix();

}

void MadProperty::SetBase(Core* pkModell)
{
	
}

void MadProperty::PlayAnimation(int iAnimNum, float fStartTime)
{
	SetGameTime();

	iActiveAnimation = iAnimNum;
	fCurrentTime = fStartTime;
	fLastUpdate = fGameTime; 
}

void MadProperty::UpdateAnimation(void)
{
	SetGameTime();

	float fDelta = fGameTime - fLastUpdate;

	fCurrentTime += fDelta;

	float fAnimLength = pkCore->GetAnimationLengthInS(iActiveAnimation);

	if(fCurrentTime >= fAnimLength)

		fCurrentTime -= fAnimLength;

	fLastUpdate = fGameTime; 
}

/*
void MadInstans::PlayAnimation(int iAnimNum, float fStartTime)

{

	SetGameTime();



	iActiveAnimation = iAnimNum;

	fCurrentTime = fStartTime;

	fLastUpdate = fGameTime; 
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

}*/
