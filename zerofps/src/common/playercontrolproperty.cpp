#include "playercontrolproperty.h"
#include "../zerofps/engine/cssphere.h"

PlayerControlProperty::PlayerControlProperty(Input *pkInput,HeightMap *pkMap)
{
	m_pkMap			= pkMap;
	m_pkFps			= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan	= static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));	
	m_pkAlSys		= static_cast<OpenAlSystem*>(g_ZFObjSys.GetObjectPtr("OpenAlSystem"));		
	m_pkAlSys		= static_cast<OpenAlSystem*>(g_ZFObjSys.GetObjectPtr("OpenAlSystem"));		
	m_pkPhyEngine	= static_cast<PhysicsEngine*>(g_ZFObjSys.GetObjectPtr("PhysicsEngine"));			

	m_pkStatusProperty=NULL;

	m_pkInput=pkInput;	
	strcpy(m_acName,"PlayerControlProperty");

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	m_fSpeed=4;
	walk=0;
	walking=false;
	m_bAlive=true;
	m_fNextTime=m_pkFps->GetTicks();
	

	m_iActionForward=m_pkInput->RegisterAction("forward");
	m_iActionStrafeRight=m_pkInput->RegisterAction("strafe_right");
	m_iActionStrafeLeft=m_pkInput->RegisterAction("strafe_left");
	m_iActionBack=m_pkInput->RegisterAction("backward");
	m_iActionJump=m_pkInput->RegisterAction("jump");
	
	m_iActionUse=m_pkInput->RegisterAction("use");
	
	m_iActionUseItem=m_pkInput->RegisterAction("useitem");
	m_iActionNextItem=m_pkInput->RegisterAction("nextitem");
	
	m_iActionZoomIn=m_pkInput->RegisterAction("zoomin");
	m_iActionZoomOut=m_pkInput->RegisterAction("zoomout");
	
	m_kInventory.clear();
	m_kCurentInv=m_kInventory.begin();
	
//	NextInvItem();
	
	walksound=new Sound();
	walksound->m_acFile="file:../data/sound/walk.wav";
	walksound->m_bLoop=false;
	m_fCamSwitchTimer = m_pkFps->GetTicks();	

	m_fFov = 90;

	m_pkUseObject = NULL;
	m_bLockCamera = false;
	m_bSkipFrame = false;
};

PlayerControlProperty::~PlayerControlProperty()
{
	m_pkAlSys->RemoveSound(walksound);
	delete walksound;
	
//	m_kInventory.clear();
}

