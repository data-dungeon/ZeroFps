#include "p_dungeongen.h" 
#include "../zerofpsv2/engine_systems/propertys/p_pfmesh.h"
#include "../zerofpsv2/basic/math.h"

P_DungeonGen::P_DungeonGen()
{
	strcpy(m_acName,"P_DungeonGen");		
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_CLIENT|PROPERTY_SIDE_SERVER;
	

	m_pkRender = static_cast<ZSSRender*>(g_ZFObjSys.GetObjectPtr("ZSSRender"));			

	m_bHaveBuild = false;
}

P_DungeonGen::~P_DungeonGen()
{
	
}

void P_DungeonGen::Init()
{
	

}

void P_DungeonGen::Update()
{
	if(!m_bHaveBuild)
	{
		m_bHaveBuild = true;
		GenerateDungeon();
	}
	
// 	if(m_bHaveBuild && m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER))
// 	{
// 		DrawDungeon();
// 	
// 	}
}

void P_DungeonGen::DrawDungeon()
{
  	ZoneData* pkZone = m_pkEntityManager->GetZoneData(m_pkEntity->GetCurrentZone());
	if(!pkZone)
 	{
 		cout<<"error dungeon gen , not in zone"<<endl;
 		return;
 	}

	if(!pkZone->m_pkZone)
		return;

	Vector3 kPos = pkZone->m_pkZone->GetWorldPosV();

	for(int y = 0;y<m_iHeight;y++)
	{
		for(int x = 0;x<m_iWidth;x++)
		{
			
			
			if(m_kTiles[y*m_iWidth+x].m_bCon[0])
				m_pkRender->Line(kPos+Vector3(x*8,0,y*8),kPos+Vector3(x*8,0,(y-1)*8));
			if(m_kTiles[y*m_iWidth+x].m_bCon[1])
				m_pkRender->Line(kPos+Vector3(x*8,0,y*8),kPos+Vector3((x+1)*8,0,y*8));
			if(m_kTiles[y*m_iWidth+x].m_bCon[2])
				m_pkRender->Line(kPos+Vector3(x*8,0,y*8),kPos+Vector3(x*8,0,(y+1)*8));
			if(m_kTiles[y*m_iWidth+x].m_bCon[3])
				m_pkRender->Line(kPos+Vector3(x*8,0,y*8),kPos+Vector3((x-1)*8,0,y*8));
				
		}
	}
}


