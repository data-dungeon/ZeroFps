#include "heightmap.h"


HeightMap::HeightMap(FileIo* pkFile) 
 : ZFObject("HeightMap") {
	m_pkFile=pkFile;
	m_iHmSize=500;	
//	m_iBoxTresh=2;
//	m_iMaxSteps=5;
	m_iError=10;
	m_kPosition=Vector3(0,0,0);
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

float HeightMap::Height(float x,float z) {
	
	x-=m_kPosition.x;
	z-=m_kPosition.z;

	if(x<0 || x>m_iHmSize || z<0 || z>m_iHmSize) 
		return 1;

	int lx=int(x);
	int lz=int(z);
	float ox=x-float(lx);
	float oz=z-float(lz);
	float bp,xp,zp;

	
	//are we on the over or under polygon in the tile
	float ry=(1.0+ox*-1.0);

	if(oz>ry){//over left
		bp=verts[(lz+1)*m_iHmSize+(lx+1)].height;
		xp=verts[(lz+1)*m_iHmSize+(lx)].height-bp;
		zp=verts[(lz)*m_iHmSize+(lx+1)].height-bp;		
		ox=1.0-ox;
		oz=1.0-oz;
	}else{//under right
		bp=verts[lz*m_iHmSize+lx].height;
		xp=verts[(lz)*m_iHmSize+(lx+1)].height-bp;
		zp=verts[(lz+1)*m_iHmSize+(lx)].height-bp;				
	}	

	return bp+m_kPosition.y+(xp*ox)+(zp*oz);	
}

Vector3 HeightMap::Tilt(float x,float z) {
	
	x-=m_kPosition.x;
	z-=m_kPosition.z;

	if(x<0 || x>m_iHmSize || z<0 || z>m_iHmSize) 
		return Vector3(1,1,1);
		
	int lx=int(x);
	int lz=int(z);
	float ox=x-float(lx);
	float oz=z-float(lz);
//	float bp,xp,zp;
	
//	return verts[lz*m_iHmSize+lx].normal;
	
	Vector3 v1,v2,n1;
	
	float ry=(1.0+ox*-1.0);
	if(oz>ry){
//		cout<<"UPP"<<endl;		
		v1=Vector3(1,(verts[(lz+1)*m_iHmSize+(lx+1)].height)	-(verts[(lz+1)*m_iHmSize+(lx)].height) ,0);
		v2=Vector3(0,(verts[(lz+1)*m_iHmSize+(lx+1)].height)	-(verts[(lz)*m_iHmSize+(lx+1)].height) ,1);	
		
		n1=v2.Cross(v1);			
	}else {
//		cout<<"DOWN"<<endl;		
		v1=Vector3(0,(verts[(lz+1)*m_iHmSize+(lx)].height)	-(verts[(lz)*m_iHmSize+(lx)].height) ,1);
		v2=Vector3(1,(verts[(lz)*m_iHmSize+(lx+1)].height)	-(verts[(lz)*m_iHmSize+(lx)].height) ,0);		
	
		n1=v1.Cross(v2);			
	}
	
	n1.Normalize();
	return n1;
}


void HeightMap::SetTileSet(char* acTileSet) {
	strcpy(m_acTileSet,acTileSet);
	
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
	
					n1=v2.Cross(v1);			
					n2=v3.Cross(v2);				
//					n1.normalize();
//					n2.normalize();
	
					med=med+n1+n2;
				}	
			}
			med=med*0.125;	//insted of  division by 8 
			med.Normalize();
			verts[z*m_iHmSize+x].normal=med;
		}
	}
}


bool HeightMap::Load(char* acFile) {
	cout<<"Loading heightmap from file "<<acFile<<endl;
	
	//setup fileheader
//	HM_fileheader k_Fh;
	
	//open file
	FILE* fp=fopen(m_pkFile->File(acFile),"rb");
	if(fp==NULL)
		return false;
	
	//write header
	fwrite(&m_iHmSize,sizeof(float),1,fp);	
//	m_iHmSize=k_Fh.m_iHmSize;
	cout<<"Size is:"<<m_iHmSize<<endl;
	
	
	delete[] verts;
	verts=new HM_vert[(m_iHmSize+m_iError)*m_iHmSize];
		
	for(int i=0;i<m_iHmSize*m_iHmSize;i++) {
		fread(&verts[i].height,sizeof(float),1,fp);
	}

//	fread(&verts[0],sizeof(HM_vert),m_iHmSize*m_iHmSize,fp);
	
	
	fclose(fp);
		
	
	return true;
}


