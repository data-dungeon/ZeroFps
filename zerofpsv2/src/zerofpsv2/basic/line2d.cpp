#include "zfassert.h"
#include "line2d.h"



Line2D::Line2D()
{
	
}

Line2D::Line2D(const Line2D& kIn)
{
	*this = kIn;
}

Line2D::Line2D( const Vector2& kPointA, const Vector2& kPointB)
{
	m_PointA = kPointA;
	m_PointB = kPointB;
}

Line2D::~Line2D()
{
	
}

Line2D& Line2D::operator=( const Line2D& kIn)
{
	m_PointA = kIn.m_PointA;
	m_PointB = kIn.m_PointB;
	m_bNormalCalculated = false;
	return (*this);
}

void Line2D::SentEndPointA( const Vector2& kPoint)
{
	m_PointA = kPoint;
}


void Line2D::SentEndPointB( const Vector2& kPoint)
{
	m_PointB = kPoint;
}


void Line2D::SetPoints( const Vector2& kPointA, const Vector2& kPointB)
{
	m_PointA = kPointA;
	m_PointB = kPointB;
}

void Line2D::SetPoints( float fAX, float fAY, float fBX, float fBY)
{
	m_PointA.x = fAX;
	m_PointA.y = fAY;
	m_PointB.x = fBX;
	m_PointB.y = fBY;
}

float Line2D::SignedDistance(const Vector2& kPoint)
{
	if (!m_bNormalCalculated)
	{
		ComputeNormal();
	}

	Vector2 kTestVector(kPoint - m_PointA);
	
	return kTestVector.Dot(m_kNormal);
}

Line2D::POINT_CLASSIFICATION Line2D::ClassifyPoint(const Vector2& kPoint, float fEpsilon) 
{
	POINT_CLASSIFICATION	kResult = ON_LINE;
   
	float fDistance = SignedDistance(kPoint);
    
	if (fDistance > fEpsilon)
   {
       kResult = RIGHT_SIDE;
   }
   else if (fDistance < -fEpsilon)
   {
       kResult = LEFT_SIDE;
   }

   return(kResult);
}

void Line2D::ComputeNormal()
{
	m_kNormal = (m_PointB - m_PointA);
	m_kNormal.Normalize();

	//
	// Rotate by -90 degrees to get normal of line
	//

	float OldYValue = m_kNormal.y;
	m_kNormal.y = -m_kNormal.x;
	m_kNormal.x = OldYValue;
	m_bNormalCalculated = true;
}

Vector2 Line2D::GetNormal()
{
	if (!m_bNormalCalculated)
	{
		ComputeNormal();
	}

	return m_kNormal;
}

Vector2 Line2D::GetMidPoint()
{
	Vector2 kCenter = (m_PointB - m_PointA);
	kCenter = kCenter * 0.5 + m_PointA;

	return kCenter;
}

