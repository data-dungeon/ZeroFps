#ifndef _P_DUNGEONGEN_H_
#define _P_DUNGEONGEN_H_

#include "../zerofpsv2/engine/property.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../zerofpsv2/script/zfscript.h"
#include "../zerofpsv2/render/zshadersystem.h"
#include "../zerofpsv2/engine_systems/tcs/tcs.h"
#include <iostream>
#include <string.h>
#include "mcommon_x.h"
#include <string>
#include <vector>

using namespace std;


class MCOMMON_API TileInfo
{
	public:
		bool	m_bVisited;
		int	m_iType;
		
		bool	m_bCon[4];
		
	TileInfo()
	{
		m_bVisited = false;
		m_iType = 0;
		
		m_bCon[0] = false;
		m_bCon[1] = false;
		m_bCon[2] = false;
		m_bCon[3] = false;
	
	}
};


class MCOMMON_API P_DungeonGen: public Property 
{
	private:
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( const string& kValueName ,const string& kValue );			
		
		ZSSRender*			m_pkRender;
		
		vector<TileInfo> 	m_kTiles;
		int					m_iWidth;
		int					m_iHeight;
		bool					m_bHaveBuild;
		
		bool VaildTile(int x,int y);
		void MakeCorridor(int x,int y,int iDir);
		bool AllVisited();
		void RotateZone(int iZoneID,const Vector3& kRot);
		int Type(int x,int y);
		void SetType(int x,int y,int iType);
		void MakeCorridor(int iStartX,int iStartY,int iStopX,int iStopY);
		
		void DrawDungeon();
		
	public:
		
		P_DungeonGen();
		~P_DungeonGen();

		void Update();
		void Init();		
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		
		
		void GenerateDungeon();
		
};


MCOMMON_API Property* Create_P_CharacterProperty();
MCOMMON_API void Register_P_DungeonGen(ZSSZeroFps* pkZeroFps);

#endif







