#include "heightmap2.h"

Heightmap2::Heightmap2(/*char* szName*/) //: I_HeightMap2("Heightmap2")
{
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));			
 	m_pkFps = static_cast<ZeroFps*>(g_ZFObjSys.GetObjectPtr("ZeroFps"));
 	
	cout<<"Heightmap 2 created"<<endl;
	
	m_iPatchWidth = 16;
	m_iPatchHeight = 16;
	
	m_fScale = 1;
	m_fDetail = 70;
		
	m_bLoaded = false;
	
	m_kHeightmapMaterial.SetRes("data/material/landshader.zmt");		
}

bool Heightmap2::LoadBasicDataFromImage(const char* acFile)
{
	Image* kImage = m_pkTexMan->LoadImage2(acFile);
	
	if(!kImage){
		return false;	
	}
	
	m_kBasicData.clear();
	
	m_iWidth = kImage->width;
	m_iHeight = kImage->height;
	
	
	HM2_vert	tempvert;
	
	for(int i=0;i < (m_iWidth*m_iHeight);i++)
	{
		tempvert.fHeight = kImage->pixels[i].r;
		m_kBasicData.push_back(tempvert);
	}
	
	delete kImage;
	
	cout<<"Loaded heightmap data "<<m_iWidth<<" "<<m_iHeight<< " Vertexs,"<<endl;

	return true;
}


bool Heightmap2::CreateHMFromImage(const char* acFile)
{
	if(!LoadBasicDataFromImage(acFile))
	{
		m_bLoaded = false;
		return false;
	}

	SmoothBasicData(2);
	GenerateNormals();
	
	GenerateRenderData();
	
	m_bLoaded = true;
	return true;
}


void Heightmap2::SmoothBasicData(int iLoops)
{
	float med;
	
	for(int l=0;l<iLoops;l++) {
		for(int y=1;y<m_iHeight-1;y++) {
			for(int x=1;x<m_iWidth-1;x++) {
				med=0;
				for(int q=-1;q<2;q++)
					for(int w=-1;w<2;w++){
						if(q==0 && w==0) {							

						} else {
							med+=GetVert(x+w,y+q)->fHeight;
						
							//med+=verts[(y+q)*m_iHmSize+(x+w)].height;							
						}
					}
					
				med /= 8;
				
				GetVert(x,y)->fHeight=med;
			}
		}
	}
	
}


void Heightmap2::GenerateNormals()
{
	GenerateNormals(0,0,m_iWidth,m_iHeight);
}


void Heightmap2::GenerateNormals(int iStartx,int iStartz,int iWidth,int iHeight)
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

	if(rx > m_iWidth)
		rx = m_iWidth;
		
	if(bz > m_iHeight)
		bz = m_iHeight;	
	
	
	for(int z=tz;z<bz-1;z++){
		for(int x=lx;x<rx-1;x++) {
			
			med=Vector3(0,0,0);  //reset medium vector
			
			for(int q=-1;q<1;q++){
				for(int w=-1;w<1;w++){
					v1=Vector3(1	,(m_kBasicData[  (z+q)*m_iWidth+(x+1+w)].fHeight)	-	(m_kBasicData[(z+q)*m_iWidth+(x+w)].fHeight)	,0);
					v2=Vector3(1	,(m_kBasicData[(z+1+q)*m_iWidth+(x+1+w)].fHeight)-	(m_kBasicData[(z+q)*m_iWidth+(x+w)].fHeight)	,1);		
					v3=Vector3(0	,(m_kBasicData[(z+q+1)*m_iWidth+(x+w)].fHeight)	-	(m_kBasicData[(z+q)*m_iWidth+(x+w)].fHeight)	,1);	
		
					n1=v2.Cross(v1);			
					n2=v3.Cross(v2);				
	
					med=med+n1+n2;
				}	
			}
			
			med=med*0.125;	//insted of  division by 8 
			med.Normalize();
			m_kBasicData[z*m_iWidth+x].kNormal=med;
		}
	}
}


void Heightmap2::GenerateRenderData()
{
	int nrx = m_iWidth  / m_iPatchWidth;
	int nry = m_iHeight / m_iPatchHeight;

	cout<<"Generating Render Data, please wait"<<endl;
	cout<<"nr of patches "<<nrx<<" "<<nry<<endl;
	
	for(int y = 0;y<nry;y++)
		for(int x = 0;x<nrx;x++)
		{
			int bx = x*m_iPatchWidth;
			int hx = x*m_iPatchWidth + m_iPatchWidth ;
			
			int by = y*m_iPatchHeight;
			int hy = y*m_iPatchHeight + m_iPatchHeight;
			
			//cout<<"from :"<<bx<<" to "<<hx<<endl;
		
			HM2_patch temp;
			
			GeneratePatch(&temp,bx,by,hx,hy);
			GenerateLodLevel(&temp,2);
			GenerateLodLevel(&temp,4);
			GenerateLodLevel(&temp,8);
			GenerateLodLevel(&temp,16);
//			GenerateLodLevel(&temp,32);
//			GenerateLodLevel(&temp,64);
			
			m_kRenderData.push_back(temp);
			
		}
}

