#include "p_car.h"

P_Car::P_Car()
{
	strcpy(m_acName,"P_Car");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = true;

}


void P_Car::Init()
{
	cout<< "New car created"<<endl;

	m_pkInputHandle = new InputHandle("CarControl");
	m_pkInputHandle->SetActive(true);

	m_kAcceleration.Set(0,0,0);
	m_kCurrentVel.Set(0,0,0);
}

void P_Car::Update()
{
	P_Tcs* pkTcs = (P_Tcs*)GetObject()->GetProperty("P_Tcs");
	
	Vector3 kForce = Vector3(0,0,0);
	
	if(GetObject()->GetParent()->IsZone())
	{
		Entity* pkCam = m_pkObjMan->GetObject("A light_red.lua");
		if(pkCam)				
			kForce = (pkCam->GetLocalPosV() - GetObject()->GetLocalPosV());	
	}
	else
	{
		Vector3 ap = GetObject()->GetWorldRotM().VectorTransform(Vector3(0,0,0.3));
		kForce = (GetObject()->GetParent()->GetLocalPosV() - (GetObject()->GetLocalPosV()+ap));	
		
		//kForce*=1;
	}

	
	pkTcs->ApplyForce(Vector3(0,0,1),kForce);
	
	
	static float t = m_pkZeroFps->GetTicks();
	if(m_pkInputHandle->Pressed(KEY_B) && (m_pkZeroFps->GetTicks() - t) > 1.0)
	{
		t = m_pkZeroFps->GetTicks();
		pkTcs->ApplyImpulsForce(Vector3(0,0,1),Vector3(0,1,0));
	
	}
	
	/*
	Entity* pkCam = m_pkObjMan->GetObject("A light_red.lua");
	
	if(pkTcs)
	{
		Vector3 kForce = Vector3(0,0,0);

				
		if(pkCam)
		{
			kForce = (pkCam->GetLocalPosV() - GetObject()->GetLocalPosV());
			//kForce *= 1;
		}
		

		
		pkTcs->ApplyForce(Vector3(0,0,2),kForce);
	
/*		float t = m_pkObjMan->GetSimDelta();
		Vector3 kForcePos;
		Vector3 kForce;
		Matrix3 kRot = GetObject()->GetLocalRotM();	
		
		kForcePos = Vector3(0,0,1);
		kForce = Vector3(0,0,1);
		
		if(pkCam)
			kForce = (pkCam->GetLocalPosV() - GetObject()->GetLocalPosV()).Unit();
		
		kForcePos = kRot.VectorTransform(kForcePos);
		
			
		Vector3 kVV = kForce.Cross(kForcePos);
		kVV *= t;
			
		Vector3 kVel = GetObject()->GetVel();
		kVel += kForce * t * 10;
			
		kRot.RadRotate(kVV);
		GetObject()->SetLocalRotM(kRot);
		//pkTcs->SetExternalForces(kForce);
		pkTcs->ApplyForce(Vector3(0,0,0),kForce);*/
//	}
/*	
	UpdateDistance();

	m_pkInputHandle->SetActive(true);

	if(m_pkInputHandle->Pressed(KEY_H))
		m_kControls[0] = true;

	if(m_pkInputHandle->Pressed(KEY_K))
		m_kControls[1] = true;

	if(m_pkInputHandle->Pressed(KEY_U))
		m_kControls[2] = true;

	if(m_pkInputHandle->Pressed(KEY_J))
		m_kControls[3] = true;


	float t = m_pkObjMan->GetSimDelta();
	float rs = m_kCurrentVel.Length() * 10;
	float ep = 5;
	m_kAcceleration.Set(0,0,0);
	float fStearFactor = 10;
	float fTraction = 1;
	float fStearingDeg = 15;
	float cs = m_kCurrentVel.Length();

	if(m_kControls[2])
		m_kAcceleration = Vector3(0,0,1);;
	if(m_kControls[3])
		m_kAcceleration = Vector3(0,0,-1);;

	//now stear and do the rest
	if(m_kControls[0] && !m_kControls[1] )
		GetObject()->RotateLocalRotV(Vector3(0,-cs*fStearFactor*t,0));

	if(m_kControls[1] && !m_kControls[0])
		GetObject()->RotateLocalRotV(Vector3(0,cs*fStearFactor*t,0));

	Matrix3 kForward = GetObject()->GetLocalRotM();
	Matrix3 kStearWeel = GetObject()->GetLocalRotM();

	//apply som stearing
	if(m_kControls[0])
		kStearWeel.Rotate(0,-fStearingDeg,0);
	if(m_kControls[1])
		kStearWeel.Rotate(0, fStearingDeg,0);


	//calculcate driving weel force
	m_kAcceleration *= ep;
	m_kAcceleration = kForward.VectorTransform(m_kAcceleration);

 	//add forces
	m_kCurrentVel += m_kAcceleration*t;

	//loosen grip if we go faster
	if( m_kCurrentVel.Length() != 0)
	{
		fTraction = 1 / m_kCurrentVel.Length();
		if(fTraction > 1)
			fTraction = 1;

	}
	//cout<<"travtion:"<<fTraction<<endl; 

	//calculate stearing weel force
	Vector3 kStear = Vector3(0,0,1);
	kStear *= m_kCurrentVel.Length() * fTraction;	
	kStear = kStearWeel.VectorTransform(kStear);

	//cacluate rest forward force
	Vector3 kRest = m_kCurrentVel * (1 - fTraction);

	//new velocity
	m_kCurrentVel = kStear + kRest;

	//apply frictioin
	m_kCurrentVel -= (m_kCurrentVel*0.4)*t;


	//update position
	Vector3 kPos = GetObject()->GetLocalPosV();
	kPos += m_kCurrentVel * t;
	GetObject()->SetLocalPosV(kPos);

	//reset controls
	m_kControls.reset();
	
*/	
}

