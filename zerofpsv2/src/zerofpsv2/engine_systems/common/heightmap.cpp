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







HeightMap::HeightMap() 
{
	m_pkTexMan			= static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));		

	verts					= NULL;
	m_pkTileFlags		= NULL;
	Clear();
}

HeightMap::~HeightMap() 
{
	if(verts)
		delete[] verts;
	if(m_pkTileFlags)
		delete[] m_pkTileFlags;
}

void HeightMap::Clear()
{
	if(verts)
		delete[] verts;
	if(m_pkTileFlags)
		delete[] m_pkTileFlags;

	verts					= NULL;
	m_pkTileFlags		= NULL;
	m_iNumOfHMVertex	= 0;
	m_iID					= -1;
	m_bInverted			= false;
	m_fTileSize			= 8;
}


bool HeightMap::AllocHMMemory(int iSize)
{
	if(verts)
		delete[] verts;
	if(m_pkTileFlags)
		delete[] m_pkTileFlags;

	m_iNumOfHMVertex = iSize * iSize;
	verts = new HM_vert[m_iNumOfHMVertex];	
	m_pkTileFlags = new unsigned char [m_iTilesSide*m_iTilesSide];
	
	
	return true;
}

void HeightMap::Create(int iTilesSide)
{
	m_iTilesSide   =  iTilesSide / m_fTileSize;
	m_iVertexSide  =  m_iTilesSide + 1;
	SetPosition(Vector3::ZERO);
	//m_iHmScaleSize =	GetSize();

	AllocHMMemory(m_iVertexSide);
	Zero();

	// AddTestLayers
	Layer_Create(string("layer0"), string("data/textures/trock.bmp"));
	Layer_Create(string("layer1"), string("data/textures/tdirt.bmp"));
	Layer_Create(string("layer2"), string("data/textures/tgrass.bmp"));
	Layer_Create(string("layer3"), string("data/textures/tsand.bmp"));
}

void HeightMap::Zero() 
{
	for(int i=0;i < m_iNumOfHMVertex;i++) 
	{
		verts[i].height	=	0;
		verts[i].normal.Set(0,1,0);
	}

	memset(m_pkTileFlags,0xFF, m_iTilesSide*m_iTilesSide);
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

	int iHmapSideSize = GetSize();

	m_kCornerOffset.Set(-iHmapSideSize/2, 0, -iHmapSideSize/2);

	m_kCornerPos.Set(m_kPosition.x	-	iHmapSideSize/2, 
		m_kPosition.y,
		m_kPosition.z	-	iHmapSideSize/2);
}

int HeightMap::GetTopLowTriangle(Vector3 kPos)
{
	kPos.x /= m_fTileSize;
	kPos.z /= m_fTileSize;
	
//	kPos.x -= m_kPosition.x - m_iTilesSide/2;
//	kPos.z -= m_kPosition.z - m_iTilesSide/2;

	if(kPos.x<0 || kPos.x>m_iTilesSide || kPos.z<0 || kPos.z>m_iTilesSide) 
		return int(m_kPosition.y);

	int lx = int(kPos.x);
	int lz = int(kPos.z);
	float ox = kPos.x - float(lx);
	float oz = kPos.z - float(lz);
//	float bp, xp, zp;

	// are we on the over or under polygon in the tile gillar min fina engelska =)
	float ry = float((1.0 + ox*-1.0));

	if(oz>ry)	// over left 
		return HM_FLAGVISIBLE1;
	else 
		return HM_FLAGVISIBLE2;

//	return 0;
}

Vector3 HeightMap::WorldToMap(Vector3 kVec)
{
	kVec.x -= m_kPosition.x;	
	kVec.z -= m_kPosition.z;

	kVec.x -= m_kCornerOffset.x;
	kVec.z -= m_kCornerOffset.z;
	return kVec;
}

Vector3 HeightMap::MapToLocal(Vector3 kVec)
{
	kVec.x += m_kCornerOffset.x;
	kVec.z += m_kCornerOffset.z;

	kVec.x += m_kPosition.x;	
	kVec.z += m_kPosition.z;
	return kVec;
}


/**	\brief	Returns height at one position in Hmap.

	Takes the x,z world coo and returns the world y position of the hmap or as if had a height of 0.0 if the position
	is outside the hmap.
*/
float HeightMap::Height(float x,float z) 
{
	x -= m_kPosition.x;	//m_iTilesSide/2;
	z -= m_kPosition.z;	//m_iTilesSide/2;

	x -= m_kCornerOffset.x;
	z -= m_kCornerOffset.z;

	x /= m_fTileSize;
	z /= m_fTileSize;

	if(x<0 || x>m_iTilesSide || z<0 || z>m_iTilesSide) 
	{
		return m_kPosition.y;
	}

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

	return m_kPosition.y + (bp+(xp*ox)+(zp*oz));	// * m_fTileSize;	
}

