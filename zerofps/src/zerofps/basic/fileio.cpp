#include "fileio.h"


char* FileIo::File(const char* acFile) {
	//default only load the file acfile
	strcpy(fil,acFile);				

	//normal files
	if(strncmp(acFile,"file:",5)==0) {
		strcpy(fil,&acFile[5]);				
	}
	
	//files in a pak-file
	//ex:   pak:textures/door.bmp@../data/images.pak
	if(strncmp(acFile,"pak:",4)==0) {
		strcpy(fil,&acFile[4]);				
	}
	
	
	return fil;
}







