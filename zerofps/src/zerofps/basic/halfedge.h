#ifndef _BASIC_HALFEDGE_H_
#define _BASIC_HALFEDGE_H_

#include <vector>
#include "basic_x.h"
#include "os.h"
#include "vector3.h"

using namespace std;

struct HE_edge;
struct HE_vert;
struct HE_face;

struct HE_edge {
	HE_vert* vert;			//this vertex at the end of the halfedge
	HE_edge* pair;			//Pointer to its pair
	HE_face* face;			//pointer to its face
	HE_edge* next;			//pointer to next edge in face
	HE_edge* prev;			//pointer to previous edge in face
};

struct HE_vert{
	Vector3 point;			//real cords
	Vector3 norm;				//normal
	Vector3 texcord;		//texture cords
	
	HE_edge* edge;			//pointer to its edge
	
};

struct HE_face {
	HE_edge* edge;			//pointer to one of the edges in this face
};

struct Polygon3{
	Vector3 p[3];			//vertexs
	Vector3 n[3];			//normals
	Vector3 t[3];			//texture cords
};


class HalfEdge {
	private:
		vector<HE_vert*> m_kVerts;
		vector<HE_edge*> m_kEdges;
		vector<HE_face*> m_kFaces;

	public:
		HalfEdge(void);
		void add(Polygon3* kPoly);
		void addarray(int iSize,Polygon3* kPoly);
//		void Draw(IoSystem* pkIo);

};

#endif








