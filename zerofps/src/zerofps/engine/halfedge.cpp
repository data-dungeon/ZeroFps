#include "halfedge.h"

HalfEdge::HalfEdge(void) {

}

void HalfEdge::add(Polygon3* kPoly) {
//	cout<<"---- adding polygon ----"<<endl;
	
	HE_edge* e[3];
	HE_vert* v[3];
	HE_face* f=new HE_face;
	
	int i;
	for(i=0;i<3;i++){
		v[i]=new HE_vert;
		e[i]=new HE_edge;			
	}		
	
	for(i=0;i<3;i++){		
		v[i]->point=kPoly->p[i];
		v[i]->norm=kPoly->n[i]*0.5;		
//		v[i]->norm=Vector3(0,0.3,0);
		v[i]->texcord=kPoly->t[i];		
		v[i]->edge=e[i];
		
		e[i]->vert=v[i];
		e[i]->face=f;
		
		e[i]->next=e[(i+1)%3];
		
		if(i==0)
			e[i]->prev=e[2];		
		else		
			e[i]->prev=e[i-1];		
				
		e[i]->pair=NULL;		
				
	
//		cout<<"Edge:"<<e[i]->vert->point.x<<" "<<e[i]->vert->point.y<<" "<<e[i]->vert->point.z<<endl;
//		cout<<"Next:"<<e[i]->next->vert->point.x<<" "<<e[i]->next->vert->point.y<<" "<<e[i]->next->vert->point.z<<endl;		
//		cout<<"Prev:"<<e[i]->prev->vert->point.x<<" "<<e[i]->prev->vert->point.y<<" "<<e[i]->prev->vert->point.z<<endl;		
	}
	f->edge=e[0];	//point to one of the vertexes
	
	for( i=0;i<3;i++) {
		for(int j=0;j<m_kEdges.size();j++) {	
			if(e[i]->vert->point==m_kEdges[j]->vert->point){
				if(e[i]->prev->vert->point==m_kEdges[j]->next->vert->point){
					e[i]->pair=m_kEdges[j]->next;		//set pair this one
					m_kEdges[j]->next->pair=e[i];		//set pair for the pair
					//cout<<i<<" width "<<j<<endl;
					//cout<<"PAIR: "<<e[i]->vert->point.x<<" "<<e[i]->vert->point.y <<" "<<e[i]->vert->point.z 
					//<<" is "
					//<<m_kEdges[j]->next->vert->point.x<<" "<<m_kEdges[j]->next->vert->point.y<<" "<<m_kEdges[j]->next->vert->point.z<<endl;
				}
			}
		}	
	}
	
	
	m_kFaces.push_back(f);	
	for( i=0;i<3;i++) {
		m_kVerts.push_back(v[i]);
		m_kEdges.push_back(e[i]);	
	}

}

void HalfEdge::addarray(int iSize,Polygon3* kData) {
	for(int i=0;i<iSize;i++) {
		add(&kData[i]);
	}
}

/*
void HalfEdge::Draw(IoSystem* pkIo) {	
	Vector3 point1,point2;
	HE_edge* e;

	for(int i=0;i<m_kFaces.size();i++) {
		pkIo->SetColor(Vector3(1,0,1));
		pkIo->Triangle(m_kFaces[i]->edge->vert->point,m_kFaces[i]->edge->next->vert->point,m_kFaces[i]->edge->next->next->vert->point);

		point1=m_kFaces[i]->edge->vert->point;
		e=m_kFaces[i]->edge->next;
		
		while(e!=m_kFaces[i]->edge){	
			point2=e->vert->point;		
			
			if(e->pair==NULL)													//edge edge ? =)
				pkIo->SetColor(Vector3(0,1,0));
			else
				pkIo->SetColor(Vector3(1,0,0));				
				
			pkIo->Line(point1,point2);							//draw edges
			pkIo->SetColor(Vector3(0,0,1));
			pkIo->Line(point2,point2+e->vert->norm);
			
			point1=point2;
			e=e->next;
		}	
				
		point2=m_kFaces[i]->edge->vert->point;		//back to first point
		if(e->pair==NULL)
			pkIo->SetColor(Vector3(0,1,0));
		else
			pkIo->SetColor(Vector3(1,0,0));
		
		pkIo->Line(point1,point2);						//draw final edge
		pkIo->SetColor(Vector3(0,0,1));
		pkIo->Line(point2,point2+e->vert->norm);		
	}
}
*/









