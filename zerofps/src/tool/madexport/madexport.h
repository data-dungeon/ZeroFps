#ifndef _MADEXP_
#define _MADEXP_

#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnTransform.h>
#include <maya/MEulerRotation.h>
#include <maya/MItDag.h>
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MPxCommand.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MFnPlugin.h>
#include <maya/MArgList.h>
#include <maya/MFnCamera.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnLight.h>
#include <maya/MColor.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MPlugArray.h>
#include <maya/MPointArray.h>
#include <maya/MVectorArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MIntArray.h>
#include <maya/MSelectionList.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSet.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MAnimControl.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MFnIkJoint.h>

using namespace std;

#include "bone.h" 

struct lstr {
	bool operator() (char *s1, char *s2) const
	{
		return (strcmp(s1, s2)< 0);
	}
};


typedef map<char *, int, struct lstr>	NameIntMap;
typedef vector<MadExpBone *>			BoneList;
typedef map<int, int>					PtLookupMap;

class MadExport : public MPxCommand
{
private:
 	NameIntMap		m_kJointMap;
	BoneList		m_kBoneList;
	bool			m_bIsAnimation;
	bool			m_bIsBaseData;

	FILE*			m_pkOutFile;

	MStatus			CalculateTriangleVertex (int vt, MVector &pt, MVector &n, float &u, float &v, MItMeshPolygon &meshPoly, PtLookupMap &ptMap);
	MStatus			parsePolySet(MItMeshPolygon &meshPoly,MStringArray rgTextures, MIntArray texMap);
	MStatus			getMesh(void);
	MStatus			LoopFrames(void);

	MStatus			GetBoneNames(void);
	MStatus			GetBoneList(void);		// Get Bone list.
	MStatus			WriteBoneList(void);	// Write Bone list to output file.
	MStatus			WriteBonePose(void);
	
	MStatus			UpdateBoneList(void);	// Updates bone list for current frame.
	
	MStatus			GetShaderTextureFileName (MString &filename, MObject &set);
	int				GetNumOfMeshTriangles(MDagPath path);

	MStatus			CreateFile(char* filename);
	MStatus			ShowHelp(void);

	MStatus			Export_SX(char* filename);
	MStatus			Export_AX(char* filename);
	MStatus			Export_MX(char* filename);
	MStatus			Export_FX(char* filename);

public:
    MStatus			doIt( const MArgList& args );
    static void*	creator();

};

#endif