#include "zeroed.h"
#include "../zerofpsv2/engine_systems/common/heightmap.h"
#include "../zerofpsv2/engine_systems/propertys/p_hmrp2.h"

// Handles input for EditMode Terrain.
void ZeroEd::Input_EditTerrain()
{
	if(m_pkInputHandle->VKIsDown("inrad+"))		m_fHMInRadius += 1 * m_pkFps->m_pkObjectMan->GetSimDelta();
	if(m_pkInputHandle->VKIsDown("inrad-"))		m_fHMInRadius -= 1 * m_pkFps->m_pkObjectMan->GetSimDelta();
	if(m_pkInputHandle->VKIsDown("outrad+"))		m_fHMOutRadius += 1 * m_pkFps->m_pkObjectMan->GetSimDelta();
	if(m_pkInputHandle->VKIsDown("outrad-"))		m_fHMOutRadius -= 1 * m_pkFps->m_pkObjectMan->GetSimDelta();
	if(m_fHMInRadius > m_fHMOutRadius)
		m_fHMInRadius = m_fHMOutRadius;

	if(m_pkInputHandle->VKIsDown("hmraise"))		HMModifyCommand(5); 
	if(m_pkInputHandle->VKIsDown("hmlower"))		HMModifyCommand(-5);
	if(m_pkInputHandle->VKIsDown("hmsm") ) 		HMModifyCommand(0.0); 

	if(m_pkInputHandle->VKIsDown("hmpaint")) 
	{
		for(set<int>::iterator itEntity = m_SelectedEntitys.begin(); itEntity != m_SelectedEntitys.end(); itEntity++ ) 
		{
			Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID((*itEntity));
			if(!pkEntity)			continue;
			P_HMRP2* hmrp = dynamic_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
			if(hmrp == NULL)		continue;
			Vector3 kLocalOffset = m_kDrawPos - hmrp->m_pkHeightMap->m_kCornerPos;
			hmrp->m_pkHeightMap->DrawMask(m_kDrawPos, m_iEditLayer,m_fHMInRadius,255,255,255,1);
/*			if(m_iEditLayer == 1)
				hmrp->m_pkHeightMap->DrawVisible(kLocalOffset, false);
			else
				hmrp->m_pkHeightMap->DrawVisible(kLocalOffset, true);*/
			}
		}

	if(m_pkInputHandle->Pressed(KEY_1)) m_iEditLayer = 1;		
	if(m_pkInputHandle->Pressed(KEY_2)) m_iEditLayer = 2;			
	if(m_pkInputHandle->Pressed(KEY_3)) m_iEditLayer = 3;			

	if(m_pkInputHandle->Pressed(KEY_4))
	{
		Entity* pkEntity = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);
		if(pkEntity)
		{
			P_HMRP2* hmrp = dynamic_cast<P_HMRP2*>(pkEntity->GetProperty("P_HMRP2"));
			if(hmrp)
            hmrp->m_pkHeightMap->Invert();			
		}
	}
}