void Heightmap2::GeneratePatch(HM2_patch* pkPatch,int iStartX,int iStartY,int iStopX,int iStopY)
{
	//cout<<"Generating patch:"<<iStartX<<"x"<<iStartY<< " TO "<<iStopX<<"x"<<iStopY<<endl;

	HM2_level temp;
	temp.kVertex.clear();
	temp.kNormal.clear();
	temp.kTexCor.clear();
	

	int w = iStopX - iStartX ;
	int h = iStopY - iStartY ;
	
	pkPatch->iWidth = w;
	pkPatch->iHeight = h;
	pkPatch->iX = iStartX;
	pkPatch->iY = iStartY;
	
	pkPatch->iReLev = 0;

	bool right = true;
	float fHeight;
	Vector3 kNormal;
	Vector2 kTexCor;
	
	for(int y = iStartY;y < iStopY; y++)
	{	
		if(right)
		{
			right = false;
			
			for(int x = iStartX;x <= iStopX; x++)
			{
				fHeight = GetVert(x,y)->fHeight;
				kNormal = GetVert(x,y)->kNormal;
				kTexCor.x = (float) x;
				kTexCor.y = (float) y;
				
				temp.kVertex.push_back(Vector3((float) x,fHeight,(float) y));
				temp.kNormal.push_back(kNormal);	
				temp.kTexCor.push_back(kTexCor);	
					
				fHeight = GetVert(x,y+1)->fHeight;
				kNormal = GetVert(x,y+1)->kNormal;
				kTexCor.x = (float) x;
				kTexCor.y = (float) y+1.0f;
				
				temp.kVertex.push_back(Vector3((float) x,fHeight,(float) y+1.0f));
				temp.kNormal.push_back(kNormal);	
				temp.kTexCor.push_back(kTexCor);				
			
			}	
		} 
		else
		{
			right = true;
			
			for(int x = iStopX;x >= iStartX; x--)
			{
				fHeight = GetVert(x,y+1)->fHeight;
				kNormal = GetVert(x,y+1)->kNormal;
				kTexCor.x = (float) x;
				kTexCor.y = (float) y+1.0f;				
				
				temp.kVertex.push_back(Vector3((float) x,fHeight,(float) y+1.0f));
				temp.kNormal.push_back(kNormal);	
				temp.kTexCor.push_back(kTexCor);	
					
				fHeight = GetVert(x,y)->fHeight;
				kNormal = GetVert(x,y)->kNormal;
				kTexCor.x = (float) x;
				kTexCor.y = (float) y;				
				
				temp.kVertex.push_back(Vector3((float) x,fHeight,(float) y));
				temp.kNormal.push_back(kNormal);	
				temp.kTexCor.push_back(kTexCor);	
			}	
			
		
		}
	}
	
	GeneratePatchMinMaxHeight(pkPatch);
	
	pkPatch->kLevels.push_back(temp);


}

