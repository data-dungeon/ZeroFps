/**
	\defgroup MadExport MadExport
	\ingroup Tools
	\brief	Maya Exporter.

	MadExport is a Maya Plugin that is used to export data to files that can be imported and used by
	MadMake. 
*/


//#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <map> 
#include <vector> 
using namespace std;

#define EXPORT_NONE		0
#define EXPORT_BONES	1
#define EXPORT_MESH		2
#define EXPORT_ANIM		3
#define EXPORT_ALL		4

#include "madexport.h"

#define NRDEG_IN_ONE_RAD	57.29577951
#define NRRAD_IN_ONE_DEG	0.0174532952

bool	bDeveloperInfo;
char	format_text[4096];

void LogIt(const char *fmt, ...)
{
	if(bDeveloperInfo == false)
		return;	

	va_list		ap;							// Pointer To List Of Arguments

	// Make sure we got something to work with.
	if (fmt == NULL)	return;					

	va_start(ap, fmt);						// Parses The String For Variables
		vsprintf(format_text, fmt, ap);		// And Convert Symbols
	va_end(ap);								// 

	// Now call our print function.
	//cout << format_text << endl;
}

 
float DegToRad(float fAngle)
{
	return fAngle;
//	return (fAngle * NRRAD_IN_ONE_DEG);
}

float RadToDeg(float fAngle)
{
	return fAngle;
//	return (fAngle * NRDEG_IN_ONE_RAD);
} 
 

#pragma warning ( disable : 4786 )

MObject findShader( MObject& setNode )
{
	MFnDependencyNode fnNode(setNode);
	MPlug shaderPlug = fnNode.findPlug("surfaceShader");
			
	if (!shaderPlug.isNull()) {			
		MPlugArray connectedPlugs;
		bool asSrc = false;
		bool asDst = true;
		shaderPlug.connectedTo( connectedPlugs, asDst, asSrc );

		if (connectedPlugs.length() != 1)
	;//		cerr << "Error getting shader\n";
		else 
			return connectedPlugs[0].node();
	}

	LogIt("Error finding surface shader for node %s\n", fnNode.name().asChar());
	return MObject::kNullObj;
}

MStatus MadExport::GetShaderTextureFileName (MString &filename, MObject &set)
{
	MStatus status;

	MFnSet fnSet( set, &status );
	if (status == MStatus::kFailure) {
		status.perror("Unable to lookup shader from set of shaders for object");
		LogIt("Unable to lookup shader from set of shaders for object");
		return status;
    }

	MObject shaderNode = findShader(set);
	if (shaderNode == MObject::kNullObj)
		return (MStatus::kFailure);

	MPlug colorPlug = MFnDependencyNode(shaderNode).findPlug("color", &status);
	if (status == MStatus::kFailure)
		return (status);

	MItDependencyGraph dgIt(colorPlug, MFn::kFileTexture,
		   MItDependencyGraph::kUpstream, 
		   MItDependencyGraph::kBreadthFirst,
		   MItDependencyGraph::kNodeLevel, 
		   &status);

	if (status == MStatus::kFailure)
		return (status);
		
	dgIt.disablePruningOnFilter();

    // If no texture file node was found, just continue.
    //
	if (dgIt.isDone()) {
		LogIt("No texture file node was found in shader");
		return (MStatus::kFailure);
	} 
		  
    // Print out the texture node name and texture file that it references.
    //
	MObject textureNode = dgIt.thisNode();
    MPlug filenamePlug = MFnDependencyNode(textureNode).findPlug("fileTextureName");
    MString textureName;
    filenamePlug.getValue(textureName);

  	MStringArray rgFolders;

	if (strchr (textureName.asChar(), '\\')) {
		textureName.split ('\\', rgFolders);
	} else {
		textureName.split ('/', rgFolders);
	}
	filename = rgFolders[rgFolders.length() -1];
	return (MStatus::kSuccess);
}	

/*
MStatus MadExport::CreateFile(char* filename)
{
	//cout << "opening file " << filename << "\n";
	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		//cout << "unable to open file for writing\n";
		return MStatus::kSuccess;
	}

	LoopFrames();

	fclose(m_pkOutFile);
	return MStatus::kSuccess;
}*/

int MadExport::GetNumOfMeshTriangles(MDagPath path)
{
	int NumOfTriangles;
	MStatus	status;
	MItMeshPolygon piter(path, MObject::kNullObj, &status);
	NumOfTriangles = 0;

	int cTri;

	for ( ; !piter.isDone(); piter.next() ) {
		status = piter.numTriangles (cTri);	
		if (!status) {
			status.perror ("error getting triangle count");
			return status;
		}

		NumOfTriangles += cTri;
		}
 
	return NumOfTriangles;
}

