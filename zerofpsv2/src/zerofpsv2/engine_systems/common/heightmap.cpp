#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstring>
#include <cstdio>
#include "heightmap.h"
#include "../../render/frustum.h"
#include "../../render/render.h"
#include "../mad/mad_core.h"



void HM_Layer::Load(ZFVFile* pkFile)
{
	char szString[256];

	pkFile->Read(szString, 256,1);
	m_strName = szString;
	pkFile->Read(szString, 256,1);
	m_strTexture = szString;
	pkFile->Read(szString, 256,1);
	m_strDetailTexture = szString;
	pkFile->Read(szString, 256,1);
	m_strMask = szString;

	cout << "Load Layer: " << m_strName << ", " << m_strDetailTexture << "," << m_strMask << endl;
}

void HM_Layer::Save(ZFVFile* pkFile)
{
	char szString[256];

	strcpy(szString, m_strName.c_str());
	pkFile->Write(szString, 256,1);
	strcpy(szString, m_strTexture.c_str());
	pkFile->Write(szString, 256,1);
	strcpy(szString, m_strDetailTexture.c_str());
	pkFile->Write(szString, 256,1);
	strcpy(szString, m_strMask.c_str());
	pkFile->Write(szString, 256,1);
	cout << "Save Layer: " << m_strName << ", " << m_strDetailTexture << "," << m_strMask << endl;
}






// Low X Effects High X.
// Nothing Effects HighZ.

HeightMap::HeightMap() 
{
	verts					= NULL;
	m_iNumOfHMVertex	= 0;
	m_iID					= -1;
	m_bInverted			= false;

	m_pkTexMan	= static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));		
	m_pkBasicFS	= static_cast<ZFBasicFS*>(g_ZFObjSys.GetObjectPtr("ZFBasicFS"));		
//	Create(4);
}

HeightMap::~HeightMap() 
{
	if(verts)
		delete[] verts;
}

bool HeightMap::AllocHMMemory(int iSize)
{
	if(verts)
		delete[] verts;

	m_iNumOfHMVertex = (iSize) * iSize;
	verts = new HM_vert[m_iNumOfHMVertex];	
	return true;
}

void HeightMap::Create(int iTilesSide)
{
	m_iTilesSide   =  iTilesSide;
	m_iVertexSide  =  m_iTilesSide + 1;
	m_iHmScaleSize =	GetSize();
	SetPosition(Vector3::ZERO);

	AllocHMMemory(m_iVertexSide);
	Zero();
	//m_iHmSize		=	iTilesSide;

	// AddTestLayers
	Layer_Create(string("layer0"), string("../data/textures/tgrass.bmp"));
	Layer_Create(string("layer1"), string("../data/textures/tdirt.bmp"));
	Layer_Create(string("layer2"), string("../data/textures/trock.bmp"));
	Layer_Create(string("layer3"), string("../data/textures/tsand.bmp"));
}

void HeightMap::Zero() 
{
	for(int i=0;i < m_iNumOfHMVertex;i++) {
		verts[i].height	=	0;
		verts[i].normal.Set(0,1,0);
	}
}

bool HeightMap::IsAllZero()
{
	for(int z=0;z<m_iVertexSide;z++)
	{
		for(int x=0;x<m_iVertexSide;x++)
		{
			int iVertexIndex = z * m_iVertexSide + x;
			if(verts[iVertexIndex].height != 0)
				return false;
		}
	}

	return true;
}

void HeightMap::SetPosition(Vector3 kNewPos) 
{
	m_kPosition	= kNewPos;

	m_kCornerPos.Set(m_kPosition.x	-	m_iHmScaleSize/2, 
		m_kPosition.y,
		m_kPosition.z	-	m_iHmScaleSize/2);

}

/*
void HeightMap::CreateBlocks()
{
	int iBlockSize = 32;

	TerrainBlock kBlock;

	for(int z=0; z<m_iVertexSide; z += iBlockSize) {
		for(int x=0; x<m_iVertexSide; x += iBlockSize) {
			kBlock.kAABB_Min.Set( float(x*HEIGHTMAP_SCALE),	1,	float(z*HEIGHTMAP_SCALE));
			kBlock.kAABB_Max.Set(float((x + iBlockSize)*HEIGHTMAP_SCALE),1,float((z + iBlockSize)*HEIGHTMAP_SCALE));
			m_kTerrainBlocks.push_back(kBlock);
		}
	}
}*/


