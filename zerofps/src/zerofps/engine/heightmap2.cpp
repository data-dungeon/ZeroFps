#include "heightmap2.h"

Heightmap2::Heightmap2()
{
	m_pkTexMan=static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));			
	
	cout<<"Heightmap 2 created"<<endl;
	
	m_iPatchWidth = 32;
	m_iPatchHeight = 32;
	
	m_fScale = 1;
		
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
}


bool Heightmap2::CreateHMFromImage(const char* acFile)
{
	if(!LoadBasicDataFromImage(acFile))
		return false;

	SmoothBasicData(2);
	GenerateNormals();
	
	GenerateRenderData();
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
			GenerateLodLevel(&temp,32);
			
			m_kRenderData.push_back(temp);
		}
}

void Heightmap2::GeneratePatch(HM2_patch* pkPatch,int iStartX,int iStartY,int iStopX,int iStopY)
{
	//cout<<"Generating patch:"<<iStartX<<"x"<<iStartY<< " TO "<<iStopX<<"x"<<iStopY<<endl;

	HM2_level temp;
	temp.kVertex.clear();
	temp.kNormal.clear();
	temp.kIndex.clear();
	
	/*
	int w = iStopX - iStartX +1;
	int h = iStopY - iStartY +1;
	
	pkPatch->iWidth = w;
	pkPatch->iHeight = h;
	
	float fHeight;
	Vector3 kNormal;
	
	//first generate all normals and vertex's
	for(int y=iStartY;y < iStopY;y++)
	{
		for(int x = iStartX;x <= iStopX;x++)
		{
			//top
			fHeight = GetVert(x,y)->fHeight;
			kNormal = GetVert(x,y)->kNormal;
			
			temp.kVertex.push_back(Vector3(x,fHeight,y));
			temp.kNormal.push_back(kNormal);				
		
			//bottom
			fHeight = GetVert(x,y+1)->fHeight;
			kNormal = GetVert(x,y+1)->kNormal;
			
			temp.kVertex.push_back(Vector3(x,fHeight,y+1));
			temp.kNormal.push_back(kNormal);				
		
		}
	}*/
	
	/*
	int i=0;
	for(int y = 0;y < h-1;y++)
	{
		for(int x = 0;x < w-1;x++)
		{
			i=(y*w +x);
			
			HM2_face tempface;
			
			//top left
			tempface.iIndex[0] = i;
			tempface.iIndex[1] = i+w;
			tempface.iIndex[2] = i+1;
			temp.kIndex.push_back(tempface);
			
			//top left
			tempface.iIndex[0] = i+1;
			tempface.iIndex[1] = i+w;
			tempface.iIndex[2] = i+w+1;
			temp.kIndex.push_back(tempface);
		
		}
	}
	*/

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
	
	for(int y = iStartY;y < iStopY; y++)
	{	
		if(right)
		{
			for(int x = iStartX;x <= iStopX; x++)
			{
				fHeight = GetVert(x,y)->fHeight;
				kNormal = GetVert(x,y)->kNormal;
				
				temp.kVertex.push_back(Vector3(x,fHeight,y));
				temp.kNormal.push_back(kNormal);	
		
					
				fHeight = GetVert(x,y+1)->fHeight;
				kNormal = GetVert(x,y+1)->kNormal;
				
				temp.kVertex.push_back(Vector3(x,fHeight,y+1));
				temp.kNormal.push_back(kNormal);	
			}	
			
			right = false;
		}
		else
		{
			for(int x = iStopX;x >= iStartX; x--)
			{
				fHeight = GetVert(x,y+1)->fHeight;
				kNormal = GetVert(x,y+1)->kNormal;
				
				temp.kVertex.push_back(Vector3(x,fHeight,y+1));
				temp.kNormal.push_back(kNormal);	
		
					
				fHeight = GetVert(x,y)->fHeight;
				kNormal = GetVert(x,y)->kNormal;
				
				temp.kVertex.push_back(Vector3(x,fHeight,y));
				temp.kNormal.push_back(kNormal);	
			}	
			
			right = true;
		
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
	newlevel.kIndex.clear(); 
	
	int w =  m_iPatchWidth;//pkPatch->iWidth/iStep;
	int h =  m_iPatchHeight;//pkPatch->iHeight/iStep;
	
	int sx = pkPatch->iX;
	int sy = pkPatch->iY;
	
	bool right =true;
	float fHeight; 
	Vector3 kNormal;	
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
			
				//this handle the right edge patch, (it has one vertex less then the others)
//				if(x+iStep > m_iWidth)
//					continue;
		
				fHeight = GetVert(x,y)->fHeight;
				kNormal = GetVert(x,y)->kNormal;				
				newlevel.kVertex.push_back(Vector3(x,fHeight,y));
				newlevel.kNormal.push_back(kNormal);	
		
				fHeight = GetVert(x,y+iStep)->fHeight;
				kNormal = GetVert(x,y+iStep)->kNormal;				
				newlevel.kVertex.push_back(Vector3(x,fHeight,y+iStep));
				newlevel.kNormal.push_back(kNormal);				
			
			}
				
			
			//add the last vertex twice else itill get messy
			newlevel.kVertex.push_back(Vector3(x-iStep,fHeight,y+iStep));
			newlevel.kNormal.push_back(kNormal);				
			
		}
		else
		{
			right =true;		
			
			for(x = sx+pkPatch->iWidth ; x >= sx ;x-=iStep)
			{
				
				//this handle the right edge patch, (it has one vertex less then the others)				
/*				if(x+iStep > m_iWidth)
				{	
					x++;
					x-=iStep;
				}
*/		
				fHeight = GetVert(x,y)->fHeight;
				kNormal = GetVert(x,y)->kNormal;				
				newlevel.kVertex.push_back(Vector3(x,fHeight,y));
				newlevel.kNormal.push_back(kNormal);			
				
				fHeight = GetVert(x,y+iStep)->fHeight;
				kNormal = GetVert(x,y+iStep)->kNormal;				
				newlevel.kVertex.push_back(Vector3(x,fHeight,y+iStep));
				newlevel.kNormal.push_back(kNormal);					
				
			}
						 
			//add the last vertex twice
			newlevel.kVertex.push_back(Vector3(x + iStep,fHeight,y+iStep));
			newlevel.kNormal.push_back(kNormal);							
			
		}
	}
	
	pkPatch->kLevels.push_back(newlevel);
}

void Heightmap2::UpdateRecLodLevel(Vector3 kCamPos)
{
	int px =  m_iWidth / m_iPatchWidth;
	int py =  m_iHeight / m_iPatchHeight;

	for(int y = 0;y<=py;y++)
	{			
		for(int x = 0;x<=px;x++)
		{
			HM2_patch* pkPatch  = &m_kRenderData[ (y*px) + x];
				
			Vector3 kCenter = Vector3(x*m_iPatchWidth + m_iPatchWidth/2,pkPatch->fAvrageHeight,y*m_iPatchHeight + m_iPatchHeight/2);
			kCenter *= m_fScale;
		
			float fDist = (kCamPos - kCenter).Length();
			
			int iLevel = int(fDist / 50);
			
			if(iLevel < 0)
				iLevel = 0;
			
			if(iLevel >5)
				iLevel = 5;
				
			pkPatch->iReLev = iLevel;
		} 
	}
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
	
	for(int y = sy; y < sy+h;y++)	
	{	
		for(int x = sx; x < sx+w;x++)
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