MStatus MadExport::GetSkinClusterWeights(void)
{	
	size_t count = 0;
	float infMax;

	// Iterate through graph and search for skinCluster nodes
	MItDependencyNodes iter( MFn::kSkinClusterFilter );
	for ( ; !iter.isDone(); iter.next() ) {
		MObject object = iter.item();
		count++;

		MFnSkinCluster skinCluster(object);

		MStatus stat;
		unsigned int nInfs = skinCluster.influenceObjects(m_rgInfs, &stat);
		if (!stat) {
			stat.perror("Error getting influence objects");
			continue;
		}

		if (0 == nInfs) {
			stat = MStatus::kFailure;
			stat.perror("Error: No influence objects found.");
			return stat;
		}
			
		// loop through the geometries affected by this cluster
		unsigned int nGeoms = skinCluster.numOutputConnections();
		for (size_t ii = 0; ii < nGeoms; ++ii) {
			unsigned int index = skinCluster.indexForOutputConnection(ii,&stat);
			if (!stat) {
				stat.perror ("Error getting geometry index.");
				return stat;
			}
			
			// get the dag path of the ii'th geometry
			stat = skinCluster.getPathAtIndex(index,m_skinPath);
			if (!stat) {
				stat.perror ("Error getting geometry path.");
				return stat;
			}

			// iterate through the components of this geometry
			MItGeometry gIter(m_skinPath);

			// print out the path name of the skin, vertexCount & influenceCount
			LogIt("Found Skin %s with %d vertices and %d influences.", 
				m_skinPath.partialPathName().asChar(),gIter.count() , nInfs );
			////cout << "found skin: '" << m_skinPath.partialPathName().asChar() << "' with " << gIter.count() <<
			//	" vertices " << "and " << nInfs << " influences\n";
		
			if(m_strMeshName == m_skinPath.partialPathName().asChar()) {
				m_strSkinCluster = skinCluster.name();
	
				// set up the array for all vertices
				if (m_rgWeights) {
					delete m_rgWeights;
				}

				m_rgWeights = new int[gIter.count(&stat)];
				if (!stat) {
					stat.perror ("Error creating array of vertices");
					return stat;
				}

				for ( /* nothing */ ; !gIter.isDone(); gIter.next() ) {
					MObject comp = gIter.component(&stat);
					if (!stat) {
						stat.perror ("Error getting component.");
						return stat;
					}


					// Get the weights for this vertex (one per influence object)
					MFloatArray wts;
					unsigned int infCount;
					stat = skinCluster.getWeights(m_skinPath,comp,wts,infCount);
					if (!stat) {
						stat.perror ("Error getting weights.");
						return stat;
					}


					if (0 == infCount) {
						stat = MStatus::kFailure;
						//cout << "Error: 0 influence objects.\n";
						return stat;
					}


					// find the strongest influencer for this vertex
					infMax = 0;
					m_rgWeights[gIter.index()] = 0;
					for (unsigned int iInf = 0; iInf < infCount ; ++iInf ) {
						int r = wts[iInf];
						if (wts[iInf] > infMax) {
							m_rgWeights[gIter.index()] = iInf;
							infMax = wts[iInf];
						}
					}
				}
			}
		}
	} 

	if (0 == count) {
		//cout << "No skinned meshes found in this scene. Is your mesh bound to a skeleton?\n";
		return MStatus::kFailure;
	} else if (m_skinPath.partialPathName().length() == 0) {
		//cout << "Could not find desired skin cluster '" << m_strSkinCluster.asChar() << "'\n";
		return MStatus::kFailure;
	} else {
		LogIt("Processing skin cluster %s", m_strSkinCluster.asChar());
	}
	return MStatus::kSuccess;
}


