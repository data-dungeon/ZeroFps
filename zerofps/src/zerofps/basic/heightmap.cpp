		#include "heightmap.h"


HeightMap::HeightMap() {
	m_iHmSize=100;	
	verts=new HM_vert[m_iHmSize*m_iHmSize];
	Zero();

}

void HeightMap::Zero() {
	for(int i=0;i<m_iHmSize*m_iHmSize;i++){
		verts[i].height=0;
		verts[i].texture=0;
	}
}

void HeightMap::GenerateNormals() {


	int x=1;
	int z=1;

	for(int z=1;z<m_iHmSize-1;z++) {
		for(int x=1;x<m_iHmSize-1;x++) {

			Vector3 v1=Vector3(1,  (verts[(z-1)*m_iHmSize+(x)].height)-(verts[(z-1)*m_iHmSize+(x-1)].height) ,0);
			Vector3 v2=Vector3(1,(verts[(z-1)*m_iHmSize+(x-1)].height)-(verts[(z)*m_iHmSize+(x)].height) ,1);	
			Vector3 n1=v2.cross(v1);			
			n1.normalize();
			
//			Vector3 v3=Vector3(1, (verts[(z)*m_iHmSize+(x+1)].height) - (verts[z*m_iHmSize+x].height) ,0);
//			Vector3 v4=Vector3(1, (verts[(z+1)*m_iHmSize+(x+1)].height) - (verts[z*m_iHmSize+x].height) ,1);	
			
			Vector3 v3=Vector3(1,  (verts[(z+1)*m_iHmSize+(x)].height)-(verts[(z+1)*m_iHmSize+(x+1)].height) ,0);
			Vector3 v4=Vector3(1,(verts[(z+1)*m_iHmSize+(x+1)].height)-(verts[(z)*m_iHmSize+(x)].height) ,1);	
			
			Vector3 n2=v4.cross(v3);			
			n2.normalize();
			
			
			Vector3 n3=(n1+n2)*0.5;
			n3.normalize();
			
			verts[z*m_iHmSize+x].normal=n1;//n1;//(n1+n2)*0.5;	
		}
	}
	
/*
		for(int y=1;y<m_iHmSize-1;y++) {
			for(int x=1;x<m_iHmSize-1;x++) {
				med=0;
				for(int q=-1;q<2;q++)
					for(int w=-1;w<2;w++){
						if(q==0 && w==0) {							

						} else {
							med+=verts[(y+q)*m_iHmSize+(x+w)].height;							
						}
					}
				med=med/8;
				
				verts[y*m_iHmSize+x].height=med;
			}
		}*/
}


bool HeightMap::Load(char* acFile) {
	cout<<"Loading new heightmap from file "<<acFile<<endl;
	
	return true;
}

void HeightMap::Random() {
	int height=3000;
	int peaks=500;
	int smooth=5;

//   nice terrain
//	int height=30000000;
//	int peaks=200;
//	int smooth=14;
	srand(time(0));

		
	int x,y;
	for(int i=0;i<peaks;i++) {
		x=rand()%(m_iHmSize-4)+2;	
		y=rand()%(m_iHmSize-4)+2;	
		verts[y*m_iHmSize+x].height=(rand()%height)/10.0;
	}
	
	float med;
	for(int l=0;l<smooth;l++) {
		for(int y=1;y<m_iHmSize-1;y++) {
			for(int x=1;x<m_iHmSize-1;x++) {
				med=0;
				for(int q=-1;q<2;q++)
					for(int w=-1;w<2;w++){
						if(q==0 && w==0) {							

						} else {
							med+=verts[(y+q)*m_iHmSize+(x+w)].height;							
						}
					}
				med=med/8;
				
				verts[y*m_iHmSize+x].height=med;
			}
		}
	}

}
