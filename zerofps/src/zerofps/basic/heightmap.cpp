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
	

}
