#ifndef _BASIC_LINE2D_H_
#define _BASIC_LINE2D_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"
#include "os.h"
#include "vector2.h"

using namespace std;

class BASIC_API Line2D
{
public:
	enum POINT_CLASSIFICATION
	{
		ON_LINE,		
		LEFT_SIDE,	
		RIGHT_SIDE	
	};

	Line2D();
	Line2D( const Line2D& kIn);
	Line2D( const Vector2& kPointA, const Vector2& kPointB );
	~Line2D();

	Line2D& operator=( const Line2D& kIn);

	void SentEndPointA( const Vector2& kPoint);
	void SentEndPointB( const Vector2& kPoint);
	void SetPoints( const Vector2& kPointA, const Vector2& kPointB);
	void SetPoints( float fAX, float fAY, float fBX, float fBY);

	float SignedDistance(const Vector2& kPoint);
	POINT_CLASSIFICATION ClassifyPoint(const Vector2& kPoint, float fEpsilon = 0.0f);

	Vector2 GetNormal();
	Vector2 GetMidPoint();

private:
	Vector2	m_PointA;	// Endpoint A.
	Vector2	m_PointB;	// Endpoint B

	Vector2	m_kNormal;	
	bool		m_bNormalCalculated; 

	void ComputeNormal();

	

};


#endif