float HeightMap::Height(float x,float z) 
{
	x /= HEIGHTMAP_SCALE;
	z /= HEIGHTMAP_SCALE;
	
	x -= m_kPosition.x - m_iTilesSide/2;
	z -= m_kPosition.z - m_iTilesSide/2;

	if(x<0 || x>m_iTilesSide || z<0 || z>m_iTilesSide) 
		return 1;

	int lx = int(x);
	int lz = int(z);
	float ox = x - float(lx);
	float oz = z - float(lz);
	float bp, xp, zp;

	// are we on the over or under polygon in the tile gillar min fina engelska =)
	float ry = float((1.0 + ox*-1.0));

	if(oz>ry)	// over left 
	{ 
		bp=verts[(lz+1)*m_iVertexSide+(lx+1)].height;
		xp=verts[(lz+1)*m_iVertexSide+(lx)].height-bp;
		zp=verts[(lz)*m_iVertexSide+(lx+1)].height-bp;		
		ox = float(1.0-ox);
		oz = float(1.0-oz);
	}
	else			// under right
	{
		bp=verts[lz*m_iVertexSide+lx].height;
		xp=verts[(lz)*m_iVertexSide+(lx+1)].height-bp;
		zp=verts[(lz+1)*m_iVertexSide+(lx)].height-bp;				
	}	

	return m_kPosition.y + (bp+(xp*ox)+(zp*oz)) * HEIGHTMAP_SCALE;	
}


Vector3 HeightMap::Tilt(float x,float z)
{
	x/=HEIGHTMAP_SCALE;
	z/=HEIGHTMAP_SCALE;
	
	x-=m_kPosition.x-m_iTilesSide/2;
	z-=m_kPosition.z-m_iTilesSide/2;

	if(x<0 || x>m_iTilesSide || z<0 || z>m_iTilesSide) 
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
		v1=Vector3(1,(verts[(lz+1)*m_iTilesSide+(lx+1)].height)	-(verts[(lz+1)*m_iTilesSide+(lx)].height) ,0);
		v2=Vector3(0,(verts[(lz+1)*m_iTilesSide+(lx+1)].height)	-(verts[(lz)*m_iTilesSide+(lx+1)].height) ,1);	
		
		n1=v2.Cross(v1);			
	}else {
//		cout<<"DOWN"<<endl;		
		v1=Vector3(0,(verts[(lz+1)*m_iTilesSide+(lx)].height)	-(verts[(lz)*m_iTilesSide+(lx)].height) ,1);
		v2=Vector3(1,(verts[(lz)*m_iTilesSide+(lx+1)].height)	-(verts[(lz)*m_iTilesSide+(lx)].height) ,0);		
	
		n1=v1.Cross(v2);			
	}
	
	n1.Normalize();
	return n1;
}

bool	HeightMap::IsIndexOutOfMap(int iIndex)
{
	if(iIndex < 0)	
		return true;
	if(iIndex >= m_iTilesSide)
		return true;

	return false;
}



void HeightMap::GenerateNormals() 
{
	Vector3 med;
	Vector3 v1,v2,v3,n1,n2;

	int iZIndex, iXIndex;
	int iNumOfSides;
	for(int z = 0; z < m_iVertexSide; z++){
		for(int x = 0; x < m_iVertexSide; x++) {
			med = Vector3(0,0,0);  //reset medium vector
			iNumOfSides = 0;

			for(int q=-1;q<1;q++){
				for(int w=-1;w<1;w++){
					iZIndex = z + q;
					iXIndex = x + w;

					if(!IsIndexOutOfMap(iZIndex) && !IsIndexOutOfMap(iXIndex) &&
						!IsIndexOutOfMap(iZIndex+1) && !IsIndexOutOfMap(iXIndex+1)) {
						v1=Vector3(1,(verts[(z+q)*m_iVertexSide+(x+1+w)].height)-(verts[(z+q)*m_iVertexSide+(x+w)].height) ,0);
						v2=Vector3(1,(verts[(z+1+q)*m_iVertexSide+(x+1+w)].height)- (verts[(z+q)*m_iVertexSide+(x+w)].height),1);		
						v3=Vector3(0,(verts[(z+q+1)*m_iVertexSide+(x+w)].height)-(verts[(z+q)*m_iVertexSide+(x+w)].height) ,1);	
		
						n1=v2.Cross(v1);			
						n2=v3.Cross(v2);				
	//					n1.normalize();
	//					n2.normalize();
						}
					else {
						n1.Set(0,1,0);
						n2.Set(0,1,0);
	
						}	
		
				med=med+n1+n2;
				}	
			}

			med=med*0.125;	//insted of  division by 8 
			med.Normalize();
			verts[z*m_iVertexSide+x].normal=med;
		}
	}

/*	Vector3 med;
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
*/
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

	if(rx > m_iTilesSide)
		rx = m_iTilesSide;
		
	if(bz > m_iTilesSide)
		bz = m_iTilesSide;	
	
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
					v1=Vector3(1,(verts[(z+q)*m_iTilesSide+(x+1+w)].height)-(verts[(z+q)*m_iTilesSide+(x+w)].height) ,0);
					v2=Vector3(1,(verts[(z+1+q)*m_iTilesSide+(x+1+w)].height)- (verts[(z+q)*m_iTilesSide+(x+w)].height),1);		
					v3=Vector3(0,(verts[(z+q+1)*m_iTilesSide+(x+w)].height)-(verts[(z+q)*m_iTilesSide+(x+w)].height) ,1);	
	
					n1=v2.Cross(v1);			
					n2=v3.Cross(v2);				
//					n1.normalize();
//					n2.normalize();
	
					med=med+n1+n2;
				}	
			}
			med=med*0.125;	//insted of  division by 8 
			med.Normalize();
			verts[z*m_iTilesSide+x].normal=med;
		}
	}
}


