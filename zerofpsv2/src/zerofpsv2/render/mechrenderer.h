#ifndef _MECHRENDER_H_
#define _MECHRENDER_H_

#include "render_x.h"
#include "../basic/vector3.h"
#include <vector>
#include "../basic/zfobjectmanger.h"

class MR_Vertex
{
	public:
		Vector3	kPos;
		Vector3	kTexture;
		Vector3	kNormal;
		
		float		fWeight;
		float		fCVD;
		int		iFriend;
		float		fInterest;
		
		int		iPolygons;		
		
		bool		bHard;

};

class MR_Polygon
{
	public:
		int		iVerts[3];
		int		iTexture;		

		bool		bEnabled;
};

class MR_Level
{
	public:
		vector<MR_Polygon>	kPolygons;
		vector<MR_Vertex>	  kVertexs;
};


class MR_Mesh
{
	public:
		vector<MR_Level>	kLevels;
};



class RENDER_API MechRenderer : public ZFObject 
{
	private:
		vector<MR_Mesh>	m_kMeshs;
	
	public:
		MechRenderer();
	
	
		void AddMesh(vector<MR_Vertex>* pkVerts,vector<MR_Polygon>* pkPols);
		void DrawMesh(int iMeshID,int iLevel,Vector3 kPos);

		void CalcWeights(int iMeshID,int iLevel);
		void GenerateNewLevel(int iMeshID,float fMinSize);
		
		float GetMeshSize(int iMeshID,int iLevel);
//		void FindUnImportantVerts(int iMeshID,int iVerts,vector<int>);
};


#endif