/**	\brief	Returns normal at one position in Hmap.

	Takes the x,z world coo and returns the normal of that position. Return (0,1,0) if position is outside hm.
*/
Vector3 HeightMap::Tilt(float x,float z)
{
	x/=m_fTileSize;
	z/=m_fTileSize;
	
	x-=m_kPosition.x-m_iTilesSide/2;
	z-=m_kPosition.z-m_iTilesSide/2;

	if(x<0 || x>m_iTilesSide || z<0 || z>m_iTilesSide) 
		return Vector3(0,1,0);
		
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

/**	\brief	Returns true if the given index (index for hmvertex) is outside the valid range.
*/
bool	HeightMap::IsIndexOutOfMap(int iIndex)
{
	if(iIndex < 0)	
		return true;
	if(iIndex >= m_iTilesSide)
		return true;

	return false;
}

/**	\brief	Generates normals for the whole Hm.
*/
void HeightMap::GenerateNormals() 
{
	Vector3 med;
	Vector3 v1,v2,v3,n1,n2;

	int iZIndex, iXIndex;
	int iNumOfSides;
	for(int z = 0; z < m_iVertexSide; z++)	{
		for(int x = 0; x < m_iVertexSide; x++)	{
			med = Vector3::ZERO;		//reset medium vector
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
						//	n1.normalize();
						//	n2.normalize();
						}
					else {
						n1.Set(0,1,0);
						n2.Set(0,1,0);
						}	
		
				med=med+n1+n2;
				}	
			}

			med=med*0.125;				//insted of  division by 8 
			med.Normalize();
			verts[z*m_iVertexSide+x].normal=med;
		}
	}
}

