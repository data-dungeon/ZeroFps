#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <map> 
#include <vector> 

#include "madexport.h"

#define NRDEG_IN_ONE_RAD	57.29577951
#define NRRAD_IN_ONE_DEG	0.0174532952

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
			cerr << "Error getting shader\n";
		else 
			return connectedPlugs[0].node();
	}

	cout << "Error finding surface shader for node " << fnNode.name().asChar() << "\n";
	return MObject::kNullObj;
}

MStatus MadExport::GetShaderTextureFileName (MString &filename, MObject &set)
{

	MStatus status;

	MFnSet fnSet( set, &status );
	if (status == MStatus::kFailure) {
		status.perror("Unable to lookup shader from set of shaders for object");
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
		//cout << "no textures found for "<< endl;	//  << colorPlug.name() << "\n";
		return (MStatus::kSuccess);
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
	cout << "Found texture file: '" << filename.asChar() << "'\n";
	return (MStatus::kSuccess);
}	

MStatus MadExport::CreateFile(char* filename)
{

	cout << "opening file " << filename << "\n";
	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		cout << "unable to open file for writing\n";
		return MStatus::kSuccess;
	}

	LoopFrames();

	fclose(m_pkOutFile);
	return MStatus::kSuccess;
}

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
	cout << "GetSkinClusterWeights: 1" << endl;
	MItDependencyNodes iter( MFn::kSkinClusterFilter );
	for ( ; !iter.isDone(); iter.next() ) {
		MObject object = iter.item();
		count++;

		MFnSkinCluster skinCluster(object);

		// did the user specify a skin cluster?
		m_strSkinCluster = skinCluster.name();

		// otherwise proceed with analyzing this skinCluster
		// get the list of influence objects. store all influence objects in the array "m_rgInfs"
		// only used if the user decides not to export all bones

		MStatus stat;
		unsigned int nInfs = skinCluster.influenceObjects(m_rgInfs, &stat);
		if (!stat) {
			stat.perror("Error getting influence objects");
			continue;
		}

		cout << "GetSkinClusterWeights: 2" << endl;
		if (0 == nInfs) {
			stat = MStatus::kFailure;
			stat.perror("Error: No influence objects found.");
			return stat;
		}
		cout << "GetSkinClusterWeights: 3" << endl;
			
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
			cout << "found skin: '" << m_skinPath.partialPathName().asChar() << "' with " << gIter.count() << " vertices " << "and " << nInfs << " influences\n";
							
			// set up the array for all vertices
			if (m_rgWeights) {
				delete m_rgWeights;
			}

			m_rgWeights = new int[gIter.count(&stat)];
			if (!stat) {
				stat.perror ("Error creating array of vertices");
				return stat;
			}
			cout << "GetSkinClusterWeights: 4" << endl;

			for ( /* nothing */ ; !gIter.isDone(); gIter.next() ) {
				MObject comp = gIter.component(&stat);
				if (!stat) {
					stat.perror ("Error getting component.");
					return stat;
				}
				cout << "GetSkinClusterWeights: 5" << endl;


				// Get the weights for this vertex (one per influence object)
				MFloatArray wts;
				unsigned int infCount;
				stat = skinCluster.getWeights(m_skinPath,comp,wts,infCount);
				if (!stat) {
					stat.perror ("Error getting weights.");
					return stat;
				}
				cout << "GetSkinClusterWeights: 6" << endl;


				if (0 == infCount) {
					stat = MStatus::kFailure;
					cout << "Error: 0 influence objects.\n";
					return stat;
				}
				cout << "GetSkinClusterWeights: 7" << endl;


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
	cout << "GetSkinClusterWeights: 100" << endl;


	if (0 == count) {
		cout << "No skinned meshes found in this scene. Is your mesh bound to a skeleton?\n";
		return MStatus::kFailure;
	} else if (m_skinPath.partialPathName().length() == 0) {
		cout << "Could not find desired skin cluster '" << m_strSkinCluster.asChar() << "'\n";
		return MStatus::kFailure;
	} else {
		cout << "Processing skin cluster '" << m_strSkinCluster.asChar() << "'\n";
	}
	return MStatus::kSuccess;
}


MStatus	MadExport::getMesh(void)
{
	MStatus	status;

	MItDependencyNodes iter( MFn::kMesh  );
	for ( ; !iter.isDone(); iter.next() ) {
		MObject object = iter.item();
	
		MFnDagNode fnNode (object);
		MDagPath path;
		status = fnNode.getPath (path);

		if (status == MStatus::kFailure)
		{
			status.perror ("unable to lookup path for child of bone");
			cout << "unable to lookup path " << endl;
			return (MStatus::kFailure);
		}
 
		if(fnNode.isIntermediateObject(&status))
			continue;
 
		status = path.extendToShape();
		if (status != MStatus::kSuccess) {
			// no geometry under this node...
			cout << "Not geometry" << endl;
			continue;
		}

		MFnMesh fnMesh(path, &status);
		if (status != MStatus::kSuccess) {
			// this object is not a mesh
			cout << "Not a Mesh" << endl;
			continue;
		}
 
		int instanceNum = 0;
		if (path.isInstanced())
			instanceNum = path.instanceNumber();
  
		MString fullpathname;
		fullpathname = path.fullPathName(&status);
		cout << object.apiTypeStr() << ":" << fullpathname.asChar() << endl;
	
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
			cout << "Shader " << i << ": " << rgTextures[i].asChar() << endl;
		}


		// Skapa en polygon iterator för objectet.
		MItMeshPolygon piter(path, MObject::kNullObj, &status);
		parsePolySet(piter,rgTextures, rgFaces);
	}

	return MS::kSuccess;
}


