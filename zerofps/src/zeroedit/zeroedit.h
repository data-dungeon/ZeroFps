#ifndef _ZEROEDIT_H_
#define _ZEROEDIT_H_

#define _MAINAPPLICATION_

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"
#include <cstdlib>

#include "ballobject.h"
//#include "worldinfoproperty.h"

//#include "normaldrawproperty.h"
//#include "playercontrolproperty.h"
//#include "ballobject.h"
//#include "playerballobject.h"
//#include "playerobject.h"
//#include "../test/bunnyobject.h"

enum EDIT_MODES{
	TEXTURE=1,
	LOWER=2,
	RAISE=3,
	FLATTEN=4,
	ADDOBJECT=5
};

class ZeroEdit :public Application {
	private:
		void ToogleMenu();
		bool InitGUI();
		enum FuncId_e
		{
			FID_LOADMAP,
			FID_LOADIMAGEMAP,			
			FID_SAVEMAP,
			FID_NEWMAP,
			FID_OBJECTTREE,
			FID_SAVETEMPLATE,
			FID_LOADTEMPLATE,
			FID_LISTTEMPLATES,
			FID_MAKETEMPLATE,
			FID_SAVEOBJECTS,
			FID_LOADOBJECTS,
			FID_SAVE,
			FID_LOAD,
			FID_FOG,
			FID_WATER,
			FID_SKYBOX,
		};	
	
		Camera* m_pkCamera;
		HeightMap* m_pkMap;


		int m_iMode;
		
		int m_iTexture;
		Vector3 m_kDrawPos;
		float m_fPointDistance;
		float m_fPointerHeight;

		Object* m_pkCurentParent;
		Object* m_pkCurentChild;
		
		string m_kCurentTemplate;
		float m_fTimer;
		float m_fDrawRate;
		bool m_bDrawing;
		bool m_iRandom;
		bool m_bMenuActive;
	
		void Input();				
		void CreateNew(int iSize);
		void SetPointer();	
		void DrawMarkers();
		void Clear();
		Object* GetClosest(Vector3 kPos);
		void SelectChild();
		void SelectParent();		
		void ListTemplates();		
		void RegisterPropertys();
		
	public:
		bool ZGWinProc( ZGuiWnd* pkWindow, unsigned int uiMessage, int iNumberOfParams, void *pkParams );
		ZeroEdit(char* aName,int iWidth,int iHeight,int iDepth);
		~ZeroEdit() {};
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		void OnServerStart(void);
		void OnClientStart(void);		
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		
};



#endif






