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

#include "ballobject.h"
#include "../game/teleportproperty.h"
#include "../game/statusproperty.h"
#include "../game/massdriverproperty.h"
#include "../game/massdriverprojectile.h"
#include "../game/adaptorsniper.h"
class Gui;


enum EDIT_MODES{
	TEXTURE=1,
	LOWER=2,
	RAISE=3,
	FLATTEN=4,
	ADDOBJECT=5
};

class LandType{
	public:
		int m_iTexture;
		Vector3	m_kColor;		
};

class ZeroEdit :public Application {
	private:

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
			FID_ADDLAND,
			FID_REMOVELAND,
			FID_LISTLAND,
			FID_SAVELAND,
			FID_LOADLAND,
			FID_VIEWMAD,
			FID_SUN,
			FID_MOON,

			FID_FINDOBJECT,				// Finds / selects a object by its number.
			FID_NEXTOBJECT,
			FID_PREVOBJECT,
				
		};	
	
		Object* m_pkCamObj;
		Camera* m_pkCamera;
		HeightMap* m_pkMap;
		
		list<LandType> m_kLandTypes;
		
		int m_iMode;
	
		Vector3 m_kDrawPos;
		float m_fPointDistance;
		float m_fPointerHeight;

		Object* m_pkCurentParent;
		Object* m_pkCurentChild;
		
		string m_kMapBaseDir;

		string m_kCurentTemplate;
		
		int m_iPencilSize;
		
		float m_fTimer;
		float m_fDrawRate;
		int m_iLandType;
		bool m_bDrawing;
		bool m_iRandom;

		bool m_bMenuActive;
	
		void Input();				
		void SetPointer();	
		void DrawMarkers();
		Object* GetClosest(Vector3 kPos);
		void SelectChild();
		void SelectParent();		
		void ListTemplates();		
		void RegisterPropertys();
		
		void AddLandtype(int iTexture,Vector3 kColor);
		bool RemoveLandtype(int iTexture,Vector3 kColor);		
		bool RemoveLandtype(int iNr);
		LandType GetLandType(int iNr);
		void ListLandTypes();
		bool LoadLandFromFile(const char* acFile);
		bool SaveLandToFile(const char* acFile);		
		
		void HeightMapDraw(Vector3 kPencilPos);

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
};



#endif






