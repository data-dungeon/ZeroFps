// PmdMake.cpp : Defines the entry point for the console application.
//

#include <cstring>
#include <iostream>
#include <string>
#include "mad.h"
#include "mdl.h"
#include "3ds.h"
#include "xxx.h"
#include "halflife.h"
#include "script.h"

using namespace std;

#define PATHSEPARATOR(c) ( (c) == '\\'  ||  (c)  ==  '/')

#define MADMAKE_VERSION	"0.1"

// FileName Support Functions
string GetFileExt(string FileName)
{
	string strExt = "";
	char* ext = strchr(FileName.c_str(), '.');
	if(ext != NULL) {
		strExt = ext;
		}
	return strExt;
}

string RemoveExtension(string strFile)
{
	string StrNewName = strFile;

	int iLenght = StrNewName.size();
	while(iLenght > 0 && StrNewName[iLenght] != '.')
	{
		iLenght--;
		if(StrNewName[iLenght] == '/')
			return StrNewName;
	}

	if(StrNewName[iLenght] == '.')
		StrNewName.erase(iLenght,500);

	return StrNewName;
}

class MadMake
{
private:
	string ucaInFile;
	string ucaOutFile;
	string ucaTextureNames;
    bool bQuit;

public:
	MadMake();
	bool Parse_CmdLine(int argc, char* argv[]);
	IMadImport* GetImportObject(string FileName);
	void Print_Help(void);
	void Print_Version(void);
	void Run(int argc, char* argv[]);
};

MadMake::MadMake()
{
	bQuit = false;
	ucaInFile = "none";
	ucaOutFile = "none";
	ucaTextureNames = "none";
}

void MadMake::Print_Version(void)
{
	cout << "madmake V " << MADMAKE_VERSION << endl;
	cout << "madmake [-option] [infile] [outfile]" << endl;
	cout << "Use option help for more information." << endl;
}

void MadMake::Print_Help(void)
{
	cout << "madmake [-option] [infile] [outfile]" << endl;
	cout << endl;

}

IMadImport* MadMake::GetImportObject(string FileName)
{
	IMadImport* pkImport = NULL;

	string strExt = GetFileExt(ucaInFile);
	
	if( strExt == ".mdl" )
		pkImport = new ModellMD1;
		//pkImport = new ModellHalfLife;

	if( strExt == ".md2" )
		pkImport = new ModellMD2;

	if( strExt == ".3ds" )
		pkImport = new Modell3DS;

	if( strExt == ".mm" )
		pkImport = new ModellXXX;
	
	return pkImport;
}

bool MadMake::Parse_CmdLine(int argc, char* argv[])
{
	if(argc == 1) {
		Print_Version();
		return 0;
		}

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
                cout << "Unknown parameter" << *argv[ iCurArg ] << endl;
            }
        } 
		else 
		{
			// No more args left.
 			FirstParameterIndex = iCurArg;
           break;  
        }
    }

	if(bQuit)
		return 0;

	if(iCurArg < argc)
		ucaInFile = argv[iCurArg];
	if((iCurArg + 1) < argc)
		ucaOutFile = argv[iCurArg + 1];
	if((iCurArg + 2) < argc)
		ucaTextureNames = argv[iCurArg + 2];
	
//	strcpy(ucaInFile, "progs/player.mdl");
//	strcpy(ucaOutFile, "player.mad");
 
	cout << "ucaInFile: "  << ucaInFile << endl;
	cout << "ucaOutFile: " << ucaOutFile << endl;
	cout << "ucaTextureNames: " << ucaTextureNames << endl;

	return 1;
}

void MadMake::Run(int argc, char* argv[])
{
	
	if(!Parse_CmdLine(argc, argv))
		return;
	if(bQuit)	
		return;

	if(argc < 3) 
	{
		cout << "To few argument" << endl;
		return;
	}
	
	IMadImport* pkImport = GetImportObject(ucaInFile.c_str());
	if(pkImport == NULL) {
		cout << "No importer found for object of type " << ucaInFile << endl;
		return;
		}

	MadExporter madexp;
	pkImport->Read(ucaInFile.c_str());
	pkImport->Export(&madexp,ucaTextureNames.c_str());
	madexp.Save_MAD(ucaOutFile.c_str());
	madexp.Save_SD("test.sd");
	madexp.Save_MD(0, "test.md");
	madexp.Save_AD(0, "test.ad");
}


int main(int argc, char* argv[])
{
	MadMake	mad;
	mad.Run(argc, argv);
	return 0;
}
