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

#include <stdio.h>
#include <assert.h>
#include <map> 
#include <vector> 

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

/** \brief	Exports MAD from Maya
	 \ingroup MadExport
*/
class MadExport : public MPxFileTranslator
{
private:
 	NameIntMap		m_kJointMap;
	BoneList		m_kBoneList;
	bool			m_bIsAnimation;
	bool			m_bIsBaseData;

	FILE*			m_pkOutFile;

	MStatus			CalculateTriangleVertex (int vt, int& iWeight, MVector &pt, MVector &n, float &u, float &v, MItMeshPolygon &meshPoly, PtLookupMap &ptMap);
	MStatus			parsePolySet(MItMeshPolygon &meshPoly,MStringArray rgTextures, MIntArray texMap, int iForceBone = -1);
	MStatus			getMesh(void);
	MStatus			LoopFrames(void);

	MStatus			GetBoneNames(void);
	MStatus			GetBoneList(void);		// Get Bone list.
	MStatus			WriteBoneList(void);	// Write Bone list to output file.
	MStatus			WriteBonePose(void);
	
	MStatus			UpdateBoneList(void);	// Updates bone list for current frame.
	
	MStatus			GetShaderTextureFileName (MString &filename, MObject &set);
	int				GetNumOfMeshTriangles(MDagPath path);

//	MStatus			CreateFile(char* filename);
	MStatus			ShowHelp(void);

	MStatus			Export_SX(const char* filename);
	MStatus			Export_AX(const char* filename);
	MStatus			Export_MX(const char* filename);
	MStatus			Export_FX(const char* filename);

	MStatus			ExportBoneGeometry();

	MStatus			GetSkinClusterWeights(void);
	MString			m_strSkinCluster;	
	MDagPathArray	m_rgInfs;	
	MDagPath		m_skinPath;	
	int				*m_rgWeights;	// for each vertex, store index of influence joint

	MString			m_strMeshName;				// Name of exported mesh
	void			PrintSelection();

	void Clear();

public:
	MadExport();
	~MadExport();

//    MStatus			doIt( const MArgList& args );
    static void*	creator();

	MStatus reader( const MFileObject& file, const MString& options, FileAccessMode mode);
	MStatus writer( const MFileObject& file, const MString& options, FileAccessMode mode);

	bool haveReadMethod() const;
    bool haveWriteMethod() const;
	bool canBeOpened () const;

	MString		defaultExtension() const;

	MPxFileTranslator::MFileKind identifyFile( const MFileObject& kFIleName, const char* pcData, short iSize) const;

};


#endif