#ifndef _ZEROFPS_H_
#define _ZEROFPS_H_

#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include "../basic/basic.pkg"
#include "../render/render.pkg"
#include "engine.pkg"
#include "../ogl/zfpsgl.h"


using namespace std;

class Application;

enum enginestates 
{
	state_normal,
	state_exit,
	state_console,
	state_pause
};

enum CamMode
{
	cam_look,
	cam_target,
};

class ZeroFps {
	private:		
		SDL_Surface* m_pkScreen;		
		void HandleArgs(int iNrOfArgs, char** paArgs);

		float m_fLastFrameTime;

		int m_iWidth,m_iHeight,m_iDepth;
		

		Camera *m_pkTempCamera;
		Camera *m_pkCamera;
		Camera *m_pkConsoleCamera;
		Camera *m_pkGameCamera;		
		Camera *m_pkDefaultCamera;
	

	public:
		Application* m_pkApp;						//application
		CmdSystem* m_pkCmd;							//realtime variable handler
		AudioManager* m_pkAudioMan;			//audio manager (sfx,music)		
		TextureManager* m_pkTexMan;			//texture manager
		Input* m_pkInput;								//keyboard mouse input
		Render* m_pkRender;					//graphic primitives
		Console* m_pkConsole;						//console handler
		FileIo* m_pkFile;								//file io
		Light* m_pkLight;
		ObjectManager* m_pkObjectMan;
		CollisionManager*  m_pkCollisionMan;
	
		int m_iState;										//curent game state see enum enginestates
		int m_iFps;											//curent FPS
		float m_fFrameTime;							//frametime in MS
		
//		Vector3 *m_kCamPos;
//		Vector3 *m_kCamRot;
//		int m_iCamMode;
		
					
		ZeroFps(void);
		
		void SetApp(void);
		void Init(int iNrOfArgs, char** paArgs);	
		void MainLoop(void);		
		void InitDisplay(int iWidth,int iHeight,int iDepth);		
		void SetDisplay(int iWidth,int iHeight,int iDepth);
		void SetDisplay();
		void Swap(void);								//swap gl buffers
		void UpdateCamera(void);
		
		void ToggleFullScreen(void);
		
		inline unsigned int GetTicks(void) {return SDL_GetTicks();};
		inline float GetFrameTime() {return m_fFrameTime;};
		
		
//		inline Vector3& CamPos(Vector3 kCamPos){*m_kCamPos=kCamPos;return *m_kCamPos;};
//		inline Vector3& CamPos(void){return *m_kCamPos;};
//		inline void SetCamPosPointer(Vector3* pkNewCamPos) {m_kCamPos=pkNewCamPos;};
		
//		inline Vector3& CamRot(Vector3 kCamRot){*m_kCamPos=kCamRot;return *m_kCamRot;};
//		inline Vector3& CamRot(void){return *m_kCamRot;};		
//		inline void SetCamRotPointer(Vector3* pkNewCamRot) {m_kCamRot=pkNewCamRot;};
	
		void SetCamera(Camera* pkCamera);
	
//		inline void SetCam(Camera* pkCamera) {m_pkCamera=pkCamera;};
//		inline void SetGameCam(Camera* pkCamera) {m_pkGameCamera=pkCamera;};
		
		inline Camera *GetCam() {return m_pkCamera;};		
		inline Camera *GetGameCam() {return m_pkGameCamera;};				
		inline Camera *GetDefaultCam() {return m_pkDefaultCamera;};		
//		inline void SetConsoleCam(Camera* pkCamera) {m_pkConsoleCamera=pkCamera;};
};





#endif




