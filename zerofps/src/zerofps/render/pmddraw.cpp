#include "pmddraw.h"
#include "../oglload/zfpsgl.h"

PmdDraw::PmdDraw()
{
	iTexture = NULL;
}

PmdDraw::~PmdDraw()
{
	if(iTexture)	
		delete [] iTexture;
}

bool PmdDraw::LoadDataAndTextures(char* filename)
{
	Load(filename);
	return true;
}

void PmdDraw::draw()
{
	glColor3f(1,1,1);
	
	glPushMatrix();

	for(int i=0; i<head.num_of_triangles; i++)
	{
//	glBindTexture(GL_TEXTURE_2D,iTexture[triangle[i].texture_num]);

	glBegin(GL_TRIANGLES);
   		glTexCoord2fv((float*) &texture_coo[triangle[i].texcoo_index[0]]);				
				glVertex3fv((float*) &frames[0].vertex[triangle[i].vertex_index[0]]);
   		glTexCoord2fv((float*) &texture_coo[triangle[i].texcoo_index[1]]);		
				glVertex3fv((float*) &frames[0].vertex[triangle[i].vertex_index[1]]);
		glTexCoord2fv((float*) &texture_coo[triangle[i].texcoo_index[2]]);		
				glVertex3fv((float*) &frames[0].vertex[triangle[i].vertex_index[2]]);
	glEnd();

	}

	glPopMatrix();
}



