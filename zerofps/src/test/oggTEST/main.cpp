//#include <gl/glut.h>
#include <al/alut.h>
#include <al/alc.h>
#include <vorbis/vorbisfile.h>

#include <SDL.h> 

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include <io.h>
#include <fcntl.h>

#include "OggMusic.h"





int main(int argc, char** argv) //finaly the main function
{

	
	alutInit(0,NULL); 
	
	OggMusic* Hej= new OggMusic(16,4096);
	Hej->LoadFile("test.ogg");
	
	if(Hej->Play())
	{	
		cout <<"fdsf";
		bool apa=true;
		int i=0;
		/*while(apa)
		{
			
			apa=Hej.Update();
		};*/
		for(i=0; i<300 ;i++)
		{
			if(i==10000)
			{
				cout<< endl <<"apa" <<endl;
				Sleep(1000);
				//Hej.SetVolume(0.1);
			}
			cout<<i;
			
			Hej->Update();
		};
	}		
	delete Hej;
	alutExit();
	return 0;
}



