#include "zeroed.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/p_hmrp2.h"
#include "../zerofpsv2/engine_systems/propertys/p_ambientsound.h"
#include "../zerofpsv2/engine_systems/propertys/p_heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/p_tcs.h"
#include "../zerofpsv2/engine/inputhandle.h"
#include "../zerofpsv2/engine/zerofps.h"

vector<HMSelectVertex> ZeroEd::GetAllSelectedHMVertex()
{
	P_HMRP2* hmrp;
	vector<HMSelectVertex> kReturnVertex;

	for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
	{
		Entity* pkEntity = m_pkEntityManager->GetEntityByID((*itEntity));
		if(!pkEntity)			continue;
		hmrp = static_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
		if(hmrp == NULL)		continue;

		Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;

		vector<HMSelectVertex> kSelVertex = hmrp->m_pkHeightMap->GetSelection(m_kDrawPos,m_fHMInRadius,m_fHMOutRadius);
		kReturnVertex.insert(kReturnVertex.begin(), kSelVertex.begin(), kSelVertex.end());
	}

	return kReturnVertex;
}


// Handles input for EditMode Terrain.
void ZeroEd::Input_EditTerrain()
{
	// Mofify radius of hmap marker.	
	if(m_pkInputHandle->VKIsDown("inrad+"))		m_fHMInRadius += 1 * m_pkZeroFps->GetFrameTime();
	if(m_pkInputHandle->VKIsDown("inrad-"))		m_fHMInRadius -= 1 * m_pkZeroFps->GetFrameTime();
	if(m_pkInputHandle->VKIsDown("outrad+"))		m_fHMOutRadius += 1 * m_pkZeroFps->GetFrameTime();
	if(m_pkInputHandle->VKIsDown("outrad-"))		m_fHMOutRadius -= 1 * m_pkZeroFps->GetFrameTime();
	
	// Validate size of hmap marker
	if(m_fHMInRadius > m_fHMOutRadius - 0.1)		m_fHMInRadius = m_fHMOutRadius - 0.1;
	if(m_fHMOutRadius < m_fHMInRadius + 0.1)		m_fHMOutRadius = m_fHMInRadius + 0.1;
	if(m_fHMInRadius < 0.5)								m_fHMInRadius = 0.5;
	if(m_fHMOutRadius < 1.0)							m_fHMOutRadius = 1.0;

	switch(m_iHMapEditMode)
	{
		
		case HMAP_DRAWMASK: 
			if(m_pkInputHandle->VKIsDown("hmraise"))		
				HMDrawTexture(m_cDrawTexture); 	
	
			break;
			
		case HMAP_EDITVERTEX: 
			if(m_pkInputHandle->VKIsDown("hmraise"))		
				HMModifyCommand(5); 
			if(m_pkInputHandle->VKIsDown("hmlower"))		
				HMModifyCommand(-5);			
			break;
		
		case HMAP_DRAWSMFLAT:
			if(m_pkInputHandle->VKIsDown("hmraise"))		
				HMSmooth();  
			break;

		case HMAP_STITCH:
			if(m_pkInputHandle->VKIsDown("hmraise"))		
				Stitch();  
			break;


// 		case HMAP_DRAWMASK:
// 			if(m_pkInputHandle->VKIsDown("hmraise"))	
// 			{
// 				for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
// 				{
// 					Entity* pkEntity = m_pkEntityManager->GetEntityByID((*itEntity));
// 					if(!pkEntity)			continue;
// 					P_HMRP2* hmrp = static_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
// 					if(hmrp == NULL)		continue;
// 					Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;
// 					hmrp->m_pkHeightMap->DrawMask(m_kDrawPos, m_iEditLayer,m_fHMInRadius,255,255,255,1);
// 				}
// 			}
// 			break;

		case HMAP_DRAWVISIBLE:
			for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
			{
				Entity* pkEntity = m_pkEntityManager->GetEntityByID((*itEntity));
				if(!pkEntity)			continue;
				P_HMRP2* hmrp = static_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
				if(hmrp == NULL)		continue;
				Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;

				if(m_pkInputHandle->VKIsDown("hmraise"))		
					hmrp->m_pkHeightMap->DrawVisible(kLocalOffset, true);
				if(m_pkInputHandle->VKIsDown("hmlower"))		
					hmrp->m_pkHeightMap->DrawVisible(kLocalOffset, false);
			}
			
			break;
	}

	if(m_pkInputHandle->Pressed(KEY_1)) m_iHMapEditMode = HMAP_DRAWMASK;			
	
	if(m_pkInputHandle->Pressed(KEY_2) && !DelayCommand() && m_cDrawTexture < 250)
	{
		m_cDrawTexture++;			
		cout<<"texure:"<<(int)m_cDrawTexture<<endl;
	}
	
	if(m_pkInputHandle->Pressed(KEY_3) && !DelayCommand() && m_cDrawTexture > -1)
	{
		m_cDrawTexture--;			
		cout<<"texure:"<<(int)m_cDrawTexture<<endl;
	}

	if(m_pkInputHandle->Pressed(KEY_4)) m_iHMapEditMode = HMAP_EDITVERTEX;		
	if(m_pkInputHandle->Pressed(KEY_5)) m_iHMapEditMode = HMAP_DRAWSMFLAT;			
	if(m_pkInputHandle->Pressed(KEY_6)) m_iHMapEditMode = HMAP_STITCH;			
	if(m_pkInputHandle->Pressed(KEY_7)) m_iHMapEditMode = HMAP_DRAWMASK;			
	if(m_pkInputHandle->Pressed(KEY_8)) m_iHMapEditMode = HMAP_DRAWVISIBLE;			
}