void Heightmap2::GenerateLodLevel(HM2_patch* pkPatch,int iStep)
{
	HM2_level* pkOriginal = &pkPatch->kLevels[0];

	HM2_level newlevel;
	newlevel.kVertex.clear();
	newlevel.kNormal.clear();
	newlevel.kTexCor.clear();	
	
	int w =  m_iPatchWidth;//pkPatch->iWidth/iStep;
	int h =  m_iPatchHeight;//pkPatch->iHeight/iStep;
	
	int sx = pkPatch->iX;
	int sy = pkPatch->iY;
	
	bool right =true;
	float fHeight; 
	Vector3 kNormal;	
	Vector2 kTexCor;
	for(int y = sy; y < sy+pkPatch->iHeight;y+=iStep)	
	{	
		if(y+iStep >= m_iHeight)
			continue;
		
		int x;
		
		if(right)
		{	
			right =false;
			
			for(x = sx; x <= sx+pkPatch->iWidth;x+=iStep)
			{				
					
				fHeight = GetVert( x, y)->fHeight;
				kNormal = GetVert( x, y)->kNormal;				
				kTexCor.x = (float) x;
				kTexCor.y = (float) y;
				newlevel.kTexCor.push_back(kTexCor);	
				newlevel.kVertex.push_back(Vector3((float) x,fHeight,(float) y));
				newlevel.kNormal.push_back(kNormal);	
		
				fHeight = GetVert(x,y+iStep)->fHeight;
				kNormal = GetVert(x,y+iStep)->kNormal;				
				kTexCor.x = (float) x;
				kTexCor.y = (float) y+(float) iStep;				
				newlevel.kTexCor.push_back(kTexCor);					
				newlevel.kVertex.push_back(Vector3((float) x,fHeight,(float) y+(float) iStep));
				newlevel.kNormal.push_back(kNormal);				
			
			}
				
			
			//add the last vertex twice else itill get messy
			newlevel.kTexCor.push_back(kTexCor);				
			newlevel.kVertex.push_back(Vector3((float) x-(float) iStep,fHeight,(float) y+(float) iStep));
			newlevel.kNormal.push_back(kNormal);				
			
		}
		else
		{
			right =true;		
			
			for(x = sx+pkPatch->iWidth ; x >= sx ;x-=iStep)
			{
				
				fHeight = GetVert(x,y)->fHeight;
				kNormal = GetVert(x,y)->kNormal;				
				kTexCor.x = (float) x;
				kTexCor.y = (float) y;				
				newlevel.kTexCor.push_back(kTexCor);					
				newlevel.kVertex.push_back(Vector3((float) x,fHeight,(float) y));
				newlevel.kNormal.push_back(kNormal);			
				
				fHeight = GetVert(x,y+iStep)->fHeight;
				kNormal = GetVert(x,y+iStep)->kNormal;				
				kTexCor.x = (float) x;
				kTexCor.y = (float) y+iStep;				
				newlevel.kTexCor.push_back(kTexCor);					
				newlevel.kVertex.push_back(Vector3((float) x,fHeight,(float) y+(float) iStep));
				newlevel.kNormal.push_back(kNormal);					
				
			}
						 
			//add the last vertex twice
			newlevel.kTexCor.push_back(kTexCor);	
			newlevel.kVertex.push_back(Vector3((float) x + (float) iStep,fHeight,(float) y+(float) iStep));
			newlevel.kNormal.push_back(kNormal);							
			
		}
	}
	
	pkPatch->kLevels.push_back(newlevel);
}

void Heightmap2::UpdateRecLodLevel(Vector3 kCamPos)
{
	WorldToLocal(&kCamPos);

	int px =  m_iWidth / m_iPatchWidth;
	int py =  m_iHeight / m_iPatchHeight;

	for(int y = 0;y<py;y++)
	{			
		for(int x = 0;x<px;x++)
		{
			HM2_patch* pkPatch  = GetPatch(x,y);//&m_kRenderData[ (y*px) + x];
				
			Vector3 kCenter = Vector3( (float) (x*m_iPatchWidth + m_iPatchWidth/2),
				pkPatch->fAvrageHeight,(float) (y*m_iPatchHeight + m_iPatchHeight/2));
			kCenter *= m_fScale;
		
			float fDist = (kCamPos - kCenter).Length();
			
			int iLevel = int(fDist / m_fDetail);
			
			if(iLevel < 0)
				iLevel = 0;
			
			if(iLevel > 4)
				iLevel = 4;
				
			pkPatch->iReLev = iLevel;
		} 
	}
	
	
	//update frustum culling while im at it
	UpdateFrustumCulling();
}

void Heightmap2::GeneratePatchMinMaxHeight(HM2_patch* pkPatch)
{
	int w = pkPatch->iWidth;
	int h = pkPatch->iHeight;
	
	int sx = pkPatch->iX;
	int sy = pkPatch->iY;	

	float fMaxHeight = 0;
	float fMinHeight = 99999999;
	float fTotalHeight = 0;
	
	for(int y = sy; y <= sy+h;y++)	
	{	
		for(int x = sx; x <= sx+w;x++)
		{
			float fHeight = GetVert(x,y)->fHeight;
			
			fTotalHeight+=fHeight;
			
			if(fHeight > fMaxHeight)
				fMaxHeight = fHeight;
		
			if(fHeight < fMinHeight)
				fMinHeight = fHeight;
		
		}
	}
	
	pkPatch->fMaxHeight = fMaxHeight;
	pkPatch->fMinHeight = fMinHeight;
	pkPatch->fAvrageHeight = fTotalHeight/(w*h);	
}

void Heightmap2::UpdateFrustumCulling()
{
	int px =  m_iWidth / m_iPatchWidth;
	int py =  m_iHeight / m_iPatchHeight;
	
	
	for(int y = 0;y<py;y++)
	{			
		for(int x = 0;x<px;x++)
		{
			HM2_patch* pkPatch = GetPatch(x,y);
	
	
			Vector3 kPos;
	
			kPos.x = (float) x*m_iPatchWidth + m_iPatchWidth/2;
			kPos.z = (float) y*m_iPatchWidth + m_iPatchWidth/2;					
			kPos.y= (pkPatch->fMaxHeight + pkPatch->fMinHeight)/2;		
		
			LocalToWorld(&kPos);
		
			if(!m_pkFps->GetCam()->m_kFrustum.CubeInFrustum(kPos.x,kPos.y,kPos.z,
				(float) m_iPatchWidth/2,
				(pkPatch->fMaxHeight - pkPatch->fMinHeight)/2,
				(float) m_iPatchHeight/2))
					pkPatch->iReLev = -1;
				
		}
	}
}

