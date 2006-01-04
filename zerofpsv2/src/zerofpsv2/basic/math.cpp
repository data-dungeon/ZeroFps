#include "globals.h"
#include "zfsystem.h"
#include "zfassert.h"
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include "math.h"
#include "vector4.h"

using namespace std;

extern ZFSystem g_ZFObjSys;

#define NRDEG_IN_ONE_RAD	57.29577951
#define NRRAD_IN_ONE_DEG	0.0174532952

const float Math::PI			= 3.1415926535897932384626433832795f;
const float Math::TWO_PI	= 2.0 * Math::PI;
const float Math::HALF_PI  = 0.5 * Math::PI;
const float Math::EPSILON  = 1.0e-4f;

float BASIC_API Math::DegToRad(float fAngle)
{
	return float((fAngle * NRRAD_IN_ONE_DEG));
}

float BASIC_API Math::RadToDeg(float fAngle)
{
	return float((fAngle * NRDEG_IN_ONE_RAD));
}


int BASIC_API Math::PowerOf2(int iVal)
{
	int list[] = {1,2,4,8,16,32,64,128};

	int smalest=999999999;
	int tal=0; // 040701: int tal;
	int blub;

	for(int i=0;i<7;i++)
	{
		blub=abs(iVal-list[i]);
		
		if( blub < smalest)
		{
			smalest=blub;
			tal=list[i];
		}		
	}
	
	return tal;
}

// Base Changes, Tarnsformations and other functions the nobody in the zerofps really understand ;).
Vector3 Math::GetYawVector2(float fAngleDeg)
{
	Vector3 kYaw;
	kYaw.x = float(cos(DegToRad(fAngleDeg)));	
	kYaw.y = 0;
	kYaw.z = float(sin(DegToRad(fAngleDeg)));	
	return kYaw;
}


float Math::GetYawAngle(Vector3 kDir)
{
	float fAngle =  float(atan2(kDir.x, - kDir.z));
	return RadToDeg(fAngle);
}

float Math::Round2(float val)
{
/*	int lower = (int) val;
	if( (val - lower) > 0.499f)
		return (float) lower + 1.0f;
	return (float) lower;*/

/*	if(val < 0)
		return -((float) (int) (fabs(val) + 0.5f));
	else
		return (float) (int) (val + 0.5f);
*/		

	if(val < 0)
		return (float) (int) (val - 0.5f);
	else
		return (float) (int) (val + 0.5f);

}


float Math::Randomf(float fInterval)
{
	float r = (float)rand() / (float)(RAND_MAX);
	return r*(fInterval);
}

int Math::Randomi(int iInterval)
{
	float r = (float)rand() / (float)(RAND_MAX);
	return int(r * iInterval );
}

void Math::SetRandomSeed(unsigned int iSeed)
{
	if(iSeed == 0)
	{
		srand(time(0));
	}
	else
	{
		srand(iSeed);
	}
}


bool Math::Equal_f(float a, float b)
{
	return( (a > (b - EPSILON) ) && (a < (b + EPSILON)) );
}

float Math::Clamp(float fValue,float fBotom,float fTop)
{
	bool bChanged=true;
	float fDiff = fTop - fBotom;
	
	while(bChanged)
	{
		bChanged = false;
		
		if(fValue < fBotom)
		{	
			fValue+=fDiff;
			bChanged=true;
		}
		if(fValue > fTop)
		{	
			fValue-=fDiff;
			bChanged=true;
		}
	}

	return fValue;
}