// Handles input for EditMode Zones.
void ZeroEd::Input_EditZone()
{
	if(m_pkInputHandle->Pressed(KEY_L) && !DelayCommand())
	{
		SendZoneListRequest();	
	}

	if((m_pkInputHandle->Pressed(MOUSELEFT) || m_pkInputHandle->Pressed(KEY_KP0)) && !DelayCommand())
	{
		SendAddZone(m_kZoneMarkerPos,m_kZoneSize,m_kZoneModelRotation,m_strActiveZoneName);
		m_kLastZonePos=m_kZoneMarkerPos; 
		m_kLastZoneSize=m_kZoneSize;
	}
	
	if(m_pkInputHandle->Pressed(MOUSEMIDDLE) && !DelayCommand())
	{
		SendAddZone(m_kZoneMarkerPos,m_kZoneSize,m_kZoneModelRotation,string(""));

	}
	
	if(m_pkInputHandle->VKIsDown("createhmapzone") && !DelayCommand())
	{
		SendAddZone(m_kZoneMarkerPos,m_kZoneSize,m_kZoneModelRotation,string(""));

		m_iCurrentMarkedZone = GetZoneID(m_kZoneMarkerPos);

 		if(ZoneData* pkData = GetZoneData(m_iCurrentMarkedZone))
 		{
 			P_Heightmap* pkHM = (P_Heightmap*)pkData->m_pkZone->AddProperty("P_Heightmap");
 			pkHM->SetSize(m_kZoneSize.x,m_kZoneSize.z);
 			pkHM->SetMaxValue(m_kZoneSize.y / 2.0);
 			
			P_Tcs* pp = (P_Tcs*)pkData->m_pkZone->AddProxyProperty("P_Tcs");
			pp->SetTestType(E_HMAP);
			pp->SetStatic(true);			
		}
	} 			
	
	if(m_pkInputHandle->VKIsDown("rotate") && !DelayCommand())
	{
		m_iCurrentMarkedZone = GetZoneID(m_kZoneMarkerPos);
		SendRotateZoneModel(m_iCurrentMarkedZone);		
	}
	
	if(m_pkInputHandle->VKIsDown("select") && !DelayCommand())
	{	
		m_iCurrentMarkedZone = GetZoneID(m_kZoneMarkerPos);

		if(ZoneData* pkData = GetZoneData(m_iCurrentMarkedZone))
		{
			Select_Toggle(pkData->m_iZoneObjectID, m_pkInputHandle->Pressed(KEY_LSHIFT));
			m_kLastZonePos = pkData->m_kPos;
		}
	}

	if(m_pkInputHandle->VKIsDown("rotatesize") && !DelayCommand())
	{
		float oz = m_kZoneSize.z;	
		m_kZoneSize.z = m_kZoneSize.x;
		m_kZoneSize.x = oz;
		m_kZoneModelRotation.y += 90;
	}
	
	//some default zone sizes, a hack kind of =D
	if(m_pkInputHandle->Pressed(KEY_1)) m_kZoneSize.Set(4,4,4);
	if(m_pkInputHandle->Pressed(KEY_2)) m_kZoneSize.Set(8,8,8);
	if(m_pkInputHandle->Pressed(KEY_3)) m_kZoneSize.Set(16,16,16);	
	if(m_pkInputHandle->Pressed(KEY_4)) m_kZoneSize.Set(16,64,16);	
	if(m_pkInputHandle->Pressed(KEY_5)) m_kZoneSize.Set(32,64,32);		
	if(m_pkInputHandle->Pressed(KEY_6)) m_kZoneSize.Set(64,64,64);	

	if(m_iAutoSnapZoneCorner != -1)
	{
		if(m_pkInputHandle->Pressed(MOUSEWUP)) 	
		{
			m_iAutoSnapZoneCorner++; if(m_iAutoSnapZoneCorner==9) m_iAutoSnapZoneCorner=1;
		}
		if(m_pkInputHandle->Pressed(MOUSEWDOWN))	
		{
			m_iAutoSnapZoneCorner--; if(m_iAutoSnapZoneCorner==0) m_iAutoSnapZoneCorner=8;
		}
	}
}