// Handles input for EditMode Zones.
void ZeroEd::Input_EditZone()
{
	if(m_pkInputHandle->Pressed(MOUSELEFT) && !DelayCommand())
	{
		AddZone(m_kZoneMarkerPos, m_kZoneSize, m_strActiveZoneName);	
	}
	
	if(m_pkInputHandle->Pressed(MOUSEMIDDLE) && !DelayCommand())
	{
		AddZone(m_kZoneMarkerPos, m_kZoneSize, m_strActiveZoneName,true);	
	}	
	
	if(m_pkInputHandle->VKIsDown("remove"))	DeleteSelected();
		
	if(m_pkInputHandle->VKIsDown("rotate") && !DelayCommand())
	{
		m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		
		/*
		ZoneData* zd = pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		if(zd)
			if(zd->m_bUnderContruction)*/
		RotateActive();
	}
	
	/*
	if(m_pkInputHandle->VKIsDown("buildmodeon") && !DelayCommand())
	{
		m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		m_pkObjectMan->SetUnderConstruction(m_iCurrentMarkedZone);
	}
	
	if(m_pkInputHandle->VKIsDown("buildmodeoff") && !DelayCommand())
	{
		m_iCurrentMarkedZone = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		m_pkObjectMan->CommitZone(m_iCurrentMarkedZone);
	}	
	*/
	if(m_pkInputHandle->VKIsDown("selectzone") && !DelayCommand())
	{	
		m_iCurrentMarkedZone =  m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		ZoneData* pkData = m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		if(pkData && pkData->m_pkZone)
			Select_Toggle(pkData->m_pkZone->GetEntityID(), m_pkInputHandle->Pressed(KEY_LSHIFT));
	}

	if(m_pkInputHandle->Pressed(KEY_1)) m_kZoneSize.Set(4,4,4);
	if(m_pkInputHandle->Pressed(KEY_2)) m_kZoneSize.Set(8,8,8);
	if(m_pkInputHandle->Pressed(KEY_3)) m_kZoneSize.Set(16,16,16);	
	if(m_pkInputHandle->Pressed(KEY_4)) m_kZoneSize.Set(32,16,32);	
	if(m_pkInputHandle->Pressed(KEY_5)) m_kZoneSize.Set(64,16,64);		
	if(m_pkInputHandle->Pressed(KEY_6)) m_kZoneSize.Set(1024,32,1024);		
}