MStatus	MadExport::getMesh(void)
{
//	MDagPath		node;
	MDagPath		path;
	MObject			component;
	MSelectionList	list;
	MFnDagNode		fnNode;

//		//cout << nodeFn.name().asChar() << "is selected\n";
	MStatus	status;

//	//cout << "Get MESH:" << endl;

//	MItDependencyNodes iter( MFn::kMesh  );
//	for ( ; !iter.isDone(); iter.next() ) {
	MGlobal::getActiveSelectionList(list);
	for(unsigned int index = 0;  index< list.length(); index++) {
		list.getDagPath(index, path, component);
		fnNode.setObject( path );
		
		//MObject object = iter.item();
		//MFnDagNode fnNode (object);
		//status = fnNode.getPath (path);

		/*if (status == MStatus::kFailure)
		{
			status.perror ("unable to lookup path for child of bone");
			//cout << "unable to lookup path " << endl;
			return (MStatus::kFailure);
		}*/
 
		if(fnNode.isIntermediateObject(&status))
			continue;
 
		status = path.extendToShape();
		if (status != MStatus::kSuccess) {
			// no geometry under this node...
			LogIt( "Node is not geometry so it's ignored\n" ); 
			continue;
		}
  
		MFnMesh fnMesh(path, &status);
		if (status != MStatus::kSuccess) {
			// this object is not a mesh
			LogIt( "Node is not a mesh so it's ignored\n" ); 
			continue;
		}
 
		int instanceNum = 0;
		if (path.isInstanced())
			instanceNum = path.instanceNumber();
  
		MString fullpathname;
		fullpathname = path.partialPathName(&status);
		m_strMeshName = fullpathname;
	 	GetSkinClusterWeights();

		int iNumOfTriangles = GetNumOfMeshTriangles(path);
		fprintf(m_pkOutFile, "Num %d\n", iNumOfTriangles);

		// Get a list of all shaders attached to this mesh
		MObjectArray rgShaders;
		MIntArray rgFaces;
		status = fnMesh.getConnectedShaders (instanceNum, rgShaders, rgFaces);
		if (status == MStatus::kFailure)
		{
			status.perror("Unable to load shaders for mesh");
			return (MStatus::kFailure);
		}
 
		// Create table of textures used by each shader.
		MString texFilename;
		MStringArray rgTextures (rgShaders.length(), "");

		for ( int i=0; i<rgShaders.length(); i++ ) {
			MObject shader = rgShaders[i];

			status = GetShaderTextureFileName (texFilename, shader);
			if (status == MStatus::kFailure) {
		        status.perror ("Unable to retrieve filename of texture");
				LogIt("Unable to retrieve filename of texture");
		        continue;
			}

			rgTextures[i] = texFilename;
			LogIt("Shader %d : %s", i, rgTextures[i].asChar());
			////cout << "Shader " << i << ": " << rgTextures[i].asChar() << endl;
		}


		// Skapa en polygon iterator för objectet.
		MItMeshPolygon piter(path, MObject::kNullObj, &status);
		parsePolySet(piter,rgTextures, rgFaces);
	}

	return MS::kSuccess;


/*
	MStatus	status;

	//cout << "Get MESH:" << endl;

	MItDependencyNodes iter( MFn::kMesh  );
	for ( ; !iter.isDone(); iter.next() ) {
		MObject object = iter.item();
	
		MFnDagNode fnNode (object);
		MDagPath path;
		status = fnNode.getPath (path);

		if (status == MStatus::kFailure)
		{
			status.perror ("unable to lookup path for child of bone");
			//cout << "unable to lookup path " << endl;
			return (MStatus::kFailure);
		}
 
		if(fnNode.isIntermediateObject(&status))
			continue;
 
		status = path.extendToShape();
		if (status != MStatus::kSuccess) {
			// no geometry under this node...
			//cout << "Not geometry" << endl;
			continue;
		}

		MFnMesh fnMesh(path, &status);
		if (status != MStatus::kSuccess) {
			// this object is not a mesh
			//cout << "Not a Mesh" << endl;
			continue;
		}
 
		int instanceNum = 0;
		if (path.isInstanced())
			instanceNum = path.instanceNumber();
  
		MString fullpathname;
		fullpathname = path.fullPathName(&status);
		//cout << object.apiTypeStr() << ":" << fullpathname.asChar() << endl;
	
		int iNumOfTriangles = GetNumOfMeshTriangles(path);
		fprintf(m_pkOutFile, "Num %d\n", iNumOfTriangles);

		// Get a list of all shaders attached to this mesh
		MObjectArray rgShaders;
		MIntArray rgFaces;
		status = fnMesh.getConnectedShaders (instanceNum, rgShaders, rgFaces);
		if (status == MStatus::kFailure)
		{
			status.perror("Unable to load shaders for mesh");
			return (MStatus::kFailure);
		}

		// Create table of textures used by each shader.
		MString texFilename;
		MStringArray rgTextures (rgShaders.length(), "");

		for ( int i=0; i<rgShaders.length(); i++ ) {
			MObject shader = rgShaders[i];


			status = GetShaderTextureFileName (texFilename, shader);
			if (status == MStatus::kFailure) {
		        status.perror ("Unable to retrieve filename of texture");
		        continue;
			}

			rgTextures[i] = texFilename;
			//cout << "Shader " << i << ": " << rgTextures[i].asChar() << endl;
		}


		// Skapa en polygon iterator för objectet.
		MItMeshPolygon piter(path, MObject::kNullObj, &status);
		parsePolySet(piter,rgTextures, rgFaces);
	}

	return MS::kSuccess;
	*/
}


