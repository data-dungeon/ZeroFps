#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include <iostream>
#include "TextureManager.h"
#include "Vector3.h"

class Primitives {
	private:
		TextureManager* m_pkTexMan;

	public:
		Primitives(TextureManager* pkTexMan);
		void Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture);
		void Pyra(float x,float y,float z);
		void PrintChar(char cChar);
		void Print(Vector3 kPos,Vector3 kHead,Vector3 kScale,char* aText);
};


#endif