// Handles input for EditMode Object.
void ZeroEd::Input_EditObject(float fMouseX, float fMouseY)
{
	if(m_pkInputHandle->VKIsDown("copy"))	EditRunCommand(FID_COPY);
	if(m_pkInputHandle->VKIsDown("paste"))	EditRunCommand(FID_PASTE);

	NetPacket kNp;

	if(m_pkInputHandle->Pressed(MOUSELEFT) && !DelayCommand() )
	{
		/*
		Entity* pkObj = m_pkObjectMan->CreateObjectFromScript(m_strActiveObjectName.c_str());
		pkObj->SetWorldPosV(m_kObjectMarkerPos);
		pkObj->SetParent(m_pkObjectMan->GetObjectByNetWorkID(
			m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone)->m_iZoneID));
		*/
		kNp.Write((char) ZFGP_EDIT);
		kNp.Write_Str("spawn");
		kNp.Write_Str(m_strActiveObjectName.c_str());
		kNp.Write(m_kObjectMarkerPos);
		m_pkFps->RouteEditCommand(&kNp);

		/*Entity* pkObj = m_pkObjectMan->CreateObjectFromScriptInZone(m_strActiveObjectName.c_str(), m_kObjectMarkerPos);

		if(pkObj)
			if(m_bPlaceObjectsOnGround)
				if(pkObj->GetCurrentZone() != -1)
					PlaceObjectOnGround(pkObj->GetEntityID());
		*/
	}
	
	if(m_pkInputHandle->VKIsDown("selectzone") && !DelayCommand())
	{		
		Entity* pkObj =  GetTargetObject();
		if(pkObj)
			Select_Toggle(pkObj->GetEntityID(), m_pkInputHandle->Pressed(KEY_LSHIFT));
	}
	
	//remove			
	if(m_pkInputHandle->VKIsDown("remove"))	DeleteSelected();

	Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);								
	if(!pkObj)
		return;		

	//return if its a static object
	if(pkObj->GetParent()->GetName() == "StaticEntity")
		return;
		
		
	//hack for collisions test
	if(m_pkInputHandle->VKIsDown("setvel"))		pkObj->SetVel(Vector3(1,0,0));

	// Move Selected Entity
	Vector3 kMove(0,0,0);
	Vector3 kClick(0,0,0);


	if(m_pkInputHandle->VKIsDown("reposent")) 
	{
		kMove = pkObj->GetLocalPosV();
		kMove = m_pkActiveCamera->GetPos() + Get3DMousePos(true) * 2;
		
		if(m_pkActiveCamera->GetViewMode() != Camera::CAMMODE_PERSP) {
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
		m_pkFps->RouteEditCommand(&kNp);
		//pkObj->SetLocalPosV( m_pkActiveCamera->SnapToGrid(kMove) );
	}
	else 
	{
		kMove.Set(0,0,0);
		if(m_pkInputHandle->VKIsDown("moveleft"))		kMove += Vector3(-1 * m_pkFps->GetFrameTime(),0,0);			
		if(m_pkInputHandle->VKIsDown("moveright"))	kMove += Vector3(1 * m_pkFps->GetFrameTime(),0,0);			
		if(m_pkInputHandle->VKIsDown("movefrw"))		kMove += Vector3(0,0,-1 * m_pkFps->GetFrameTime());			
		if(m_pkInputHandle->VKIsDown("moveback"))		kMove += Vector3(0,0,1 * m_pkFps->GetFrameTime());			
		if(m_pkInputHandle->VKIsDown("moveup"))		kMove += Vector3(0,1 * m_pkFps->GetFrameTime(),0);			
		if(m_pkInputHandle->VKIsDown("movedown"))		kMove += Vector3(0,-1 * m_pkFps->GetFrameTime(),0);

		if(m_pkInputHandle->VKIsDown("moveent")) 
		{
			kMove += m_pkActiveCamera->GetOrthoMove( Vector3(fMouseX, fMouseY,0) );
		}

		kNp.Clear();
		kNp.Write((char) ZFGP_EDIT);
		kNp.Write_Str("move");
		kNp.Write(m_iCurrentObject);
		kNp.Write(kMove);
		m_pkFps->RouteEditCommand(&kNp);

		//pkObj->SetLocalPosV(pkObj->GetLocalPosV() + kMove);
	}

/*	if(m_pkInputHandle->VKIsDown("rotent")) 
	{
		pkObj->RotateLocalRotV( Vector3(0, fMouseX,0));
	}*/

	// Rotate Selected Entity
	/*
	if(m_pkInputHandle->VKIsDown("rotx+"))			pkObj->RotateLocalRotV(Vector3(100*m_pkFps->GetFrameTime(),0,0));			
	if(m_pkInputHandle->VKIsDown("rotx-"))			pkObj->RotateLocalRotV(Vector3(-100*m_pkFps->GetFrameTime(),0,0));			
	if(m_pkInputHandle->VKIsDown("roty+"))			pkObj->RotateLocalRotV(Vector3(0,100*m_pkFps->GetFrameTime(),0));			
	if(m_pkInputHandle->VKIsDown("roty-"))			pkObj->RotateLocalRotV(Vector3(0,-100*m_pkFps->GetFrameTime(),0));			
	if(m_pkInputHandle->VKIsDown("rotz+"))			pkObj->RotateLocalRotV(Vector3(0,0,100*m_pkFps->GetFrameTime()));			
	if(m_pkInputHandle->VKIsDown("rotz-"))			pkObj->RotateLocalRotV(Vector3(0,0,-100*m_pkFps->GetFrameTime()));			
	*/
	kMove.Set(0,0,0);
	if(m_pkInputHandle->VKIsDown("rotx+"))			kMove.x = 100*m_pkFps->GetFrameTime();		
	if(m_pkInputHandle->VKIsDown("rotx-"))			kMove.x = -100*m_pkFps->GetFrameTime();			
	if(m_pkInputHandle->VKIsDown("roty+"))			kMove.y = 100*m_pkFps->GetFrameTime();			
	if(m_pkInputHandle->VKIsDown("roty-"))			kMove.y = -100*m_pkFps->GetFrameTime();			
	if(m_pkInputHandle->VKIsDown("rotz+"))			kMove.z = 100*m_pkFps->GetFrameTime();			
	if(m_pkInputHandle->VKIsDown("rotz-"))			kMove.z = -100*m_pkFps->GetFrameTime();			

	kNp.Clear();
	kNp.Write((char) ZFGP_EDIT);
	kNp.Write_Str("rot");
	kNp.Write(m_iCurrentObject);
	kNp.Write(kMove);
	m_pkFps->RouteEditCommand(&kNp);

}

