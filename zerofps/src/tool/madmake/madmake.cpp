// PmdMake.cpp : Defines the entry point for the console application.
//

#include <string>
#include <iostream>
#include "pmd.h"
#include "mad.h"
#include "qpack.h"

using namespace std;

#define PMDMAKE_VERSION	"0.1"

int Read3DS(char *filename, pmd_c* to);

/*
	madmake [-options] [infile] [outfile]

	Om inga argument ges så skrivs hjälp texten ut.

	-?
	-h
	-help
		Listar help texten.

	[infile]
		Anger infile som ska konveteras. Filändelse avgör
		vilken typ av format filen är. 

	[outfile]
		Namnet påfilen som ska skapas. Blir .mad format men 
		filändelsen ska anges ändå. Infile måste anges om
		outfile ska användas.


*/

char ucaInFile[255];
char ucaOutFile[255];
char ucaTextureNames[255];

bool Print_Help(void)
{
	cout << "pmdmake V " << PMDMAKE_VERSION << endl;
	cout << "Copyright (C) 2002 DUD.  All Rights Reserved.\n";
	cout << endl;
	cout << "pmdmake [-options] [infile] [outfile]" << endl;

	return true;
}

bool ReadQuakeSeriesModell(pmd_c* pmd, char* filename);

int main(int argc, char* argv[])
{
    bool bQuit = false;
	int FirstParameterIndex = 0;

    // Parse commandline options
    int iCurArg;
    for( iCurArg = 1; iCurArg < argc; ++iCurArg ) {
		// Check if its a option switch.
        if( *argv[ iCurArg ] == '-' ) 
		{
            if( !stricmp( argv[ iCurArg ], "-?" ) ||
				!stricmp( argv[ iCurArg ], "-h" ) ||
				!stricmp( argv[ iCurArg ], "-help" ) )
			{
				Print_Help();
				bQuit = true;
			}
	
			else if (!stricmp( argv[ iCurArg ], "-c" ) ) 
			{
                cout << "Ett -c :)" << endl;
			}
			
			else 
			{
                Print_Help();
				bQuit = true;
            }
        } 
		else 
		{
			// No more args left.
			FirstParameterIndex = iCurArg;
            break;  
        }
    }

	if(bQuit)	return 0;

	if(argc < 3) 
	{
		cout << "To few argument" << endl;
		return 0;
	}
	

	strcpy(ucaInFile, argv[iCurArg]);
	strcpy(ucaOutFile, argv[iCurArg + 1]);

	
	//strcpy(ucaInFile, "tris.md2");
	//strcpy(ucaOutFile, "quad.mad");
	strcpy(ucaTextureNames, "testtex.tga");

	cout << "InFile: " << ucaInFile << endl;
	cout << "ucaOutFile: " << ucaOutFile << endl;

	char* ext = strchr(ucaInFile, '.');
	
	pmd_c Test;
	
	if(strcmp(ext, ".3ds") == 0)
		Read3DS( ucaInFile ,&Test);
	if(strcmp(ext, ".md2") == 0)
	{
		g_PakFileSystem.RegisterPak("c:\\spel\\quake2\\baseq2\\pak0.pak");
		g_PakFileSystem.RegisterPak("c:\\spel\\quake2\\baseq2\\pak1.pak");
		ReadQuakeSeriesModell(&Test,ucaInFile);
	}

	MadExporter madexp;
	madexp.ImportPMD(&Test);
	madexp.Save(ucaOutFile);

	return 0;

}