bool HeightMap::Load(const char* acFile) 
{
	int i;

// Load Vertex Data
	string hmfile = acFile;
	hmfile += ".hm";
	
	HM_fileheader k_Fh;
	
	ZFVFile savefile;
	if(!savefile.Open(hmfile.c_str(),0,false))
	{
		cout<<"Could not Load heightmap"<<endl;
		return false;
	}

	// Read File Header
	savefile.Read((void*)&k_Fh, sizeof(HM_fileheader),1);
	
	// Alloc Memory
	m_iVertexSide		= k_Fh.m_iHmSize;
	m_iTilesSide      = m_iVertexSide - 1;
	m_iHmScaleSize		=	GetSize();

	AllocHMMemory(m_iVertexSide);
	
	// Read Layer Data.
	HM_Layer kLayer;
	m_kLayer.clear();
	for(i=0; i<k_Fh.m_iNumOfLayers; i++) {
		kLayer.Load( &savefile );
		m_kLayer.push_back(kLayer);
		}

	// Read VertexData
	savefile.Read((void*)&verts[0],sizeof(HM_vert), m_iVertexSide*m_iVertexSide);
	savefile.Close();
	
// Load Layers.	
	bool exist = false;		
	i=2;		


//	ClearSet();
//	AddSet("../data/textures/nodetail1.bmp","../data/textures/detail1.bmp","FEL");
	
/*	cout << "Loading Layers: ";
	do
	{
		string file=acFile;
		file += "mask";
		char nr[5] = "    ";
		IntToChar(nr,i);
		file+=nr;
		file+=".tga";		
			
		exist = m_pkBasicFS->FileExist(file.c_str());
		
		if(exist)
		{
			string nodetail = "../data/textures/nodetail";
			string detail = "../data/textures/detail";			
		
			nodetail+=nr;
			detail+=nr;
			
			nodetail+=".bmp";
			detail+=".bmp";
	
			cout << file.c_str() << ", ";
			//Layer_Create();
			//AddSet(nodetail.c_str(),detail.c_str(),file.c_str());
		}
		i++;
		
	} while(exist);
	
	cout << endl;*/


	return true;
}


bool HeightMap::Save(const char* acFile) 
{
// Save Vertex Data
	string hmfile = acFile;
	hmfile += ".hm";
	
	// Setup FileHeader
	HM_fileheader k_Fh;
	k_Fh.m_iHmSize			= m_iVertexSide;
	k_Fh.m_iNumOfLayers	= m_kLayer.size();

	ZFVFile savefile;
	if(!savefile.Open(hmfile.c_str(),0,true))
	{
		cout<<"Could not save heightmap"<<endl;
		return false;
	}
	
	// Write FileHeader.
	savefile.Write((void*)&k_Fh, sizeof(HM_fileheader),1);

	// Write Layer Table
	for(int i=0; i<m_kLayer.size(); i++) {
		m_kLayer[i].Save( &savefile );
		}

	// Write VertexData
	savefile.Write((void*)&verts[0],sizeof(HM_vert), m_iVertexSide*m_iVertexSide);
	savefile.Close();
	
	for(int i=0; i<m_kLayer.size(); i++) {
		string file=m_kLayer[i].m_strMask;
		ZFResourceHandle m_kConsoleText;
		m_kConsoleText.SetRes(m_kLayer[i].m_strMask);	
		ResTexture* pkTexture = static_cast<ResTexture*>(m_kConsoleText.GetResourcePtr());
		m_pkTexMan->BindTexture(pkTexture->m_iTextureID);
		m_pkTexMan->SaveTexture(file.c_str(),0);
		}

// Save Layers
/*	DOO
	if(m_kSets.size() > 1)
	{
		for(unsigned int i=1;i<m_kSets.size();i++)
		{
			string file=acFile;
			file += "mask";
			char nr[5] = "    ";
			IntToChar(nr,i+1);
			file+=nr;
			file+=".tga";
			
			ZFResourceHandle m_kConsoleText;
			m_kConsoleText.SetRes(m_kSets[i].m_acMask);	
			ResTexture* pkTexture = static_cast<ResTexture*>(m_kConsoleText.GetResourcePtr());
			m_pkTexMan->BindTexture(pkTexture->m_iTextureID);
			m_pkTexMan->SaveTexture(file.c_str(),0);
		}
	}
*/
	
	return true;
}


