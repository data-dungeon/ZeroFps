#ifndef _SDL_PMDDRAW_H_
#define _SDL_PMDDRAW_H_

#include <string>
#include "render_x.h"
#include "pmd.h"
using namespace std;

class RENDER_API PmdDraw : public pmd_c
{
public:
	PmdDraw();
	~PmdDraw();

	int*	iTexture;
	bool	LoadDataAndTextures(char* filename);
	void	draw(void);
};

#endif


