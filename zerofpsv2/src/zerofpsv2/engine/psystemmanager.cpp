#include "psystemmanager.h"
#include "../engine_systems/propertys/psproperties/colorpsprop.h"
#include "../engine_systems/propertys/psproperties/texturepsprop.h"
#include "../engine_systems/propertys/psproperties/movepsprop.h"
#include "../engine_systems/propertys/psproperties/blendpsprop.h"
#include "../engine_systems/propertys/psproperties/sizepsprop.h"
#include "../engine_systems/propertys/psproperties/lightpsprop.h"
#include "../engine_systems/propertys/psproperties/alphatestprop.h"

#include "../basic/zfobjectmanger.h"
#include "../basic/zfini.h"
#include "../render/texturemanager.h"

#include <iostream>
	using namespace std;

PSystemManager *PSystemManager::m_pkInstance;

// ------------------------------------------------------------------------------------------

PSystemManager::PSystemManager()
{
	m_pkInstance = this;
}

// ------------------------------------------------------------------------------------------

PSystem* PSystemManager::GetPSSystem ( string kPSName )
{
	// Look for the wanted PSystem
	map<string, PSystemType>::iterator kIterator = m_kPSystemTypes.find ( kPSName );

	// If the PSystem wasn't found
	if ( kIterator == m_kPSystemTypes.end() )
		// try to load PSystem
		if ( !LoadNewPSystem (kPSName) )
		{	
			cout << "Warning! Couln't find PSystem type " << kPSName << endl;
			return 0;
		}

	// Create new PSystem
	PSystem *pkPS = new PSystem( &m_kPSystemTypes[kPSName] );

	// Add PSystemProperties
	for ( unsigned int i = 0;
			i < m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_kProperties.size();
			i++ )
	{

		// Color property
		if ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_kProperties[i] == "Color" )
		{
			pkPS->AddPSProperty ( new ColorPSProp ( pkPS ) );
			
			// Add color array!
			pkPS->SetColors (  CreateVerticeColors ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_iMaxParticles )  );
		}

		// Texture property
		if ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_kProperties[i] == "Texture" )
		{
			pkPS->AddPSProperty ( new TexturePSProp ( pkPS, 
										 m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_uiTexture) );
			// Add texture coordinates
			pkPS->SetTexCoords( CreateTexCoords(m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_iMaxParticles) );

		}

		// Blending property
		if ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_kProperties[i] == "Blend" )
			pkPS->AddPSProperty ( new BlendPSProp ( 
										 m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_iBlendDST,
										 m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_iBlendSRC) );

		// Change Size property
		if ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_kProperties[i] == "Size" )
			pkPS->AddPSProperty ( new SizePSProp ( pkPS ) );

		// Alphatest property
		if ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_kProperties[i] == "AlphaTest" )
			pkPS->AddPSProperty ( new AlphaTestPSProp ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_uiAlphaTest ) );

		// Light property
		pkPS->AddPSProperty ( new LightPSProp ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_bLightOn ) );

		// Move property
		pkPS->AddPSProperty ( new MovePSProp ( pkPS ) );	

	}


	// Create vertices for PS
	pkPS->SetVertices( CreateVertices(&m_kPSystemTypes[kPSName]) );
	// Create indices for PS
	pkPS->SetIndices ( CreateIndices(m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_iMaxParticles) );

	// Set max nr of particles in ParticleSystem
	pkPS->SetParticles ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_iMaxParticles );

	// if PSystem loops in infinity, make it have particles from start
	if ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_fLifeTime == -1 )
		pkPS->TimeoffSet();

	return pkPS;

}

// ------------------------------------------------------------------------------------------

float* PSystemManager::CreateTexCoords (int iParticles)
{
	float* pfTexCoords = new float[6 + iParticles * 6];

	for ( int i = 0; i < iParticles * 6; i += 6 )
	{
		pfTexCoords[i] = 0.5f;
		pfTexCoords[i+1] = -0.50f;

		pfTexCoords[i+2] = 1.7f;
		pfTexCoords[i+3] = 1;

		pfTexCoords[i+4] = -0.7f;
		pfTexCoords[i+5] = 1;
	}

	return pfTexCoords;
}