void HeightMap::Random() 
{
	int height=250;
	int peaks=25;
	int smooth=1;

	srand(time(0));
	
	int i;

	int x,y;
	for(i=0;i<peaks;i++) {
		x=rand()%(m_iTilesSide-4)+2;	
		y=rand()%(m_iTilesSide-4)+2;	
		verts[y*m_iTilesSide+x].height = float((rand()%height)/10.0);
	}
	
	for(i=0;i<peaks/4;i++) {
		x=rand()%(m_iTilesSide-4)+2;	
		y=rand()%(m_iTilesSide-4)+2;	
		verts[y*m_iTilesSide+x].height = float((rand()%height*4)/10.0);
	}
	
	float med;
	for(int l=0;l<smooth;l++) {
		for(int y=1;y<m_iTilesSide-1;y++) {
			for(int x=1;x<m_iTilesSide-1;x++) {
				med=0;
				for(int q=-1;q<2;q++)
					for(int w=-1;w<2;w++){
						if(q==0 && w==0) {							

						} else {
							med+=verts[(y+q)*m_iTilesSide+(x+w)].height;							
						}
					}
				med=med/8;
				
				verts[y*m_iTilesSide+x].height=med;
			}
		}
	}
}


HM_vert* HeightMap::GetVert(int x,int z) 
{
	return &verts[ z*m_iVertexSide +x];
}



/*
bool HeightMap::LoadImageHmap(const char* acFile) 
{
	int smooth=1;

	Image kImage;
	Uint8 data;
//	Uint32 pixel;
	
	bool bSuccess = kImage.load(acFile);
	if(!bSuccess)
	{
		cout<<"Failed to load heightmap "<<acFile<<endl<<endl<<endl;
		return false;
	}

	m_iHmSize=kImage.m_iWidth;
//	cout<<"Image Size is:"<<m_iHmSize<<endl;
	
	if(verts)
		delete[] verts;

	verts=new HM_vert[(m_iHmSize)*m_iHmSize];	

	for(int y=0;y<m_iHmSize;y++)
		for(int x=0;x<m_iHmSize;x++)
		{							
			color_rgba color;
			kImage.get_pixel(x,y,color); 
		
			data = color.r;
			verts[y*m_iHmSize+x].height = (float) (data/3);		
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

	return false;
}*/	


Uint32 HeightMap::GetPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    // Here p is the address to the pixel we want to retrieve 
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
        return 0;       // shouldn't happen, but avoids warnings
    }
}



void HeightMap::RebuildVertex()
{
	for(int i=0;i<(m_iTilesSide)*m_iTilesSide;i++){
//		m_pkVertex[i].Set(0,0,0);
	}

}

void HeightMap::GetMapXZ(float& x,float& z)
{
	x /= HEIGHTMAP_SCALE;
	z /= HEIGHTMAP_SCALE;
	
	x -= m_kPosition.x - m_iTilesSide/2;
	z -= m_kPosition.z - m_iTilesSide/2;

}

void HeightMap::Smooth(int iStartx,int iStartz,int iWidth,int iHeight)
{
	cout << "Smooth " << iStartx << "," << iStartz << endl;
	cout << "Smooth size " << iWidth << "," << iHeight << endl;


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

	if(rx > m_iTilesSide)
		rx = m_iTilesSide;
		
	if(bz > m_iTilesSide)
		bz = m_iTilesSide;
		

	for(int z=tz; z<bz-1; z++) {
		for(int x=lx; x<rx-1; x++) {
			float med=0;
			for(int q=-1;q<2;q++)
				for(int w=-1;w<2;w++){
					if(q==0 && w==0) 
					{							
					} else 
					{
						med+=verts[(z+q)*m_iVertexSide+(x+w)].height;							
					}
				}
			med=med/8;
			
			verts[z*m_iVertexSide+x].height = med;
		}
	}
	
	GenerateNormals(iStartx,iStartz,iWidth,iHeight);
}