/**	\brief	Generates normals for a selected part of the Hm.
*/
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
	
	for(int z=tz;z<bz-1;z++){
		for(int x=lx;x<rx-1;x++) {
			med = Vector3::ZERO;				// Reset medium vector
			for(int q=-1;q<1;q++){
				for(int w=-1;w<1;w++){
					v1=Vector3(1,(verts[(z+q)*m_iTilesSide+(x+1+w)].height)-(verts[(z+q)*m_iTilesSide+(x+w)].height) ,0);
					v2=Vector3(1,(verts[(z+1+q)*m_iTilesSide+(x+1+w)].height)- (verts[(z+q)*m_iTilesSide+(x+w)].height),1);		
					v3=Vector3(0,(verts[(z+q+1)*m_iTilesSide+(x+w)].height)-(verts[(z+q)*m_iTilesSide+(x+w)].height) ,1);	
	
					n1=v2.Cross(v1);			
					n2=v3.Cross(v2);				
					//	n1.normalize();
					//	n2.normalize();
					med=med+n1+n2;
				}	
			}
			med=med*0.125;					//insted of  division by 8 
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
	m_fTileSize       = k_Fh.m_fTileSize;
	m_bInverted       = k_Fh.m_bInverted;
	//m_iHmScaleSize		=	GetSize();

	AllocHMMemory(m_iVertexSide);
	
	// Read Layer Data.
	HM_Layer kLayer;
	m_kLayer.clear();
	for(i=0; i<k_Fh.m_iNumOfLayers; i++) {
		kLayer.Load( &savefile );
		m_kLayer.push_back(kLayer);
		}

	//read tileflags
	savefile.Read((void*)m_pkTileFlags,sizeof(unsigned char), m_iTilesSide*m_iTilesSide);			

	// Read VertexData
	savefile.Read((void*)&verts[0],sizeof(HM_vert), m_iVertexSide*m_iVertexSide);
	savefile.Close();
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
	k_Fh.m_fTileSize = m_fTileSize;
	k_Fh.m_bInverted = m_bInverted;

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
	
	
	//write tileflags
	savefile.Write((void*)m_pkTileFlags,sizeof(unsigned char), m_iTilesSide*m_iTilesSide);		

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

/**	\brief	Returns ptr to hm vertex at index (x,z).
*/
HM_vert* HeightMap::GetVert(int x,int z) 
{
	return &verts[ z*m_iVertexSide + x];
}



void HeightMap::GetMapXZ(float& x,float& z)
{
	x /= m_fTileSize;
	z /= m_fTileSize;
	
	x -= m_kPosition.x - m_iTilesSide/2;
	z -= m_kPosition.z - m_iTilesSide/2;

}

//void HeightMap::Smooth(int iStartx,int iStartz,int iWidth,int iHeight)
void HeightMap::Smooth(vector<HMSelectVertex> kSelected)
{
	int iX, iZ;
	int iIndex;


	for(int i=0; i<kSelected.size(); i++) {
		iIndex = kSelected[i].m_iIndex;

		iX = iIndex % (m_iTilesSide + 1);
		iZ = iIndex / (m_iTilesSide + 1);

		if(iX <= 0)					continue;
		if(iX >= m_iTilesSide)	continue;
		if(iZ <= 0)					continue;
		if(iZ >= m_iTilesSide)	continue;
			
		float med=0;
		for(int q=-1;q<2;q++)
			for(int w=-1;w<2;w++){
				if(q==0 && w==0) 
				{							
				} else 
				{
					med+=verts[(iZ+q)*m_iVertexSide+(iX+w)].height;							
				}
			}
		med=med/8;
		verts[kSelected[i].m_iIndex].height = med;
		}

	GenerateNormals();

//			int iVertexIndex = z * (m_iTilesSide + 1) + x;

/*	cout << "Smooth " << iStartx << "," << iStartz << endl;
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
	
	GenerateNormals(iStartx,iStartz,iWidth,iHeight);*/
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
			Vector3 kWorld = Vector3(x * m_fTileSize, verts[iVertexIndex].height ,z * m_fTileSize);	//*m_fTileSize
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
		
//	m_pkTexMan->BindTexture(m_kLayer[iMask].m_strMask.c_str(),0);
	
	if(!m_pkTexMan->EditStart(m_kLayer[iMask].m_strMask.c_str())) {
		cout << "Failed to edit texture" << endl;
		return;
		}
	
	//get texture pos
	float fSizeW = ((float)GetSize() / (float)m_pkTexMan->EditGetImage(m_kLayer[iMask].m_strMask.c_str())->m_iWidth);
	float xpos = (float) ((kPos.x) / (float)GetSize())  ;
	float ypos = (float) ((kPos.z) / (float)GetSize())  ;
	
	xpos *= (float)m_pkTexMan->EditGetImage(m_kLayer[iMask].m_strMask.c_str())->m_iWidth;
	ypos *= (float)m_pkTexMan->EditGetImage(m_kLayer[iMask].m_strMask.c_str())->m_iHeight;
	cout << "Texture pos: "<< xpos << ", " << ypos << endl;

	float fRealSize = GetBrushSizeInAlphaUVSpace( fSize );	// * (float)m_pkTexMan->GetImage()->w;
//	int size=fRealSize;
	
	cout << "Brush size: " << fRealSize << endl;
	//cout << "Img Size: " << m_pkTexMan->GetImage()->m_iWidth << "," << m_pkTexMan->GetImage()->m_iHeight;

	Image* pkImg = m_pkTexMan->EditGetImage( m_kLayer[iMask].m_strMask.c_str() );
	int iW, iH;
	iW = m_pkTexMan->EditGetImage(m_kLayer[iMask].m_strMask.c_str())->m_iWidth;
	iH = m_pkTexMan->EditGetImage(m_kLayer[iMask].m_strMask.c_str())->m_iHeight;


	for(float i=0;i<fRealSize;i+=0.5)
	{
		for(int z=0;z<360;z+=1)
		{
			int x = int(xpos + sin(z/degtorad)*i);
			int y = int(ypos + cos(z/degtorad)*i);
		
			if(x<0 || x >= iW
				|| y <0 || y>= iH)	
				continue;		
			
			color_rgba kColor;
			pkImg->get_pixel(x,y,kColor);	
			
			int pr,pg,pb,pa;
			
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

			pkImg->set_pixel(x,y,pr,pg,pb,pa);	//m_pkTexMan->PsetRGBA(x,y,pr,pg,pb,pa);
		}
	
	}
	
/*	if(!m_pkTexMan->SwapTexture()) {
		cout << "Failed to swap back texture" << endl;
		}*/

	m_pkTexMan->EditCommit(m_kLayer[iMask].m_strMask.c_str());
	m_pkTexMan->EditEnd(m_kLayer[iMask].m_strMask.c_str());
}

void HeightMap::DrawVisible(Vector3 kPos, bool bVisible)
{
	int iX = int(kPos.x / m_fTileSize);	
	int iZ = int(kPos.z / m_fTileSize);	
	
	int iIndex = iZ * m_iTilesSide + iX;
	int iTri = GetTopLowTriangle( kPos );

	if(bVisible)
		m_pkTileFlags[iIndex] |= iTri;	
	else
		m_pkTileFlags[iIndex] &= ~iTri;	

	cout << "Index: " <<iTri << endl;
}


HM_vert* HeightMap::LinePick(Vector3 kPos,Vector3 kDir,Vector3 kCenterPos,int iWidth,Vector3& kHitPos)
{
	float minx = kCenterPos.x - iWidth/2; 
	float maxx = kCenterPos.x + iWidth/2; 
	float minz = kCenterPos.z - iWidth/2; 
	float maxz = kCenterPos.z + iWidth/2; 
	
	kPos+=Vector3(1,0,1)*m_fTileSize;

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
			verts[0].y = ((GetVert((int)verts[0].x,(int)verts[0].z)->height))+m_kPosition.y;	// *m_fTileSize
			verts[1].y = ((GetVert((int)verts[1].x,(int)verts[1].z)->height))+m_kPosition.y; // *m_fTileSize
			verts[2].y = ((GetVert((int)verts[2].x,(int)verts[2].z)->height))+m_kPosition.y;	// *m_fTileSize	 	
						
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
			verts[0].y = ((GetVert((int)verts[0].x,(int)verts[0].z)->height))+m_kPosition.y; // *m_fTileSize
			verts[1].y = ((GetVert((int)verts[1].x,(int)verts[1].z)->height))+m_kPosition.y;	// *m_fTileSize	 
			verts[2].y = ((GetVert((int)verts[2].x,(int)verts[2].z)->height))+m_kPosition.y;	// *m_fTileSize	
			
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
		kHitPos.x *= m_fTileSize;
		kHitPos.z *= m_fTileSize;		
		kHitPos-=Vector3(1,0,1)*m_fTileSize;
		
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




Point HeightMap::GetSqrFromPos(Vector3 pos)
{
	int iSquareX = int(m_iTilesSide/2+ceil(pos.x / m_fTileSize));
	int iSquareY = int(m_iTilesSide/2+ceil(pos.z / m_fTileSize));

	return Point(iSquareX,iSquareY);
}

Vector3 HeightMap::GetPosFromSqr(Point square)
{
	float x = (float) (-(m_iTilesSide/2)*m_fTileSize + square.x*m_fTileSize);
	float z = (float) (-(m_iTilesSide/2)*m_fTileSize + square.y*m_fTileSize);

	x -= m_fTileSize/2;	// Translate to center 
	z -= m_fTileSize/2;	// of square.*/

	float y = Height(x,z);

	return Vector3(x,y,z);
}

void HeightMap::GetCollData(vector<Mad_Face>* pkFace,vector<Vector3>* pkVertex , vector<Vector3>* pkNormal)
{
	int iIndex = 0;
	Mad_Face kFace;
	Vector3 kNorm;
	Vector3 V1,V2,V3;
	int iTileIndex;

	for(int z=0; z<m_iTilesSide; z++) {
		for(int x=0; x<m_iTilesSide; x++) {
			iTileIndex = z * m_iTilesSide + x;

			if(m_pkTileFlags[iTileIndex] & HM_FLAGVISIBLE2)
			{
				V1 = Vector3((float)x*m_fTileSize, verts[z*m_iVertexSide+x].height, (float)z*m_fTileSize);
				V2 = Vector3((float)(x + 1)*m_fTileSize, verts[(z+1)*m_iVertexSide+(x+1)].height, (float)(z + 1.0f)*m_fTileSize);
				V3 = Vector3((float)(x + 1)*m_fTileSize, verts[(z)*m_iVertexSide+(x+1)].height, (float)z*m_fTileSize);
				pkVertex->push_back( V1 );		kFace.iIndex[0] = iIndex++;		
				pkVertex->push_back( V2 );		kFace.iIndex[1] = iIndex++;		
				pkVertex->push_back( V3 );		kFace.iIndex[2] = iIndex++;	
				pkFace->push_back(kFace);
				Vector3 kDir1 = V1 - V2;
				Vector3 kDir2 = V3 - V2;
				kNorm = kDir2.Cross(kDir1);
				kNorm.Normalize();
				pkNormal->push_back( kNorm );
			}

			if(m_pkTileFlags[iTileIndex] & HM_FLAGVISIBLE1)
			{
				V1 = Vector3((float)x*m_fTileSize, verts[z*m_iVertexSide+x].height, (float)z*m_fTileSize); 
				V2 = Vector3((float)x*m_fTileSize , verts[(z+1)*m_iVertexSide+x].height, (float)(z + 1.0f)*m_fTileSize);
				V3 = Vector3((float)(x + 1)*m_fTileSize, verts[(z+1)*m_iVertexSide+(x+1)].height, (float)(z + 1.0f)*m_fTileSize);

				pkVertex->push_back( V1 );	kFace.iIndex[0] = iIndex++;		
				pkVertex->push_back( V2 );	kFace.iIndex[1] = iIndex++;		
				pkVertex->push_back( V3 );	kFace.iIndex[2] = iIndex++;	
				pkFace->push_back(kFace);
				Vector3 kDir1 = V1 - V2;
				Vector3 kDir2 = V3 - V2;
				kNorm = kDir2.Cross(kDir1);
				kNorm.Normalize();
				pkNormal->push_back( kNorm );
			}
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






















