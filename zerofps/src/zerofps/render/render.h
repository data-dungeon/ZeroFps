#define TEXT_MAX_LENGHT 256
#define FONTWIDTH 32

#ifndef _RENDER_H_
#define _RENDER_H_

#include "render_x.h"
#include <cstring>
#include <iostream>
#include "texturemanager.h"
#include "../engine/engine.pkg"

using namespace std;

class RENDER_API Render {
	private:
		TextureManager* m_pkTexMan;
		char aCurentFont[256];

	public:
		Render(TextureManager* pkTexMan);
		void Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture);
		void Pyra(float x,float y,float z);
		void PrintChar(char cChar);
		void Print(Vector3 kPos,Vector3 kHead,Vector3 kScale,char* aText);
		void SetFont(char* aFont);
};


#endif



