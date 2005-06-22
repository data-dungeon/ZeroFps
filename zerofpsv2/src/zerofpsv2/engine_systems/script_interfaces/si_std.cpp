#include "si_std.h"
#include <cmath>                    // for trigonometry functions
#include "../../basic/zfvfs.h"
#include "../../engine/entitymanager.h"
#include "../../engine/zerofps.h"

ZFScriptSystem* StdLua::g_pkScript;
ZFVFileSystem*	StdLua::g_pkVFS;
ZeroFps*	StdLua::g_pkZeroFps;

extern EntityManager* g_pkObjMan;

namespace StdLua
{
/**	\class SIStd 
 		\ingroup NewSi
   */

void Init(ZFScriptSystem* pkScript, ZFVFileSystem* pkVFS,ZeroFps* pkZeroFps)
{
	cout << "Add SI: Std" << endl;

	g_pkScript = pkScript;
	g_pkVFS = pkVFS;
	g_pkZeroFps = pkZeroFps;
	
	pkScript->ExposeFunction("Normalize",	StdLua::NormalizeLua);
	pkScript->ExposeFunction("VectorSub",	StdLua::VectorSubLua);
	
	pkScript->ExposeFunction("Print",		StdLua::PrintLua);
	pkScript->ExposeFunction("Sin",			StdLua::SinLua);	
	pkScript->ExposeFunction("Cos",			StdLua::CosLua);		
	pkScript->ExposeFunction("Tan",			StdLua::TanLua);
	pkScript->ExposeFunction("Pow",			StdLua::PowLua);
	pkScript->ExposeFunction("GetTicks",	StdLua::GetTicksLua);
	pkScript->ExposeFunction("GetFilesInFolder", StdLua::GetFilesInFolderLua);		
}

/** \fn Print( szText )
	\relates SIStd
	\brief Print text to std out device.
	\param szText Text to print.
*/

int VectorSubLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 2)
	{
		vector<TABLE_DATA> vkData;
		g_pkScript->GetArgTable(pkLua, 1, vkData);

		Vector3 kPos1 = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData));
			
		//vector<TABLE_DATA> vkData;
		vkData.clear();
		g_pkScript->GetArgTable(pkLua, 3, vkData);  //FUNKAR FAN INTE

		Vector3 kPos2 = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData));			
			
		kPos1.Print();
		kPos2.Print();
			
		Vector3 kPos = kPos1 - kPos2;

		TABLE_DATA temp;
		vkData.clear();

		temp.bNumber = true;
		temp.pData = new double(kPos.x);
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double(kPos.y);
		vkData.push_back(temp);
		
		temp.bNumber = true;
		temp.pData = new double(kPos.z);
		vkData.push_back(temp);						
// 		temp.bNumber = true;
// 		temp.pData = new double;
// 		(*(double*) temp.pData) = kPos.x;
// 		vkData.push_back(temp);
// 
// 		temp.bNumber = true;
// 		temp.pData = new double;
// 		(*(double*) temp.pData) = kPos.y;
// 		vkData.push_back(temp);
// 
// 		temp.bNumber = true;
// 		temp.pData = new double;
// 		(*(double*) temp.pData) = kPos.z;
// 		vkData.push_back(temp);

		g_pkScript->AddReturnValueTable(pkLua, vkData);

		//cout<< "YEAH! :)" << endl;

		return 1;
	}
	return 0;

}

int NormalizeLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) == 1)
	{
		vector<TABLE_DATA> vkData;
		g_pkScript->GetArgTable(pkLua, 1, vkData);

		Vector3 kPos = Vector3(
			(float) (*(double*) vkData[0].pData),
			(float) (*(double*) vkData[1].pData),
			(float) (*(double*) vkData[2].pData));

		kPos.Normalize();

		TABLE_DATA temp;

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = kPos.x;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = kPos.y;
		vkData.push_back(temp);

		temp.bNumber = true;
		temp.pData = new double;
		(*(double*) temp.pData) = kPos.z;
		vkData.push_back(temp);

		g_pkScript->AddReturnValueTable(pkLua, vkData);

		//cout<< "YEAH! :)" << endl;

		return 1;
	}
	return 0;
}

