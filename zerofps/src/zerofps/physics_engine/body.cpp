#include "body.h"

Body::Body()
{
	cout<<"new body created"<<endl;
	
	Reset();
}


void Body::Reset()
{
	m_pkMad =				NULL;
	m_fBoundingRadius =	0.5;
	m_bPolygonCheck =		false;

}



