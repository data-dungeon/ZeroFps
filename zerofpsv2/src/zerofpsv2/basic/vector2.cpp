#include "vector2.h"

const Vector2 Vector2::ZERO(0,0);
const Vector2 Vector2::AXIS_X(1,0);
const Vector2 Vector2::AXIS_Y(0,1);

float& Vector2::operator[] (const int i) 
{
	return *((&x) + i);
}

string Vector2::ToString()
{
	char szVec[128];
	sprintf(szVec, "%f,%f,%f", x,y);
	return string(szVec);	
}
