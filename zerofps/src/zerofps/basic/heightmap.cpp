#include "heightmap.h"


HeightMap::HeightMap() {
	m_iHmSize=500;	
	m_iBoxTresh=2;
	m_iMaxSteps=5;
	m_iError=10;
	m_kPosition=Vector3(-50,0,-50);
	verts=new HM_vert[(m_iHmSize+m_iError)*m_iHmSize];
	Zero();
	strcpy(m_acTileSet,"file:../data/textures/grass.bmp");
}

void HeightMap::Zero() {
	for(int i=0;i<(m_iHmSize+m_iError)*m_iHmSize;i++){
		verts[i].height=0;
		verts[i].texture=0;
	}
}

void HeightMap::SetPosition(Vector3 kNewPos) {
	m_kPosition=kNewPos;

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
}


bool HeightMap::Load(char* acFile) {
	cout<<"Loading new heightmap from file "<<acFile<<endl;
	
	return true;
}

bool HeightMap::Save(char* acFile) {
	cout<<"Save heightmap to file "<<acFile<<endl;
	
	return true;
}


void HeightMap::Random() {
	int height=8000;
	int peaks=3000;
	int smooth=10;

	srand(time(0));
		
	int x,y;
	for(int i=0;i<peaks;i++) {
		x=rand()%(m_iHmSize-4)+2;	
		y=rand()%(m_iHmSize-4)+2;	
		verts[y*m_iHmSize+x].height=(rand()%height)/10.0;
	}
	
	for(int i=0;i<peaks/4;i++) {
		x=rand()%(m_iHmSize-4)+2;	
		y=rand()%(m_iHmSize-4)+2;	
		verts[y*m_iHmSize+x].height=(rand()%height*4)/10.0;
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
	
//	for(int i=0;i<m_iHmSize*m_iHmSize;i++)
//		verts[i].height+=(rand()%1000)/5000.0;
	
}

HM_vert* HeightMap::GetVert(int x,int z) {
//	cout<<"hora:"<<z*m_iHmSize+x<<endl;
	return &verts[z*m_iHmSize+x];
}


void HeightMap::GenerateTextures() {
	float slope;
	Vector3 diff;
	for(int z=0;z<m_iHmSize-1;z++) {
		for(int x=0;x<m_iHmSize-1;x++) {
			slope=0;
			diff=Vector3(0,0,0);
			for(int q=0;q<2;q++){	
				for(int w=0;w<2;w++){	
					diff+=GetVert(x+w,z+q)->normal;
//					diff = GetVert(x,z)->height - GetVert(x+q,z+w)->height;
				}
			}
			diff.normalize();
//			cout<<diff.x<<" "<<diff.y<<" "<<diff.z<<endl;
			slope=diff.dot(Vector3(0,1,0)) *degtorad;
						
//			cout<<"Slope:"<<slope<<endl;
			if(slope<30)
				GetVert(x,z)->texture=3;
			else if(slope<50)				
				GetVert(x,z)->texture=2;
			else 
				GetVert(x,z)->texture=1;				
			
//			if(GetVert(x,z)->texture==1)
//				if(slope<40)
//					cout<<"CP!!!:"<<slope<<endl;
		}
	}
}