void P_DungeonGen::GenerateDungeon()
{
	m_iWidth = 32;
	m_iHeight = 32;
	int iTiles = m_iWidth * m_iHeight;
	

	m_kTiles.clear();

	//create grid and reset visible status;
	TileInfo temp;
	for(int i =0;i<iTiles;i++)
	{
		m_kTiles.push_back(temp);
	}

	cout<<"creating rooms"<<endl;
	//create X nr of rooms of random size	
	vector<pair<int,int> > kRooms;
	
	for(int i = 0;i<iTiles/50;i++)
	{
	 	int iX = Math::Randomi(m_iWidth);
 		int iY = Math::Randomi(m_iHeight);
		int iSize = Math::Randomi(4)+2;
		
		if(i == 0)
		{
			iX = m_iWidth / 2.0;
			iY = 0;
			iSize = 4;
		}		
		
		bool bOK = true;
		for(int y = iY-iSize/2;y< iY+iSize /2;y++)
			for(int x = iX-iSize/2;x< iX+iSize /2;x++)
				if(Type(x,y) > 0 )
					bOK = false;
			
		if(bOK)
		{
			kRooms.push_back(pair<int,int>(iX,iY));
			
			for(int y = iY-iSize/2;y< iY+iSize /2;y++)
			{
				for(int x = iX-iSize/2;x< iX+iSize /2;x++)
				{				
					int iType = 1;
					if(x == iX-iSize/2) iType = 6;
					if(x == (iX+iSize/2)-1) iType = 7;
					if(y == iY-iSize/2) iType = 8;
					if(y == (iY+iSize/2)-1) iType = 9;
					
					
					if(x == iX-iSize/2 && y == iY-iSize/2) iType = 5;
					if(x == (iX+iSize/2)-1 && y == iY-iSize/2) iType = 2;
					if(x == (iX+iSize/2)-1 && y == (iY+iSize/2)-1) iType = 3;
					if(x == iX-iSize/2 && y == (iY+iSize/2)-1) iType = 4;
				
					SetType(x,y,iType);
				}
			}
		}
	}
	
	cout<<"creating corridors, rooms:"<<kRooms.size()<<endl;
	//create connections between rooms
	if(kRooms.size()> 1)
	{
		for(int i = 0;i<kRooms.size();i++)
		{
			int iOtherRoom = i;
	 		
	 		while(iOtherRoom == i)
				iOtherRoom = Math::Randomi(kRooms.size());
						
			MakeCorridor(	kRooms[i].first,kRooms[i].second,
								kRooms[iOtherRoom].first,kRooms[iOtherRoom].second);
		}
	}
	
	cout<<"------------  LEVEL ----------"<<endl;
	for(int y = 0;y<m_iHeight;y++)
	{
		for(int x = 0;x<m_iWidth;x++)
		{	
			switch(m_kTiles[y*m_iWidth+x].m_iType)
			{
				case 0: cout<<".";break;
				case 1: cout<<"#";break;
				case 2: cout<<"o";break;
				case 3: cout<<"O";break;
				case 4: cout<<"*";break;
				
				case 5: cout<<"L";break;
				case 6: cout<<"/";break;			
				case 7: cout<<"\\";break;			
				case 8: cout<<"/";break;			
				case 9: cout<<"^";break;			
				case 10: cout<<">";break;			
				case 11: cout<<"=";break;			
				case 12: cout<<"<";break;			
				case 13: cout<<"+";break;			
				case 14: cout<<"?";break;			
			}
		
// 			cout<<" ";
		}

		cout<<endl;
	}



  	ZoneData* pkZone = m_pkEntityManager->GetZoneData(m_pkEntity->GetCurrentZone());
	if(!pkZone)
 	{
 		cout<<"error dungeon gen , not in zone"<<endl;
 		return;
 	}
 
 	vector<int> kNewZones;
 
 	Vector3 pos = pkZone->m_kPos + Vector3(-m_iWidth*8 / 2,0,8);
	Vector3 kTempPos;

	for(int y = 0;y<m_iHeight;y++)
	{
		for(int x = 0;x<m_iWidth;x++)
		{	
			if(Type(x,y) == 0)
				continue;
		
			kTempPos.x = pos.x + x * 8;
			kTempPos.z = pos.z + y * 8;
			kTempPos.y = pos.y;
									
	 		int iNew = m_pkEntityManager->CreateZone(kTempPos,Vector3(8,8,8));
			if(iNew == -1)
				continue;
			
 			m_pkEntityManager->LoadZone(iNew);				

					//room types
					// 1 floor
					// 2 "|
					// 3 _|
					// 4 |_
					// 5 |"
					// 6 |>
					// 7 <|
					// 8 "
					// 9 _
					// corridors
					// 10 -
					// 11 |
					// 12 _|
					// 13 |_
					// 14 |"
					// 15 "|
					// 16 +

			switch(m_kTiles[y*m_iWidth+x].m_iType)
			{
				case 1:	m_pkEntityManager->SetZoneModel("zones/cave/room/cave_floor-8x8x8.mad",iNew);break;
				
				// "|  _|  |_  |"				
				case 5:	RotateZone(iNew,Vector3(0,90,0));
				case 4:	RotateZone(iNew,Vector3(0,90,0));
				case 3:	RotateZone(iNew,Vector3(0,90,0));				
				case 2:	m_pkEntityManager->SetZoneModel("zones/cave/room/cave_wall_edge_floor-8x8x8.mad",iNew);
							break;
				
				
				//  "
				case 9:	RotateZone(iNew,Vector3(0,90,0));
							RotateZone(iNew,Vector3(0,90,0));
				//  _
				case 8:	m_pkEntityManager->SetZoneModel("zones/cave/room/cave_floor_wall-8x8x8.mad",iNew);
							break;
				
				
				//  |>
				case 6:	RotateZone(iNew,Vector3(0,90,0));
							RotateZone(iNew,Vector3(0,90,0));							
				//  <|
				case 7:	RotateZone(iNew,Vector3(0,90,0));
							m_pkEntityManager->SetZoneModel("zones/cave/room/cave_floor_wall-8x8x8.mad",iNew);
							break;

				
			
				case 10: RotateZone(iNew,Vector3(0,90,0));
				case 11: m_pkEntityManager->SetZoneModel("zones/cave/tunnel/cave_tunnel-8x8x8.mad",iNew);
							break;
			
			
				case 15: RotateZone(iNew,Vector3(0,90,0));
				case 14: RotateZone(iNew,Vector3(0,90,0));
				case 13: RotateZone(iNew,Vector3(0,90,0));
				case 12: m_pkEntityManager->SetZoneModel("zones/cave/tunnel/cave_turn-8x8x8.mad",iNew);
							break;
			
				case 16: m_pkEntityManager->SetZoneModel("zones/cave/tunnel/cave_x-8x8x8.mad",iNew);
							break;
			
/*			
				case 11: RotateZone(iNew,Vector3(0,90,0));
				case 10: RotateZone(iNew,Vector3(0,90,0));
				case 9: RotateZone(iNew,Vector3(0,90,0));
				case 12: m_pkEntityManager->SetZoneModel("zones/cave/tunnel/cave_t-8x8x8.mad",iNew);break;	*/
			}
		}
	}

}

