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
		ON_LINE,					// The point is on the line.
		LEFT_SIDE,				// Looking from A to B the point is on the right side.
		RIGHT_SIDE				// Looking from A to B the point is on the left side.
	};

	enum LINE_CLASSIFICATION
	{
		COLLINEAR,				// 
		PARALELL,					// The lines are paralell.
		LINES_INTERSECT,	
		SEGMENTS_INTERSECT,	
		A_BISECTS_B,		
		B_BISECTS_A,	
	};

// Constructors
	Line2D();
	Line2D( const Line2D& kIn);
	Line2D( const Vector2& kPointA, const Vector2& kPointB );
	~Line2D();

// Operators
	Line2D& operator=( const Line2D& kIn);

// Methods
	void SentEndPointA( const Vector2& kPoint);
	void SentEndPointB( const Vector2& kPoint);
	void SetPoints( const Vector2& kPointA, const Vector2& kPointB);
	void SetPoints( float fAX, float fAY, float fBX, float fBY);

	float SignedDistance(const Vector2& kPoint);
	POINT_CLASSIFICATION ClassifyPoint(const Vector2& kPoint, float fEpsilon = 0.0f);
	Line2D::LINE_CLASSIFICATION Line2D::Intersection(Line2D& Line, Vector2* pIntersectPoint);

// Accessors 
	Vector2 GetNormal();
	Vector2 GetMidPoint();
	const Vector2& EndPointA() const;
	const Vector2& EndPointB() const;

private:
// Data
	Vector2	m_PointA;	// Endpoint A.
	Vector2	m_PointB;	// Endpoint B

	Vector2	m_kNormal;	
	bool		m_bNormalCalculated; 

	void ComputeNormal();

	

};


#endif