bool HeightMap::Save(char* acFile) {
	cout<<"Save heightmap to file "<<acFile<<endl;
	
	//setup fileheader
	HM_fileheader k_Fh;
	k_Fh.m_iHmSize=m_iHmSize;
	
	//open file
	FILE* fp=fopen(m_pkFile->File(acFile),"wb");
	if(fp==NULL)
		return false;
	
	//write header
	fwrite(&m_iHmSize,sizeof(float),1,fp);
	
	//write heightmap data
	for(int i=0;i<m_iHmSize*m_iHmSize;i++) {
		fwrite(&verts[i].height,sizeof(float),1,fp);
	}

//		fwrite(&verts[0],sizeof(HM_vert),m_iHmSize*m_iHmSize,fp);

//	}
	
	
	
	//close file
	fclose(fp);
	
	return true;
}


void HeightMap::Random() {
	int height=8000;
	int peaks=3000;
	int smooth=10;

	srand(time(0));
	
	int i;

	int x,y;
	for(i=0;i<peaks;i++) {
		x=rand()%(m_iHmSize-4)+2;	
		y=rand()%(m_iHmSize-4)+2;	
		verts[y*m_iHmSize+x].height=(rand()%height)/10.0;
	}
	
	for(i=0;i<peaks/4;i++) {
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
}


HM_vert* HeightMap::GetVert(int x,int z) {
//	cout<<"hora:"<<z*m_iHmSize+x<<endl;
	return &verts[z*m_iHmSize+x];
}


void HeightMap::GenerateTextures() {
	float slope;
	float height;
	Vector3 diff;
	for(int z=0;z<m_iHmSize-1;z++) {
		for(int x=0;x<m_iHmSize-1;x++) {
			slope=0;
			diff=Vector3(0,0,0);
			for(int q=0;q<2;q++){	
				for(int w=0;w<2;w++){	
					diff+=GetVert(x+w,z+q)->normal;
				}
			}
			diff.Normalize();
			slope=diff.Dot(Vector3(0,1,0)) *degtorad;
			height=GetVert(x,z)->height;						
//			cout<<"Slope:"<<slope<<endl;

//			GetVert(x,z)->color=Vector3(1,1,1);			
			if(slope<35) {
				GetVert(x,z)->texture=3;//stone
				GetVert(x,z)->color=Vector3(0.7,0.7,0.7);			
			}
			else if(slope<50){
				GetVert(x,z)->texture=2;//slope
				GetVert(x,z)->color=Vector3(.6,.45,0.3);				
			}				
			else {
				if(height<5) {  //if we are very low draw nice sand =)
					GetVert(x,z)->texture=0;
					GetVert(x,z)->color=Vector3(.80,.70,.60);
					if(height<2) {//Make a big bad hole
						GetVert(x,z)->color=Vector3(0.8 * GetVert(x,z)->height -.55, 0.7 * GetVert(x,z)->height -.55, 0.6 *GetVert(x,z)->height -.55); //(.001,.001,.51);					
						GetVert(x,z)->height-=10 + (1 - GetVert(x,z)->height)*10;
						GetVert(x,z)->normal=Vector3(0,1,0);
					}
				} else {//else i like som grass
					GetVert(x,z)->texture=1;
//					GetVert(x,z)->color=Vector3(.35,.55,.10);				
					GetVert(x,z)->color=Vector3(.2,.5,.2);				
				}
			}
//			GetVert(x,z)->color.Normalize();
		}
	}
}

bool HeightMap::LoadImageHmap(char* acFile) {
	int smooth=1;

	SDL_Surface *image;
	Uint8 data;
	Uint32 pixel;
	
	image=IMG_Load(m_pkFile->File(acFile));
	if(!image)
		return false;
		
	m_iHmSize=image->w;
	cout<<"Image Size is:"<<m_iHmSize<<endl;
	
	delete[] verts;
	verts=new HM_vert[(m_iHmSize+m_iError)*m_iHmSize];	

	for(int y=0;y<m_iHmSize;y++)
		for(int x=0;x<m_iHmSize;x++){						
			pixel=GetPixel(image,x,y);			
		
			SDL_GetRGB(pixel, image->format, &data,&data, &data);
			verts[y*m_iHmSize+x].height=data/3;			
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
	
//	delete image;
	return true;
}

Uint32 HeightMap::GetPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

/*
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
*/