// Handles input for EditMode Object.
void ZeroEd::Input_EditObject(float fMouseX, float fMouseY)
{
	NetPacket kNp;

	if(m_pkInputHandle->VKIsDown("copy") && !DelayCommand())		EditRunCommand(FID_COPY);
	if(m_pkInputHandle->VKIsDown("paste") && !DelayCommand())	EditRunCommand(FID_PASTE);
	if(m_pkInputHandle->VKIsDown("clone") && !DelayCommand())	EditRunCommand(FID_CLONE);	
	
	if(m_pkInputHandle->Pressed(MOUSELEFT) && !DelayCommand())
	{
		kNp.Clear();
		kNp.Write((char) ZFGP_EDIT);
		kNp.Write_Str("spawn");
		kNp.Write_Str(m_strActiveObjectName.c_str());
		kNp.Write(m_kObjectMarkerPos);
		m_pkZeroFps->RouteEditCommand(&kNp);

      if(m_bPlaceObjectsOnGround)
      {
		   Entity* pkObj; 
		   if((pkObj = GetTargetObject()))
		      if(pkObj->GetCurrentZone() != -1)
				   PlaceObjectOnGround(pkObj->GetEntityID());
      }		
	}
	
	if(m_pkInputHandle->VKIsDown("select") && !DelayCommand())
	{	
		Entity* pkObj = GetTargetObject2();
		if(pkObj)
      {
			int sel_object = m_iCurrentObject;

			Select_Toggle(pkObj->GetEntityID(), m_pkInputHandle->Pressed(KEY_LSHIFT));  

			if(m_iCurrentObject == -1)
			{
				Select_Toggle(sel_object, false);  
				OpenObjectMenu();
			}
      }
	}
			
	if(m_pkInputHandle->VKIsDown("togglehide"))	
	{
		Entity* pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject);
		if(pkEnt && !DelayCommand())
			pkEnt->m_bHide = !pkEnt->m_bHide;
	}

	if(m_pkInputHandle->VKIsDown("toggleshowall"))	
	{
		if(!DelayCommand())
			m_pkEntityManager->m_bAllowHide = !m_pkEntityManager->m_bAllowHide;
	}

	// The followin handles to move or/and rotate a entity. If we have no entity we drop.
	Entity* pkObj = m_pkEntityManager->GetEntityByID(m_iCurrentObject);								
	if(!pkObj)
		return;		

	Vector3 kMove(0,0,0);
	Vector3 kClick(0,0,0);

	if(m_pkInputHandle->VKIsDown("reposent")) 
	{
		kMove = m_kObjectMarkerPos;
		
		if(m_pkActiveCamera->GetViewMode() != Camera::CAMMODE_PERSP) 
		{
			// In Ortho mode we keep the old coo for the axis that go into the screen.
			Vector3 kAxisX, kAxisY, kAxisZ;
			kAxisX = m_pkActiveCamera->GetOrthoAxisX();
			kAxisY = m_pkActiveCamera->GetOrthoAxisY();
			kAxisZ = m_pkActiveCamera->GetOrthoAxisZ();
			kAxisX.Abs();
			kAxisY.Abs();
			kAxisZ.Abs();

			kMove = kMove.PEP(kAxisX) + 
				kMove.PEP(kAxisY) + 
				pkObj->GetLocalPosV().PEP(kAxisZ);
		}

		kMove = m_pkActiveCamera->SnapToGrid(kMove);
		kNp.Clear();
		kNp.Write((char) ZFGP_EDIT);
		kNp.Write_Str("setpos");
		kNp.Write(m_iCurrentObject);
		kNp.Write(kMove);
		m_pkZeroFps->RouteEditCommand(&kNp);
	}
	else 
	{
		kMove.Set(0,0,0);
		
		if(!m_pkInputHandle->Pressed(KEY_LALT))
		{
			if(m_pkInputHandle->VKIsDown("moveleft"))		kMove += Vector3(-1 * m_pkZeroFps->GetFrameTime(),0,0);			
			if(m_pkInputHandle->VKIsDown("moveright"))	kMove += Vector3(1 * m_pkZeroFps->GetFrameTime(),0,0);			
			if(m_pkInputHandle->VKIsDown("moveup"))		kMove += Vector3(0,0,-1 * m_pkZeroFps->GetFrameTime());			
			if(m_pkInputHandle->VKIsDown("movedown"))		kMove += Vector3(0,0,1 * m_pkZeroFps->GetFrameTime());			
			if(m_pkInputHandle->VKIsDown("movefrw"))		kMove += Vector3(0,1 * m_pkZeroFps->GetFrameTime(),0);			
			if(m_pkInputHandle->VKIsDown("moveback"))		kMove += Vector3(0,-1 * m_pkZeroFps->GetFrameTime(),0);

			Camera::CamMode eCamMode = m_pkActiveCamera->GetViewMode();
			if(eCamMode == Camera::CAMMODE_PERSP)
			{
				;
			}
			else
			{
				kMove = m_pkActiveCamera->GetOrthoMove(kMove);
			}
		}

		kNp.Clear();
		kNp.Write((char) ZFGP_EDIT);
		kNp.Write_Str("move");
		AddSelected(&kNp);
		kNp.Write(kMove);
		m_pkZeroFps->RouteEditCommand(&kNp);
	}

	//handle rotation
	Vector3 kRot(0,0,0);
	if(m_pkInputHandle->VKIsDown("rotx+"))			kRot.x =  100*m_pkZeroFps->GetFrameTime();		
	if(m_pkInputHandle->VKIsDown("rotx-"))			kRot.x = -100*m_pkZeroFps->GetFrameTime();			
	if(m_pkInputHandle->VKIsDown("roty+"))			kRot.y =  100*m_pkZeroFps->GetFrameTime();			
	if(m_pkInputHandle->VKIsDown("roty-"))			kRot.y = -100*m_pkZeroFps->GetFrameTime();			
	if(m_pkInputHandle->VKIsDown("rotz+"))			kRot.z =  100*m_pkZeroFps->GetFrameTime();			
	if(m_pkInputHandle->VKIsDown("rotz-"))			kRot.z = -100*m_pkZeroFps->GetFrameTime();			

	if(kRot != Vector3::ZERO)
	{
		SendRotateEntity(m_iCurrentObject,kRot);	
	}
}