void ZeroEd::Input_Camera(float fMouseX, float fMouseY)
{
	if(!m_pkActiveCameraObject)
		return;

//	if(m_pkInputHandle->Pressed(KEY_Z))		SetCamera(0);
//	if(m_pkInputHandle->Pressed(KEY_X))		SetCamera(1);
//	if(m_pkInputHandle->Pressed(KEY_C))		SetCamera(0);
//	if(m_pkInputHandle->Pressed(KEY_V))		SetCamera(1);

	if(m_pkInputHandle->Pressed(KEY_Z))		SetViewPort("vp1");
	if(m_pkInputHandle->Pressed(KEY_X))		SetViewPort("vp2");
	if(m_pkInputHandle->Pressed(KEY_C))		SetViewPort("vp3");
	if(m_pkInputHandle->Pressed(KEY_V))		SetViewPort("vp4");



	if(m_pkInputHandle->VKIsDown("slow")){
		m_CamMoveSpeed *= 0.25;
	}

	float fSpeedScale = m_pkFps->GetFrameTime()*m_CamMoveSpeed;

	if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_PERSP) 
	{

		Vector3 newpos = m_pkActiveCameraObject->GetLocalPosV();
		
		Matrix4 kRm;
		kRm = m_pkActiveCameraObject->GetLocalRotM();

		//kRm.Transponse();

		
		Vector3 xv = kRm.GetAxis(0);
		Vector3 zv = kRm.GetAxis(2);

		xv.y = 0;
		zv.y = 0;
		
		xv.Normalize();
		zv.Normalize();

		if(m_pkInputHandle->VKIsDown("right"))		newpos += xv * fSpeedScale;		
		if(m_pkInputHandle->VKIsDown("left"))		newpos += xv * -fSpeedScale;		
		if(m_pkInputHandle->VKIsDown("forward"))	newpos += zv * -fSpeedScale;
		if(m_pkInputHandle->VKIsDown("back"))		newpos += zv * fSpeedScale;	

		if(m_pkInputHandle->VKIsDown("down"))		newpos.y += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("up"))			newpos.y -= fSpeedScale;
				
		//mouse wheel test
		if(m_pkInputHandle->Pressed(MOUSEWUP)) 	newpos.y += 1.0;
		if(m_pkInputHandle->Pressed(MOUSEWDOWN))	newpos.y -= 1.0;
		
		
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
			SDL_ShowCursor(false);
			m_pkActiveCameraObject->SetLocalRotM(kRm);	
		}
		else
			SDL_ShowCursor(true);
	}

	else 
	{
		Vector3 kMove = Vector3::ZERO;

		if(m_pkInputHandle->VKIsDown("forward"))	kMove.y += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("back"))		kMove.y -= fSpeedScale;
		if(m_pkInputHandle->VKIsDown("right"))		kMove.x += fSpeedScale;
		if(m_pkInputHandle->VKIsDown("left"))		kMove.x -= fSpeedScale;	
		
		if(m_pkInputHandle->VKIsDown("down"))		m_pkActiveCamera->OrthoZoom(0.9);
		if(m_pkInputHandle->VKIsDown("up"))			m_pkActiveCamera->OrthoZoom(1.1);

		
		
		if(m_pkInputHandle->VKIsDown("pancam"))
		{
			kMove += Vector3(-fMouseX * fSpeedScale,fMouseY * fSpeedScale,0);
		}


		if(m_pkCameraObject[1]->GetParent() == m_pkCameraObject[0])
		{
			// If Cameras are linked.
			kMove = m_pkActiveCamera->GetOrthoMove(kMove);
			//kMove.Print();
			//cout << endl;
			m_pkCameraObject[0]->SetLocalPosV(m_pkCameraObject[0]->GetLocalPosV() + kMove);
		}
		else 
		{
			P_Camera* pkCam = dynamic_cast<P_Camera*>(m_pkActiveCameraObject->GetProperty("P_Camera"));
			if(pkCam)
				pkCam->OrthoMove(kMove);
		}
	}
}

