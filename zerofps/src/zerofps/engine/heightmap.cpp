#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <cstdio>
#include "heightmap.h"

#include "../render/render.pkg"


HeightMap::HeightMap() 
 : ZFObject("HeightMap") {
 
 	m_pkFile=static_cast<FileIo*>(g_ZFObjSys.GetObjectPtr("FileIo"));		
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));		
	
	m_iError=4;
	
	verts =NULL;

	Create(128);
	

}

void HeightMap::Create(int iHmSize)
{
	m_iHmSize=iHmSize;
	m_iHmScaleSize = this->GetSize();
	m_kPosition.Set(0,0,0);

	delete[] verts;
	verts=new HM_vert[(m_iHmSize+m_iError)*m_iHmSize];	
//	delete[] m_pkVertex;
//	m_pkVertex =new Vector3[(m_iHmSize+m_iError)*m_iHmSize];	

	Zero();
	SetTileSet("file:../data/textures/landbw.bmp");
	
	ClearSet();
	AddSet("file:../data/textures/nodetail1.bmp","file:../data/textures/detail1.bmp","FEL");
}

void HeightMap::Zero() {
	for(int i=0;i<(m_iHmSize+m_iError)*m_iHmSize;i++){
		verts[i].height=5;
		verts[i].texture=0;
//		m_pkVertex[i].Set(0,0,0);
	}
}

void HeightMap::SetPosition(Vector3 kNewPos) {
	m_kPosition=kNewPos;

}

float HeightMap::Height(float x,float z) {
	
	x/=HEIGHTMAP_SCALE;
	z/=HEIGHTMAP_SCALE;
	
	x-=m_kPosition.x-m_iHmSize/2;
	z-=m_kPosition.z-m_iHmSize/2;

	if(x<0 || x>m_iHmSize || z<0 || z>m_iHmSize) 
		return 1;

	int lx=int(x);
	int lz=int(z);
	float ox=x-float(lx);
	float oz=z-float(lz);
	float bp,xp,zp;

	
	//are we on the over or under polygon in the tile   gillar min fina engelska =)
	float ry= float((1.0+ox*-1.0));

	if(oz>ry){//over left
		bp=verts[(lz+1)*m_iHmSize+(lx+1)].height;
		xp=verts[(lz+1)*m_iHmSize+(lx)].height-bp;
		zp=verts[(lz)*m_iHmSize+(lx+1)].height-bp;		
		ox= float(1.0-ox);
		oz= float(1.0-oz);
	}else{//under right
		bp=verts[lz*m_iHmSize+lx].height;
		xp=verts[(lz)*m_iHmSize+(lx+1)].height-bp;
		zp=verts[(lz+1)*m_iHmSize+(lx)].height-bp;				
	}	

	return m_kPosition.y+(bp+(xp*ox)+(zp*oz))*HEIGHTMAP_SCALE;	
}