// Handles input for Edit Ambient Areas
void ZeroEd::Input_EditAmbientSounds()
{
	if(m_pkInputHandle->Pressed(MOUSELEFT) && !DelayCommand())
	{
		Entity* pkEnt;
		P_AmbientSound* pkProp;

		if((pkEnt = m_pkEntityManager->GetEntityByID(m_iCurrentObject)))
		{
			if(pkProp = (P_AmbientSound*) pkEnt->GetProperty("P_AmbientSound"))
			{
				Vector2 localpos = Vector2(m_kObjectMarkerPos.x, m_kObjectMarkerPos.z) - 
					Vector2(pkEnt->GetWorldPosV().x, pkEnt->GetWorldPosV().z);

				//pkProp->m_kPolygon.push_back(localpos);	

				float fRoof, fFloor;
				vector<Vector2> kArea;
				pkProp->GetArea(kArea, fRoof, fFloor);
				kArea.push_back(localpos);	

				if(localpos.y < fRoof)
					fRoof = localpos.y;
				if(localpos.y > fFloor)
					fFloor = localpos.y;

				pkProp->SetArea(kArea, fRoof, fFloor);
			}
		}

	}
}

void ZeroEd::Input_Camera(float fMouseX, float fMouseY)
{
	if(!m_pkActiveCameraObject)
		return;
	if(!m_pkActiveCamera)
		return;

	if(m_pkInputHandle->VKIsDown("vp1"))		SetViewPort("vp1");
	if(m_pkInputHandle->VKIsDown("vp2"))		SetViewPort("vp2");
	if(m_pkInputHandle->VKIsDown("vp3"))		SetViewPort("vp3");
	if(m_pkInputHandle->VKIsDown("vp4"))		SetViewPort("vp4");

	if(m_pkInputHandle->VKIsDown("speedup"))		m_CamSpeedScale += 0.1;
	if(m_pkInputHandle->VKIsDown("speeddown"))	m_CamSpeedScale -= 0.1;
	if(m_CamSpeedScale <= 0)							m_CamSpeedScale = 0.1;
	if(m_pkInputHandle->VKIsDown("fast"))			m_CamMoveSpeed = 20;

	float fSpeedScale = m_pkZeroFps->GetFrameTime() * (m_CamMoveSpeed * m_CamSpeedScale);

	Camera::CamMode eCamMode = m_pkActiveCamera->GetViewMode();

	if(eCamMode == Camera::CAMMODE_PERSP)
	{
		Vector3 newpos = m_pkActiveCameraObject->GetLocalPosV();
		
		Matrix4 kRm;
		kRm = m_pkActiveCameraObject->GetLocalRotM();
		
		Vector3 xv = kRm.GetAxis(0);
		Vector3 zv = kRm.GetAxis(2);

		xv.y = -xv.y;
		zv.y = -zv.y;
		
		if(m_bPlaneMovement)
 			zv.y = 0;
		
		xv.Normalize();
		zv.Normalize();

		if(m_pkInputHandle->VKIsDown("right"))		newpos += xv * -fSpeedScale;		
		if(m_pkInputHandle->VKIsDown("left"))		newpos += xv * fSpeedScale;		
		if(m_pkInputHandle->VKIsDown("forward"))	newpos += zv * fSpeedScale;
		if(m_pkInputHandle->VKIsDown("back"))		newpos += zv * -fSpeedScale;	

		if(m_pkInputHandle->VKIsDown("down"))		newpos.y += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("up"))			newpos.y -= fSpeedScale;
				
		//zonemarker distance toggle
		if(m_iAutoSnapZoneCorner == -1)
		{
			if(m_iEditMode == EDIT_ZONES || m_iEditMode == EDIT_HMAP)
			{
				if(m_pkInputHandle->Pressed(MOUSEWUP))
					m_fZoneMarkerDistance += 1.0;
				if(m_pkInputHandle->Pressed(MOUSEWDOWN) && m_fZoneMarkerDistance > 0)
					m_fZoneMarkerDistance -= 1.0;
			}
			else if(m_iEditMode == EDIT_OBJECTS)
			{
				if(m_pkInputHandle->Pressed(MOUSEWUP))
					m_fObjectMarkerDistance += 0.2;
				if(m_pkInputHandle->Pressed(MOUSEWDOWN) && m_fObjectMarkerDistance > 0.5)
					m_fObjectMarkerDistance -= 0.2;
			}
		}
		
		Vector3 rot;
		rot.Set(float(-fMouseY / 5.0),float(-fMouseX / 5.0),0);

		kRm.Transponse();		
		kRm.Rotate(rot);
		kRm.Transponse();		
		Vector3 bla = Vector3(0,0,1);
		bla = kRm.VectorTransform(bla);
		kRm.LookDir(bla,Vector3(0,1,0));
		kRm.Transponse();			
				
		m_pkActiveCameraObject->SetLocalPosV(newpos);		
		if(m_pkInputHandle->VKIsDown("pancam"))
		{
			
			m_pkActiveCameraObject->SetLocalRotM(kRm);	
			
			//m_pkInput->ShowCursor(false);
			//SDL_ShowCursor(false);
		}
		//else
		//	m_pkInput->ShowCursor(true);
			//SDL_ShowCursor(true);
	}

	else	// ORTHO MODE
	{
		Vector3 kMove = Vector3::ZERO;
		
		if(m_pkInputHandle->VKIsDown("down"))		m_pkActiveCamera->OrthoZoom(0.9);
		if(m_pkInputHandle->VKIsDown("up"))			m_pkActiveCamera->OrthoZoom(1.1);
		
		if(m_pkInputHandle->VKIsDown("pancam"))
		{
			if(m_pkInputHandle->VKIsDown("forward"))	m_pkActiveCamera->OrthoZoom(0.9);
			if(m_pkInputHandle->VKIsDown("back"))		m_pkActiveCamera->OrthoZoom(1.1);
		
			kMove += Vector3(-fMouseX * fSpeedScale,fMouseY * fSpeedScale,0);
		}
		else
		{
			if(m_pkInputHandle->VKIsDown("forward"))	kMove.y += fSpeedScale;
			if(m_pkInputHandle->VKIsDown("back"))		kMove.y -= fSpeedScale;
			if(m_pkInputHandle->VKIsDown("right"))		kMove.x += fSpeedScale;
			if(m_pkInputHandle->VKIsDown("left"))		kMove.x -= fSpeedScale;	
		}

		if(m_pkCameraObject[1]->GetParent() == m_pkCameraObject[0])
		{
			// If Cameras are linked.
			kMove = m_pkActiveCamera->GetOrthoMove(kMove);
			m_pkCameraObject[0]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV() + kMove);
		}
		else 
		{
			P_Camera* pkCam = static_cast<P_Camera*>(m_pkActiveCameraObject->GetProperty("P_Camera"));
			if(pkCam)
				pkCam->OrthoMove(kMove);
		}
	}
}

