#include "heightmap.h"


HeightMap::HeightMap() {
	m_iHmSize=100;	
	m_iBoxTresh=2;
	m_iMaxSteps=5;
	verts=new HM_vert[(m_iHmSize+1)*m_iHmSize];
	Zero();
	strcpy(m_acTileSet,"file:../data/textures/grass.bmp");
}

void HeightMap::Zero() {
	for(int i=0;i<m_iHmSize*m_iHmSize;i++){
		verts[i].height=0;
		verts[i].texture=0;
	}
}

float HeightMap::Height(int x,int z) {
	return verts[z*m_iHmSize+x].height;
}

void HeightMap::SetTileSet(char* acTileSet) {
	strcpy(m_acTileSet,acTileSet);
	
}

void HeightMap::MakeQuadTree() {
	m_kCenter=CreateQuad(m_iHmSize/2,m_iHmSize/2,m_iHmSize,0,true);
	
	cout<<"Generation of Quad-tree is complete"<<endl;

}


HM_vert* HeightMap::CreateQuad(int x,int z,int width,int step,bool more) {
	if(more==true)		
		cout<<"adding quad "<<x<<" "<<z;
	else
		cout<<"adding vertex "<<x<<" "<<z;
	cout<<" step:"<<step<<endl;


	step++;
	HM_vert *nVert=new HM_vert;	
	nVert->height=GetVert(x,z)->height;
	nVert->normal=GetVert(x,z)->normal;
	for(int i=0;i<8;i++) 				
		nVert->childs[i]=NULL;
	
	//check if we are on an edge
	if(x<1 || x>m_iHmSize-1 || z<0 || z>m_iHmSize-1){
		cout<<"neer edge"<<endl;
		return nVert;   
	}	
	
	//check if we have reached max child steps
	if(step>m_iMaxSteps) {
		cout<<"max steps reached"<<endl;
		return nVert;
	}
	
	
	if(more) {
		int i=0;
		for(int q=-1;q<2;q++){
			for(int w=-1;w<2;w++) {
				if(q==0 && w==0) {
//					nVert->childs[i]=CreateQuad(x+(width/2)*w,z+(width/2)*q,width/2,false);
				} else {			
					if(BoxTest(x+(width/4)*w,z+(width/4)*q,width))
						nVert->childs[i]=CreateQuad(x+(width/4)*w,z+(width/44)*q,width/2,step,true);
					else
						nVert->childs[i]=CreateQuad(x+(width/2)*w,z+(width/2)*q,width/2,step,false);
					i++;
				}
			}
		}		
	}
	return nVert;
}

bool HeightMap::BoxTest(int x,int z,int width) {
	float total=0;
	for(int q=-1;q<2;q++){
		for(int w=-1;w<2;w++) {
			if(q==0 && w==0) {
			
			}else {
				if((GetVert(x,z)->height-GetVert(x+w,z+w)->height)<0)
					total-=(GetVert(x,z)->height-GetVert(x+w,z+w)->height);					
				else
					total+=(GetVert(x,z)->height-GetVert(x+w,z+w)->height);
			}
		}
	}
	cout<<"TOTAL FOR "<<x<<" "<<z<<" is "<<total<<endl;
	
	//check if the diference is more then the treshold
	if(total>m_iBoxTresh)
		return true;
	else
		return false;
}


void HeightMap::GenerateNormals() {
	Vector3 med;
	Vector3 v1,v2,v3,n1,n2;
	
	
	for(int z=1;z<m_iHmSize-1;z++){
		for(int x=1;x<m_iHmSize-1;x++) {
			med=Vector3(0,0,0);  //reset medium vector
			for(int q=-1;q<1;q++){
				for(int w=-1;w<1;w++){
					v1=Vector3(1,(verts[(z+q)*m_iHmSize+(x+1+w)].height)-(verts[(z+q)*m_iHmSize+(x+w)].height) ,0);
					v2=Vector3(1,(verts[(z+1+q)*m_iHmSize+(x+1+w)].height)- (verts[(z+q)*m_iHmSize+(x+w)].height),1);		
					v3=Vector3(0,(verts[(z+q+1)*m_iHmSize+(x+w)].height)-(verts[(z+q)*m_iHmSize+(x+w)].height) ,1);	
	
					n1=v2.cross(v1);			
					n2=v3.cross(v2);				
//					n1.normalize();
//					n2.normalize();
	
					med=med+n1+n2;
				}	
			}
			med=med*0.125;
			med.normalize();
			verts[z*m_iHmSize+x].normal=med;
		}
	}
	
/*	
	for(int z=1;z<m_iHmSize-1;z++) {
		for(int x=1;x<m_iHmSize-1;x++) {

			Vector3 v1=Vector3(1,  (verts[(z-1)*m_iHmSize+(x)].height)-(verts[(z-1)*m_iHmSize+(x-1)].height) ,0);
			Vector3 v2=Vector3(1,(verts[(z-1)*m_iHmSize+(x-1)].height)-(verts[(z)*m_iHmSize+(x)].height) ,1);	
			Vector3 n1=v2.cross(v1);			
			n1.normalize();
						
			Vector3 v3=Vector3(1,  (verts[(z+1)*m_iHmSize+(x)].height)-(verts[(z+1)*m_iHmSize+(x+1)].height) ,0);
			Vector3 v4=Vector3(1,(verts[(z+1)*m_iHmSize+(x+1)].height)-(verts[(z)*m_iHmSize+(x)].height) ,1);				
			Vector3 n2=v4.cross(v3);			
			n2.normalize();
			
			
			Vector3 n3=(n1+n2)*0.5;
			n3.normalize();
			
			verts[z*m_iHmSize+x].normal=n1;//n1;//(n1+n2)*0.5;	
		}
	}
*/	
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
	int smooth=4;

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

HM_vert* HeightMap::GetVert(int x,int z) {
//	cout<<"hora:"<<z*m_iHmSize+x<<endl;
	return &verts[z*m_iHmSize+x];
}