MStatus MadExport::parsePolySet(MItMeshPolygon &meshPoly,MStringArray rgTextures, MIntArray texMap, int iForceBone)
{
	int	iNumOfMissingTextures = 0;

	MStatus	status;

	int cTri;
	MPointArray rgpt;
	MIntArray rgint;
	PtLookupMap ptMap;
	int i;
	int iW;

	for ( ; !meshPoly.isDone(); meshPoly.next() ) {
		status = meshPoly.numTriangles (cTri);	
		if (!status) {
			status.perror ("error getting triangle count");
			return status;
		}

		ptMap.clear();
		for (i=0; i<meshPoly.polygonVertexCount(); i++) {
			ptMap.insert (PtLookupMap::value_type(meshPoly.vertexIndex(i), i) );
		}

		// verify polygon has UV information
		/*if (!meshPoly.hasUVs (&status)) {
			status = MS::kFailure;
			//cout << "polygon is missing UV information\n";
			return status;
		}*/
		 
 		for (int i=0; i < cTri; i++) {

			// for each triangle, first get the triangle data
			rgpt.clear();
			rgint.clear();

			//status = meshPoly.getTriangle (i, rgpt, rgint, MSpace::kObject);
			status = meshPoly.getTriangle (i, rgpt, rgint, MSpace::kWorld);

			if (!status) {
				status.perror ("error getting triangle for mesh poly");
				return status;
			}

			if ((rgpt.length() != 3) || (rgint.length() != 3)) {
				//cout << "3 points not returned for triangle\n";
				status = MS::kFailure;
				return status;
			} 
  
			MVector normal, pt;
			float ucoo,vcoo;

			fprintf(m_pkOutFile, " ");

			// set the bitmap filename for the triangle
			if(m_bIsBaseData) 
			{
				int iTexture = texMap[meshPoly.index()];
				if (iTexture >= 0) {
					if(rgTextures[iTexture] == "") {
						iNumOfMissingTextures++;
						////cout << "Mesh"
						////cout << "Missing Texture on surface" << endl;
						fprintf(m_pkOutFile, "error.bmp ");
						}
					else
						fprintf(m_pkOutFile, "%s ", rgTextures[iTexture].asChar());
					////cout << "Texture : " << iTexture <<"," << rgTextures[iTexture].asChar() << endl;
				}
	 			else 
				{
					//cout << "No Valid Texture: " << endl;
					fprintf(m_pkOutFile, "error.bmp ");
				}
			}
   
			for(int v = 0; v < 3; v++)
			{
				pt = rgpt[v];
				CalculateTriangleVertex (rgint[v], iW,pt, normal, ucoo, vcoo, meshPoly, ptMap);
				if(iForceBone != -1)
					iW = iForceBone;
	 			fprintf(m_pkOutFile, " <%d, %f,%f,%f, %f,%f,%f, %f,%f> ", iW , pt[0] ,pt[1] ,pt[2],
					normal[0], normal[1], normal[2], 
					ucoo, vcoo );
			}
			fprintf(m_pkOutFile, "\n");
			} 
		}

	if(iNumOfMissingTextures) {
		//cout << "Mesh: " << m_strMeshName.asChar() << " had " << iNumOfMissingTextures << " surfaces without textures" << endl;
		}

	return MS::kSuccess;


}

