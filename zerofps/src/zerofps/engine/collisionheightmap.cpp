#include "collisionheightmap.h"

CollisionHeightMap::CollisionHeightMap(HeightMap *pkHeightMap)
{
	m_pkHM=pkHeightMap;
}

bool CollisionHeightMap::Collide(CollisionObject *kOther,bool bContinue)
{	
	if(typeid(*kOther)==typeid(CollisionSphere)){
		CollisionSphere *kCs=dynamic_cast<CollisionSphere*>(kOther);
		return CollideSphere(kCs);

	} else if(typeid(*kOther)==typeid(CollisionPoint)){
		CollisionPoint *kCp = dynamic_cast<CollisionPoint*>(kOther);			
		return CollidePoint(kCp);
		
	} else if(bContinue){
		cout<<"Unhandled Collision,Asking kOther"<<endl;	
		return kOther->Collide(this,false);
	
	}
		
	return false;
}

bool CollisionHeightMap::CollideSphere(CollisionSphere *pkCs){

	return false;
}

bool CollisionHeightMap::CollidePoint(CollisionPoint *pkCp){
	if( pkCp->m_kPos->x >= m_pkHM->m_kPosition.x  &&  pkCp->m_kPos->x <= m_pkHM->m_kPosition.x+m_pkHM->m_iHmSize )
	{
		if( pkCp->m_kPos->z >= m_pkHM->m_kPosition.z  &&  pkCp->m_kPos->z <= m_pkHM->m_kPosition.z+m_pkHM->m_iHmSize )
		{
			cout<<"on heightmap"<<endl;
		

		}
	}
	return false;
}


