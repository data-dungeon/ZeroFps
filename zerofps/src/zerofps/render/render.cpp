#include "render.h"

Render::Render(TextureManager* pkTexMan) {
	m_pkTexMan=pkTexMan;
	
	m_iSlicesize=32;	//grid size of lod tiles
	m_iDetail=25;//height meens greater detail att longer range	
}

void Render::Quad(Vector3 kPos,Vector3 kHead,Vector3 kScale,int iTexture){
	glPushMatrix();
		
	glTranslatef(kPos.x,kPos.y,kPos.z);	
	glRotatef(kHead.x, 1, 0, 0);
	glRotatef(kHead.y, 0, 1, 0);	
	glRotatef(kHead.z, 0, 0, 1);
	glScalef(kScale.x,kScale.y,kScale.z);

//	glBlendFunc(GL_ONE,GL_ZERO);
 		
  	m_pkTexMan->BindTexture(iTexture);  

   glBegin(GL_QUADS);			
	glColor4f(1.0,1.0,1.0,1.0);  	  
  	glNormal3f(1,0,0);
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
	
	
	
//	glBlendFunc(GL_ONE,GL_ALPHA); 		
 	m_pkTexMan->BindTexture(aCurentFont);  

	glPushMatrix();
	  glBegin(GL_QUADS);			
//			glColor4f(1.0,1.0,1.0,1.0);  	  
 	  	glNormal3f(0,0,1);
 	  
	   	glTexCoord2f(x				,y);				glVertex3f(-.5,-0.5,0);		 
  	 	glTexCoord2f(x+width	,y);				glVertex3f(.5,-0.5,0);		
	  	glTexCoord2f(x+width	,y+width);	glVertex3f(.5,0.5,0);    
	  	glTexCoord2f(x				,y+width);	glVertex3f(-0.5,0.5,0);    
		glEnd();				
	glPopMatrix();
	
//	m_pkTexMan->BindTexture(0);
	
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


/*
void Render::DrawHMQT(HeightMap *kmap) {
	glPushMatrix();
	glTranslatef(-50,-10,-50);
	SetColor(Vector3(255,255,255));
	
	DrawQuad(kmap->m_kCenter,kmap->m_iHmSize/2);
//	Dot(kmap->m_iHmSize/2,kmap->m_kCenter->height,kmap->m_iHmSize/2);	
	
	glPopMatrix();
}

void Render::DrawQuad(HM_vert* vert,int width) {
	Dot(0,vert->height,0);


	int i=0;
	for(int q=-1;q<2;q++){
		for(int w=-1;w<2;w++) {
			if(q==0 && w==0) {
			}else {
				if(vert->childs[i]!=NULL){
				glPushMatrix();
					glTranslatef(width*q,0,width*w);
					DrawQuad(vert->childs[i],width/2);
				glPopMatrix();				
				}
			}
		}
	}
	

/*
	for(int i=0;i<8;i++) {
		if(vert->childs[i]!=NULL){
			glPushMatrix();	
				
				DrawQuad
		}	
	}
}
*/

void Render::Dot(float x,float y,float z) {
	Line(Vector3(x,y,z),Vector3(x+0.05,y+0.05,z+0.05));

}




void Render::DrawConsole(char* m_aCommand,vector<char*>* m_kText) {
	SetFont("file:../data/textures/text/console.bmp");

	Quad(Vector3(0,0,-1.1),Vector3(0,0,0),Vector3(2.2,2.2,2.2),m_pkTexMan->Load("file:../data/textures/background.bmp"));
	
	Print(Vector3(-0.90,-0.90,-1),Vector3(0,0,0),Vector3(.06,.06,.06),m_aCommand);		
	
	for(int i=0;i<22;i++) {
		if((*m_kText)[i]!=NULL){
			Print(Vector3(-0.90,-0.80+i/(float)13,-1),Vector3(0,0,0),Vector3(.06,.06,.06),(*m_kText)[i]);		
		}
	}
}

//this funktion calculates the texture cordinat for a subtexture in a 1024x1024 texture
void Render::GiveTexCor(float &iX,float &iY,int iNr) {	
	iX=(iNr%4);	
	iY=(iNr-(iNr%4))/4;

	iX*=0.25;
	iY*=0.25;
	iY=1-iY;
	
//	cout<<"X: "<<iX<< "  Y: "<<iY<<endl;
}


