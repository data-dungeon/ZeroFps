#include "render.h"

Render::Render(TextureManager* pkTexMan) {
	m_pkTexMan=pkTexMan;
	
}

void Render::Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture){
	glPushMatrix();
		
		glTranslatef(kPos.x,kPos.y,kPos.z);	
		glRotatef(kHead.x, 1, 0, 0);
		glRotatef(kHead.y, 0, 1, 0);	
		glRotatef(kHead.z, 0, 0, 1);
		glScalef(kScale.x,kScale.y,kScale.z);

 		glBlendFunc(GL_ONE,GL_ZERO);
 		
  	m_pkTexMan->BindTexture(iTexture);  

    glBegin(GL_QUADS);			
			glColor4f(1.0,1.0,1.0,1.0);  	  
  	  glNormal3f(0,1,0);
    	glTexCoord2f(0.0,0.0);glVertex3f(-.5,-0.5,0);		 
    	glTexCoord2f(1.0,0.0);glVertex3f(.5,-0.5,0);		
 		  glTexCoord2f(1.0,1.0);glVertex3f(.5,0.5,0);    
	  	glTexCoord2f(0.0,1.0);glVertex3f(-0.5,0.5,0);    
		glEnd();			

	glPopMatrix();
}

void Render::Pyra(float x,float y,float z) {
//		glDepthMask(GL_FALSE);
		GLfloat mat_specular[]={1,1,1,1};
		GLfloat mat_shininess[]={10};
		GLfloat mat_ambient[]={1,0,0,0};
  	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	  glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
	  glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);

		glPushMatrix();
		glTranslatef(x,y,z);
// 		glRotatef(i*1, 0.0, 0.1, 0.0);

//		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glBegin(GL_TRIANGLES);				// start drawing a polygon
			glColor4f(0,0,0,1.0);

			glNormal3f(0.0,0.5,0.5);
			glColor3f(1.0,0.0,0.0);
			glVertex3f( 0.0f, .5f, 0.0f);		// Top
			glColor3f(0.0,1.0,0.0);
			glVertex3f( 0.5f,-.5f, .5f);		// Bottom Right
			glColor3f(0.0,0.0,1.0);
			glVertex3f(-.5f,-.5f,.5f);		// Bottom Left


			glNormal3f(0.5,0.5,0.0);
			glColor3f(1.0,0.0,0.0);
			glVertex3f( 0.0f, .5f, 0.0f);		// Top
			glColor3f(0.0,1.0,0.0);
			glVertex3f( .5f,-.5f, .5f);		// Bottom Right
			glColor3f(0.0,0.0,1.0);
			glVertex3f(.5f,-.5f, -.5f);		// Bottom Left

			glNormal3f(0.0,0.5,-0.5);
			glColor3f(1.0,0.0,0.0);
			glVertex3f( 0.0f, .5f, 0.0f);		// Top
			glColor3f(0.0,0.0,1.0);
			glVertex3f( .5f,-.5f, -.5f);		// Bottom Right
			glColor3f(0.0,1.0,0.0);
			glVertex3f(-.5f,-.5f, -.5f);		// Bottom Left

			glNormal3f(-0.5,0.5,0.0);
		  glColor3f(1.0,0.0,0.0);
			glVertex3f( 0.0f, .5f, 0.0f);		// Top
			glColor3f(0.0,1.0,0.0);
			glVertex3f(-.5f,-.5f, -.5f);		// Bottom Right
			glColor3f(0.0,0.0,1.0);
			glVertex3f(-.5f,-.5f, .5f);		// Bottom Left

		glEnd();					// we're done with the polygon
		glPopMatrix();
//		glDepthMask(GL_TRUE);
}

void Render::PrintChar(char cChar) {
//	cout<<"CHAR: "<<cChar<<endl;
	
	int texwidth=FONTWIDTH*16;	
	int pos=int(cChar)*FONTWIDTH;		
	float glu=1.0/texwidth;				//opengl texture cordinats is 0-1
	float width=glu*FONTWIDTH;
	
	float y=1.0-(float(int(pos/texwidth)*FONTWIDTH)*glu+width);
	float x=float(pos%texwidth)*glu;//+width/2;
	
	
//	cout<<"Att texture position:"<<pos<<" "<<x<<" "<<y<<endl;
	
	
	
	glBlendFunc(GL_ONE,GL_ALPHA); 		
 	m_pkTexMan->BindTexture(aCurentFont);  

	glPushMatrix();
	  glBegin(GL_QUADS);			
			glColor4f(1.0,1.0,1.0,1.0);  	  
 	  	glNormal3f(0,1,0);
 	  
	   	glTexCoord2f(x				,y);				glVertex3f(-.5,-0.5,0);		 
  	 	glTexCoord2f(x+width	,y);				glVertex3f(.5,-0.5,0);		
	  	glTexCoord2f(x+width	,y+width);	glVertex3f(.5,0.5,0);    
	  	glTexCoord2f(x				,y+width);	glVertex3f(-0.5,0.5,0);    
		glEnd();				
	glPopMatrix();
	
	m_pkTexMan->BindTexture(0);
	
/*	char aTexture[255]="data/textures/text/ .bmp";
		
	aTexture[19]=cChar;
				
	if(cChar=='+')			
		strcpy(aTexture,"data/textures/text/plus.bmp");
	if(cChar=='-')			
		strcpy(aTexture,"data/textures/text/minus.bmp");	
		
				
	if(aTexture[19]!=' ')
		Quad(Vector3(0,0,0),Vector3(0,0,0),Vector3(1,1,1),m_pkTexMan->Load(aTexture));*/


}