MStatus MadExport::CalculateTriangleVertex (int vt, int &iWeight, MVector &pt, MVector &n, float &u, float &v, MItMeshPolygon &meshPoly, PtLookupMap &ptMap)
{
	MStatus	status;
	int vtLocal;
	int vtUV;

	// get face-relative vertex
	PtLookupMap::iterator mapIt;

	mapIt = ptMap.find(vt);
	if (mapIt != ptMap.end()) {
		vtLocal = (*mapIt).second;
	}

	status = meshPoly.getNormal (vtLocal, n, MSpace::kWorld);
	if (!status && m_bIsBaseData) {
		LogIt("error getting normal for local vertex %d and object vertex %d",vtLocal, vt);
		////cout << "error getting normal for local vertex " << vtLocal << " and object vertex " << vt << "\n";
		return status;
	}

	status = meshPoly.getUVIndex (vtLocal, vtUV, u, v);
	if (!status && m_bIsBaseData) {
		LogIt("error getting UV Index for local vertex %d and object vertex %d",vtLocal, vt);
		////cout << "error getting UV Index for local vertex " << vtLocal << " and object vertex " << vt << "\n";
		return status;
	}

/*	int offset;
	offset = u;
	if (offset >= 1 || offset <= -1) {
		u = u - offset;
	}
	// now bring U into the 0 <= U <= 1 space
	if (u < 0) {
		u = u + 1;
	}
	offset = v;
	if (offset >= 1 || offset <= -1) {
		v = v - offset;
	}
	if (v < 0) {
		v = v + 1;
	}*/



	if (m_rgWeights) {
		iWeight = m_rgWeights[vt];

		// now need to look up appropriate bone for this vertex
		MDagPath path = m_rgInfs[iWeight];
		MFnDependencyNode joint (path.node());
		char *name = (char *)joint.name().asChar();
		NameIntMap::iterator mapIt2;
		mapIt2 = m_kJointMap.find(name);
		if (mapIt2 != m_kJointMap.end()) {
			iWeight = (*mapIt2).second;
		} else {
			// error - bone not found!
			//cout << "Error: can't find weight bone with index " << iWeight << " and name '" << name << "' in joint map\n";
			return MS::kFailure;
		}
	} else {
		iWeight = 0;
	}
 
	return MS::kSuccess;
}


MStatus MadExport::ExportBoneGeometry( )
{
	MStatus status;
  
	BoneList::iterator itBones;
	for (itBones = m_kBoneList.begin(); itBones != m_kBoneList.end(); itBones++)
	{
		MFnDagNode fnJoint = (*itBones)->m_kPath.node(&status);
	
		// go through children looking for geometry
		for (unsigned int i=0; i < fnJoint.childCount (&status); i++) {
			MObject obj = fnJoint.child(i, &status);
			if (status == MStatus::kFailure)
			{
				//cout << "Unable to load child for bone" << endl;
				status.perror("Unable to load child for bone");
				return (MStatus::kFailure);
			}
			MFnDagNode fnNode (obj);
			MDagPath path;
			status = fnNode.getPath (path);
			if (status == MStatus::kFailure)
			{
				//cout << "unable to lookup path for child of bone" << endl;
				status.perror ("unable to lookup path for child of bone");
				return (MStatus::kFailure);
			}
 
			//cout << "Name: " << fnNode.name().asChar() << endl;

			// Have to make the path include the shape below it so that
			// we can determine if the underlying shape node is instanced.
			// By default, dag paths only include transform nodes.
			//
			status = path.extendToShape();
			if (status != MStatus::kSuccess) {
				// no geometry under this node...
				//cout << "No Geo"<< endl;
				continue;
			}
 
			// If the shape is instanced then we need to determine which
			// instance this path refers to.
			//
			int instanceNum = 0;
			if (path.isInstanced())
				instanceNum = path.instanceNumber();

			MFnMesh fnMesh(path, &status);
			if (status != MStatus::kSuccess) {
				// this object is not a mesh
				// no geometry under this node...
				//cout << "No Mesh"<< endl;
				continue;
			}

			// Get a list of all shaders attached to this mesh
			MObjectArray rgShaders;
			MIntArray rgFaces;
			status = fnMesh.getConnectedShaders (instanceNum, rgShaders, rgFaces);
			if (status == MStatus::kFailure)
			{
				status.perror("Unable to load shaders for mesh");
				return (MStatus::kFailure);
			}

			MString texFilename;
			MStringArray rgTextures (rgShaders.length(), "");

			for ( int i=0; i<rgShaders.length(); i++ ) {
				MObject shader = rgShaders[i];


				status = GetShaderTextureFileName (texFilename, shader);
				if (status == MStatus::kFailure) {
		            status.perror ("Unable to retrieve filename of texture");
		            continue;
			    }

				rgTextures[i] = texFilename;
			}

			// now iterate through all polygons and set up that data
			MItMeshPolygon piter(path, MObject::kNullObj, &status);
			parsePolySet(piter, rgTextures, rgFaces, (*itBones)->m_iId);
		}
	}

	return MStatus::kSuccess;
}

MStatus	MadExport::LoopFrames(void)
{
 	int		frameFirst;
	int		frameLast;
	MTime	tmNew;
	MStatus	status;

	// Remember the frame the scene was at so we can restore it later.
	MTime currentFrame = MAnimControl::currentTime();
	MTime startFrame = MAnimControl::minTime();
	MTime endFrame = MAnimControl::maxTime();

	frameFirst = (int) startFrame.as( MTime::uiUnit() );
	frameLast = (int) endFrame.as( MTime::uiUnit() );

	tmNew.setUnit (MTime::uiUnit() );

	for (int i=frameFirst; i<=frameLast; i++)
	{
		fprintf(m_pkOutFile, "Frame %d\n", i);
		tmNew.setValue (i);
		MGlobal::viewFrame( tmNew );

		// Do Things for each frame.
		getMesh();
	}
	MGlobal::viewFrame( currentFrame );

	return status;
}

