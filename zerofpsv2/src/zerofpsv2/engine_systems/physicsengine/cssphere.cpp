#include "cssphere.h"
 
CSSphere::CSSphere(float fRadius)
{
	m_fRadius=fRadius;
}
 
Collision* CSSphere::Test(CollisionShape* kOther,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CSSphere)){
		CSSphere *kCs=dynamic_cast<CSSphere*>(kOther);
		
		return Collide_CSSphere(kCs);		
	} else if(bContinue)
	{
		return kOther->Test(this,false);
	};
	
	//cout<<"Unhandled collision"<<endl;
	return NULL;
}

Collision* CSSphere::Collide_CSSphere(CSSphere* kOther)
{
	Object* O1=m_pkPP->GetObject();
	Object* O2=kOther->m_pkPP->GetObject();

	//caluculate movevector for both spheres
	Vector3 movevec1=m_pkPP->m_kNewPos - O1->GetPos();//-     m_pkPhysEngine->GetNewVel(m_pkPP);
	Vector3 movevec2=kOther->m_pkPP->m_kNewPos - O2->GetPos();//m_pkPhysEngine->GetNewVel(kOther->m_pkPP);
	
	//relative move vector
	Vector3 movevec = movevec1-movevec2;
	
	//if both spheres velocity is the same they cant collide
	if(movevec==Vector3(0,0,0))
		return NULL;
	

	//sum of the two spheres radius
	double sumRadii = (m_fRadius + kOther->m_fRadius);
	
	//normalize a movment vetor
	Vector3 N = movevec;
	N.Normalize();
	
	//center of a to center of b
	Vector3 C = O2->GetPos() - O1->GetPos();
	
	double D = N.Dot(C);
	
	// Another early escape: Make sure that A is moving
	// towards B! If the dot product between the movevec and
	// B.center - A.center is less that or equal to 0,
	// A isn't isn't moving towards B
	if(D<=0)
		return NULL;
				
	// Find the length of the vector C
	double lengthC = C.Length();
	
	double F = (lengthC * lengthC) - (D * D);
	
	// Escape test: if the closest that A will get to B
	// is more than the sum of their radii, there's no
	// way they are going collide
	double sumRadiiSquared = sumRadii * sumRadii;
	if(F >= sumRadiiSquared){
	  return NULL;
	}
	
	// We now have F and sumRadii, two sides of a right triangle.
	// Use these to find the third side, sqrt(T)
	double T = sumRadiiSquared - F;
	
	// If there is no such right triangle with sides length of
	// sumRadii and sqrt(f), T will probably be less than 0.
	// Better to check now than perform a square root of a
	// negative number.
	if(T < 0){
		return NULL;
	}
	
	// Therefore the distance the circle has to travel along
	// movevec is D - sqrt(T)
	double distance = D - sqrt(T);
	
	// Get the magnitude of the movement vector
	double mag = movevec.Length();
	
	// Finally, make sure that the distance A has to move
	// to touch B is not greater than the magnitude of the
	// movement vector.
	if(mag < distance){
	  return NULL;
	}
	
/*	cout << "distance: " << distance << endl;
	cout << "movevec.Length(): " << movevec.Length() << endl;
	cout << "absmovevec.Length(): " << fabs(movevec.Length()) << endl;*/

	float bla = float( distance / fabs(movevec.Length()) );

	Vector3 statmov1=movevec1 * bla;
	Vector3 statmov2=movevec2 * bla;	

	Vector3 newmov1=O1->GetPos() + (movevec1*bla);
	Vector3 newmov2=O2->GetPos() + (movevec2*bla);	
	
	Vector3 normal1 = (newmov2 - newmov1).Unit();
	Vector3 normal2 = (newmov1 - newmov2).Unit();	
	
	
	if(m_pkPP->m_bGlide) {
		Vector3 bu = movevec1-statmov1;
		float bulen=bu.Length();
		if(bulen!=0)
			bu.Normalize();		
		
		bu+=normal2;
		
		newmov1 = statmov1 + bu*bulen;
	} else {
		newmov1 = movevec1 * bla;
	}
	
	
	if(kOther->m_pkPP->m_bGlide) {		
		Vector3 bu = movevec2-statmov2;
		float bulen=bu.Length();
		if(bulen!=0)
			bu.Normalize();
		
		bu+=normal1;
		
		newmov2 = statmov2 + bu*bulen;
	} else {
		newmov2 = movevec2 * bla;
	}
	
	
	
	
	//assemble collision data
	Collision* tempdata = new Collision;
	
	tempdata->m_pkPP2 = kOther->m_pkPP;
	tempdata->m_kPos2 = O2->GetPos()+ newmov2;//(movevec2*bla);
	tempdata->m_fDistance2 = (tempdata->m_kPos2 - O2->GetPos()).Length();
	tempdata->m_kNormal2=normal2;
	
	tempdata->m_pkPP1 = m_pkPP;
	tempdata->m_kPos1 = O1->GetPos()+newmov1; //(movevec1*bla);
	tempdata->m_fDistance1 = (tempdata->m_kPos1 - O1->GetPos()).Length();
	tempdata->m_kNormal1=normal1;
	
//	tempdata->m_kNormal=Vector3(0,0,0);
	
	return tempdata;
}