void ZeroEd::Input()
{
	//set speed depending on edit mode
	if(m_iEditMode == EDIT_HMAP)		m_CamMoveSpeed = 20;
	if(m_iEditMode == EDIT_ZONES)		m_CamMoveSpeed = 20;
	if(m_iEditMode == EDIT_OBJECTS)	m_CamMoveSpeed = 5;
	
	int x = 0, z = 0;		
	m_pkInputHandle->RelMouseXY(x,z);	

	// First see if we clicked to change view port.
	if(m_pkInputHandle->Pressed(MOUSELEFT) || m_pkInputHandle->Pressed(KEY_0))
	{
		int mx,my;
		m_pkInputHandle->SDLMouseXY(mx,my);
		my = m_iHeight - my; 
		int iClickedViewPort = GetView(float(mx), float(my));
		if(SetCamera(iClickedViewPort))
			m_fDelayTime = m_pkFps->GetEngineTime() + float(0.5);
	}

	if(m_pkInputHandle->VKIsDown("makeland")) {
		Entity* pkObj = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);								
		if(!pkObj)
			return;		
		//int id = m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		//ZoneData* z = m_pkObjectMan->GetZoneData(id);
		//m_pkFps->AddHMProperty(z, z->m_iZoneObjectID,z->m_kSize);
		m_pkFps->AddHMProperty(pkObj, pkObj->GetEntityID(),m_kZoneSize);
	}  
	/*
	if(m_pkInputHandle->Pressed(KEY_F6)) 
	{
		Entity* pkEnt = m_pkObjectMan->GetObjectByNetWorkID(m_iCurrentObject);

		//m_iCurrentMarkedZone =  m_pkObjectMan->GetZoneIndex(m_kZoneMarkerPos,-1,false);
		//ZoneData* pkData = m_pkObjectMan->GetZoneData(m_iCurrentMarkedZone);
		//if(pkData && pkData->m_pkZone) 
		//{
			pkEnt->AddProperty("P_PfMesh");
			P_PfMesh* pkMesh = (P_PfMesh*)pkEnt->GetProperty("P_PfMesh");
			pkMesh->AutoMakeNaviMesh();
			//P_HMRP2* pkHmap = (P_HMRP2*)pkEnt->GetProperty("P_HMRP2");
			//if(pkHmap)
			//	pkMesh->SetHmap(pkHmap);
		//}
	}
	*/
	Vector3 kMove(0,0,0);
//	Vector3 kRotate(0,0,0);

	if(m_pkInputHandle->Pressed(KEY_H))	m_pkRender->DumpGLState("zzz.txt");			

	if(m_pkInputHandle->Pressed(KEY_8))	assert(0);
	if(m_pkInputHandle->Pressed(KEY_9))	ZFAssert(0, "Fet med test");


	if(m_pkCameraObject)	
	{	
		Input_Camera(float(x),float(z));
	
		if(m_pkInputHandle->VKIsDown("modezone"))			m_iEditMode = EDIT_ZONES;
		if(m_pkInputHandle->VKIsDown("modeobj"))			m_iEditMode = EDIT_OBJECTS;		
		if(m_pkInputHandle->VKIsDown("modehmvertex"))		m_iEditMode = EDIT_HMAP;		

		if(m_pkInputHandle->VKIsDown("lighton"))			m_pkZShader->SetForceLighting(LIGHT_ALWAYS_ON);	
		if(m_pkInputHandle->VKIsDown("lightoff"))			m_pkZShader->SetForceLighting(LIGHT_ALWAYS_OFF);
		if(m_pkInputHandle->VKIsDown("lightstd"))			m_pkZShader->SetForceLighting(LIGHT_MATERIAL);
	
		if(m_iEditMode == EDIT_HMAP)				Input_EditTerrain();
		if(m_iEditMode == EDIT_ZONES)				Input_EditZone();
		if(m_iEditMode == EDIT_OBJECTS)			Input_EditObject(float(x),float(z));

		if(m_pkInputHandle->VKIsDown("solo"))				SoloToggleView();
	}
};