MStatus MadExport::GetBoneNames(void)
{
	BoneList::iterator itBones;
	NameIntMap::iterator mapIt;

	for (itBones = m_kBoneList.begin(); itBones != m_kBoneList.end(); itBones++)
	{
		mapIt = m_kJointMap.find((*itBones)->m_ucpParentName);
		if (mapIt != m_kJointMap.end()) {
			(*itBones)->m_iParentId = (*mapIt).second;
		}
	}

	return MStatus::kSuccess;
}

MStatus MadExport::GetBoneList(void)
{
	MStatus		status;
	int			iLastJointID = -1;
	m_kJointMap.clear();

	MItDependencyNodes iter( MFn::kJoint);

	for ( ; !iter.isDone(); iter.next() ) {
		MObject kObject = iter.item();
		MDagPath kJointPath;

		MFnDagNode kJoint(kObject);
		char *name = (char *)kJoint.name().asChar();
		NameIntMap::iterator mapIt;
		mapIt = m_kJointMap.find(name);
		
		if (mapIt == m_kJointMap.end()) {
			// this is a new bone; need to add it to the list
			if (kJoint.isFromReferencedFile(&status)) {
					MString nspace = kJoint.parentNamespace (&status);
				}
 
			MadExpBone *bone = new MadExpBone;
			bone->m_iId = ++iLastJointID;
			bone->m_ucpName = strdup(kJoint.name().asChar());
			kJoint.getPath(kJointPath);
			bone->m_kPath = kJointPath;

			// Add this joint to our joint map so we can look it up by name
			m_kJointMap.insert( NameIntMap::value_type(bone->m_ucpName, bone->m_iId) );
			bone->m_iParentId = -1;

			// Pop one up the path stack to get our parent
			kJointPath.pop(1);

	 		MFnDagNode parentNode(kJointPath, &status);
			bone->m_ucpParentName = strdup(parentNode.name().asChar());

			/*if ( !status ) {
				//cout  << "No parent \n";
				status.perror("MFnDagNode constructor");
				return status;
				}
			else {
				char *parentName = (char *)parentNode.name().asChar();
				mapIt = m_kJointMap.find(parentName);
				if (mapIt != m_kJointMap.end()) {
					bone->m_iParentId = (*mapIt).second;
					//cout << "Parent: " << bone->m_iParentId << endl;
				}
				else 
					//cout  << "No parent2 \n";
			}*/

			m_kBoneList.push_back(bone);
			}
		}

	GetBoneNames();

	return MStatus::kSuccess;
}
 

MStatus MadExport::UpdateBoneList (void)
{
	MStatus status;

	BoneList::iterator itBones;
	
	for (itBones = m_kBoneList.begin(); itBones != m_kBoneList.end(); itBones++)
	{
		MFnIkJoint fnJoint = (*itBones)->m_kPath.node(&status);
		if (!status) {
			status.perror("Unable to cast as Transform!");
			return status;
		}

		// Get rotation/translation from the transformation matrix
		//MTransformationMatrix::RotationOrder rotOrder = MTransformationMatrix::kXYZ;

		//double adRotateAxis[3];
		//double adRotate[3];
		//double adJointOrient[3];
		//fnJoint.getScaleOrientation ( adRotateAxis, rotOrder );
		//fnJoint.getRotation ( adRotate, rotOrder );
		//fnJoint.getRotation ( adJointOrient, rotOrder );
	
		//MTransformationMatrix Local;
		//Local.addRotation (adRotateAxis, MTransformationMatrix::kXYZ, MSpace::kTransform); 
		//Local.addRotation (adRotate, MTransformationMatrix::kXYZ, MSpace::kTransform); 
		//Local.addRotation (adJointOrient, MTransformationMatrix::kXYZ, MSpace::kTransform); 
		//kRot = Local.eulerRotation();
		
		MEulerRotation kRot;
		MTransformationMatrix mat = fnJoint.transformationMatrix(&status);
		kRot = mat.eulerRotation();
	  
		MVector kTrans;
		kTrans = fnJoint.translation(MSpace::kTransform);	//kTransform

		(*itBones)->m_kOrient.x = RadToDeg(kRot.x);
		(*itBones)->m_kOrient.y = RadToDeg(kRot.y);
		(*itBones)->m_kOrient.z = RadToDeg(kRot.z);


		(*itBones)->m_kTrans.x = kTrans.x;
		(*itBones)->m_kTrans.y = kTrans.y;
		(*itBones)->m_kTrans.z = kTrans.z;

	}
	return MS::kSuccess;	
}
 
