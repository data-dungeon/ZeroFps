// i_zerofps.h: interface for the IZeroFps class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _I_ZEROFPS_H_
#define _I_ZEROFPS_H_

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
		virtual	~I_ZeroFps();
	
		virtual	float GetRealTime() const = 0;
		virtual	float GetFrameTime() const = 0;
		virtual	float GetLastGameUpdateTime() const= 0;
		virtual	float GetEngineTime() const = 0;
		virtual	Camera* GetCam() const= 0;								
		virtual	bool GetShadowMap() const = 0;						
		virtual	bool GetSpecMap() const = 0;							
		virtual	DevStringPage* DevPrint_FindPage(const string& strName)= 0;	
		virtual	void DrawDevStrings()= 0;
		virtual	void DevPrintf(const char* szName, const char *fmt, ...)= 0;
		virtual	void DevPrint_Show(bool bVisible)= 0;	
		virtual	void HandleNetworkPacket(NetPacket* pkNetPacket)= 0;			
		virtual	void RegisterPropertys()= 0;
		virtual	void QuitEngine()= 0;	
		virtual	void GetEngineCredits(vector<string>& kCreditsStrings)= 0;				
		virtual	void AddRenderCamera(Camera* pkCamera)= 0;
		virtual	void RemoveRenderCamera(Camera* pkCamera)= 0;
		virtual	void ClearRenderCameras()= 0;
};

#endif