void PlayerControlProperty::Update() {
	m_pkObject->GetObjectType()=OBJECT_TYPE_PLAYER;
	
	if(m_pkStatusProperty==NULL)
	{
		m_pkStatusProperty=static_cast<StatusProperty*>(m_pkObject->GetProperty("StatusProperty"));
	}
	else
	{
		if(m_pkStatusProperty->m_fHealth <= 0){
			//cout<<"PLAYER DIED !!!!"<<endl;
			
			PhysicProperty* pp = dynamic_cast<PhysicProperty*>(m_pkObject->GetProperty("PhysicProperty"));
			static_cast<CSSphere*>(pp->GetColSphere())->m_fRadius=0.2;
			
			m_bAlive=false;
		}
	}

	if(!m_bAlive)
		return;

	if(SkipFrame())
		return;

	walking=false;
	Vector3 vel(0,m_pkObject->GetVel().y,0);	
	
	if(!m_bLockCamera)
	{
		if(m_pkInput->Action(m_iActionStrafeRight)){
			walking=true;				
			vel += GetYawVector2(m_pkObject->GetRot().y + 90) * m_fSpeed;
		}
		if(m_pkInput->Action(m_iActionStrafeLeft)){
			walking=true;		
			vel += GetYawVector2(m_pkObject->GetRot().y - 90) * m_fSpeed;
		}
		if(m_pkInput->Action(m_iActionForward)){
			walking=true;
			vel += GetYawVector2(m_pkObject->GetRot().y) * m_fSpeed;
		}
		if(m_pkInput->Action(m_iActionBack)){
			walking=true;
			vel += GetYawVector2(m_pkObject->GetRot().y + 180) * m_fSpeed;
		}
	}
	if(m_pkInput->Action(m_iActionNextItem))
	{
		if(m_pkFps->GetTicks() - m_fNextTime >=.2)
		{
			m_fNextTime=m_pkFps->GetTicks();
	
			NextInvItem();
		}
	}

	if(m_pkInput->Action(m_iActionUseItem))
	{
		UseInvItem();	
	}
	
	m_pkUseObject = NULL;

	if(m_pkInput->Action(m_iActionUse))
	{
		Object* bla = GetObject();
		
		if(bla != NULL){
			ItemProperty* ip = static_cast<ItemProperty*>(bla->GetProperty("ItemProperty"));
			cout << "OBJECT :"<<ip->m_kItemName<<endl;
			
			//PickUp(bla);
		}

		m_pkUseObject = bla;
	}
	
	
	if( (onGround || m_pkObject->GetPos().y < 0.5) )// && m_fGroundAngle < 65)
		m_pkObject->GetVel()=vel;	
	

	if(onGround && m_fGroundAngle >= 60)	{
		Vector3  res(GroundNormal.x,-1,GroundNormal.z);		
		m_pkObject->GetVel()=res * 4;
	}	


	if(m_pkInput->Action(m_iActionJump) && !m_bLockCamera)
	{
		if(onGround && m_fGroundAngle < 75){
			//cout<<"walking on normal: "<<GroundNormal.x<<" "<<GroundNormal.y<<" "<<GroundNormal.z<<endl;
			
			m_pkObject->GetVel()+=GroundNormal*5;	
		
			//vel.y=5;
			walking=false;
		}
	}
	
	
	
	//camera tilting when walking
	if(walk>6.28)
		walk=0;
	if(walk<0)
		walk=6.28;
	if(walking)
		walk+=m_pkFps->GetFrameTime()*5;
	else
		walk*=0.99;
//	m_pkObject->GetRot().z = (sin(walk))*2.5;	
	
	// Get mouse x,y		
	int x,z;		
	m_pkInput->RelMouseXY(x,z);

	m_pkCameraProperty	=	static_cast<CameraProperty*>(m_pkObject->GetProperty("CameraProperty"));
	
	if(m_pkInput->Pressed(KEY_X) == false && !m_bLockCamera){
		// Rrotate the camera and scale with fov.		
		Vector3 newrot = m_pkObject->GetRot();
		
		newrot.x += z / (180 / m_fFov);
		newrot.y += x / (180 / m_fFov);

		if(newrot.x>90)
			newrot.x=90;
		
		if(newrot.x<-90)
			newrot.x=-90;
			
		m_pkObject->SetRot(newrot);
	}
	else {
		if(m_pkCameraProperty) {
			m_pkCameraProperty->GetDynamicAngles().x += z / (180 / m_fFov);
			m_pkCameraProperty->GetDynamicAngles().y += x / (180 / m_fFov);
			}
		}
	
	//update sound possition
	walksound->m_kPos=m_pkObject->GetPos();

	if(walking && onGround)
		m_pkAlSys->AddSound(walksound);


// Set FOV
	if(m_pkCameraProperty) {
		if(m_pkInput->Action(m_iActionZoomIn)) {
			m_fFov -= 5;
			if(m_fFov < 2)
				m_fFov = 2;
			m_pkCameraProperty->SetFpFov(m_fFov);
			}

		if(m_pkInput->Action(m_iActionZoomOut)) {
			m_fFov += 5;
			if(m_fFov > 90)
				m_fFov = 90;
			m_pkCameraProperty->SetFpFov(m_fFov);
			}

		if(m_pkInput->Pressed(KEY_C)) {
			if(m_fCamSwitchTimer < m_pkFps->GetTicks()) {
				m_fCamSwitchTimer = m_pkFps->GetTicks() + 0.5;	
				m_pkCameraProperty->NextType((CameraProperty::CamType_e) 0);
				}
			}
		}



	onGround=false;
};

void PlayerControlProperty::Touch(Collision* pkCol)
{
	Object* pkOther;
	PhysicProperty* pkPP;
	Vector3 kNormal;
	
	if(pkCol->m_pkPP1->GetObject() == (Object*)this)
	{
		pkOther=pkCol->m_pkPP2->GetObject();
		pkPP=pkCol->m_pkPP2;
		kNormal=pkCol->m_kNormal2;
	}
	else
	{
		pkOther=pkCol->m_pkPP1->GetObject();
		pkPP=pkCol->m_pkPP1;		
		kNormal=pkCol->m_kNormal1;		
	}

	if(!pkPP->m_bSolid)
		return;

	//	cout<<"walking on normal: "<<kNormal.x<<" "<<kNormal.y<<" "<<kNormal.z<<endl;
	m_fGroundAngle = RadToDeg(Vector3(0,1,0).Angle(kNormal));
	//cout<<"angle:"<<m_fGroundAngle<<endl;

	if(m_fGroundAngle < 90)
	{
		GroundNormal=kNormal.Unit();
		onGround=true;
	}
}