void P_DungeonGen::MakeCorridor(int iStartX,int iStartY,int iStopX,int iStopY)
{
	


	float fXdiff = iStopX - iStartX;
	float fYdiff = iStopY - iStartY;
	
	if(fabs(fYdiff) <= fabs(fXdiff))
	{
		float p = 1;	
		if(fXdiff != 0)
			p = fYdiff / fXdiff;
		
		int iLast= iStartY;
		for(int x = 0;x<(iStopX-iStartX);x++)
		{			
			int sx = iStartX + x;
			int sy = iStartY + float(x)*p;
			
			SetType(sx,sy,10);
			
			if(abs(sy-iLast) > 0)
			{
				if(p > 0)
				{
					SetType(sx,sy-1,13);	//			|_
  					SetType(sx,sy,15);
				}
				
				if(p < 0)
				{
 					SetType(sx,sy+1,14);	//			|"
  					SetType(sx,sy,12);
				
				}
			}
			
			iLast = sy;						
		}
	}
	else
	{
		float p = 1;	
		if(fYdiff != 0)
			p = fXdiff / fYdiff;
		
		int iLast = iStartX;
		for(int y = 0;y<(iStopY-iStartY);y++)
		{
			int sx = iStartX + float(y)*p;
			int sy = iStartY + y;
		
			SetType(sx,sy,11);	
		
			if(abs(sx-iLast) > 0)
			{
				if(p > 0)
				{
					SetType(sx-1,sy,15);	//  "|
					SetType(sx,sy,13);
				}
				
				if(p < 0)
				{			
					SetType(sx+1,sy,14); //  |"
					SetType(sx,sy,12);
				}
			}			
		
			iLast = sx;
		
		}	
	}


// 	SetType(iStartX,iStartY,3);
// 	SetType(iStopX,iStopY,4);
}

void P_DungeonGen::SetType(int x,int y,int iType)
{
	if(x < 0 || x >= m_iWidth || y < 0 || y >= m_iHeight)
		return;
		
	m_kTiles[y*m_iWidth+x].m_iType = iType;
}

int P_DungeonGen::Type(int x,int y)
{
	if(x < 0 || x >= m_iWidth || y < 0 || y >= m_iHeight)
		return -1;
		
	return m_kTiles[y*m_iWidth+x].m_iType;

}

void P_DungeonGen::RotateZone(int iZoneID,const Vector3& kRot)
{
	if(ZoneData* pkData = m_pkEntityManager->GetZoneData(iZoneID)) 
	{
		if(pkData->m_pkZone)
		{
			pkData->m_pkZone->RotateLocalRotV( kRot ); 
	
			// Update PFind Mesh				
			if(P_PfMesh* pkMesh = (P_PfMesh*)pkData->m_pkZone->GetProperty("P_PfMesh"))
				pkMesh->CalcNaviMesh();
		}		
	}		
}

bool P_DungeonGen::AllVisited()
{
	for(int i = 0;i<m_kTiles.size();i++)
		if(!m_kTiles[i].m_bVisited)
			return false;
			
	return true;
}

