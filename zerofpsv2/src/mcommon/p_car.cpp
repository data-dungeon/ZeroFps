#include "p_car.h"

P_Car::P_Car()
{
	strcpy(m_acName,"P_Car");
	m_iType=PROPERTY_TYPE_NORMAL;
	m_iSide=PROPERTY_SIDE_SERVER;

	bNetwork = true;

}

P_Car::~P_Car()
{
	delete m_pkInputHandle;
}


void P_Car::Init()
{
	cout<< "New car created"<<endl;

	m_pkInputHandle = new InputHandle("CarControl");
	m_pkInputHandle->SetActive(true);

}

void P_Car::Update()
{
		P_Tcs* pkTcs = (P_Tcs*)GetObject()->GetProperty("P_Tcs");
			
		if(!pkTcs)
		{
			cout<<"ERROR: car whitout P_Tcs property"<<endl;
			return;	
		}

		pkTcs->ClearExternalForces();
		
				
		Vector3 kStearPos = Vector3(0,0,-2);
		Vector3 kDrivePos = Vector3(0,0, 2);
		
		Vector3 kDriveForce = Vector3(0,0,-10);
		Vector3 kStearForce = Vector3( 6,0,0);
				
		kDriveForce = GetObject()->GetLocalRotM().VectorTransform(kDriveForce);
		kStearForce = GetObject()->GetLocalRotM().VectorTransform(kStearForce);

		if(m_pkInputHandle->Pressed(KEY_U))
			pkTcs->ApplyForce(kDrivePos,kDriveForce);

		if(m_pkInputHandle->Pressed(KEY_J))
			pkTcs->ApplyForce(kDrivePos,-kDriveForce);
			
			
		if(m_pkInputHandle->Pressed(KEY_K))
			pkTcs->ApplyForce(kStearPos,kStearForce);

		if(m_pkInputHandle->Pressed(KEY_H))
			pkTcs->ApplyForce(kStearPos,-kStearForce);
						
			
		//apply some trust
		
		Trust(Vector3( 0.75,0.2,-1.5),Vector3(0,-0.35,0));
		Trust(Vector3(-0.75,0.2,-1.5),Vector3(0,-0.35,0));

		Trust(Vector3( 0.75,0.2,1.5),Vector3(0,-0.35,0));
		Trust(Vector3(-0.75,0.2,1.5),Vector3(0,-0.35,0));
				
}

void P_Car::Trust(Vector3 kPos,Vector3 kForce)
{
	P_Tcs* pkTcs = (P_Tcs*)GetObject()->GetProperty("P_Tcs");
	float		fMaxForce = 10;

	Vector3 kUpForce  = GetObject()->GetLocalRotM().VectorTransform(-kForce);
	Vector3 kWPos = GetObject()->GetLocalRotM().VectorTransform(kPos);
	kWPos += GetObject()->GetLocalPosV();

	Vector3 kGround = LineTest(kWPos,kWPos + (kForce.Unit() * 50));		
	
	float fFd = kGround.DistanceTo(kWPos); 
	if(fFd > 3)
		return;
	
	Vector3 kRealForce = kUpForce / (fFd * fFd) ;
	
	if(kRealForce.Length() > fMaxForce)
		kRealForce = kRealForce.Unit() * fMaxForce;
	
	pkTcs->ApplyForce(kPos,kRealForce);
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