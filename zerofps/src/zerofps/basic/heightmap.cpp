#include "heightmap.h"


HeightMap::HeightMap() {
}

void HeightMap::Zero() {
	for(int i=0;i<HM_SIZE*HM_SIZE;i++){
		verts[i].height=0;
		verts[i].texture=0;
	}
}


bool HeightMap::Load(char* acFile) {
	cout<<"Loading new heightmap from file "<<acFile<<endl;
	
	return true;
}

void HeightMap::Random() {

	int height=30000000;
	int peaks=300;
	int smooth=14;

//   nice terrain
//	int height=30000000;
//	int peaks=200;
//	int smooth=14;

	int x,y;
	for(int i=0;i<peaks;i++) {
		x=rand()%(HM_SIZE-3)+2;	
		y=rand()%(HM_SIZE-3)+2;	
		verts[y*HM_SIZE+x].height=(rand()%height)/10.0;
	}
	
	float med;
	for(int l=0;l<smooth;l++) {
		for(int y=1;y<HM_SIZE-1;y++) {
			for(int x=1;x<HM_SIZE-1;x++) {
				med=0;
				for(int q=0;q<3;q++)
					for(int w=0;w<3;w++){
						if(q!=0 && w!=0) {
							med+=verts[(y+q)*HM_SIZE+(x+w)].height;

						}
					}
				med=med/8;
				
				verts[y*HM_SIZE+x].height=med;
			}
		}
	}
	

}