// ------------------------------------------------------------------------------------------

float* PSystemManager::CreateVertices ( PSystemType *kPSystemType )
{
	return new float[9 + kPSystemType->m_kPSystemBehaviour.m_iMaxParticles * 9];
}

// ------------------------------------------------------------------------------------------

float* PSystemManager::CreateVerticeColors ( int iParticles )
{
	return new float[12 + iParticles * 12];
}

// ------------------------------------------------------------------------------------------

unsigned int* PSystemManager::CreateIndices ( int iParticles )
{
	unsigned int* pfIndices = new unsigned int[3 + iParticles * 3];

	for ( int i = 0; i < 3 + iParticles * 3; i++)
		pfIndices[i] = i;


	return pfIndices;
}

// ------------------------------------------------------------------------------------------


bool PSystemManager::LoadNewPSystem ( string kName )
{
	string kLoadName = "../data/psystems/" + kName + ".zps";

	// Open file
	if( !m_kIniLoader.Open(kLoadName.c_str(), 0) )		
		return false;

	// Create new PSystemType
	PSystemType kNewType;

	LoadData		  ( &kNewType );
	SetProperties ( &kNewType );

	// Calculate max number of particles the PSystem can emitt
	kNewType.m_kPSystemBehaviour.m_iMaxParticles = 
		kNewType.m_kParticleBehaviour.m_fLifeTime /
		kNewType.m_kPSystemBehaviour.m_fParticlesPerSec;

	cout << "PSystemtype " << kName << " loaded!" << endl;

	// Add new type
	m_kPSystemTypes[kName] = kNewType;

	return true;

}

// ------------------------------------------------------------------------------------------