MStatus	MadExport::WriteBoneList(void)
{
	BoneList::iterator itBone;
	
	for(itBone = m_kBoneList.begin(); itBone != m_kBoneList.end(); itBone++)
	{
		(*itBone)->Print(m_pkOutFile);
	}

	return MStatus::kSuccess;
}

MStatus	MadExport::WriteBonePose(void)
{
	BoneList::iterator itBone;
	
	for(itBone = m_kBoneList.begin(); itBone != m_kBoneList.end(); itBone++)
	{
		(*itBone)->PrintPose(m_pkOutFile);
	}

	return MStatus::kSuccess;
}


MStatus	MadExport::Export_SX(const char* filename)
{
	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		//cout << "Unable to open file " << filename << " for writing\n";
		return MStatus::kFailure;
	}

	GetBoneList();

	int		frameFirst;
	int		frameLast;
	MTime	tmNew;
	MStatus	status;
	// Remember the frame the scene was at so we can restore it later.
	MTime currentFrame	= MAnimControl::currentTime();
	MTime startFrame	= MAnimControl::minTime();
	MTime endFrame		= MAnimControl::maxTime();

	frameFirst	= (int) startFrame.as( MTime::uiUnit() );
	frameLast	= (int) endFrame.as( MTime::uiUnit() );

	tmNew.setUnit (MTime::uiUnit() );
	
	tmNew.setValue (frameFirst);
	MGlobal::viewFrame( tmNew );
	UpdateBoneList();
	WriteBoneList();

	fclose(m_pkOutFile);
	return MStatus::kSuccess;
}

MStatus	MadExport::Export_AX(const char* filename)
{
	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		//cout << "Unable to open file " << filename << " for writing\n";
		return MStatus::kFailure;
	}

	GetBoneList();

 	int		frameFirst;
	int		frameLast;
	MTime	tmNew;
	MStatus	status;

	// Remember the frame the scene was at so we can restore it later.
	MTime currentFrame	= MAnimControl::currentTime();
	MTime startFrame	= MAnimControl::minTime();
	MTime endFrame		= MAnimControl::maxTime();

	frameFirst	= (int) startFrame.as( MTime::uiUnit() );
	frameLast	= (int) endFrame.as( MTime::uiUnit() );

	tmNew.setUnit (MTime::uiUnit() );

	for (int i=frameFirst; i<=frameLast; i++)
	{
		fprintf(m_pkOutFile, "Frame %d\n", i);
		tmNew.setValue (i);
		MGlobal::viewFrame( tmNew );

		// Do Things for each frame.
		UpdateBoneList();
		WriteBonePose();
	}
	MGlobal::viewFrame( currentFrame );

	fclose(m_pkOutFile);
	return MStatus::kSuccess;
}
  
MStatus	MadExport::Export_MX(const char* filename)
{
	m_bIsAnimation	= false;
	m_bIsBaseData	= true;

	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		//cout << "Unable to open file " << filename << " for writing\n";
		return MStatus::kFailure;
	}
 
	GetBoneList();
	MTime	tmNew;
	MStatus	status;

	// Remember the frame the scene was at so we can restore it later.
	MTime currentFrame	= MAnimControl::currentTime();
	MGlobal::viewFrame( currentFrame );

	UpdateBoneList();
	getMesh();
//	ExportBoneGeometry();

	fclose(m_pkOutFile);
	return MStatus::kSuccess;
}
 
MStatus	MadExport::Export_FX(const char* filename)
{
	m_bIsAnimation	= true;
	m_bIsBaseData	= false;

	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		//cout << "Unable to open file " << filename << " for writing\n";
		return MStatus::kFailure;
	}

	LoopFrames();

	fclose(m_pkOutFile);
	return MStatus::kSuccess;
}

MStatus MadExport::ShowHelp(void)
{
	//cout << "madexport for ZeroFps." << endl;
	//cout << "'madexport s' to export a skelleton file" << endl;
	//cout << "'madexport a' to export a animation file" << endl;
	//cout << "'madexport m' to export a base file" << endl;
	//cout << "'madexport f' to export a frame file" << endl;
	return MStatus::kSuccess;
}

void MadExport::PrintSelection()
{
	MDagPath		node;
	MObject			component;
	MSelectionList	list;
	MFnDagNode		nodeFn;

	MGlobal::getActiveSelectionList(list);
	for(unsigned int i = 0;  i< list.length(); i++) {
		list.getDagPath(i, node, component);
		nodeFn.setObject( node );
		//cout << nodeFn.name().asChar() << "is selected\n";
		}
}