Vector3 HeightMap::Tilt(float x,float z) {
	x/=HEIGHTMAP_SCALE;
	z/=HEIGHTMAP_SCALE;
	
	x-=m_kPosition.x-m_iHmSize/2;
	z-=m_kPosition.z-m_iHmSize/2;

	if(x<0 || x>m_iHmSize || z<0 || z>m_iHmSize) 
		return Vector3(1,1,1);
		
	int lx=int(x);
	int lz=int(z);
	float ox=x-float(lx);
	float oz=z-float(lz);
//	float bp,xp,zp;
	
//	return verts[lz*m_iHmSize+lx].normal;
	
	Vector3 v1,v2,n1;
	
	float ry= float ((1.0+ox*-1.0));
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

void HeightMap::GenerateNormals(int iStartx,int iStartz,int iWidth,int iHeight)
{
	Vector3 med;
	Vector3 v1,v2,v3,n1,n2;
	
	
	int lx,rx;
	int tz,bz;

	lx = iStartx;
	rx = iStartx+iWidth;
	
	tz = iStartz;
	bz = iStartz+iHeight;

	if(lx < 1)
		lx = 1;
		
	if(tz < 1)
		tz = 1;

	if(rx > m_iHmSize)
		rx = m_iHmSize;
		
	if(bz > m_iHmSize)
		bz = m_iHmSize;	
	
/*	if(iPosX>m_iHmSize || iPosZ>m_iHmSize)
		return;
	if(iPosX<0 || iPosZ <0)
		return;	
	if(iPosX+iWidth>m_iHmSize || iPosZ+iHeight>m_iHmSize){
		iWidth=iWidth-(iPosX+iWidth-m_iHmSize);
		iHeight=iHeight-(iPosZ+iHeight-m_iHmSize);
	}*/
	
	for(int z=tz;z<bz-1;z++){
		for(int x=lx;x<rx-1;x++) {
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


bool HeightMap::Load(const char* acFile) {
	cout<<"Loading heightmap from file "<<acFile<<endl;
	
	HM_fileheader k_Fh;
	
	ZFFile savefile;
	if(!savefile.Open(acFile,false)){
		cout<<"Could not Load heightmap"<<endl;
		return false;
	}
//	savefile.Read((void*)&k_Fh,sizeof(k_Fh));
	savefile.Read((void*)&k_Fh, sizeof(HM_fileheader));
	
	m_iHmSize=k_Fh.m_iHmSize;
	m_iHmScaleSize = GetSize();

	cout<<"MAP SIZE IS:"<<m_iHmSize<<endl;
	
	delete[] verts;
	verts=new HM_vert[(m_iHmSize+m_iError)*m_iHmSize];
//	delete[] m_pkVertex;
//	m_pkVertex =new Vector3[(m_iHmSize+m_iError)*m_iHmSize];	

	cout<<"SIZE:"<<sizeof(HM_vert)<<endl;
	
	int i;
	for(i=0;i<(m_iHmSize*m_iHmSize);i++) 
	{
		savefile.Read((void*)&verts[i],sizeof(HM_vert));
	}
	
	savefile.Close();
	
	
	return true;
}


bool HeightMap::Save(const char* acFile) {
	cout<<"Save heightmap to file "<<acFile<<endl;
	
	//setup fileheader
	HM_fileheader k_Fh;
	k_Fh.m_iHmSize=m_iHmSize;
	
	ZFFile savefile;
	if(!savefile.Open(acFile,true)){
		cout<<"Could not save heightmap"<<endl;
		return false;
	}
	savefile.Write((void*)&k_Fh, sizeof(HM_fileheader));
	
	for(int i=0;i<(m_iHmSize*m_iHmSize);i++) 
	{
		savefile.Write((void*)&verts[i],sizeof(HM_vert));
	}
	
	savefile.Close();
	
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
		verts[y*m_iHmSize+x].height = float((rand()%height)/10.0);
	}
	
	for(i=0;i<peaks/4;i++) {
		x=rand()%(m_iHmSize-4)+2;	
		y=rand()%(m_iHmSize-4)+2;	
		verts[y*m_iHmSize+x].height = float((rand()%height*4)/10.0);
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
			slope=diff.Dot(Vector3(0,1,0)) * degtorad;
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
						GetVert(x,z)->color=Vector3(0.5 * GetVert(x,z)->height -0.5, 0.5 * GetVert(x,z)->height-0.5, 0.1 + 0.5 *GetVert(x,z)->height-0.5 ); //(.001,.001,.51);					
//						GetVert(x,z)->color=Vector3(0,0, 1 * GetVert(x,z)->height ); //(.001,.001,.51);											
						GetVert(x,z)->height-=10 + (1 - GetVert(x,z)->height)*10;
						GetVert(x,z)->normal=Vector3(0,1,0);
					}
				} else {//else i like som grass
					GetVert(x,z)->texture=1;
//					GetVert(x,z)->color=Vector3(.35,.55,.10);				
//					GetVert(x,z)->color=Vector3(.2,.5,.2);				
					GetVert(x,z)->color=Vector3(0.19,0.53,0.11);				
//					GetVert(x,z)->color=Vector3(.5,.5,.5);				
				}
			}
		}
	}
}