MStatus MadExport::parsePolySet(MItMeshPolygon &meshPoly,MStringArray rgTextures, MIntArray texMap, int iForceBone)
{
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
			cout << "polygon is missing UV information\n";
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
				cout << "3 points not returned for triangle\n";
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
					fprintf(m_pkOutFile, "%s ", rgTextures[iTexture].asChar());
				}
				else 
				{
					cout << "No Valid Texture: " << endl;
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
		cout << "error getting normal for local vertex " << vtLocal << " and object vertex " << vt << "\n";
		return status;
	}

	status = meshPoly.getUVIndex (vtLocal, vtUV, u, v);
	if (!status && m_bIsBaseData) {
		cout << "error getting UV Index for local vertex " << vtLocal << " and object vertex " << vt << "\n";
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
			cout << "Error: can't find weight bone with index " << iWeight << " and name '" << name << "' in joint map\n";
			return MS::kFailure;
		}
	} else {
		iWeight = 0;
	}
 
	return MS::kSuccess;
}


MStatus MadExport::ExportBoneGeometry( )
{
	cout << "ExportBoneGeometry: 1" << endl;
	MStatus status;
  
	BoneList::iterator itBones;
	for (itBones = m_kBoneList.begin(); itBones != m_kBoneList.end(); itBones++)
	{
		cout << "Bone: ";
		MFnDagNode fnJoint = (*itBones)->m_kPath.node(&status);
	
		// go through children looking for geometry
		for (unsigned int i=0; i < fnJoint.childCount (&status); i++) {
			cout << "/";
			MObject obj = fnJoint.child(i, &status);
			if (status == MStatus::kFailure)
			{
				cout << "Unable to load child for bone" << endl;
				status.perror("Unable to load child for bone");
				return (MStatus::kFailure);
			}
			MFnDagNode fnNode (obj);
			MDagPath path;
			status = fnNode.getPath (path);
			if (status == MStatus::kFailure)
			{
				cout << "unable to lookup path for child of bone" << endl;
				status.perror ("unable to lookup path for child of bone");
				return (MStatus::kFailure);
			}
 
			cout << "Name: " << fnNode.name().asChar() << endl;

			// Have to make the path include the shape below it so that
			// we can determine if the underlying shape node is instanced.
			// By default, dag paths only include transform nodes.
			//
			status = path.extendToShape();
			if (status != MStatus::kSuccess) {
				// no geometry under this node...
				cout << "No Geo"<< endl;
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
				cout << "No Mesh"<< endl;
				continue;
			}

			cout << "processing mesh " << fnMesh.name().asChar() << "\n";

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
			cout << "Parent: " << (*itBones)->m_iParentId << endl;
		}
		else 
			cout  << "No parent2 \n";

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
			cout << "Bone found: " << kJoint.name().asChar();
			if (kJoint.isFromReferencedFile(&status)) {
					MString nspace = kJoint.parentNamespace (&status);
					cout << " (from referenced file " << nspace.asChar() << ")\n";
				} else {
					cout << "";
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
				cout  << "No parent \n";
				status.perror("MFnDagNode constructor");
				return status;
				}
			else {
				char *parentName = (char *)parentNode.name().asChar();
				mapIt = m_kJointMap.find(parentName);
				if (mapIt != m_kJointMap.end()) {
					bone->m_iParentId = (*mapIt).second;
					cout << "Parent: " << bone->m_iParentId << endl;
				}
				else 
					cout  << "No parent2 \n";
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
		MTransformationMatrix::RotationOrder rotOrder = MTransformationMatrix::kXYZ;

		double adRotateAxis[3];
		double adRotate[3];
		double adJointOrient[3];
		fnJoint.getScaleOrientation ( adRotateAxis, rotOrder );
		fnJoint.getRotation ( adRotate, rotOrder );
		fnJoint.getRotation ( adJointOrient, rotOrder );
	
		MTransformationMatrix Local;
		Local.addRotation (adRotateAxis, MTransformationMatrix::kXYZ, MSpace::kTransform); 
		Local.addRotation (adRotate, MTransformationMatrix::kXYZ, MSpace::kTransform); 
		Local.addRotation (adJointOrient, MTransformationMatrix::kXYZ, MSpace::kTransform); 
		
		MEulerRotation kRot;
		//kRot = Local.eulerRotation();
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


MStatus	MadExport::Export_SX(char* filename)
{
	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		cout << "unable to open file " << filename << " for writing\n";
		return MStatus::kSuccess;
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

MStatus	MadExport::Export_AX(char* filename)
{
	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		cout << "unable to open file " << filename << " for writing\n";
		return MStatus::kSuccess;
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
 
MStatus	MadExport::Export_MX(char* filename)
{
	m_bIsAnimation	= false;
	m_bIsBaseData	= true;

	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		cout << "unable to open file " << filename << " for writing\n";
		return MStatus::kSuccess;
	}
 
	cout << "Export_MX: 0" << endl;
	GetSkinClusterWeights();
	cout << "Export_MX: 1" << endl;
	GetBoneList();
	cout << "Export_MX: 2" << endl;


	MTime	tmNew;
	MStatus	status;

	// Remember the frame the scene was at so we can restore it later.
	MTime currentFrame	= MAnimControl::currentTime();
	MGlobal::viewFrame( currentFrame );

	UpdateBoneList();
	cout << "Export_MX: 3" << endl;
	getMesh();
//	ExportBoneGeometry();
	cout << "Export_MX: 4" << endl;

	fclose(m_pkOutFile);
	return MStatus::kSuccess;
}
 
MStatus	MadExport::Export_FX(char* filename)
{
	m_bIsAnimation	= true;
	m_bIsBaseData	= false;

	m_pkOutFile = fopen(filename, "w+");
	if (!m_pkOutFile) {
		cout << "unable to open file " << filename << " for writing\n";
		return MStatus::kSuccess;
	}

	LoopFrames();

	fclose(m_pkOutFile);
	return MStatus::kSuccess;
}

MStatus MadExport::ShowHelp(void)
{
	cout << "madexport for ZeroFps." << endl;
	cout << "'madexport m' to export a base file" << endl;
	cout << "'madexport f' to export a frame file" << endl;
	cout << "'madexport s' to export a skelleton file" << endl;
	cout << "'madexport a' to export a animation file" << endl;
	return MStatus::kSuccess;
}



// PUBLIC / Interface

void* MadExport::creator() {
    return new MadExport;
}

MStatus MadExport::doIt( const MArgList& args ) 
{
	m_bIsAnimation	= false;
	m_bIsBaseData	= false;

	m_rgWeights = NULL;

	if(strcmp(args.asString( 0 ).asChar(), "help") == 0)
		return ShowHelp();

	if(strcmp(args.asString( 0 ).asChar(), "f") == 0)
		return Export_FX("c:\\test.fx");

	if(strcmp(args.asString( 0 ).asChar(), "m") == 0)
		return Export_MX("c:\\test.mx");

	if(strcmp(args.asString( 0 ).asChar(), "s") == 0)
		return Export_SX("c:\\test.sx");
 
	if(strcmp(args.asString( 0 ).asChar(), "a") == 0)
		return Export_AX("c:\\test.ax");


	return MS::kSuccess;
}


// DLL Startup / Shutdown

MStatus initializePlugin( MObject obj )
{ 
    MFnPlugin plugin( obj, "ZeroFps", "1.0", "Any" );
    plugin.registerCommand( "madexport", MadExport::creator );
    return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj )
{
    MFnPlugin plugin( obj );
    plugin.deregisterCommand( "madexport" );

    return MS::kSuccess;
}