void ZeroEd::Input_SandBox(float fMouseX, float fMouseY)
{
	NetPacket kNp;

   if(m_pkInputHandle->VKIsDown("eventuse")  && !DelayCommand())	
	{
		kNp.Clear();
		kNp.Write((char) ZFGP_EDIT);
		kNp.Write_Str("use");
		kNp.Write( m_iCurrentObject );
		m_pkZeroFps->RouteEditCommand(&kNp);
	}

	if(m_pkInputHandle->VKIsDown("applyforce") )
	{	
		if(!m_bGrabing)
		{
			m_iGrabEntity = GetTargetTCS(&m_kGrabPos);
			if(m_iGrabEntity != -1)
			{
				cout<<"grabbing entity:"<<m_iGrabEntity<<endl;
				
				m_kLocalGrabPos = m_kGrabPos - m_pkEntityManager->GetEntityByID(m_iGrabEntity)->GetWorldPosV();			
				m_bGrabing = true;
				m_fArmLength = m_pkActiveCamera->GetPos().DistanceTo(m_kGrabPos);
				m_kGrabCurrentPos = m_kGrabPos;				
			
			}
		}
	}
	else
		m_bGrabing = false;	
	
	if(m_pkInputHandle->Pressed(KEY_LALT) )
	{
		if(P_Tcs* pkTcs = (P_Tcs*)m_pkEntityManager->GetPropertyFromEntityID(m_iCurrentObject,"P_Tcs"))
		{
// 			float fForce = m_pkZeroFps->GetFrameTime();
			Vector3 kForce = Vector3::ZERO;
			
			if(m_pkInputHandle->VKIsDown("moveleft")) kForce.x -= 1;
			if(m_pkInputHandle->VKIsDown("moveright"))kForce.x += 1;
			if(m_pkInputHandle->VKIsDown("movefrw")) 	kForce.z -= 1;
			if(m_pkInputHandle->VKIsDown("moveback")) kForce.z += 1;
			
			Matrix4 kRot = m_pkActiveCamera->GetRotM();			
			kRot.Transponse();
			
			kForce = kRot.VectorTransform(kForce);
			kForce.y = 0;
			kForce.Normalize();
			
			if(m_pkInputHandle->VKIsDown("moveup"))	kForce.y += 1;
			if(m_pkInputHandle->VKIsDown("movedown")) kForce.y -= 1;
			
			kForce *= m_pkZeroFps->GetFrameTime() * 20;
			
			pkTcs->ApplyImpulsForce(kForce);			
		}
	}
}

