// i_zerofps.h: interface for the IZeroFps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IZEROFPS_H__8841D12A_728A_4690_895A_EE183F23E28D__INCLUDED_)
#define AFX_IZEROFPS_H__8841D12A_728A_4690_895A_EE183F23E28D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../basic/zfsystem.h"
#include "engine_x.h"

class Camera;
class DevStringPage;
class NetPacket;

/// Interface to ZeroFPS.
class ENGINE_API I_ZeroFps : public ZFSubSystem
{
public:
	I_ZeroFps(char* szName);
	virtual ~I_ZeroFps();

	virtual float GetTicks() = 0;
	virtual float GetFrameTime() = 0;
//	virtual float GetGameTime() = 0;
//	virtual float GetGameFrameTime() = 0;
	virtual float GetLastGameUpdateTime() = 0;
	virtual float GetEngineTime() = 0;

	virtual void SetCamera(Camera* pkCamera)= 0;	
	virtual void UpdateCamera()= 0;
	virtual Camera* GetCam()= 0;		
//	virtual	int	NumberOfArgs()= 0;			
//	virtual	string GetArg(int iArgIndex)= 0;
						
	virtual DevStringPage* DevPrint_FindPage(const char* szName)= 0;	
	virtual	void DrawDevStrings()= 0;
	virtual	void DevPrintf(const char* szName, const char *fmt, ...)= 0;
	virtual	void DevPrint_Show(bool bVisible)= 0;

	virtual	void HandleNetworkPacket(NetPacket* pkNetPacket)= 0;
		
	virtual	void RegisterPropertys()= 0;
	virtual	void QuitEngine()= 0;

	virtual	void GetEngineCredits(vector<string>& kCreditsStrings)= 0;
		 	
	virtual	void SetRenderTarget(Camera* pkCamera)= 0;
	virtual void RemoveRenderTarget(Camera* pkCamera)= 0;
		
	virtual int GetWidth()= 0;
	virtual int GetHeight()= 0;
	virtual int GetDepth() = 0;
};

#endif // !defined(AFX_IZEROFPS_H__8841D12A_728A_4690_895A_EE183F23E28D__INCLUDED_)