void HeightMap::Flatten(vector<HMSelectVertex> kSelected, Vector3 kPos)
{	
	float fHeight = Height(kPos.x,kPos.z);

	for(int i=0; i<kSelected.size(); i++) {
		verts[kSelected[i].m_iIndex].height = fHeight;
		}

}


vector<HMSelectVertex> HeightMap::GetSelection(Vector3 kCenter, float fInRadius, float fOutRadius)
{
	vector<HMSelectVertex> kSelection;
	HMSelectVertex kSel;
	kSel.m_fValue = 1.0;

	float fInOutDiff = fOutRadius - fInRadius;

	int iMinX, iMinZ, iMaxX, iMaxZ;		// Min/Max X/Y Index of vertices that can be selected.

	iMinX = 0;
	iMinZ = 0;
	iMaxX = m_iTilesSide + 1;
	iMaxZ = m_iTilesSide + 1;
	float fLen;
	float fLen2;

//	int x,y;

	for(int z = iMinZ ; z < iMaxZ; z++) {
		for(int x = iMinX ; x < iMaxX ; x++) {	
			// Calc World Pos of the vertex.
			int iVertexIndex = z * (m_iTilesSide + 1) + x;
			kSel.m_iIndex = iVertexIndex;
			Vector3 kWorld = Vector3(x * HEIGHTMAP_SCALE, verts[iVertexIndex].height*HEIGHTMAP_SCALE ,z * HEIGHTMAP_SCALE);
			kWorld += m_kCornerPos;
			//kVertex += (CamPos + kMap->m_kCornerPos);
	
			Vector3 kDiff = kWorld - kCenter;
			kDiff.y = 0;

			fLen = kDiff.Length();

			// If inside inner circle set value to max.
			if(fLen < fInRadius) {
				kSel.m_fValue = 1.0;
				kSelection.push_back(kSel);
				}
			if (fLen >= fInRadius && fLen <= fOutRadius) {
				fLen2 = fLen - fInRadius;

				kSel.m_fValue = 1.0 - (fLen2 / fInOutDiff);
				//cout << kSel.m_fValue << ",";
				kSelection.push_back(kSel);
				}
			}
	}

//	cout << "Num Of Vertices Selected: " << kSelection.size();
//	cout << endl;

	return kSelection;
}

void HeightMap::Raise(vector<HMSelectVertex> kSelected, float fSize)
{
	for(int i=0; i<kSelected.size(); i++) {
		verts[kSelected[i].m_iIndex].height += fSize * kSelected[i].m_fValue;
		}

	GenerateNormals();
}

float HeightMap::GetBrushSizeInAlphaUVSpace(float fSize)
{
	float fTextelsInWorld = 128.0 / GetSize();
	return fSize * fTextelsInWorld;
}