int PrintLua(lua_State* pkLua)
{
	for(int i=0;i<g_pkScript->GetNumArgs(pkLua);i++)
	{
		//char acText[256];
		string strText;
		g_pkScript->GetArgString(pkLua, i, strText);
	
		cout<<strText;
	}
	
	cout<<endl;
	
	return 1;
}

/**	\fn Sin( Angle )
 		\relates SIStd
		\brief Gets sin of angle.
		\param Angle Angle in radians.
		\return Return Sin of angle.
*/
int SinLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1)
	{
		cout<<"Sin takes 1 argument(s)"<<endl;				
		return 0;
	}	
		
	double dTal;
	g_pkScript->GetArgNumber(pkLua, 0, &dTal);
	
	g_pkScript->AddReturnValue(pkLua,sin(dTal));
	
	return 1;
}

/**	\fn Pow( val,pow )
 		\relates SIStd
		\brief Exponent.
		\param val input.
		\param pow power value
		\return Return val power of pow
*/
int PowLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 2)
	{
		cout<<"Pow takes 2 argument(s)"<<endl;				
		return 0;
	}	
		
	double dTal;
	double dPow;
	g_pkScript->GetArgNumber(pkLua, 0, &dTal);
	g_pkScript->GetArgNumber(pkLua, 1, &dPow);
	
	g_pkScript->AddReturnValue(pkLua,pow(dTal,dPow));
	
	return 1;
}

/**	\fn Cos( Angle )
 		\relates SIStd
		\brief Gets cos of angle.
		\param Angle Angle in radians.
		\return Return cos of angle.
*/
int CosLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1)
	{
		cout<<"Cos takes 1 argument(s)"<<endl;		
		return 0;
	}	
		
	double dTal;
	g_pkScript->GetArgNumber(pkLua, 0, &dTal);
	
	g_pkScript->AddReturnValue(pkLua,cos(dTal));
	
	return 1;
}

int GetTicksLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 0)
	{
		cout<<"GetTicks takes 0 argument(s)"<<endl;
		return 0;
	}
		
	g_pkScript->AddReturnValue(pkLua,g_pkZeroFps->GetEngineTime() );
	
	return 1;		
}
		
/**	\fn Tan( Angle )
 		\relates SIStd
		\brief Gets tan of angle.
		\param Angle Angle in radians.
		\return Return tan of angle.
*/
int TanLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1)
	{
		cout<<"Tan takes 1 argument(s)"<<endl;
		return 0;
	}	
		
	double dTal;
	g_pkScript->GetArgNumber(pkLua, 0, &dTal);
	
	g_pkScript->AddReturnValue(pkLua,tan(dTal));
	
	return 1;
}

// 1:st argument = Path to folder with files (char*)
/**	\fn GetFilesInFolder( szPath )
 		\relates SIStd
		\brief Returns table of all files in the given path.
		\param szPath The path to list the files. 
		\return Table with filenames.
*/
int GetFilesInFolderLua(lua_State* pkLua)
{
	if(g_pkScript->GetNumArgs(pkLua) != 1)
	{
		printf("StdLua::GetFilesInFolderLua Failed!: Incorrect number of argumets\n");
		return 0;
	}

	char acPath[256];
	g_pkScript->GetArgString(pkLua, 0, acPath);

	string strRealPath;

	unsigned int i;

	vector<TABLE_DATA> vkData;

	vector<string> vkFileNames;
	g_pkVFS->ListDir(&vkFileNames, acPath, false);

	unsigned int uiNumFiles = vkFileNames.size();

	// Lägg till antalet filer som första argument
	TABLE_DATA temp;
	temp.pData = new double;
	(*(double*) temp.pData) = uiNumFiles;
	temp.bNumber = true;
	vkData.push_back(temp);

	for(i=0; i<uiNumFiles; i++)
	{
		unsigned int legth = vkFileNames[i].size();

		if(legth > 0)
		{
			TABLE_DATA temp;
			temp.pData = new char[legth+1];
			strcpy( (char*) temp.pData, vkFileNames[i].c_str());
			temp.bNumber = false;
			vkData.push_back(temp);
		}
	}

	g_pkScript->AddReturnValueTable(pkLua, vkData);

	return 1;
	
}

}