bool HeightMap::LoadImageHmap(const char* acFile) {
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
//	delete[] m_pkVertex;
//	m_pkVertex=new Vector3[(m_iHmSize+m_iError)*m_iHmSize];	
	 
	for(int y=0;y<m_iHmSize;y++)
		for(int x=0;x<m_iHmSize;x++){						
			pixel=GetPixel(image,x,y);			
		
			SDL_GetRGB(pixel, image->format, &data,&data, &data);
			verts[y*m_iHmSize+x].height = data/3;		
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

void HeightMap::RunCommand(int cmdid, const CmdArgument* kCommand)
{

}

void HeightMap::AddSet(const char* acTexture,const char* acDetailTexture,const char* acMask)
{
	TileSet temp;
	strcpy(temp.m_acTexture,acTexture);
	strcpy(temp.m_acDetailTexture,acDetailTexture);	
	strcpy(temp.m_acMask,acMask);
	
	m_kSets.push_back(temp);
}

void HeightMap::ClearSet()
{
	m_kSets.clear();
}


void HeightMap::RebuildVertex()
{
	for(int i=0;i<(m_iHmSize+m_iError)*m_iHmSize;i++){
//		m_pkVertex[i].Set(0,0,0);
	}

}

void HeightMap::GetMapXZ(float& x,float& z)
{
	x/=HEIGHTMAP_SCALE;
	z/=HEIGHTMAP_SCALE;
	
	x-=m_kPosition.x-m_iHmSize/2;
	z-=m_kPosition.z-m_iHmSize/2;

}

void HeightMap::Smooth(int iStartx,int iStartz,int iWidth,int iHeight)
{
	int lx,rx;
	int tz,bz;

	lx = iStartx;
	rx = iStartx+iWidth;
	
	tz = iStartz;
	bz = iStartz+iHeight;

	if(lx < 1)
		lx = 1;
		
	if(tz < 1)
		tz = 1;

	if(rx > m_iHmSize)
		rx = m_iHmSize;
		
	if(bz > m_iHmSize)
		bz = m_iHmSize;
		

	for(int z=tz; z<bz-1; z++) {
		for(int x=lx; x<rx-1; x++) {
			float med=0;
			for(int q=-1;q<2;q++)
				for(int w=-1;w<2;w++){
					if(q==0 && w==0) 
					{							
					} else 
					{
						med+=verts[(z+q)*m_iHmSize+(x+w)].height;							
					}
				}
			med=med/8;
			
			verts[z*m_iHmSize+x].height=med;
		}
	}
	
	GenerateNormals(iStartx,iStartz,iWidth,iHeight);
}


void HeightMap::Flatten(int iPosx,int iPosy,int iSize)
{			
	if(iPosx - (iSize/2) >=0 &&
		iPosx + (iSize/2) <=m_iHmSize &&
		iPosy - (iSize/2) >=0 &&
		iPosy + (iSize/2) <=m_iHmSize)
	{		

		float height=GetVert(iPosx,iPosy)->height;

		for(int xp=-(iSize/2);xp<=(iSize/2);xp++){
			for(int yp=-(iSize/2);yp<=(iSize/2);yp++){
				GetVert(iPosx+xp,iPosy+yp)->height=height;				
			}
		}
				
		GenerateNormals(iPosx - (iSize/2),iPosy- (iSize/2),iSize,iSize);
	}
}


void HeightMap::Raise(int iPosx,int iPosy,int iMod,int iSize,bool bSmooth)
{
	if(iPosx - (iSize/2) >=0 &&
		iPosx + (iSize/2) <=m_iHmSize &&
		iPosy - (iSize/2) >=0 &&
		iPosy + (iSize/2) <=m_iHmSize)
	{		
	
		for(int xp=-(iSize/5);xp<=(iSize/5);xp++){
			for(int yp=-(iSize/5);yp<=(iSize/5);yp++){
				GetVert(iPosx+xp,iPosy+yp)->height+=iMod;				
			}
		}
			
		if(bSmooth)
			Smooth(iPosx-(iSize/2),iPosy-(iSize/2),iSize,iSize);				
	}
}

void HeightMap::DrawMask(int iPosX,int iPosy,int iMask,int iSize,int r,int g,int b,int a)
{
	if(iMask <= 0 || iMask >= m_kSets.size())
		return;
	
	
	m_pkTexMan->BindTexture(m_kSets[iMask].m_acMask,0);
	
	
	if(!m_pkTexMan->MakeTextureEditable())
		return;
	
	//get texture pos
	float xpos =(iPosX * HEIGHTMAP_SCALE) * (GetSize() / m_pkTexMan->GetImage()->w)  ;
	float ypos =(iPosy * HEIGHTMAP_SCALE) * (GetSize() / m_pkTexMan->GetImage()->h)  ;
		
	int size=iSize;

	for(float i=0;i<size;i+=0.5)
	{
		for(int z=0;z<360;z+=10)
		{
			int x = int(xpos + sin(z/degtorad)*i);
			int y = int(ypos + cos(z/degtorad)*i);
		
			Uint32 old = m_pkTexMan->GetPixel(x,y);
			
			Uint8 cr,cg,cb,ca;			
			int pr,pg,pb,pa;
			
			SDL_GetRGBA(old,  m_pkTexMan->GetImage()->format ,&cr,&cg,&cb,&ca);
					
			//grr hata Uint8
			pr=cr;
			pg=cg;			
			pb=cb;			
			pa=ca;			
			
			pr+=r;
			pg+=g;
			pb+=b;			
			pa+=a;
			
			//fula men nödvändiga =P
			if(pr >255)
				pr = 255;
			if(pg >255)
				pg = 255;
			if(pb >255)
				pb = 255;
			if(pa >255)
				pa = 255;

			if(pr <0)
				pr = 0;
			if(pg <0)
				pg = 0;
			if(pb <0)
				pb = 0;
			if(pa <0)
				pa = 0;

			m_pkTexMan->PsetRGBA(x,y,pr,pg,pb,pa);
		}
	
	}
	m_pkTexMan->SwapTexture();
}


