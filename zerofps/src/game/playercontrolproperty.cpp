#include "playercontrolproperty.h"
#include "../zerofps/engine/cssphere.h"

PlayerControlProperty::PlayerControlProperty(Input *pkInput,HeightMap *pkMap)
{
	m_pkMap			= pkMap;
	m_pkFps			= static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
	m_pkObjectMan	= static_cast<ObjectManager*>(g_ZFObjSys.GetObjectPtr("ObjectManager"));	
	m_pkAlSys		= static_cast<OpenAlSystem*>(g_ZFObjSys.GetObjectPtr("OpenAlSystem"));		
	m_pkAlSys		= static_cast<OpenAlSystem*>(g_ZFObjSys.GetObjectPtr("OpenAlSystem"));		

	m_pkStatusProperty=NULL;

	m_pkInput=pkInput;	
	strcpy(m_acName,"PlayerControlProperty");

	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	walk=0;
	walking=false;
	m_fNextTime=m_pkFps->GetTicks();
	

	m_iActionForward=m_pkInput->RegisterAction("forward");
	m_iActionStrafeRight=m_pkInput->RegisterAction("strafe_right");
	m_iActionStrafeLeft=m_pkInput->RegisterAction("strafe_left");
	m_iActionBack=m_pkInput->RegisterAction("backward");
	m_iActionJump=m_pkInput->RegisterAction("jump");
	
	m_iActionUseItem=m_pkInput->RegisterAction("useitem");
	m_iActionNextItem=m_pkInput->RegisterAction("nextitem");
	
	m_iActionZoomIn=m_pkInput->RegisterAction("zoomin");
	m_iActionZoomOut=m_pkInput->RegisterAction("zoomout");
	
	
	m_kCurentInv=m_kInventory.begin();	
//	NextInvItem();
	
	walksound=new Sound();
	walksound->m_acFile="file:../data/sound/walk.wav";
	walksound->m_bLoop=false;
	m_fCamSwitchTimer = m_pkFps->GetTicks();	

	m_fFov = 90;
};

PlayerControlProperty::~PlayerControlProperty()
{
	m_pkAlSys->RemoveSound(walksound);
	delete walksound;
}

void PlayerControlProperty::Update() {
	m_pkObject->GetObjectType()=OBJECT_TYPE_PLAYER;
	
	if(m_pkStatusProperty==NULL)
	{
		m_pkStatusProperty=static_cast<StatusProperty*>(m_pkObject->GetProperty("StatusProperty"));
	}
	else
	{
		if(m_pkStatusProperty->m_fHealth<0){
			//cout<<"PLAYER DIED !!!!"<<endl;
			
			PhysicProperty* pp = dynamic_cast<PhysicProperty*>(m_pkObject->GetProperty("PhysicProperty"));
			static_cast<CSSphere*>(pp->GetColSphere())->m_fRadius=0.2;
		}
	}

	float lutning=acos(Vector3(0,1,0).Dot(m_pkMap->Tilt(m_pkObject->GetPos().x,m_pkObject->GetPos().z)))*degtorad;
//	cout<<"LUTNING:"<<lutning<<endl;

	
	float speed=5;
	walking=false;
	Vector3 vel(0,m_pkObject->GetVel().y,0);
	
	
	if(onGround && lutning>35)	{
		Vector3 hora = m_pkMap->Tilt(m_pkObject->GetPos().x,m_pkObject->GetPos().z);
		Vector3  res(hora.x,-4,hora.z);
		m_pkObject->GetAcc()+=res*200;
	}
	
	if(m_pkInput->Pressed(KEY_X)){
		speed*=0.5;
	}
	
	if(m_pkInput->Action(m_iActionStrafeRight)){
		walking=true;		
		
		//vel.x+=cos((m_pkObject->GetRot().y+90)/degtorad)*speed;
		//vel.z+=sin((m_pkObject->GetRot().y+90)/degtorad)*speed;
		vel += GetYawVector2(m_pkObject->GetRot().y + 90) * speed;
	}
	if(m_pkInput->Action(m_iActionStrafeLeft)){
		walking=true;		
		
		//vel.x+=cos((m_pkObject->GetRot().y-90)/degtorad)*speed;
		//vel.z+=sin((m_pkObject->GetRot().y-90)/degtorad)*speed;		
		vel += GetYawVector2(m_pkObject->GetRot().y - 90) * speed;
	}
	if(m_pkInput->Action(m_iActionForward)){
		walking=true;
		
		//vel.x+=cos((m_pkObject->GetRot().y)/degtorad)*speed;
		//vel.z+=sin((m_pkObject->GetRot().y)/degtorad)*speed;	
		vel += GetYawVector2(m_pkObject->GetRot().y) * speed;
	}
	if(m_pkInput->Action(m_iActionBack)){
		walking=true;
		
		//vel.x+=cos((m_pkObject->GetRot().y+180)/degtorad)*speed;
		//vel.z+=sin((m_pkObject->GetRot().y+180)/degtorad)*speed;		
		vel += GetYawVector2(m_pkObject->GetRot().y + 180) * speed;
	}
	if(m_pkInput->Action(m_iActionNextItem))
	{
		if(m_pkFps->GetTicks() - m_fNextTime >=1)
		{
			m_fNextTime=m_pkFps->GetTicks();
	
			NextInvItem();
		}
	}

	if(m_pkInput->Action(m_iActionUseItem))
	{
		UseInvItem();	
	}
	
	if(m_pkInput->Action(m_iActionJump))
	{
		if(onGround){
			vel.y=5;
			walking=false;
		}
	}
	
	
	m_pkObject->GetVel()=vel;
	
	
	//camera tilting when walking
	if(walk>6.28)
		walk=0;
	if(walk<0)
		walk=6.28;
	if(walking)
		walk+=m_pkFps->GetFrameTime()*5;
	else
		walk*=0.99;
	m_pkObject->GetRot().z = (sin(walk))*2.5;	
	
	// Get mouse x,y		
	int x,z;		
	m_pkInput->RelMouseXY(x,z);

	m_pkCameraProperty	=	static_cast<CameraProperty*>(m_pkObject->GetProperty("CameraProperty"));
	
	if(m_pkInput->Pressed(KEY_X) == false){
		// Rrotate the camera and scale with fov.		
		m_pkObject->GetRot().x += z / (180 / m_fFov);
		m_pkObject->GetRot().y += x / (180 / m_fFov);

		if(m_pkObject->GetRot().x>90)
			m_pkObject->GetRot().x=90;
		
		if(m_pkObject->GetRot().x<-90)
			m_pkObject->GetRot().x=-90;
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

void PlayerControlProperty::Touch(Object* pkObject)
{
	Vector3 Dis=pkObject->GetPos()-m_pkObject->GetPos();	
		
	if(Dis.y<0)
		onGround=true;
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
	if(pkProperty == (*m_kCurentInv))
	{
		//if active try chaning
		NextInvItem();
		
		//if still active set pointer to begin and cross your fingers
		if(pkProperty == (*m_kCurentInv))
			m_kCurentInv=m_kInventory.begin();
	}
	
	m_kInventory.remove(pkProperty);	
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
	}
	else
	{
		return false;
	}
}