void P_Car::UpdateDistance()
{
	Vector3 kFront(0,0,0.5);
	Vector3 kBack(0,0,-0.5);

	kFront = GetObject()->GetLocalRotM().VectorTransform(kFront);
	kBack = GetObject()->GetLocalRotM().VectorTransform(kBack);

	kFront += GetObject()->GetLocalPosV();
	kBack += GetObject()->GetLocalPosV();

	Vector3 kFroundGround = LineTest(kFront,kFront+Vector3(0,-100,0));
	Vector3 kBackGround = LineTest(kBack,kBack+Vector3(0,-100,0));

	Vector3 kGround = (kFroundGround + kBackGround) / 2;

	if(kFroundGround.y > kFroundGround.y)
		GetObject()->RotateLocalRotV(Vector3(0,0,1));
	else
		GetObject()->RotateLocalRotV(Vector3(0,0,-1));

	Vector3 kNewPos = GetObject()->GetLocalPosV();
	kNewPos.y = kGround.y + 0.2;
	GetObject()->SetLocalPosV(Vector3(kNewPos));

	//Vector3 blib(1,1,0);
	//blib.Unit();

	//(kFront - kFront)

	Matrix4 kRotM;

	kRotM.LookDir((kFroundGround - kBackGround).Unit(),Vector3(0,1,0));
	kRotM.Transponse();
	m_pkObject->SetLocalRotM(kRotM);
}

Vector3 P_Car::LineTest(Vector3 kStart,Vector3 kStop)
{
	Vector3 start = kStart;
	Vector3 dir = kStop;

	vector<Entity*> kObjects;
	kObjects.clear();

	m_pkObjMan->GetZoneObject()->GetAllEntitys(&kObjects);

	Vector3 kClosest;
	float closest = 999999999;
	float d;
	Vector3 cp;
	Entity* pkClosest = NULL;
	for(unsigned int i=0;i<kObjects.size();i++)
	{

		//objects that should not be clicked on (special cases)
		if(kObjects[i]->GetEntityID() <100000)
			continue;

		if(!kObjects[i]->IsZone())
			continue;

		//-------------

		//get mad property and do a linetest
		P_Mad* mp = (P_Mad*)kObjects[i]->GetProperty("P_Mad");
		if(mp)
		{
			if(mp->TestLine(start,dir))
			{
				cp = mp->GetLastColPos();
				d = start.DistanceTo(cp);

				if(d < closest)
				{
					closest = d;
					pkClosest = kObjects[i];
					kClosest = cp;
				}
			}
		}
	}

	return kClosest;
}

Property* Create_P_Car()
{
	return new P_Car;
}