/*	Return 3D postion of mouse in world. */
Vector3 ZeroEd::Get3DMouseDir(bool bMouse)
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;

	Vector3 kViewSize, kViewCorner;
	kViewSize = m_pkActiveCamera->GetViewPortSize();
	kViewCorner = m_pkActiveCamera->GetViewPortCorner();
	
	if(bMouse)
	{
		// Zeb was here! Nu kör vi med operativsystemets egna snabba musmarkör
		// alltså måste vi använda den position vi får därifrån.
		//	m_pkInputHandle->UnitMouseXY(x,y);
		//x = -0.5f + (float) m_pkInputHandle->m_iSDLMouseX / (float) m_pkApp->m_iWidth;
		//y = -0.5f + (float) m_pkInputHandle->m_iSDLMouseY / (float) m_pkApp->m_iHeight;
		int mx;		
		int my;
		
		m_pkInputHandle->SDLMouseXY(mx,my);
		
		x = -0.5f + (float) (mx - kViewCorner.x) / (float) kViewSize.x;
		y = -0.5f + (float) ((m_pkApp->m_iHeight - my) - kViewCorner.y) / (float) kViewSize.y;

		if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_PERSP)
		{
			dir.Set(x*xp,y*yp,-1.5);
			dir.Normalize();
		}
		else
		{
			dir.Set(0,0,-1);
			//dir.Normalize();
			//return dir;
		}
	}
	else
	{
		dir.Set(0,0,-1.5);
		dir.Normalize();	
	}
	
	Matrix4 rm = m_pkActiveCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}

/*	Returns 3D dir of mouse click in world. */
Vector3 ZeroEd::Get3DMousePos(bool m_bMouse=true)
{
	Vector3 dir;
	float x,y;		
	
	//screen propotions
	float xp=4;
	float yp=3;

	Vector3 kViewSize, kViewCorner;
	kViewSize = m_pkActiveCamera->GetViewPortSize();
	kViewCorner = m_pkActiveCamera->GetViewPortCorner();

	if(m_bMouse)
	{
		// Zeb was here! Nu kör vi med operativsystemets egna snabba musmarkör
		// alltså måste vi använda den position vi får därifrån.
		//	m_pkInputHandle->UnitMouseXY(x,y);
		/*
			x = -0.5f + (float) m_pkInputHandle->m_iSDLMouseX / (float) m_pkApp->m_iWidth;
			y = -0.5f + (float) m_pkInputHandle->m_iSDLMouseY / (float) m_pkApp->m_iHeight;
		*/
		int mx;		
		int my;
		
		m_pkInputHandle->SDLMouseXY(mx,my);		
		
		x = -0.5f + (float) (mx - kViewCorner.x) / (float) kViewSize.x;
		y = -0.5f + (float) ((m_pkApp->m_iHeight - my) - kViewCorner.y) / (float) kViewSize.y;
		
		//cout << "ViewCorner: " << kViewCorner.x << ", " << kViewCorner.y << endl;
		//cout << "kViewSize: " << kViewSize.x << ", " << kViewSize.y << endl;

		if(m_pkActiveCamera->GetViewMode() == Camera::CAMMODE_PERSP) 
		{
			dir.Set(x*xp,y*yp,-1.5);
			dir.Normalize();
		}
		else 
		{
			dir.x = x* m_pkActiveCamera->GetOrthoSize().x;
			dir.y = y* m_pkActiveCamera->GetOrthoSize().y;
			dir.z = -1.5; 
//			cout << "Cam XY: " << dir.x << "," << dir.y << endl;
		}
	}
	else
	{
		dir.Set(0,0,-1.5);
		dir.Normalize();	
	}
	
	Matrix4 rm = m_pkActiveCamera->GetRotM();
	rm.Transponse();
	dir = rm.VectorTransform(dir);
	
	return dir;
}