HM2_patch* Heightmap2::GetPatch(int x,int y)
{
	int nrx = m_iWidth  / m_iPatchWidth;
	int nry = m_iHeight / m_iPatchHeight;


	if(x<0)
		cout<<"error getting patch x"<<x<<endl;
	if(x>=nrx)
		cout<<"error getting patch x"<<x<<endl;
	if(y<0)
		cout<<"error getting patch y"<<y<<endl;
	if(y>=nry)
		cout<<"error getting patch y"<<y<<endl;
		
	
	return &m_kRenderData[ (y*nrx) + x];
}

void Heightmap2::WorldToLocal(Vector3* pkPos)
{
	(*pkPos) -= m_kPos;
	(*pkPos) += Vector3((float)(m_iWidth*m_fScale / 2.0),0,
		(float)(m_iHeight*m_fScale / 2.0));	
}

void Heightmap2::LocalToWorld(Vector3* pkPos)
{
	(*pkPos) += m_kPos;
	(*pkPos) -= Vector3((float)(m_iWidth*m_fScale / 2.0),0,
		(float)(m_iHeight*m_fScale / 2.0));	
}

float Heightmap2::Height(float x,float z)
{
	Vector3 kPos;
	kPos.Set(x,0,z);
	
	WorldToLocal(&kPos);

	x=kPos.x;
	z=kPos.z;

	
	if(x<0 || x>=m_iWidth-1 || z<0 || z>=m_iHeight-1) 
		return 1;

	int lx=int(x);
	int lz=int(z);
	float ox=x-float(lx);
	float oz=z-float(lz);
	float bp,xp,zp;

	float ry= float((1.0+ox*-1.0));

	if(oz>ry){//over left
		bp=m_kBasicData[(lz+1)*m_iWidth+(lx+1)].fHeight;
		xp=m_kBasicData[(lz+1)*m_iWidth+(lx)].fHeight-bp;
		zp=m_kBasicData[(lz)*m_iWidth+(lx+1)].fHeight-bp;		
		ox= float(1.0-ox);
		oz= float(1.0-oz);
	}else{//under right
		bp=m_kBasicData[lz*m_iWidth+lx].fHeight;
		xp=m_kBasicData[(lz)*m_iWidth+(lx+1)].fHeight-bp;
		zp=m_kBasicData[(lz+1)*m_iWidth+(lx)].fHeight-bp;				
	}	

	

	return m_kPos.y+(bp+(xp*ox)+(zp*oz))*m_fScale;	

}


Vector3 Heightmap2::Tilt(float x,float z) {
	Vector3 kPos;
	kPos.Set(x,0,z);
	
	WorldToLocal(&kPos);

	x=kPos.x;
	z=kPos.z;

	
	if(x<0 || x>=m_iWidth-1 || z<0 || z>=m_iHeight-1) 
		return Vector3(0,1,0);
		
	int lx=int(x);
	int lz=int(z);
	float ox=x-float(lx);
	float oz=z-float(lz);
	
	Vector3 v1,v2,n1;
	
	float ry= float ((1.0+ox*-1.0));
	if(oz>ry){
//		cout<<"UPP"<<endl;		
		v1=Vector3(1,(m_kBasicData[(lz+1)*m_iWidth+(lx+1)].fHeight)	-(m_kBasicData[(lz+1)*m_iWidth+(lx)].fHeight) ,0);
		v2=Vector3(0,(m_kBasicData[(lz+1)*m_iWidth+(lx+1)].fHeight)	-(m_kBasicData[(lz)*m_iWidth+(lx+1)].fHeight) ,1);	
		
		n1=v2.Cross(v1);			
	}else {
//		cout<<"DOWN"<<endl;		
		v1=Vector3(0,(m_kBasicData[(lz+1)*m_iWidth+(lx)].fHeight)	-(m_kBasicData[(lz)*m_iWidth+(lx)].fHeight) ,1);
		v2=Vector3(1,(m_kBasicData[(lz)*m_iWidth+(lx+1)].fHeight)	-(m_kBasicData[(lz)*m_iWidth+(lx)].fHeight) ,0);		
	
		n1=v1.Cross(v2);			
	}
	
	n1.Normalize();
	return n1;
}
