//#include <gl/glut.h>
#include <al/alut.h>
#include <al/alc.h>
#include <vorbis/vorbisfile.h>


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include <io.h>
#include <fcntl.h>

#include "OggMusic.h"

#define NUM_BUFFERS 2
#define NUM_SOURCES 2
#define NUM_ENVIRONMENTS 1

//ALfloat listenerPos[]={0.0,0.0,4.0};
//ALfloat listenerVel[]={0.0,0.0,0.0};
//ALfloat	listenerOri[]={0.0,0.0,1.0, 0.0,1.0,0.0};

//ALfloat source0Pos[]={ -2.0, 0.0, 0.0};
//ALfloat source0Vel[]={ 0.0, 0.0, 0.0};




ALuint	buffer[NUM_BUFFERS];
ALuint	source[NUM_SOURCES];
ALuint  environment[NUM_ENVIRONMENTS];

int GLwin ;

ALsizei size,freq, bit;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;
ALenum error;


void init(void)
{


	//alListenerfv(AL_POSITION,listenerPos);
	//alListenerfv(AL_VELOCITY,listenerVel);
	//alListenerfv(AL_ORIENTATION,listenerOri);

	alGenBuffers(NUM_BUFFERS,buffer);

	if (alGetError()!=AL_NO_ERROR)
	{
		printf("- Error creating buffers !!\n");
		return;
	}


	alutLoadWAVFile("pong.wav",&format,&data,&size ,&freq, &loop);
	alBufferData(buffer[0],format,data,size,freq);
	ALint hej;
	alGetBufferi(buffer[0], AL_SIZE, &hej);
	printf("%i" ,hej);
	alutUnloadWAV(format,data,size,freq);


	if (alGetError()!=AL_NO_ERROR)
	{
		printf("- Error creating sources !!\n");
		return;
	}

	alGenSources(1,source);
	alSourcef(source[0],AL_PITCH,1.0f);
	alSourcef(source[0],AL_GAIN,1.0f);
	//	alSourcefv(source[0],AL_POSITION,source0Pos);
	//	alSourcefv(source[0],AL_VELOCITY,source0Vel);
	alSourcei(source[0],AL_BUFFER,buffer[0]);
	alSourcei(source[0],AL_LOOPING,AL_TRUE);




	//alSourcei(source[0],AL_BUFFER,buffer[0]);


	if (alGetError()!=AL_NO_ERROR)
	{
		printf("- Error Sourcing !!\n");

	}
}




int main(int argc, char** argv) //finaly the main function
{
	//_setmode( _fileno( stdin ), _O_BINARY );
	//_setmode( _fileno( stdout ), _O_BINARY );

	//initialise openAL
	/*alutInit(0,NULL) ;


	//init() ;

	FILE* MusicFile;

	MusicFile = fopen("test.ogg" , "rb" );
	if(MusicFile==NULL)
	{
		printf("file error");
	}
	else
	{
		OggVorbis_File OggFile;
		if(ov_open(MusicFile,&OggFile ,NULL,0)<0)
			printf("error ov_open");
		else
		{
			long nrstreams = ov_streams(&OggFile);
			printf("number of bitstreams: %i \n ",nrstreams);
			vorbis_info* Info = ov_info(&OggFile, 0);
			printf("freq: %i \n ",Info->rate);
			printf("channels: %i \n",Info->channels);
			//alSourcePlay(source[0]);
			printf("bitrate nominal: %i upper: %i lower: %i\n",
				Info->bitrate_nominal, Info->bitrate_upper, Info->bitrate_lower );

			char ch= getch();

			ov_clear(&OggFile);
			alSourceStop(source[0]);
		}
	};
	alutExit();*/
	
	alutInit(0,NULL); 
	
	OggMusic Hej(12,4096);
	Hej.LoadFile("test3.ogg");
	if(Hej.Play()) cout <<"fdsf";
	bool apa=true;
	while(apa)
	{
		apa=Hej.Update();
	};
			
	alutExit();
	return 0;
}