void P_DungeonGen::MakeCorridor(int iCurrentX,int iCurrentY,int iDir)
{
// 	types
// 	|   1
//    -   2
//
//    L   5   |_
//    /	 6   |"
//    \   7  "|
//    /   8	 _|
//
//    ^   9
//    >   10
//    =   11
//    <   12
//    +   13
//    ?   14


	int x,y;
	if(iDir == 0){	x = iCurrentX;		y = iCurrentY-1;	};
	if(iDir == 1){	x = iCurrentX+1;	y = iCurrentY;		};
	if(iDir == 2){	x = iCurrentX;		y = iCurrentY+1;	};
	if(iDir == 3){	x = iCurrentX-1;	y = iCurrentY;		};	

		
	TileInfo* pkTile = &m_kTiles[iCurrentY*m_iWidth+iCurrentX];
	int iType = m_kTiles[iCurrentY*m_iWidth+iCurrentX].m_iType;
	int iNType;
	int iCType = iType;

	if(iDir == 0 || iDir == 2)
		iNType = 1;
	else
		iNType = 2;


	//no connections
	if(!pkTile->m_bCon[0] && !pkTile->m_bCon[1] && !pkTile->m_bCon[2] && !pkTile->m_bCon[3])
	{
		if(iDir == 0 || iDir == 2)
			iCType = 1;
		else
			iCType = 2;
	}
	
	//all connections
	if(pkTile->m_bCon[0] && pkTile->m_bCon[1] && pkTile->m_bCon[2] && pkTile->m_bCon[3])
	{
		iCType = 13;
	}	
	
	//up
	if(pkTile->m_bCon[0] && !pkTile->m_bCon[1] && !pkTile->m_bCon[2] && !pkTile->m_bCon[3])
	{
		if(iDir == 1) iCType = 5;
		if(iDir == 2) iCType = 1;
		if(iDir == 3) iCType = 8;
	}
	
	//right
	if(!pkTile->m_bCon[0] && pkTile->m_bCon[1] && !pkTile->m_bCon[2] && !pkTile->m_bCon[3])
	{
		if(iDir == 0) iCType = 5;
		if(iDir == 2) iCType = 6;
		if(iDir == 3) iCType = 2;
	}
	
	//down
	if(!pkTile->m_bCon[0] && !pkTile->m_bCon[1] && pkTile->m_bCon[2] && !pkTile->m_bCon[3])
	{
		if(iDir == 0) iCType = 1;
		if(iDir == 1) iCType = 6;
		if(iDir == 3) iCType = 7;
	}
		
	//left
	if(!pkTile->m_bCon[0] && !pkTile->m_bCon[1] && !pkTile->m_bCon[2] && pkTile->m_bCon[3])
	{
		if(iDir == 0) iCType = 8;
		if(iDir == 1) iCType = 2;
		if(iDir == 2) iCType = 7;
	}


// 	switch(iType)
// 	{
// 		case 0:
// 			if(iDir == 0){	iCType = 1;iNType = 1;};
// 			if(iDir == 1){	iCType = 2;iNType = 2;};
// 			if(iDir == 2){	iCType = 1;iNType = 1;};
// 			if(iDir == 3){ iCType = 2;iNType = 2;};
// 			break;	
// 	
// 		case 1:
// 			if(iDir == 0) iNType = 1;
// 			if(iDir == 1){	iCType = 10;iNType = 2;};
// 			if(iDir == 2) iNType = 1;
// 			if(iDir == 3){ iCType = 12;iNType = 2;};
// 			break;
// 				
// 		case 2:
// 			if(iDir == 0){	iCType = 9;iNType = 1;};
// 			if(iDir == 1) iNType = 2;
// 			if(iDir == 2){	iCType = 11;iNType = 1;};
// 			if(iDir == 3) iNType = 2;
// 			break;				
// 	}



	m_kTiles[iCurrentY*m_iWidth+iCurrentX].m_iType = iCType;
	m_kTiles[y*m_iWidth+x].m_iType = iNType;
	
	m_kTiles[iCurrentY*m_iWidth+iCurrentX].m_bCon[iDir] = true;
	
	if(iDir==0) iDir = 2;
	if(iDir==1) iDir = 3;
	if(iDir==2) iDir = 0;
	if(iDir==3) iDir = 1;
	
	m_kTiles[y*m_iWidth+x].m_bCon[iDir] = true;
	
}