bool PSystemManager::LoadData ( PSystemType *pkPSType )
{
	// See if the file really contains a PSystem
	if( !m_kIniLoader.SectionExist("ParticleSystem") )
	{
		cout << "This file doesn't contain a PSystem!!" << endl;
		return false;
	}


	// Load sizedata

	// start width
	if( m_kIniLoader.KeyExist("start_size", "width") )
		pkPSType->m_kParticleBehaviour.m_kStartSize.x = m_kIniLoader.GetFloatValue("start_size", "width");
	else
		pkPSType->m_kParticleBehaviour.m_kStartSize.x = 1;

	// start height
	if( m_kIniLoader.KeyExist("start_size", "height") )
		pkPSType->m_kParticleBehaviour.m_kStartSize.y = m_kIniLoader.GetFloatValue("start_size", "height");
	else
		pkPSType->m_kParticleBehaviour.m_kStartSize.y = 1;

	// Random startsize value
	if( m_kIniLoader.KeyExist("start_size", "random") )
		pkPSType->m_kParticleBehaviour.m_iStartSizeRandom = m_kIniLoader.GetFloatValue("start_size", "random");
	else
		pkPSType->m_kParticleBehaviour.m_iStartSizeRandom = 0;


	// end width
	if( m_kIniLoader.KeyExist("end_size", "width") )
		pkPSType->m_kParticleBehaviour.m_kEndSize.x = m_kIniLoader.GetFloatValue("end_size", "width");
	else
		pkPSType->m_kParticleBehaviour.m_kEndSize.x = pkPSType->m_kParticleBehaviour.m_kStartSize.x;
	
	// end height
	if( m_kIniLoader.KeyExist("end_size", "height") )
		pkPSType->m_kParticleBehaviour.m_kEndSize.y = m_kIniLoader.GetFloatValue("end_size", "height");
	else
		pkPSType->m_kParticleBehaviour.m_kEndSize.y = pkPSType->m_kParticleBehaviour.m_kStartSize.y;

	// Random endsize value
	if( m_kIniLoader.KeyExist("end_size", "random") )
		pkPSType->m_kParticleBehaviour.m_iEndSizeRandom = m_kIniLoader.GetFloatValue("end_size", "random");
	else
		pkPSType->m_kParticleBehaviour.m_iEndSizeRandom = 0;




	// Particles / second
	if( m_kIniLoader.KeyExist("particle_lifetime", "time") )
		pkPSType->m_kParticleBehaviour.m_fLifeTime = m_kIniLoader.GetFloatValue("particle_lifetime", "time");
	else
		pkPSType->m_kParticleBehaviour.m_fLifeTime = 1;


	
	// Blending
	if( m_kIniLoader.KeyExist("blend", "src") )
		pkPSType->m_kPSystemBehaviour.m_iBlendSRC = GL_BlendSRC[m_kIniLoader.GetIntValue("blend", "src")];
	else
		pkPSType->m_kPSystemBehaviour.m_iBlendSRC = 0;
	
	if( m_kIniLoader.KeyExist("blend", "dst") )
		pkPSType->m_kPSystemBehaviour.m_iBlendDST = GL_BlendDST[m_kIniLoader.GetIntValue("blend", "dst")];
	else
		pkPSType->m_kPSystemBehaviour.m_iBlendDST = 0;



	// Start RGBA
	if( m_kIniLoader.KeyExist("start_color", "r") )
		pkPSType->m_kParticleBehaviour.m_kStartColor.r = m_kIniLoader.GetFloatValue("start_color", "r");
	else
		pkPSType->m_kParticleBehaviour.m_kStartColor.r =1;

	if( m_kIniLoader.KeyExist("start_color", "g") )
		pkPSType->m_kParticleBehaviour.m_kStartColor.g = m_kIniLoader.GetFloatValue("start_color", "g");
	else
		pkPSType->m_kParticleBehaviour.m_kStartColor.g = 1;

	if( m_kIniLoader.KeyExist("start_color", "b") )
		pkPSType->m_kParticleBehaviour.m_kStartColor.b = m_kIniLoader.GetFloatValue("start_color", "b");
	else
		pkPSType->m_kParticleBehaviour.m_kStartColor.b = 1;

	if( m_kIniLoader.KeyExist("start_color", "a") )
		pkPSType->m_kParticleBehaviour.m_kStartColor.a = m_kIniLoader.GetFloatValue("start_color", "a");
	else
		pkPSType->m_kParticleBehaviour.m_kStartColor.a = 1;
	


	// End RGBA
	if( m_kIniLoader.KeyExist("end_color", "r") )
		pkPSType->m_kParticleBehaviour.m_kEndColor.r = m_kIniLoader.GetFloatValue("end_color", "r");
	else
		pkPSType->m_kParticleBehaviour.m_kEndColor.r = pkPSType->m_kParticleBehaviour.m_kStartColor.r;

	if( m_kIniLoader.KeyExist("end_color", "g") )
		pkPSType->m_kParticleBehaviour.m_kEndColor.g = m_kIniLoader.GetFloatValue("end_color", "g");
	else
		pkPSType->m_kParticleBehaviour.m_kEndColor.g = pkPSType->m_kParticleBehaviour.m_kStartColor.g;

	if( m_kIniLoader.KeyExist("end_color", "b") )
		pkPSType->m_kParticleBehaviour.m_kEndColor.b = m_kIniLoader.GetFloatValue("end_color", "b");
	else
		pkPSType->m_kParticleBehaviour.m_kEndColor.b = pkPSType->m_kParticleBehaviour.m_kStartColor.b;

	if( m_kIniLoader.KeyExist("end_color", "a") )
		pkPSType->m_kParticleBehaviour.m_kEndColor.a = m_kIniLoader.GetFloatValue("end_color", "a");
	else
		pkPSType->m_kParticleBehaviour.m_kEndColor.a = pkPSType->m_kParticleBehaviour.m_kStartColor.a;



	// Start area, (random size start area)
		// Start outer
	if( m_kIniLoader.KeyExist("start_area", "start_outer_x") )
		pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.x = m_kIniLoader.GetFloatValue("start_area", "start_outer_x");
	else
		pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.x = 0;

	if( m_kIniLoader.KeyExist("start_area", "start_outer_y") )
		pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.y = m_kIniLoader.GetFloatValue("start_area", "start_outer_y");
	else
		pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.y = 0;

	if( m_kIniLoader.KeyExist("start_area", "start_outer_z") )
		pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.z = m_kIniLoader.GetFloatValue("start_area", "start_outer_z");
	else
		pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.z = 0;

	// start inner
	if( m_kIniLoader.KeyExist("start_area", "start_inner_x") )
		pkPSType->m_kPSystemBehaviour.m_kStart_InnerStartArea.x = m_kIniLoader.GetFloatValue("start_area", "start_inner_x");
	else
		pkPSType->m_kPSystemBehaviour.m_kStart_InnerStartArea.x = 0;

	if( m_kIniLoader.KeyExist("start_area", "start_inner_y") )
		pkPSType->m_kPSystemBehaviour.m_kStart_InnerStartArea.y = m_kIniLoader.GetFloatValue("start_area", "start_inner_y");
	else
		pkPSType->m_kPSystemBehaviour.m_kStart_InnerStartArea.y = 0;

	if( m_kIniLoader.KeyExist("start_area", "start_inner_z") )
		pkPSType->m_kPSystemBehaviour.m_kStart_InnerStartArea.z = m_kIniLoader.GetFloatValue("start_area", "start_inner_z");
	else
		pkPSType->m_kPSystemBehaviour.m_kStart_InnerStartArea.z = 0;

		// end outer
	if( m_kIniLoader.KeyExist("start_area", "end_outer_x") )
		pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.x = m_kIniLoader.GetFloatValue("start_area", "end_outer_x");
	else
		pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.x = 0;

	if( m_kIniLoader.KeyExist("start_area", "end_outer_y") )
		pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.y = m_kIniLoader.GetFloatValue("start_area", "end_outer_y");
	else
		pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.y = 0;

	if( m_kIniLoader.KeyExist("start_area", "end_outer_z") )
		pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.z = m_kIniLoader.GetFloatValue("start_area", "end_outer_z");
	else
		pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.z = 0;

		// end inner
	if( m_kIniLoader.KeyExist("start_area", "end_inner_x") )
		pkPSType->m_kPSystemBehaviour.m_kEnd_InnerStartArea.x = m_kIniLoader.GetFloatValue("start_area", "end_inner_x");
	else
		pkPSType->m_kPSystemBehaviour.m_kEnd_InnerStartArea.x = 0;

	if( m_kIniLoader.KeyExist("start_area", "end_inner_y") )
		pkPSType->m_kPSystemBehaviour.m_kEnd_InnerStartArea.y = m_kIniLoader.GetFloatValue("start_area", "end_inner_y");
	else
		pkPSType->m_kPSystemBehaviour.m_kEnd_InnerStartArea.y = 0;

	if( m_kIniLoader.KeyExist("start_area", "end_inner_z") )
		pkPSType->m_kPSystemBehaviour.m_kEnd_InnerStartArea.z = m_kIniLoader.GetFloatValue("start_area", "end_inner_z");
	else
		pkPSType->m_kPSystemBehaviour.m_kEnd_InnerStartArea.z = 0;




		// Start type
	if( m_kIniLoader.KeyExist("start_area", "type") )
		pkPSType->m_kPSystemBehaviour.m_bCirkularStart = m_kIniLoader.GetBoolValue("start_area", "type");
	else
		pkPSType->m_kPSystemBehaviour.m_bCirkularStart = false;



	// Particles / second
	if( m_kIniLoader.KeyExist("particles_persecond", "time") )
		pkPSType->m_kPSystemBehaviour.m_fParticlesPerSec = m_kIniLoader.GetFloatValue("particles_persecond", "time");
	else
		pkPSType->m_kPSystemBehaviour.m_fParticlesPerSec = 0.2f;


	// Particles from start
	if( m_kIniLoader.KeyExist("particles_fromstart", "count") )
		pkPSType->m_kPSystemBehaviour.m_iParticlesFromStart = m_kIniLoader.GetIntValue("particles_fromstart", "count");
	else
		pkPSType->m_kPSystemBehaviour.m_iParticlesFromStart = 0;


	// Light
	if( m_kIniLoader.KeyExist("light", "on") )
		pkPSType->m_kPSystemBehaviour.m_bLightOn = m_kIniLoader.GetBoolValue("light", "on");
	else
		pkPSType->m_kPSystemBehaviour.m_bLightOn = false;


	// texture
	if ( m_kIniLoader.KeyExist("texture", "file") )
	{
		TextureManager* m_pkTexMan = static_cast<TextureManager*>(g_ZFObjSys.GetObjectPtr("TextureManager"));	

		pkPSType->m_kPSystemBehaviour.m_uiTexture = m_pkTexMan->Load( m_kIniLoader.GetValue("texture", "file"), 16); // 16 = clamp
	} 
	else
		pkPSType->m_kPSystemBehaviour.m_uiTexture = 0;


	// PSystem lifetime
	if ( m_kIniLoader.KeyExist("ps_lifetime", "time") )
		pkPSType->m_kPSystemBehaviour.m_fLifeTime = m_kIniLoader.GetFloatValue("ps_lifetime", "time");
	else
		pkPSType->m_kPSystemBehaviour.m_fLifeTime = -1;

	if ( m_kIniLoader.KeyExist("ps_lifetime", "random") )
		pkPSType->m_kParticleBehaviour.m_iLifeTimeRandom = m_kIniLoader.GetFloatValue("ps_lifetime", "random") / 100.f;
	else
		pkPSType->m_kParticleBehaviour.m_iLifeTimeRandom = 0;


	// Start speed
	if ( m_kIniLoader.KeyExist("speed", "startspeed") )
		pkPSType->m_kParticleBehaviour.m_fStartSpeed = m_kIniLoader.GetFloatValue("speed", "startspeed");
	else
		pkPSType->m_kParticleBehaviour.m_fStartSpeed = 0;


	// Force
	if ( m_kIniLoader.KeyExist("force", "x") )
		pkPSType->m_kParticleBehaviour.m_kForce.x = m_kIniLoader.GetFloatValue("force", "x");
	else
		pkPSType->m_kParticleBehaviour.m_kForce.x = 0;

	if ( m_kIniLoader.KeyExist("force", "y") )
		pkPSType->m_kParticleBehaviour.m_kForce.y = m_kIniLoader.GetFloatValue("force", "y");
	else
		pkPSType->m_kParticleBehaviour.m_kForce.y = 0;

	if ( m_kIniLoader.KeyExist("force", "z") )
		pkPSType->m_kParticleBehaviour.m_kForce.z = m_kIniLoader.GetFloatValue("force", "z");
	else
		pkPSType->m_kParticleBehaviour.m_kForce.z = 0;


	// Direction
	if ( m_kIniLoader.KeyExist("direction", "x") )
		pkPSType->m_kParticleBehaviour.m_kDirection.x = m_kIniLoader.GetFloatValue("direction", "x");
	else
		pkPSType->m_kParticleBehaviour.m_kDirection.x = 0;

	if ( m_kIniLoader.KeyExist("direction", "y") )
		pkPSType->m_kParticleBehaviour.m_kDirection.y = m_kIniLoader.GetFloatValue("direction", "y");
	else
		pkPSType->m_kParticleBehaviour.m_kDirection.y = 0;

	if ( m_kIniLoader.KeyExist("direction", "z") )
		pkPSType->m_kParticleBehaviour.m_kDirection.z = m_kIniLoader.GetFloatValue("direction", "z");
	else
		pkPSType->m_kParticleBehaviour.m_kDirection.z = 0;


	// Wideness
	if ( m_kIniLoader.KeyExist("wideness", "x") )
		pkPSType->m_kParticleBehaviour.m_kWideness.x = m_kIniLoader.GetFloatValue("wideness", "x");
	else
		pkPSType->m_kParticleBehaviour.m_kWideness.x = 0;

	if ( m_kIniLoader.KeyExist("wideness", "y") )
		pkPSType->m_kParticleBehaviour.m_kWideness.y = m_kIniLoader.GetFloatValue("wideness", "y");
	else
		pkPSType->m_kParticleBehaviour.m_kWideness.y = 0;

	if ( m_kIniLoader.KeyExist("wideness", "z") )
		pkPSType->m_kParticleBehaviour.m_kWideness.z = m_kIniLoader.GetFloatValue("wideness", "z");
	else
		pkPSType->m_kParticleBehaviour.m_kWideness.z = 0;


	// Billboarding
	if ( m_kIniLoader.KeyExist("billboard", "y") )
		pkPSType->m_kParticleBehaviour.m_bBillBoardY = m_kIniLoader.GetBoolValue("billboard", "y");
	else
		pkPSType->m_kParticleBehaviour.m_bBillBoardY = true;


	// Alphatest
	if ( m_kIniLoader.KeyExist("alphatest", "type") )
		pkPSType->m_kPSystemBehaviour.m_uiAlphaTest = ui_aGLTest[m_kIniLoader.GetIntValue("alphatest", "type")];
	else
		pkPSType->m_kPSystemBehaviour.m_uiAlphaTest = 0;

	// Depthtest
	if ( m_kIniLoader.KeyExist("depthmask", "type") )
		pkPSType->m_kPSystemBehaviour.m_uiDepthMask = ui_aGLTest[m_kIniLoader.GetIntValue("depthmask", "type")];
	else
		pkPSType->m_kPSystemBehaviour.m_uiDepthMask = ui_aGLTest[GL_LEQUAL];


	// close the file
	//m_kIniLoader.Close();

	return true;

}