void Render::Print(Vector3 kPos,Vector3 kHead,Vector3 kScale,char* aText) {
	char paText[TEXT_MAX_LENGHT];
	
	strcpy(paText,aText);
	
	glPushMatrix();
		
		glTranslatef(kPos.x,kPos.y,kPos.z);	
		glRotatef(kHead.x, 1, 0, 0);
		glRotatef(kHead.y, 0, 1, 0);	
		glRotatef(kHead.z, 0, 0, 1);
		glScalef(kScale.x,kScale.y,kScale.z);
		
		int i=0;
		while(paText[i]!='\0') {
			PrintChar(paText[i]);
			glTranslatef(1,0,0);
		
			i++;
		}

	glPopMatrix();
}


void Render::SetFont(char* aFont) {
	strcpy(aCurentFont,aFont);

}

void Render::Line(Vector3 kPos1,Vector3 kPos2)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
		glVertex3f(kPos1.x,kPos1.y,kPos1.z);
		glVertex3f(kPos2.x,kPos2.y,kPos2.z);		
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void Render::SetColor(Vector3 kColor)
{
	glColor3f(kColor.x,kColor.y,kColor.z);
}

void Render::DrawHM(HeightMap *kmap) {
	glPushMatrix();
	Vector3 p1;
	Vector3 p2;
	Vector3 p3;
	Vector3 p4;
	
	glTranslatef(-50,-10,-50);
	m_pkTexMan->BindTexture("file:../data/textures/grass.bmp");
	
	GLfloat mat_specular[]={1,1,1,1};
	GLfloat mat_shininess[]={10};
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	
	int x;	
//	glFrontFace(GL_CW);// jag ritar medsols så d så
	for(int z=0;z<kmap->m_iHmSize-1;z++){

		x=0;		
		glBegin(GL_TRIANGLE_STRIP);		
		p1=Vector3(x,kmap->verts[z*kmap->m_iHmSize+x].height,z);				
	   glNormal3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x].normal);
 		glTexCoord2f(0.0,0.0);glVertex3fv((float*)&p1);
 		
		p2=Vector3(x,kmap->verts[(z+1)*kmap->m_iHmSize+x].height,z+1);			 		
	   glNormal3fv((float*)&kmap->verts[(z+1)*kmap->m_iHmSize+x].normal);
 		glTexCoord2f(0.0,1.0);glVertex3fv((float*)&p2);		 				
 		
		for(x=1;x<kmap->m_iHmSize-1;x++) {			
			p3=Vector3(x+1,kmap->verts[z*kmap->m_iHmSize+x+1].height,z);							
		   glNormal3fv((float*)&kmap->verts[z*kmap->m_iHmSize+x+1].normal);
  			glTexCoord2f(x,0.0);glVertex3fv((float*)&p3);    			
  			
			p4=Vector3(x+1,kmap->verts[(z+1)*kmap->m_iHmSize+x+1].height,z+1);
		   glNormal3fv((float*)&kmap->verts[(z+1)*kmap->m_iHmSize+x+1].normal);  			
  			glTexCoord2f(x,1.0);glVertex3fv((float*)&p4);    				
	
		}
		glEnd();
	}
//	glFrontFace(GL_CCW);
	
	//this draw the normals of the heightmap
/*	
	SetColor(Vector3(255,0,0));
	for(int z=0;z<kmap->m_iHmSize;z++)
		for(int x=0;x<kmap->m_iHmSize;x++) {
			p1=Vector3(x,kmap->verts[z*kmap->m_iHmSize+x].height,z);
 			Line(p1,p1+kmap->verts[z*kmap->m_iHmSize+x].normal);
	
		}
*/	
	glPopMatrix();			
}


void Render::DrawConsole(char* m_aCommand,vector<char*>* m_kText) {
	SetFont("file:../data/textures/text/console.bmp");

	Quad(Vector3(0,0,-.50),Vector3(0,0,0),Vector3(1,1,1),m_pkTexMan->Load("file:../data/textures/background.bmp"));
	
	Print(Vector3(-0.45,-0.45,-0.499),Vector3(0,0,0),Vector3(.03,.03,.03),m_aCommand);		
	
	for(int i=0;i<22;i++) {
		if((*m_kText)[i]!=NULL){
			Print(Vector3(-0.45,-0.40+i/(float)25,-0.499),Vector3(0,0,0),Vector3(.03,.03,.03),(*m_kText)[i]);		
		}
	}
}



