#ifndef _VECTOR2_H_
#define _VECTOR2_H_

#include <iostream>
#include <cmath>
#include "basic_x.h"
#include "os.h"

using namespace std;

class BASIC_API Vector2 {
	private:
		
	
	public:
		float x,y;

		Vector2(float fX,float fY) : x(fX), y(fY){ };
		Vector2() {};

		// Assign
		Vector2& operator=(const Vector2 &kOtherV2) {
			x=kOtherV2.x;
			y=kOtherV2.y;
			return *this;
			}	

		void Set(float fXIn, float fYIn) {
			x = fXIn;
			y = fYIn;
			}

	

};

#endif