void	Math::GenerateTangents(const Vector3* akVertises,const Vector3* akNormals,const Vector2* akTexCoord,vector<Vector3>& kTangents,vector<Vector3>& kBiTangents,int iVertises)
{
	Vector3 *tan1 = new Vector3[iVertises];
	Vector3 *tan2 = new Vector3[iVertises]; 		
	

	for(int i = 0;i<iVertises;i+=3)
	{		
		int i1 = i;
		int i2 = i+1;
		int i3 = i+2;
		
		const Vector3& v1 = akVertises[i1];
		const Vector3& v2 = akVertises[i2];
		const Vector3& v3 = akVertises[i3];
		
		const Vector2& w1 = akTexCoord[i1];
		const Vector2& w2 = akTexCoord[i2];
		const Vector2& w3 = akTexCoord[i3];
		
		float x1 = v1.x - v2.x;
		float x2 = v1.x - v3.x;
		float y1 = v1.y - v2.y;
		float y2 = v1.y - v3.y;
		float z1 = v1.z - v2.z;
		float z2 = v1.z - v3.z;
		
		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;
		
		float r = 1.0F / (s1 * t2 - s2 * t1);
		Vector3 sdir(	(t2 * x1 - t1 * x2) * r, 
							(t2 * y1 - t1 * y2) * r,
							(t2 * z1 - t1 * z2) * r);
		Vector3 tdir(	(s1 * x2 - s2 * x1) * r, 
							(s1 * y2 - s2 * y1) * r,
							(s1 * z2 - s2 * z1) * r);

		tan1[i1] = sdir;
		tan1[i2] = sdir;
		tan1[i3] = sdir;
		
		tan2[i1] = tdir;
		tan2[i2] = tdir;
		tan2[i3] = tdir;
		
	}
		
	for(int i = 0;i<iVertises;i++)
	{
		const Vector3& n = akNormals[i];
		const Vector3& t = tan1[i];
			
		// Gram-Schmidt orthogonalize
		Vector3 kTangent3 = (t - n * n.Dot(t)).Unit();

		// Calculate handedness
/*		float inv = (n.Cross(t).Dot(tan2[i]) > 0.0F) ? -1.0F : 1.0F;		
		
		kTangent3 *= inv;
		kTangents.push_back(kTangent3);
		kBiTangents.push_back(n.Cross(kTangent3) );			*/
		
		
		// Calculate handedness
		float inv = (n.Cross(t).Dot(tan2[i]) < 0.0F) ? -1.0F : 1.0F;				
		
		kTangents.push_back(kTangent3);
		kBiTangents.push_back(n.Cross(kTangent3)*inv );			

	}
	
	delete tan1;
	delete tan2;
}

void	Math::GenerateTangents(const Vector3* akVertises,const Vector3* akNormals,const Vector2* akTexCoord,const int* aiFaces,vector<Vector3>& kTangents,vector<Vector3>& kBiTangents,int iFaces)
{
	Vector3 *tan1 = new Vector3[iFaces];
	Vector3 *tan2 = new Vector3[iFaces]; 		
	

	for(int i = 0;i<iFaces;i+=3)
	{
		
		int i1 = aiFaces[i];
		int i2 = aiFaces[i+1];
		int i3 = aiFaces[i+2];
		
		const Vector3& v1 = akVertises[i1];
		const Vector3& v2 = akVertises[i2];
		const Vector3& v3 = akVertises[i3];
		
		const Vector2& w1 = akTexCoord[i1];
		const Vector2& w2 = akTexCoord[i2];
		const Vector2& w3 = akTexCoord[i3];
		
		float x1 = v1.x - v2.x;
		float x2 = v1.x - v3.x;
		float y1 = v1.y - v2.y;
		float y2 = v1.y - v3.y;
		float z1 = v1.z - v2.z;
		float z2 = v1.z - v3.z;
		
		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;
		
		float r = 1.0F / (s1 * t2 - s2 * t1);
		Vector3 sdir(	(t2 * x1 - t1 * x2) * r, 
							(t2 * y1 - t1 * y2) * r,
							(t2 * z1 - t1 * z2) * r);
		Vector3 tdir(	(s1 * x2 - s2 * x1) * r, 
							(s1 * y2 - s2 * y1) * r,
							(s1 * z2 - s2 * z1) * r);

// 		Vector3* bla = (Vector3*)akNormals;
// 		bla[i1] = (v2-v1).Cross(v3-v1).Unit();
// 		bla[i2] = bla[i1];
// 		bla[i3] = bla[i1];


		tan1[i1] = sdir;
		tan1[i2] = sdir;
		tan1[i3] = sdir;
		
		tan2[i1] = tdir;
		tan2[i2] = tdir;
		tan2[i3] = tdir;
		
	}
		
	for(int i = 0;i<iFaces;i++)
	{
		const Vector3& n = akNormals[aiFaces[i]];
		const Vector3& t = tan1[aiFaces[i]];
			
		// Gram-Schmidt orthogonalize
		Vector3 kTangent3 = (t - n * n.Dot(t)).Unit();

		// Calculate handedness
		float inv = (n.Cross(t).Dot(tan2[aiFaces[i]]) < 0.0F) ? -1.0F : 1.0F;
		
		
		kTangents.push_back(kTangent3);
		kBiTangents.push_back(n.Cross(kTangent3)*inv );
	}
	
	delete tan1;
	delete tan2;
}