MadExport::MadExport()
{
	Clear();
}

MadExport::~MadExport()
{

}

void MadExport::Clear()
{
	m_kJointMap.clear();
	m_kBoneList.clear();
	m_bIsAnimation = false;
	m_bIsBaseData = false;
	m_pkOutFile = NULL;
}



// PUBLIC / Interface

void* MadExport::creator() {
    return new MadExport;
}

MStatus MadExport::reader( const MFileObject& file, const MString& options, FileAccessMode mode)
{
	return MS::kSuccess;
}


MStatus MadExport::writer( const MFileObject& file, const MString& options, FileAccessMode mode)
{
	Clear();

	MString FullPathName = file.fullName();
	MString FileExt;
	MString TempName;

//	//cout << "FileName " << FullPathName.asChar() << "\n";
//	//cout << "Options: " << options.asChar() << "\n";

	int iExportFlag = EXPORT_NONE;

	if (options.length() > 0) {
		// Start parsing.
		MStringArray optionList;
		MStringArray theOption;
		options.split(';', optionList);	// break out all the options.
		
		for( int i = 0; i < optionList.length(); ++i ){
			theOption.clear();
			optionList[i].split( '=', theOption );

			if( theOption[0] == MString("Developer") && theOption.length() > 1 ) {
				if( theOption[1] == MString("on"))
					bDeveloperInfo = true;
				if( theOption[1] == MString("off"))
					bDeveloperInfo = false;
				}
			
			LogIt("Option[%d]: %s / %s \n", i, theOption[0].asChar(), theOption[1].asChar());

			if( theOption[0] == MString("madType") && theOption.length() > 1 ) {
				if( theOption[1] == MString("s"))
					iExportFlag = EXPORT_BONES;

				if( theOption[1] == MString("m"))
					iExportFlag = EXPORT_MESH;

				if( theOption[1] == MString("a"))
					iExportFlag = EXPORT_ANIM;

				if( theOption[1] == MString("e"))
					iExportFlag = EXPORT_ALL;
			}
		}
	}

	m_bIsAnimation	= false;
	m_bIsBaseData	= false;

	m_rgWeights = NULL;

	switch(iExportFlag) {
		case EXPORT_BONES:
			FileExt = ".sx";
			FullPathName += FileExt;
			Export_SX(FullPathName.asChar());
			break;
		case EXPORT_MESH:
			FileExt = ".mx";
			FullPathName += FileExt;
			Export_MX(FullPathName.asChar());
			break;
	
		case EXPORT_ANIM:
			FileExt = ".ax";
			FullPathName += FileExt;
			Export_AX(FullPathName.asChar());
			break;

		case EXPORT_NONE:
			//cout << "Exporting Nothing\n";
			ShowHelp();
			break;

		case EXPORT_ALL:
			TempName = FullPathName + ".sx";
			//Export_SX(FullPathName.asChar());
			TempName = FullPathName + ".mx";
			//Export_MX(FullPathName.asChar());
			TempName = FullPathName + ".ax";
			//Export_AX(FullPathName.asChar());

			break;
		}

	LogIt("Real FileName: %s", FullPathName.asChar());

	return MS::kSuccess;
}
 

bool MadExport::haveReadMethod() const
{
	return false;
}
 
bool MadExport::haveWriteMethod() const
{
	return true;
}

MString	MadExport::defaultExtension() const
{
	return "";
}

bool MadExport::canBeOpened () const
{
	return true;
}
 
MPxFileTranslator::MFileKind MadExport::identifyFile( const MFileObject& kFIleName, const char* pcData, short iSize) const
{
    return kCouldBeMyFileType;

	const char* name = kFIleName.name().asChar();
	int iLen = strlen(name);

	if((iLen > 4) && strcmp( name + iLen - 4, ".mad") == 0 )
        return kCouldBeMyFileType;
    else
        return kNotMyFileType;
}
 
// DLL Startup / Shutdown
MStatus initializePlugin( MObject obj )
{ 
	MStatus   status;

    MFnPlugin plugin( obj, "ZeroFps", "1.0", "Any" );
    status = plugin.registerFileTranslator( "MAD" , "none", MadExport::creator,
		"MadExportOptions", "", true );

	if (!status) {
		//cout << "Error :(.\n";
		return status;
	}

    return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj )
{
    MFnPlugin plugin( obj );
	plugin.deregisterFileTranslator( "madus" );
    return MS::kSuccess;
}
 