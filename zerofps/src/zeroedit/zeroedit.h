#ifndef _ZEROEDIT_H_
#define _ZEROEDIT_H_

#ifndef _DONT_MAIN
	#define _MAINAPPLICATION_
	#define _DONT_MAIN
#endif

#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "../zerofps/engine/application.h"
#include <cstdlib>

#include "../common/common.pkg"

/*
#include "ballobject.h"
#include "../game/teleportproperty.h"
#include "../game/statusproperty.h"
#include "../game/massdriverproperty.h"
#include "../game/massdriverprojectile.h"
#include "../game/adaptorsniper.h"
#include "../game/treeidleproperty.h"
*/

class Gui;
class WorkPanelDlg;
class GameScript;


enum EDIT_MODES{
	TEXTURE=1,
	LOWER=2,
	RAISE=3,
	FLATTEN=4,
	ADDOBJECT=5,
	SMOOTH=6,
};

enum CameraMode
{
	FreeFlight=0,
	Precision,
};

class LandType{
	public:
		int m_iTexture;
		Vector3	m_kColor;		
};

class ZeroEdit :public Application {
	private:
		bool RotateCamera(int relMouseX, int relMouseY);

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
			FID_SKYBOX6,
			FID_ADDLAND,
			FID_REMOVELAND,
			FID_LISTLAND,
			FID_SAVELAND,
			FID_LOADLAND,
			FID_VIEWMAD,
			FID_SUN,
			FID_MOON,
			FID_AMBIENT,			

			FID_FINDOBJECT,				// Finds / selects a object by its number.
			FID_NEXTOBJECT,
			FID_PREVOBJECT,
				
			FID_MASSSPAWN,
		
			FID_DUPLICATE,
			FID_COPY,
			FID_PASTE,
			FID_DELETE,

			FID_LINKOBJECT,
			FID_UNLINKOBJECT,

			FID_QUIT,
			FID_OPENDLG,
			FID_FILEOPENDLG,

			/* Test Functions that don't deserve a own name yet. This is NOT 
				a place for functions that will remain for a long time.  - VIM */
			FID_FUNC1,	
			FID_FUNC2,
			FID_FUNC3,
			FID_FUNC4,

		};	

		CameraMode m_eCameraMode;
	
		Object* m_pkCamObj;
		Camera* m_pkCamera;
		HeightMap* m_pkMap;
		
	
		int m_iMode;
	
		Vector3 m_kDrawPos;
		float m_fPointDistance;
		float m_fPointerHeight;

		Object* m_pkCurentParent;
		Object* m_pkCurentChild;
		
		string m_kMapBaseDir;

		string m_kCurentTemplate;
		
		int m_iPencilSize;		
		
		int m_iEditMask;
		bool m_iSmooth;
		
		int m_iMaskColorR;		
		int m_iMaskColorG;				
		int m_iMaskColorB;				
		int m_iMaskColorA;				
		
		
		float m_fTimer;
		float m_fDrawRate;
		bool m_bDrawing;
		int m_iRandom;		

		bool m_bMenuActive;
	
		int m_iCopyNetWorkID;

		void Input();				
		void SetPointer();	
		void DrawMarkers();
		Object* GetClosest(Vector3 kPos);
		void SelectChild();
		void SelectParent();		
		void ListTemplates();		
		void RegisterPropertys();
		
	
		void HeightMapDraw(Vector3 kPencilPos);

		void PasteObject(Vector3 kPos);	// Klistrar in markerat object.
		void DeleteSelected();

	public:

		ZeroEdit(char* aName,int iWidth,int iHeight,int iDepth);
		~ZeroEdit() {};
	 	void OnInit(void);
		void OnIdle(void);
		void OnHud(void);
		void OnServerStart(void);
		void OnClientStart(void);		
		void RunCommand(int cmdid, const CmdArgument* kCommand);
		Gui* m_pkGui;
		
	friend class Gui;
	friend class WorkPanelDlg;
};



#endif