void PlayerControlProperty::AddObject(InventoryProperty* pkProperty)
{
	cout<<"Adding "<<pkProperty->m_acName<<" to players inventory"<<endl;	
	m_kInventory.push_back(pkProperty);

	NextInvItem();
}

void PlayerControlProperty::RemoveObject(InventoryProperty* pkProperty)
{
	cout<<"Removing "<<pkProperty->m_acName<<" from players inventory"<<endl;

	//check if trying to remove active item
//	if(pkProperty == (*m_kCurentInv))
//	{
		//if active try chaning
		//NextInvItem();
		
		//if still active set pointer to begin and cross your fingers
		//if(pkProperty == (*m_kCurentInv))
//cout<<"bla"<<endl;			

//		cout<<"bla"<<endl;
//	}
/*
	cout<<"SIZE"<<m_kInventory.size()<<endl;
	cout<<"1"<<endl;
	for(list<InventoryProperty*>::iterator it=m_kInventory.begin();it!=m_kInventory.end();it++)
	{
		cout<<"1.1"<<endl;	
		if(pkProperty==(*it)){
			cout<<"1.2"<<endl;
			m_kInventory.erase(it);
			cout<<"1.3"<<endl;
			break;
		}
//		delete (*it);
	}*/
	
	m_kInventory.remove(pkProperty);	
//	cout<<"2"<<endl;
	m_kCurentInv=m_kInventory.end();	
	
	cout<<"removed"<<endl;
}

void PlayerControlProperty::NextInvItem()
{
	if(m_kCurentInv==m_kInventory.end())
	{
		m_kCurentInv=m_kInventory.begin();
	}
	else
	{
		m_kCurentInv++;
		if(m_kCurentInv==m_kInventory.end())
			m_kCurentInv=m_kInventory.begin();
	}
	
	if((*m_kCurentInv) != NULL)
	{
		cout<<"Curent selected item "<<(*m_kCurentInv)->m_acName<<endl;	
	}
	else
	{
		cout<<"No item selected"<<endl;
	}
	
}

bool PlayerControlProperty::UseInvItem()
{
	if((*m_kCurentInv)!=NULL)
	{
		(*m_kCurentInv)->m_bUse=true;	
		return true;
	}
	else
	{
		return false;
	}
	
	return false;
}


Object* PlayerControlProperty::GetObject()
{
	list<Object*> pkObjects;	
	
	Vector3 kPos= m_pkObject->GetPos()+Vector3(0,0.95,0);
	Vector3 kVec= m_pkObject->GetRot().AToU();
	
	if(!m_pkPhyEngine->TestLine(&pkObjects,kPos,kVec))
		return NULL;
	
	float distance = 3;
	Object* pkObject = NULL;
	
	for(list<Object*>::iterator it=pkObjects.begin();it!=pkObjects.end();it++)
	{
		if((*it) == m_pkObject)
			continue;
			
		if((*it)->GetProperty("ItemProperty") == NULL)
			continue;
	
		float bla = ((*it)->GetPos() - kPos).Length();
		if( bla < distance)
		{
			distance = bla;
			pkObject = (*it);
		}		
	}
	
	return pkObject;
}


bool PlayerControlProperty::PickUp(Object* pkObject)
{
	ContainerProperty* CP = static_cast<ContainerProperty*>(m_pkObject->GetProperty("ContainerProperty"));
	ItemProperty* IP = static_cast<ItemProperty*>(pkObject->GetProperty("ItemProperty"));	
	
	if(CP == NULL)
		return false;
		
	if(IP == NULL)
		return false;
		
	if(!IP->m_bPickable)
		return false;
		
	cout<<"picking up object"<<endl;

	if(!CP->m_kContainer.AddItem(pkObject))
		return false;
	
	pkObject->SetParent(pkObject);

	return true;
}


bool PlayerControlProperty::SkipFrame()
{
	bool bSkip = false;

	if(m_bSkipFrame == true)
	{
		static float c_fPrevtime = m_pkFps->GetGameTime();
		static float c_fTime = 1.00f;
		float fCurrTime = m_pkFps->GetGameTime();

		if(((fCurrTime-c_fPrevtime) < c_fTime))
		{
			bSkip = true;
			m_bSkipFrame = true;			
			return true;
		}
		else
		{
			// m_pkInput->Reset();
			c_fPrevtime = fCurrTime;
			m_bSkipFrame = false;
		}
	}

	return bSkip;
}