void ZeroEd::Input()
{
	//set speed depending on edit mode
	if(m_iEditMode == EDIT_HMAP)		m_CamMoveSpeed = 20;
	if(m_iEditMode == EDIT_ZONES)		m_CamMoveSpeed = 4 * m_kSnapSize.Length();
	if(m_iEditMode == EDIT_OBJECTS)	m_CamMoveSpeed = 5;
	
	// Read Relative Coo from mouse.
	float x = 0, z = 0;		
	m_pkInputHandle->RelMouseXY(x,z);	

	// First see if we clicked to change view port.
	if(!m_bSoloMode && m_pkInputHandle->Pressed(MOUSELEFT))
	{
		int mx,my;
		m_pkInputHandle->SDLMouseXY(mx,my);
		my = m_iHeight - my; 
		int iClickedViewPort = GetView(float(mx), float(my));
		if(SetCamera(iClickedViewPort))
			m_fDelayTime = m_pkZeroFps->GetEngineTime() + float(0.5);
	}

	//Vector3 kMove(0,0,0);

	// For everything else we need a active camera so we bail until we get one.
	if(!m_pkCameraObject)	
		return;

	Input_Camera(float(x),float(z));

	if(m_pkInputHandle->VKIsDown("modezone"))			m_iEditMode = EDIT_ZONES;
	if(m_pkInputHandle->VKIsDown("modeobj"))			m_iEditMode = EDIT_OBJECTS;		
	if(m_pkInputHandle->VKIsDown("modehmvertex"))	m_iEditMode = EDIT_HMAP;		

	if(m_iEditMode == EDIT_HMAP)							Input_EditTerrain();
	if(m_iEditMode == EDIT_ZONES)							Input_EditZone();
	if(m_iEditMode == EDIT_OBJECTS)						Input_EditObject(float(x),float(z));
	if(m_iEditMode == EDIT_AMBIENTSOUNDS)				Input_EditAmbientSounds();

	if(m_pkInputHandle->VKIsDown("solo"))				SoloToggleView();

	// Delete selected entity ( the server cheks if its a zone or an normal entity)
	if(m_pkInputHandle->VKIsDown("remove"))	
		SendDeleteSelected();

	Input_SandBox(float(x),float(z));
}


/*	Return 3D postion of mouse in world. */
Vector3 ZeroEd::Get3DMouseDir(bool bMouse)
{
	int x;
	int y;
	m_pkInputHandle->SDLMouseXY(x,y);		
	
	return m_pkActiveCamera->Get3DCursorDir(x,y,bMouse);
}