bool P_DungeonGen::VaildTile(int x,int y)
{
	if(x >= m_iWidth || x < 0 || y >= m_iHeight || y < 0)
		return false;
		
	if(m_kTiles[y*m_iWidth+x].m_bVisited == true)
		return false;	

	return true;
}


void P_DungeonGen::Save(ZFIoInterface* pkPackage)
{	
	pkPackage->Write(m_bHaveBuild);
}

void P_DungeonGen::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read(m_bHaveBuild);
}


vector<PropertyValues> P_DungeonGen::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);
		
// 	kReturn[0].kValueName = "scale";
// 	kReturn[0].iValueType = VALUETYPE_FLOAT;
// 	kReturn[0].pkValue    = (void*)&m_fScale;
// 
// 	kReturn[1].kValueName = "offset";
// 	kReturn[1].iValueType = VALUETYPE_VECTOR3;
// 	kReturn[1].pkValue    = (void*)&m_kOffset;
	
	return kReturn;
}

bool P_DungeonGen::HandleSetValue( const string& kValueName ,const string& kValue )
{
// 	if(strcmp(kValueName.c_str(), "scale") == 0) 
// 	{
// 		m_fScale = float(atof(kValue.c_str()));	
// 		m_bHaveSet = true;
// 		return true;
// 	}
	
	return false;
}

Property* Create_P_DungeonGen()
{
	return new P_DungeonGen;
}

void Register_P_DungeonGen(ZSSZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_DungeonGen", Create_P_DungeonGen);					

	// Register Property Script Interface
	
	//item handling
// 	g_pkScript->ExposeFunction("PickupItem",		SI_P_CharacterProperty::PickupItemLua);

}









// 	int iCurrentX = Randomi(m_iWidth);
// 	int iCurrentY = Randomi(m_iHeight);
// 	
// 	//pick random grid
// 	m_kTiles[iCurrentY*m_iWidth+iCurrentX].m_bVisited = true;
// 	m_kTiles[iCurrentY*m_iWidth+iCurrentX].m_iType = 0;
// 	
// 	while(1)
// 	{
// 		if(AllVisited())
// 			break;
// 	
// 		//any vaild tiles? if not pick a new random position
// 		if(!VaildTile(iCurrentX,iCurrentY-1) &&
// 			!VaildTile(iCurrentX,iCurrentY+1) &&
// 			!VaildTile(iCurrentX-1,iCurrentY) &&
// 			!VaildTile(iCurrentX+1,iCurrentY))
// 		{
// 			while(m_kTiles[iCurrentY*m_iWidth+iCurrentX].m_bVisited)
// 			{
// 				iCurrentX = Randomi(m_iWidth);
// 				iCurrentY = Randomi(m_iHeight);									
// 			}			
// 		
// 			m_kTiles[iCurrentY*m_iWidth+iCurrentX].m_bVisited = true;
// 			m_kTiles[iCurrentY*m_iWidth+iCurrentX].m_iType = 0;		
// 		}
// 	
// 		//pick random dir
// 		int iRandDir = Randomi(4);
// 		int x,y;		
// 	
// 		if(iRandDir == 0){	x = iCurrentX;		y = iCurrentY-1;	};
// 		if(iRandDir == 1){	x = iCurrentX+1;	y = iCurrentY;		};
// 		if(iRandDir == 2){	x = iCurrentX;		y = iCurrentY+1;	};
// 		if(iRandDir == 3){	x = iCurrentX-1;	y = iCurrentY;		};
// 	
// 		if(!VaildTile(x,y))
// 			continue;				
// 		
// 		m_kTiles[y*m_iWidth+x].m_bVisited = true;
// 			
// 		MakeCorridor(iCurrentX,iCurrentY,iRandDir);
// 
// 		iCurrentX = x;
// 		iCurrentY = y;			
// 	}	

// 	//make turns
// 	for(int y = 0;y< m_iHeight;y++)
// 	{
// 		for(int x = 0;x< m_iWidth;x++)
// 		{
// 			if(Type(x,y) == 1 &&
// 				Type(x,y-1) == 1 &&
// 				Type(x,y+1) == 1)
// 				continue;
// 		
// 			if(Type(x,y) == 2 &&
// 				Type(x,y-1) == 2 &&
// 				Type(x,y+1) == 2)
// 				continue;		
// 		
// 		}
// 	}
