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