// ------------------------------------------------------------------------------------------

void PSystemManager::SetProperties ( PSystemType *pkPSType )
{

	// check if PSType uses blending
	if ( pkPSType->m_kPSystemBehaviour.m_iBlendSRC ||
		pkPSType->m_kPSystemBehaviour.m_iBlendDST )
		pkPSType->m_kPSystemBehaviour.m_kProperties.push_back ("Blend");

	// color
	if ( pkPSType->m_kParticleBehaviour.m_kStartColor.r != 1 || 
		  pkPSType->m_kParticleBehaviour.m_kEndColor.r   != 1 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColor.g != 1 || 
		  pkPSType->m_kParticleBehaviour.m_kEndColor.g   != 1 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColor.b != 1 ||
		  pkPSType->m_kParticleBehaviour.m_kEndColor.b   != 1 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColor.a != 1 || 
		  pkPSType->m_kParticleBehaviour.m_kEndColor.a   != 1  )
		pkPSType->m_kPSystemBehaviour.m_kProperties.push_back ("Color");

	// size
	if ( pkPSType->m_kParticleBehaviour.m_kStartSize.x !=
		pkPSType->m_kParticleBehaviour.m_kEndSize.x || 
		pkPSType->m_kParticleBehaviour.m_kStartSize.y !=
		pkPSType->m_kParticleBehaviour.m_kEndSize.y )
		pkPSType->m_kPSystemBehaviour.m_kProperties.push_back ("Size");

	// Texture
	if ( pkPSType->m_kPSystemBehaviour.m_uiTexture )
		pkPSType->m_kPSystemBehaviour.m_kProperties.push_back ("Texture");

	
	// Alphatest
	if ( pkPSType->m_kPSystemBehaviour.m_uiAlphaTest )
		pkPSType->m_kPSystemBehaviour.m_kProperties.push_back ("AlphaTest");
}

// ------------------------------------------------------------------------------------------