void HeightMap::DrawMask(Vector3 kPos,int iMask,float fSize,int r,int g,int b,int a)
{
	kPos -= m_kCornerPos;

	if(iMask <= 0 || iMask >= int(m_kLayer.size()))
		return;
		
//	ZFResourceHandle m_kConsoleText;
//	m_kConsoleText.SetRes(m_kLayer[iMask].m_strMask.c_str());	

//	ResTexture* pkTexture = static_cast<ResTexture*>(m_kConsoleText.GetResourcePtr());
//	m_pkTexMan->BindTexture(pkTexture->m_iTextureID);

	m_pkTexMan->BindTexture(m_kLayer[iMask].m_strMask.c_str(),0);
	
	if(!m_pkTexMan->MakeTextureEditable()) {
		cout << "Failed to edit texture" << endl;
		return;
		}
	
	//get texture pos
	float fSizeW = ((float)GetSize() / (float)m_pkTexMan->GetImage()->m_iWidth);
	float xpos = (float) ((kPos.x * HEIGHTMAP_SCALE) / (float)GetSize())  ;
	float ypos = (float) ((kPos.z * HEIGHTMAP_SCALE) / (float)GetSize())  ;
	
	xpos *= (float)m_pkTexMan->GetImage()->m_iWidth;
	ypos *= (float)m_pkTexMan->GetImage()->m_iHeight;
	
	float fRealSize = GetBrushSizeInAlphaUVSpace( fSize );	// * (float)m_pkTexMan->GetImage()->w;
	int size=fRealSize;
	
	cout << "Brush size: " << fRealSize << endl;
	cout << "Img Size: " << m_pkTexMan->GetImage()->m_iWidth << "," << m_pkTexMan->GetImage()->m_iHeight;

	for(float i=0;i<fRealSize;i+=0.5)
	{
		for(int z=0;z<360;z+=1)
		{
			int x = int(xpos + sin(z/degtorad)*i);
			int y = int(ypos + cos(z/degtorad)*i);
		
			if(x<0 || x >= m_pkTexMan->GetImage()->m_iWidth 
				|| y <0 || y>=m_pkTexMan->GetImage()->m_iHeight)	
				continue;		
			
			color_rgba kColor = m_pkTexMan->GetPixel(x,y);
			
			int pr,pg,pb,pa;
			
			/*SDL_GetRGBA(old,  m_pkTexMan->GetImage()->format ,&cr,&cg,&cb,&ca);
					
			//grr hata Uint8*/	
			pr=kColor.r;
			pg=kColor.g;			
			pb=kColor.b;			
			pa=kColor.a;		
			
			pr		=	r;
			pg		=	g;
			pb		=	b;			
			pa		+=	a;
			
			//fula men nödvändiga =P
			if(pr >255)
				pr = 255;
			if(pg	 >255)
				pg	 = 255;
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
	
	if(!m_pkTexMan->SwapTexture()) {
		cout << "Failed to swap back texture" << endl;
		}

//	_pkTexMan->BindTexture(pkTexture->m_iTextureID);
}


HM_vert* HeightMap::LinePick(Vector3 kPos,Vector3 kDir,Vector3 kCenterPos,int iWidth,Vector3& kHitPos)
{
	float minx = kCenterPos.x - iWidth/2; 
	float maxx = kCenterPos.x + iWidth/2; 
	float minz = kCenterPos.z - iWidth/2; 
	float maxz = kCenterPos.z + iWidth/2; 
	
	kPos+=Vector3(1,0,1)*HEIGHTMAP_SCALE;

	Vector3 kPos2 = kPos + (kDir * 500);
	
	//convert to map cordinats
	GetMapXZ(minx,minz);	
	GetMapXZ(maxx,maxz);	
	GetMapXZ(kPos.x,kPos.z);
	GetMapXZ(kPos2.x,kPos2.z);
		
	
	if(minx < 0)
		minx = 0;
	
	if(minz < 0)
		minz = 0;
		
	if(maxx > m_iTilesSide)
		maxx = (float) m_iTilesSide;
		
	if(maxz > m_iTilesSide)
		maxz = (float) m_iTilesSide;
	
		
	float cdist=99999999;	
	HM_vert* closest=NULL;
		
		
	for(float x = minx;x<maxx-1;x++)
	{
		for(float z = minz;z<maxz-1;z++)
		{			
			Vector3 kColPos;
			Vector3 verts[3];
			
			//over left polygon
			verts[0]=Vector3(x,0,z);
			verts[1]=Vector3(x,0,z+1);
			verts[2]=Vector3(x+1,0,z);
			verts[0].y = ((GetVert((int)verts[0].x,(int)verts[0].z)->height)*HEIGHTMAP_SCALE)+m_kPosition.y;
			verts[1].y = ((GetVert((int)verts[1].x,(int)verts[1].z)->height)*HEIGHTMAP_SCALE)+m_kPosition.y;			
			verts[2].y = ((GetVert((int)verts[2].x,(int)verts[2].z)->height)*HEIGHTMAP_SCALE)+m_kPosition.y;			
						
			if(LineVSPolygon(verts,kPos,kPos2,kColPos))
			{
				float dist = (kPos - kColPos).Length();
				if(dist < cdist)
				{
					cdist=dist;
					closest=GetVert((int)x,(int)z);
					kHitPos = kColPos;
				}
			}
			
			//down right polygon
			verts[0]=Vector3(x+1,0,z+1);
			verts[1]=Vector3(x+1,0,z);
			verts[2]=Vector3(x,0,z+1);
			verts[0].y = ((GetVert((int)verts[0].x,(int)verts[0].z)->height)*HEIGHTMAP_SCALE)+m_kPosition.y;
			verts[1].y = ((GetVert((int)verts[1].x,(int)verts[1].z)->height)*HEIGHTMAP_SCALE)+m_kPosition.y;		
			verts[2].y = ((GetVert((int)verts[2].x,(int)verts[2].z)->height)*HEIGHTMAP_SCALE)+m_kPosition.y;		
			
			if(LineVSPolygon(verts,kPos,kPos2,kColPos))
			{
				float dist = (kPos - kColPos).Length();
				if(dist < cdist)
				{
					cdist=dist;
					closest=GetVert((int)x,(int)z);
					kHitPos = kColPos;					
				}
			}	
		}
	}
	
	if(closest != NULL)
	{
		kHitPos = (kHitPos - Vector3((float)(m_iTilesSide/2),0,(float)(m_iTilesSide/2))); 
		kHitPos.x *= HEIGHTMAP_SCALE;
		kHitPos.z *= HEIGHTMAP_SCALE;		
		kHitPos-=Vector3(1,0,1)*HEIGHTMAP_SCALE;
		
		//cout<<"HM: "<<kHitPos.x<< " "<<kHitPos.z<<endl;
	}

	return closest;
}

bool HeightMap::LineVSPolygon(Vector3* pkVerts,Vector3 kPos1,Vector3 kPos2,Vector3& kColPos)
{
	Plane P;
	
	Vector3 V1 = pkVerts[1] - pkVerts[0];
	Vector3 V2 = pkVerts[2] - pkVerts[0];		
	Vector3 Normal= V1.Cross(V2);	
	
	if(Normal.Length() == 0)
	{
		return false;
	}

	Normal.Normalize();
	P.m_fD = -Normal.Dot(pkVerts[0]);	
	P.m_kNormal = Normal;	


	if(P.LineTest(kPos1, kPos2 ,&kColPos))
	{
		if(TestSides(pkVerts,&Normal,kColPos))		
		{
			return true;
		}
	}

	return false;
}

bool HeightMap::TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos)
{
	Plane side[6];
	
	Vector3 V1 = kVerts[1] - kVerts[0];
	Vector3 V2 = kVerts[2] - kVerts[1];	
	Vector3 V3 = kVerts[0] - kVerts[2];	
	
	side[0].m_kNormal = pkNormal->Cross(V1).Unit();
	side[1].m_kNormal = pkNormal->Cross(V2).Unit();	
	side[2].m_kNormal = pkNormal->Cross(V3).Unit();

	side[3].m_kNormal = (side[0].m_kNormal + side[2].m_kNormal).Unit();
	side[4].m_kNormal = (side[0].m_kNormal + side[1].m_kNormal).Unit();
	side[5].m_kNormal = (side[1].m_kNormal + side[2].m_kNormal).Unit();


	side[0].m_fD = -side[0].m_kNormal.Dot(kVerts[0]);
	side[1].m_fD = -side[1].m_kNormal.Dot(kVerts[1]);	
	side[2].m_fD = -side[2].m_kNormal.Dot(kVerts[2]);	

	side[3].m_fD = -side[3].m_kNormal.Dot(kVerts[0]);
	side[4].m_fD = -side[4].m_kNormal.Dot(kVerts[1]);	
	side[5].m_fD = -side[5].m_kNormal.Dot(kVerts[2]);	
	
	
	bool inside = true;
	
	for(int i=0;i<6;i++)
	{
		if(!side[i].PointInside(kPos)){
			inside=false;
		}
	}
		
	return inside;	
}

float HeightMap::GetAlpha(float x,float y,int iTexture)
{
/*	DOO
	//return -1 if outside heightmap
	if(x <0 &&
		x >=m_iTilesSide &&
		y <0 &&
		y >=m_iTilesSide)
	{
		return -1;
	}

	//return -1 if texture does not extist
	if(iTexture >= int(m_kSets.size()))
		return -1;
	
	//texture 0 dont have any alpha mask, therefor alpha is always 1
	if(iTexture == 0)
		return 1;
			
	//bind mask
	//m_pkTexMan->BindTexture(m_kSets[iTexture].m_acMask,0);
		
	ZFResourceHandle m_kConsoleText;
	m_kConsoleText.SetRes(m_kSets[iTexture].m_acMask);			
	ResTexture* pkTexture = static_cast<ResTexture*>(m_kConsoleText.GetResourcePtr());
	m_pkTexMan->BindTexture(pkTexture->m_iTextureID);
		
		
	float dw = (float) (m_pkTexMan->GetImage()->w / m_iTilesSide);
	float dh = (float) (m_pkTexMan->GetImage()->h / m_iTilesSide);	
	
	//convert to texture cordinats
	x*=dw;
	y*=dh;
	
	//get color
	Uint32 color = m_pkTexMan->GetPixel(int(x),int(y));
	
	Uint8 r,g,b,a;
		
	//get alpha value
	SDL_GetRGBA(color,  m_pkTexMan->GetImage()->format ,&r,&g,&b,&a);
	
	return (a/255.0f);*/

	return 0;
}

int HeightMap::GetMostVisibleTexture(float x,float y)
{
/*	DOO
	//return -1 if outside heightmap
	if(x <0 &&
		x >=m_iTilesSide &&
		y <0 &&
		y >=m_iTilesSide)
	{
		return -1;
	}	


	int t=-1;
	float v=0;
	
	for(unsigned int i=0;i<m_kSets.size();i++)
	{
		float a=GetAlpha(x,y,i);
		
		if(a>0.5)
		{
			v=a;
			t=i;
		}	
	}
	return t;

	*/
		
	return -1;
}



Point HeightMap::GetSqrFromPos(Vector3 pos)
{
	int iSquareX = int(m_iTilesSide/2+ceil(pos.x / HEIGHTMAP_SCALE));
	int iSquareY = int(m_iTilesSide/2+ceil(pos.z / HEIGHTMAP_SCALE));

	return Point(iSquareX,iSquareY);
}

Vector3 HeightMap::GetPosFromSqr(Point square)
{
	float x = (float) (-(m_iTilesSide/2)*HEIGHTMAP_SCALE + square.x*HEIGHTMAP_SCALE);
	float z = (float) (-(m_iTilesSide/2)*HEIGHTMAP_SCALE + square.y*HEIGHTMAP_SCALE);

	x -= HEIGHTMAP_SCALE/2;	// Translate to center 
	z -= HEIGHTMAP_SCALE/2;	// of square.*/

	float y = Height(x,z);

	return Vector3(x,y,z);
}

void HeightMap::GetCollData(vector<Mad_Face>* pkFace,vector<Vector3>* pkVertex , vector<Vector3>* pkNormal)
{
	int iIndex = 0;
	Mad_Face kFace;

	for(int z=0; z<m_iTilesSide; z++) {
		for(int x=0; x<m_iTilesSide; x++) {
			pkVertex->push_back( Vector3((float)x, verts[z*m_iTilesSide+x].height, (float)z) );				kFace.iIndex[0] = iIndex++;		
			pkVertex->push_back( Vector3((float)x + 1, verts[(z+1)*m_iTilesSide+(x+1)].height, (float)z + 1.0f) );		kFace.iIndex[1] = iIndex++;		
			pkVertex->push_back( Vector3((float)x + 1, verts[(z)*m_iTilesSide+(x+1)].height, (float)z) );			kFace.iIndex[2] = iIndex++;	
			pkFace->push_back(kFace);

			pkVertex->push_back( Vector3((float)x, verts[z*m_iTilesSide+x].height, (float)z) );				kFace.iIndex[0] = iIndex++;		
			pkVertex->push_back( Vector3((float)x , verts[(z+1)*m_iTilesSide+x].height, (float)z + 1.0f) );			kFace.iIndex[1] = iIndex++;		
			pkVertex->push_back( Vector3((float)x + 1, verts[(z+1)*m_iTilesSide+(x+1)].height, (float)z + 1.0f) );			kFace.iIndex[2] = iIndex++;	
			pkFace->push_back(kFace);
			}
		}

}

bool HeightMap::Layer_Create(string strName, string strTexture)
{
	HM_Layer kLayer;

	kLayer.m_strName				= strName;
	kLayer.m_strTexture			= strTexture;
	kLayer.m_strDetailTexture	= strTexture;

	char szMaskName[256];
	sprintf(szMaskName, "a-mask%d_%d.tga", m_iID, m_kLayer.size());
	
	kLayer.m_strMask				= string(szMaskName);
		
	Image kImg;
	kImg.CreateEmpty(128,128);
	kImg.fill(0,0,128,128,0,0,0,0);
	kImg.Save(szMaskName, true);

	//kLayer.m_kMaskHandle.SetRes(kLayer.m_strMask.c_str());	

	m_kLayer.push_back(kLayer);

	return true;
}

bool HeightMap::Layer_Delete(string strName)
{
/*	for(int i=0; i<m_kLayer.size(); i++) {
		if(m_kLayer[i].m_strName == strName) {
			m_kLayer.erase(m_kLayer[i]);
			return true;
			}

		}*/
	
	return false;
}

bool HeightMap::Layer_Clone(string strNameFrom, string strName)
{

	
	return true;
}


vector<string>	HeightMap::Layers_GetNames()
{
	vector<string>	strName;
	for(int i=0; i<m_kLayer.size(); i++)
		strName.push_back( m_kLayer[i].m_strName );

	return strName;

}





/*bool HeightMap::LoadImageHmap(const char* acFile) {
	int smooth=1;

	SDL_Surface *image;
	Uint8 data;
	Uint32 pixel;
	
	image=IMG_Load(acFile);
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
}*/

/*void HeightMap::Raise(int iPosx,int iPosy,int iMod,int iSize,bool bSmooth)
{
	cout << "Raise: " << iPosx << "," << iPosy << endl;

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
}*/

/*
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
}*/

/*void HeightMap::RunCommand(int cmdid, const CmdArgument* kCommand)
{

}*/

/*
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
	//m_kSets.clear();
}
*/