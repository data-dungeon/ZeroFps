#include "psystemmanager.h"
#include "../engine_systems/propertys/psproperties/colorpsprop.h"
#include "../engine_systems/propertys/psproperties/movepsprop.h"
#include "../engine_systems/propertys/psproperties/sizepsprop.h"
#include "../engine_systems/propertys/psproperties/lightpsprop.h"

#include "../basic/zfsystem.h"
#include "../basic/zfini.h"

#include <iostream>
	using namespace std;

PSystemManager *PSystemManager::m_pkInstance;

// ------------------------------------------------------------------------------------------

PSystemManager::PSystemManager()
{
	m_pkInstance = this;
}

// ------------------------------------------------------------------------------------------

PSystem* PSystemManager::GetPSystem ( string kPSName )
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

		// Change Size property
		if ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_kProperties[i] == "Size" )
			pkPS->AddPSProperty ( new SizePSProp ( pkPS ) );

		// Light property
		if ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_kProperties[i] == "Light" )
			pkPS->AddPSProperty ( new LightPSProp ( pkPS ) );

	}

	// Add texture coordinates
	pkPS->SetTexCoords( CreateTexCoords(m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_iMaxParticles) );

	// Move property
	pkPS->AddPSProperty ( new MovePSProp ( pkPS ) );	

	// Create vertices for PS
	pkPS->SetVertices( CreateVertices(&m_kPSystemTypes[kPSName]) );

	// Create indices for PS
	pkPS->SetIndices ( CreateIndices(m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_iMaxParticles) );

	// Set max nr of particles in ParticleSystem
	pkPS->SetParticles ( m_kPSystemTypes[kPSName].m_kPSystemBehaviour.m_iMaxParticles );

	return pkPS;

}

// ------------------------------------------------------------------------------------------

Vector2* PSystemManager::CreateTexCoords (int iParticles)
{
	Vector2* pkTexCoords = new Vector2[4 + iParticles * 4];

	for ( int i = 0; i < iParticles * 4; i += 4 )
	{
		pkTexCoords[i].x = 0;
		pkTexCoords[i].y = 0;

		pkTexCoords[i+1].x = 1;
		pkTexCoords[i+1].y = 0;

		pkTexCoords[i+2].x = 1;
		pkTexCoords[i+2].y = 1;

		pkTexCoords[i+3].x = 0;
		pkTexCoords[i+3].y = 1;
	}

	return pkTexCoords;
}

// ------------------------------------------------------------------------------------------

float* PSystemManager::CreateVertices ( PSystemType *kPSystemType )
{
	return new float[12 + kPSystemType->m_kPSystemBehaviour.m_iMaxParticles * 12];
}

// ------------------------------------------------------------------------------------------

float* PSystemManager::CreateVerticeColors ( int iParticles )
{
	return new float[16 + iParticles * 16];
}

// ------------------------------------------------------------------------------------------

unsigned int* PSystemManager::CreateIndices ( int iParticles )
{
	unsigned int* pfIndices = new unsigned int[4 + iParticles * 4];

	for ( int i = 0; i < 4 + iParticles * 4; i++)
		pfIndices[i] = i;


	return pfIndices;
}

// ------------------------------------------------------------------------------------------


bool PSystemManager::LoadNewPSystem ( string kName )
{
	string kLoadName = "/data/psystems/" + kName + ".zps";

	// Open file
	if( !m_kIniLoader.Open(kLoadName.c_str(), 0) )		
		return false;

	// Create new PSystemType
	PSystemType kNewType;

	LoadData		  ( &kNewType );
	SetProperties ( &kNewType );
	CalculateMaxSize ( &kNewType );

	// Calculate max number of particles the PSystem can emitt
	if (kNewType.m_kParticleBehaviour.m_fLifeTime > kNewType.m_kPSystemBehaviour.m_fLifeTime &&
			kNewType.m_kPSystemBehaviour.m_fLifeTime != -9999999)
	{
		kNewType.m_kPSystemBehaviour.m_iMaxParticles = int(kNewType.m_kPSystemBehaviour.m_fLifeTime /
			kNewType.m_kPSystemBehaviour.m_fParticlesPerSec + 0.5f);
	}
	else
	{
		kNewType.m_kPSystemBehaviour.m_iMaxParticles = 
			int (kNewType.m_kParticleBehaviour.m_fLifeTime /
			kNewType.m_kPSystemBehaviour.m_fParticlesPerSec );
	}

   kNewType.m_kName = kName;

	//cout << "PSystemtype " << kName << " loaded!" << endl;

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
		pkPSType->m_kParticleBehaviour.m_iStartSizeRandom = m_kIniLoader.GetIntValue("start_size", "random");
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
		pkPSType->m_kParticleBehaviour.m_iEndSizeRandom = m_kIniLoader.GetIntValue("end_size", "random");
	else
		pkPSType->m_kParticleBehaviour.m_iEndSizeRandom = 0;




	// Particles / second
	if( m_kIniLoader.KeyExist("particle_lifetime", "time") )
		pkPSType->m_kParticleBehaviour.m_fLifeTime = m_kIniLoader.GetFloatValue("particle_lifetime", "time");
	else
		pkPSType->m_kParticleBehaviour.m_fLifeTime = 1;

	if ( m_kIniLoader.KeyExist("particle_lifetime", "random") )
		pkPSType->m_kParticleBehaviour.m_iLifeTimeRandom = m_kIniLoader.GetIntValue("particle_lifetime", "random");
	else
		pkPSType->m_kParticleBehaviour.m_iLifeTimeRandom = 1;


	// Start RGBA
	if( m_kIniLoader.KeyExist("start_color", "r") )
		pkPSType->m_kParticleBehaviour.m_kStartColor.x = m_kIniLoader.GetFloatValue("start_color", "r");
	else
		pkPSType->m_kParticleBehaviour.m_kStartColor.x = 1;

	if( m_kIniLoader.KeyExist("start_color", "g") )
		pkPSType->m_kParticleBehaviour.m_kStartColor.y = m_kIniLoader.GetFloatValue("start_color", "g");
	else
		pkPSType->m_kParticleBehaviour.m_kStartColor.y = 1;

	if( m_kIniLoader.KeyExist("start_color", "b") )
		pkPSType->m_kParticleBehaviour.m_kStartColor.z = m_kIniLoader.GetFloatValue("start_color", "b");
	else
		pkPSType->m_kParticleBehaviour.m_kStartColor.z = 1;

	if( m_kIniLoader.KeyExist("start_color", "a") )
		pkPSType->m_kParticleBehaviour.m_kStartColor.w = m_kIniLoader.GetFloatValue("start_color", "a");
	else
		pkPSType->m_kParticleBehaviour.m_kStartColor.w = 1;
	
	// Start RGB Random
	pkPSType->m_kParticleBehaviour.m_kStartColorRandom = 0;

	if( m_kIniLoader.KeyExist("start_color", "random") )
		pkPSType->m_kParticleBehaviour.m_kStartColorRandom = m_kIniLoader.GetFloatValue("start_color", "random");

	if( m_kIniLoader.KeyExist("start_color", "random_r") )
		pkPSType->m_kParticleBehaviour.m_kStartColorRandom.x = m_kIniLoader.GetFloatValue("start_color", "random_r");
	if( m_kIniLoader.KeyExist("start_color", "random_g") )
		pkPSType->m_kParticleBehaviour.m_kStartColorRandom.y = m_kIniLoader.GetFloatValue("start_color", "random_g");
	if( m_kIniLoader.KeyExist("start_color", "random_b") )
		pkPSType->m_kParticleBehaviour.m_kStartColorRandom.z = m_kIniLoader.GetFloatValue("start_color", "random_b");
	if( m_kIniLoader.KeyExist("start_color", "random_a") )
		pkPSType->m_kParticleBehaviour.m_kStartColorRandom.w = m_kIniLoader.GetFloatValue("start_color", "random_a");

	if( m_kIniLoader.KeyExist("start_color", "same_randomvalue") )
		pkPSType->m_kParticleBehaviour.m_bSameStartColorRandom = m_kIniLoader.GetBoolValue("start_color", "same_randomvalue");
	else
		pkPSType->m_kParticleBehaviour.m_bSameStartColorRandom = false;


	pkPSType->m_kParticleBehaviour.m_kStartColorRandom /= 100.f;

	// End RGBA
	if( m_kIniLoader.KeyExist("end_color", "r") )
		pkPSType->m_kParticleBehaviour.m_kEndColor.x = m_kIniLoader.GetFloatValue("end_color", "r");
	else
		pkPSType->m_kParticleBehaviour.m_kEndColor.x = pkPSType->m_kParticleBehaviour.m_kStartColor.x;

	if( m_kIniLoader.KeyExist("end_color", "g") )
		pkPSType->m_kParticleBehaviour.m_kEndColor.y = m_kIniLoader.GetFloatValue("end_color", "g");
	else
		pkPSType->m_kParticleBehaviour.m_kEndColor.y = pkPSType->m_kParticleBehaviour.m_kStartColor.y;

	if( m_kIniLoader.KeyExist("end_color", "b") )
		pkPSType->m_kParticleBehaviour.m_kEndColor.z = m_kIniLoader.GetFloatValue("end_color", "b");
	else
		pkPSType->m_kParticleBehaviour.m_kEndColor.z = pkPSType->m_kParticleBehaviour.m_kStartColor.z;

	if( m_kIniLoader.KeyExist("end_color", "a") )
		pkPSType->m_kParticleBehaviour.m_kEndColor.w = m_kIniLoader.GetFloatValue("end_color", "a");
	else
		pkPSType->m_kParticleBehaviour.m_kEndColor.w = pkPSType->m_kParticleBehaviour.m_kStartColor.w;

	// End RGB Random
	pkPSType->m_kParticleBehaviour.m_kEndColorRandom = 0;

	if( m_kIniLoader.KeyExist("end_color", "random") )
	{
		pkPSType->m_kParticleBehaviour.m_kEndColorRandom = m_kIniLoader.GetFloatValue("end_color", "random");
	}
	if( m_kIniLoader.KeyExist("end_color", "random_r") )
		pkPSType->m_kParticleBehaviour.m_kEndColorRandom.x = m_kIniLoader.GetFloatValue("end_color", "random_r");
	if( m_kIniLoader.KeyExist("end_color", "random_g") )
		pkPSType->m_kParticleBehaviour.m_kEndColorRandom.y = m_kIniLoader.GetFloatValue("end_color", "random_g");
	if( m_kIniLoader.KeyExist("end_color", "random_b") )
		pkPSType->m_kParticleBehaviour.m_kEndColorRandom.z = m_kIniLoader.GetFloatValue("end_color", "random_b");
	if( m_kIniLoader.KeyExist("end_color", "random_a") )
		pkPSType->m_kParticleBehaviour.m_kEndColorRandom.w = m_kIniLoader.GetFloatValue("end_color", "random_a");

	if( m_kIniLoader.KeyExist("end_color", "same_randomvalue") )
		pkPSType->m_kParticleBehaviour.m_bSameEndColorRandom = m_kIniLoader.GetBoolValue("end_color", "same_randomvalue");
	else
		pkPSType->m_kParticleBehaviour.m_bSameEndColorRandom = false;

	pkPSType->m_kParticleBehaviour.m_kEndColorRandom /= 100.f;

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
	if( m_kIniLoader.KeyExist("start_area", "create_style") )
		pkPSType->m_kPSystemBehaviour.m_kCreateStyle = m_kIniLoader.GetValue("start_area", "create_style");
	else
		pkPSType->m_kPSystemBehaviour.m_kCreateStyle = "circular";



	// Particles / second
	if( m_kIniLoader.KeyExist("particles_persecond", "time") )
		pkPSType->m_kPSystemBehaviour.m_fParticlesPerSec = 1.f / m_kIniLoader.GetIntValue("particles_persecond", "time");
	else
		pkPSType->m_kPSystemBehaviour.m_fParticlesPerSec = 0.2f;

	// material
	if ( m_kIniLoader.KeyExist("material", "file") )
	{
		pkPSType->m_kParticleBehaviour.m_pkMaterial = new ZFResourceHandle;

		pkPSType->m_kParticleBehaviour.m_strMaterialFile = m_kIniLoader.GetValue("material", "file");
		pkPSType->m_kParticleBehaviour.m_pkMaterial->SetRes(pkPSType->m_kParticleBehaviour.m_strMaterialFile.c_str());
	}
	else
		pkPSType->m_kParticleBehaviour.m_pkMaterial = 0;


	// PSystem lifetime
	if ( m_kIniLoader.KeyExist("ps_lifetime", "time") )
		pkPSType->m_kPSystemBehaviour.m_fLifeTime = m_kIniLoader.GetFloatValue("ps_lifetime", "time");
	else
		pkPSType->m_kPSystemBehaviour.m_fLifeTime = -9999999;

	// Start speed
	if ( m_kIniLoader.KeyExist("speed", "startspeed") )
		pkPSType->m_kParticleBehaviour.m_fStartSpeed = m_kIniLoader.GetFloatValue("speed", "startspeed");
	else
		pkPSType->m_kParticleBehaviour.m_fStartSpeed = 0;

	if ( m_kIniLoader.KeyExist("speed", "random") )
		pkPSType->m_kParticleBehaviour.m_iStartSpeedRand = m_kIniLoader.GetIntValue("speed", "random");
	else
		pkPSType->m_kParticleBehaviour.m_iStartSpeedRand = 0;


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

	if ( m_kIniLoader.KeyExist("force", "force_inherit_rotation") )
		pkPSType->m_kParticleBehaviour.m_bForceInheritDirection = m_kIniLoader.GetBoolValue("force", "force_inherit_rotation");
	else
		pkPSType->m_kParticleBehaviour.m_bForceInheritDirection = true;



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

	if ( m_kIniLoader.KeyExist("direction", "dir_inherit_rotation") )
		pkPSType->m_kParticleBehaviour.m_bStartSpeedInheritDirection = m_kIniLoader.GetBoolValue("direction", "dir_inherit_rotation");
	else
		pkPSType->m_kParticleBehaviour.m_bStartSpeedInheritDirection = true;


	// Wideness
	if ( m_kIniLoader.KeyExist("wideness", "x") )
		pkPSType->m_kParticleBehaviour.m_kWideness.x = m_kIniLoader.GetFloatValue("wideness", "x") / 2.f;
	else
		pkPSType->m_kParticleBehaviour.m_kWideness.x = 1;

	if ( m_kIniLoader.KeyExist("wideness", "y") )
		pkPSType->m_kParticleBehaviour.m_kWideness.y = m_kIniLoader.GetFloatValue("wideness", "y") / 2.f;
	else
		pkPSType->m_kParticleBehaviour.m_kWideness.y = 1;

	if ( m_kIniLoader.KeyExist("wideness", "z") )
		pkPSType->m_kParticleBehaviour.m_kWideness.z = m_kIniLoader.GetFloatValue("wideness", "z") / 2.f;
	else
		pkPSType->m_kParticleBehaviour.m_kWideness.z = 1;


	// Billboarding
	if ( m_kIniLoader.KeyExist("billboard", "y") )
		pkPSType->m_kParticleBehaviour.m_bBillBoardY = m_kIniLoader.GetBoolValue("billboard", "y");
	else
		pkPSType->m_kParticleBehaviour.m_bBillBoardY = true;


	// Position offset
	if ( m_kIniLoader.KeyExist("position_offset", "x") )
		pkPSType->m_kPSystemBehaviour.m_kPosOffset.x = m_kIniLoader.GetFloatValue("position_offset", "x");
	else
		pkPSType->m_kPSystemBehaviour.m_kPosOffset.x = 0;

	if ( m_kIniLoader.KeyExist("position_offset", "y") )
		pkPSType->m_kPSystemBehaviour.m_kPosOffset.y = m_kIniLoader.GetFloatValue("position_offset", "y");
	else
		pkPSType->m_kPSystemBehaviour.m_kPosOffset.y = 0;

	if ( m_kIniLoader.KeyExist("position_offset", "z") )
		pkPSType->m_kPSystemBehaviour.m_kPosOffset.z = m_kIniLoader.GetFloatValue("position_offset", "z");
	else
		pkPSType->m_kPSystemBehaviour.m_kPosOffset.z = 0;

	// culling test-type
	if ( m_kIniLoader.KeyExist("culling", "type") )
		pkPSType->m_kPSystemBehaviour.m_kCullingTest = m_kIniLoader.GetValue("culling", "type");
	else
		pkPSType->m_kPSystemBehaviour.m_kCullingTest = "cube";

	// culling test-type
	if ( m_kIniLoader.KeyExist("remove_on_death", "value") )
		pkPSType->m_kPSystemBehaviour.m_kCullingTest = m_kIniLoader.GetBoolValue("remove_on_death", "value");
	else
		pkPSType->m_kPSystemBehaviour.m_bRemoveParentOnFinish = true;
   

   pkPSType->m_kPSystemBehaviour.m_kPosOffset.x -= pkPSType->m_kParticleBehaviour.m_kStartSize.x;
   pkPSType->m_kPSystemBehaviour.m_kPosOffset.y -= pkPSType->m_kParticleBehaviour.m_kStartSize.y;
   pkPSType->m_kPSystemBehaviour.m_kPosOffset.z -= pkPSType->m_kParticleBehaviour.m_kStartSize.x;


	// Light Stuff
		// lightcolor start
	if ( m_kIniLoader.KeyExist("light_start_color", "r") )
		pkPSType->m_kPSystemBehaviour.m_fLightStartColorR = m_kIniLoader.GetFloatValue("light_start_color", "r");
	else 
		pkPSType->m_kPSystemBehaviour.m_fLightStartColorR = -1;
	if ( m_kIniLoader.KeyExist("light_start_color", "g") )
		pkPSType->m_kPSystemBehaviour.m_fLightStartColorG = m_kIniLoader.GetFloatValue("light_start_color", "g");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightStartColorG = -1;
	if ( m_kIniLoader.KeyExist("light_start_color", "b") )
		pkPSType->m_kPSystemBehaviour.m_fLightStartColorB = m_kIniLoader.GetFloatValue("light_start_color", "b");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightStartColorB = -1;
		
		// lightcolor end
	if ( m_kIniLoader.KeyExist("light_end_color", "r") )
		pkPSType->m_kPSystemBehaviour.m_fLightEndColorR = m_kIniLoader.GetFloatValue("light_end_color", "r");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightEndColorR = -1;
	if ( m_kIniLoader.KeyExist("light_end_color", "g") )
		pkPSType->m_kPSystemBehaviour.m_fLightEndColorG = m_kIniLoader.GetFloatValue("light_end_color", "g");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightEndColorG = -1;
	if ( m_kIniLoader.KeyExist("light_end_color", "b") )
		pkPSType->m_kPSystemBehaviour.m_fLightEndColorB = m_kIniLoader.GetFloatValue("light_end_color", "b");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightEndColorB = -1;

	// quad atten
	if ( m_kIniLoader.KeyExist("light_atten", "start_quad_att") )
		pkPSType->m_kPSystemBehaviour.m_fLightStartQuadAtt = m_kIniLoader.GetFloatValue("light_atten", "start_quad_att");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightStartQuadAtt = -1;
	if ( m_kIniLoader.KeyExist("light_atten", "end_quad_att") )
		pkPSType->m_kPSystemBehaviour.m_fLightEndQuadAtt = m_kIniLoader.GetFloatValue("light_atten", "end_quad_att");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightEndQuadAtt = -1;

		// lin atten
	if ( m_kIniLoader.KeyExist("light_atten", "start_lin_att") )
		pkPSType->m_kPSystemBehaviour.m_fLightStartLinAtt = m_kIniLoader.GetFloatValue("light_atten", "start_lin_att");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightStartLinAtt = -1;
	if ( m_kIniLoader.KeyExist("light_atten", "end_lin_att") )
		pkPSType->m_kPSystemBehaviour.m_fLightEndLinAtt = m_kIniLoader.GetFloatValue("light_atten", "end_lin_att");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightEndLinAtt = -1;

		// const atten
	if ( m_kIniLoader.KeyExist("light_atten", "start_const_att") )
		pkPSType->m_kPSystemBehaviour.m_fLightStartConstAtt = m_kIniLoader.GetFloatValue("light_atten", "start_const_att");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightStartConstAtt = -1;
	if ( m_kIniLoader.KeyExist("light_atten", "end_const_att") )
		pkPSType->m_kPSystemBehaviour.m_fLightEndConstAtt = m_kIniLoader.GetFloatValue("light_atten", "end_const_att");
	else
		pkPSType->m_kPSystemBehaviour.m_fLightEndConstAtt = -1;


	// close the file
	//m_kIniLoader.Close();

	return true;

}

// ------------------------------------------------------------------------------------------

void PSystemManager::SetProperties ( PSystemType *pkPSType )
{
	// color
	if ( pkPSType->m_kParticleBehaviour.m_kStartColor.x != 1 || 
		  pkPSType->m_kParticleBehaviour.m_kEndColor.x   != 1 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColor.y != 1 || 
		  pkPSType->m_kParticleBehaviour.m_kEndColor.y   != 1 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColor.z != 1 ||
		  pkPSType->m_kParticleBehaviour.m_kEndColor.z   != 1 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColor.w != 1 || 
		  pkPSType->m_kParticleBehaviour.m_kEndColor.w   != 1 ||
		  pkPSType->m_kParticleBehaviour.m_kEndColorRandom.x != 0 ||
		  pkPSType->m_kParticleBehaviour.m_kEndColorRandom.y != 0 ||
		  pkPSType->m_kParticleBehaviour.m_kEndColorRandom.z != 0 ||
		  pkPSType->m_kParticleBehaviour.m_kEndColorRandom.w != 0 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColorRandom.x != 0 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColorRandom.y != 0 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColorRandom.z != 0 ||
		  pkPSType->m_kParticleBehaviour.m_kStartColorRandom.w != 0
		  )
		pkPSType->m_kPSystemBehaviour.m_kProperties.push_back ("Color");

	// size
	if ( pkPSType->m_kParticleBehaviour.m_kStartSize.x !=
		pkPSType->m_kParticleBehaviour.m_kEndSize.x || 
		pkPSType->m_kParticleBehaviour.m_kStartSize.y !=
		pkPSType->m_kParticleBehaviour.m_kEndSize.y )
		pkPSType->m_kPSystemBehaviour.m_kProperties.push_back ("Size");

	pkPSType->m_kPSystemBehaviour.m_kProperties.push_back ("Light");
}

// ------------------------------------------------------------------------------------------

void PSystemManager::CalculateMaxSize ( PSystemType *pkPSType )
{
	Vector3 kOuterCircle;


	if ( pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.x > pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.x )
		kOuterCircle.x = pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.x;
	else
		kOuterCircle.x = pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.x;

	if ( pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.y > pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.y )
		kOuterCircle.y = pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.y;
	else
		kOuterCircle.y = pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.y;

	if ( pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.z > pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.z )
		kOuterCircle.z = pkPSType->m_kPSystemBehaviour.m_kEnd_OuterStartArea.z;
	else
		kOuterCircle.z = pkPSType->m_kPSystemBehaviour.m_kStart_OuterStartArea.z;

	float fLifeTime = pkPSType->m_kParticleBehaviour.m_fLifeTime * 
							(1 + (pkPSType->m_kParticleBehaviour.m_iLifeTimeRandom/100.f));

	float fWidth  = (pkPSType->m_kParticleBehaviour.m_kEndSize.x * 
						 (1 + (pkPSType->m_kParticleBehaviour.m_iEndSizeRandom/100.f)) ) / 2;
	float fHeight = (pkPSType->m_kParticleBehaviour.m_kEndSize.y * 
						 (1 + (pkPSType->m_kParticleBehaviour.m_iEndSizeRandom/100.f)) ) / 2;


	kOuterCircle = kOuterCircle / 2.f;

	pkPSType->m_kPSystemBehaviour.m_kCullPosOffset = Vector3(0,0,0);

	Vector3 kRandomDir, kRandomDir2, kRandomDir3, kRandomDir4;

	kRandomDir = kRandomDir2 = kRandomDir3 = kRandomDir4 =
					 pkPSType->m_kParticleBehaviour.m_kDirection;


	float fMaxWidthX, fMaxHeightX, fMaxWidthZ, fMaxHeightZ;

	// Max widthX
	if ( 90%int(pkPSType->m_kParticleBehaviour.m_kWideness.x*2) == 90 )
		fMaxWidthX = 90;
	else
		fMaxWidthX = pkPSType->m_kParticleBehaviour.m_kWideness.x;

	// Max widthZ
	if ( 90%int(pkPSType->m_kParticleBehaviour.m_kWideness.z*2) == 90 )
		fMaxWidthZ = 90;
	else
		fMaxWidthZ = pkPSType->m_kParticleBehaviour.m_kWideness.z;

	// Max heightX
	if ( 180%int(pkPSType->m_kParticleBehaviour.m_kWideness.x*2) == 180 )
		fMaxHeightX = 180;
	else
		fMaxHeightX = pkPSType->m_kParticleBehaviour.m_kWideness.x;

	// Max heightZ
	if ( 180%int(pkPSType->m_kParticleBehaviour.m_kWideness.z*2) == 180)
		fMaxHeightZ = 180;
	else
		fMaxHeightZ = pkPSType->m_kParticleBehaviour.m_kWideness.z;



	kRandomDir.x = fMaxWidthX;
	kRandomDir.z = fMaxWidthZ;

	kRandomDir2.x = fMaxHeightX;
	kRandomDir2.z = fMaxHeightZ;


	kRandomDir3.x = -fMaxWidthX;
	kRandomDir3.z = -fMaxWidthZ;

	kRandomDir4.x = -fMaxHeightX;
	kRandomDir4.z = -fMaxHeightZ;


	
	kRandomDir.y *= float( cos(kRandomDir.x / degtorad) * cos(kRandomDir.z / degtorad) );
	kRandomDir.x = float( sin(kRandomDir.x / degtorad) + 
						pkPSType->m_kParticleBehaviour.m_kDirection.x );
	kRandomDir.z = float( sin(kRandomDir.z / degtorad) + 
						pkPSType->m_kParticleBehaviour.m_kDirection.z );

	kRandomDir2.y *= float( cos(kRandomDir2.x / degtorad) * cos(kRandomDir2.z / degtorad) );
	kRandomDir2.x = float( sin(kRandomDir2.x / degtorad) + 
						pkPSType->m_kParticleBehaviour.m_kDirection.x );
	kRandomDir2.z = float( sin(kRandomDir2.z / degtorad) + 
						pkPSType->m_kParticleBehaviour.m_kDirection.z );

	kRandomDir3.y *= float( -cos(kRandomDir3.x / degtorad) * cos(kRandomDir3.z / degtorad) );
	kRandomDir3.x = float( sin(kRandomDir3.x / degtorad) + 
						pkPSType->m_kParticleBehaviour.m_kDirection.x );
	kRandomDir3.z = float( sin(kRandomDir3.z / degtorad) + 
						pkPSType->m_kParticleBehaviour.m_kDirection.z );

	kRandomDir4.y *= float( -cos(kRandomDir4.x / degtorad) * cos(kRandomDir4.z / degtorad) );
	kRandomDir4.x = float( sin(kRandomDir4.x / degtorad) + 
						pkPSType->m_kParticleBehaviour.m_kDirection.x );
	kRandomDir4.z = float( sin(kRandomDir4.z / degtorad) + 
						pkPSType->m_kParticleBehaviour.m_kDirection.z );


	Vector3 kSize1, kSize2, kSize3, kSize4, kSize5;

	// x
	kSize1.x = float( ((pkPSType->m_kParticleBehaviour.m_kDirection.x * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.x * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.x) * 
				  fLifeTime + fWidth );

	kSize2.x = float( ((kRandomDir.x * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.x * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.x) * 
				  fLifeTime + fWidth );

	kSize3.x = float( ((kRandomDir2.x * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.x * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.x) * 
				  fLifeTime + fWidth );

	kSize4.x = float( ((kRandomDir3.x * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.x * 
				  pow(fLifeTime, 2) / 2.f - kOuterCircle.x) * 
				  fLifeTime - fWidth );

	kSize5.x = float( ((kRandomDir4.x * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.x * 
				  pow(fLifeTime, 2) / 2.f - kOuterCircle.x) * 
				  fLifeTime - fWidth );
	// y
	kSize1.y = float( (pkPSType->m_kParticleBehaviour.m_kDirection.y *
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed) * fLifeTime + 
				  pkPSType->m_kParticleBehaviour.m_kForce.y * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.y
				  + (fHeight/2.f) );
 
	kSize2.y = float( (kRandomDir.y *
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed) * fLifeTime + 
				  pkPSType->m_kParticleBehaviour.m_kForce.y * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.y
				  + (fHeight/2.f) );

	kSize3.y = float( (kRandomDir2.y *
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed) * fLifeTime + 
				  pkPSType->m_kParticleBehaviour.m_kForce.y * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.y
				  + (fHeight/2.f) );

	kSize4.y = float( (kRandomDir3.y *
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed) * fLifeTime + 
				  pkPSType->m_kParticleBehaviour.m_kForce.y * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.y
				  + (fHeight/2.f) );

	kSize5.y = float( (kRandomDir4.y *
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed) * fLifeTime + 
				  pkPSType->m_kParticleBehaviour.m_kForce.y * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.y
				  + (fHeight/2.f) );
	// z
	kSize1.z = float( ((pkPSType->m_kParticleBehaviour.m_kDirection.z * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.z * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.z) * 
				  fLifeTime + (fHeight/2.f) );

	kSize2.z = float( ((kRandomDir.z * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.z * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.z) * 
				  fLifeTime + (fHeight/2.f) );

	kSize3.z = float( ((kRandomDir2.z * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.z * 
				  pow(fLifeTime, 2) / 2.f + kOuterCircle.z) * 
				  fLifeTime + (fHeight/2.f) );
	kSize4.z = float( ((kRandomDir3.z * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.z * 
				  pow(fLifeTime, 2) / 2.f - kOuterCircle.z) * 
				  fLifeTime - (fHeight/2.f) );

	kSize5.z = float( ((kRandomDir4.z * 
				  pkPSType->m_kParticleBehaviour.m_fStartSpeed ) +
				  pkPSType->m_kParticleBehaviour.m_kForce.z * 
				  pow(fLifeTime, 2) / 2.f - kOuterCircle.z) * 
				  fLifeTime - (fHeight/2.f) );
	//

	pkPSType->m_kPSystemBehaviour.m_kCullPosOffset = Vector3(0,0,0);

	Vector3 kMaxValues, kMinValues;

	kMaxValues = kMinValues = Vector3(0,0,0);


	// X-Max
	if ( kSize1.x >= kSize2.x && kSize1.x >= kSize3.x && kSize1.x >= kSize4.x && kSize1.x >= kSize5.x && kSize1.x > 0)
		kMaxValues.x = kSize1.x;
	else if ( kSize2.x >= kSize1.x && kSize2.x >= kSize3.x && kSize2.x >= kSize4.x && kSize2.x >= kSize5.x && kSize2.x > 0)
		kMaxValues.x = kSize2.x;
	else if ( kSize3.x >= kSize1.x && kSize3.x >= kSize2.x && kSize3.x >= kSize4.x && kSize3.x >= kSize5.x && kSize3.x > 0)
		kMaxValues.x = kSize3.x;
	else if ( kSize4.x >= kSize1.x && kSize4.x >= kSize3.x && kSize4.x >= kSize2.x && kSize4.x >= kSize5.x && kSize4.x > 0)
		kMaxValues.x = kSize4.x;
	else if ( kSize5.x >= kSize1.x && kSize5.x >= kSize3.x && kSize5.x >= kSize2.x && kSize5.x >= kSize4.x && kSize5.x > 0)
		kMaxValues.x = kSize5.x;

	// Y-Max
	if ( kSize1.y >= kSize2.y && kSize1.y >= kSize3.y && kSize1.y >= kSize4.y && kSize1.y >= kSize5.y && kSize1.y > 0)
		kMaxValues.y = kSize1.y;
	else if ( kSize2.y >= kSize1.y && kSize2.y >= kSize3.y && kSize2.y >= kSize4.y && kSize2.y >= kSize5.y && kSize2.y > 0)
		kMaxValues.y = kSize2.y;
	else if ( kSize3.y >= kSize1.y && kSize3.y >= kSize2.y && kSize3.y >= kSize4.y && kSize3.y >= kSize5.y && kSize3.y > 0)
		kMaxValues.y = kSize3.y;
	else if ( kSize4.y >= kSize1.y && kSize4.y >= kSize3.y && kSize4.y >= kSize2.y && kSize4.y >= kSize5.y && kSize4.y > 0)
		kMaxValues.y = kSize4.y;
	else if ( kSize5.y >= kSize1.y && kSize5.y >= kSize3.y && kSize5.y >= kSize2.y && kSize5.y >= kSize4.y && kSize5.y > 0)
		kMaxValues.y = kSize5.y;

	// Z-Max
	if ( kSize1.z >= kSize2.z && kSize1.z >= kSize3.z && kSize1.z >= kSize4.z && kSize1.z >= kSize5.z && kSize1.z > 0)
		kMaxValues.z = kSize1.z;
	else if ( kSize2.z >= kSize1.z && kSize2.z >= kSize3.z && kSize2.z >= kSize4.z && kSize2.z >= kSize5.z && kSize2.z > 0)
		kMaxValues.z = kSize2.z;
	else if ( kSize3.z >= kSize1.z && kSize3.z >= kSize2.z && kSize3.z >= kSize4.z && kSize3.z >= kSize5.z && kSize3.z > 0)
		kMaxValues.z = kSize3.z;
	else if ( kSize4.z >= kSize1.z && kSize4.z >= kSize3.z && kSize4.z >= kSize2.z && kSize4.z >= kSize5.z && kSize4.z > 0)
		kMaxValues.z = kSize4.z;
	else if ( kSize5.z >= kSize1.z && kSize5.z >= kSize3.z && kSize5.z >= kSize2.z && kSize5.z >= kSize4.z && kSize5.z > 0)
		kMaxValues.z = kSize5.z;


	// X-Min
	if ( kSize1.x <= kSize2.x && kSize1.x <= kSize3.x && kSize1.x <= kSize4.x && kSize1.x <= kSize5.x && kSize1.x < 0 )
		kMinValues.x = kSize1.x;
	else if ( kSize2.x <= kSize1.x && kSize2.x <= kSize3.x && kSize2.x <= kSize4.x && kSize2.x <= kSize5.x && kSize2.x < 0 )
		kMinValues.x = kSize2.x;
	else if ( kSize3.x <= kSize1.x && kSize3.x <= kSize2.x && kSize3.x <= kSize4.x && kSize3.x <= kSize5.x && kSize3.x < 0 )
		kMinValues.x = kSize3.x;
	else if ( kSize4.x <= kSize1.x && kSize4.x <= kSize3.x && kSize4.x <= kSize2.x && kSize4.x <= kSize5.x && kSize4.x < 0 )
		kMinValues.x = kSize4.x;
	else if ( kSize5.x <= kSize1.x && kSize5.x <= kSize3.x && kSize5.x <= kSize2.x && kSize5.x <= kSize4.x && kSize5.x < 0 )
		kMinValues.x = kSize5.x;

	// Y-Min
	if ( kSize1.y <= kSize2.y && kSize1.y <= kSize3.y && kSize1.y <= kSize4.y && kSize1.y <= kSize5.y && kSize1.y < 0)
		kMinValues.y = kSize1.y;
	else if ( kSize2.y <= kSize1.y && kSize2.y <= kSize3.y && kSize2.y <= kSize4.y && kSize2.y <= kSize5.y && kSize2.y < 0)
		kMinValues.y = kSize2.y;
	else if ( kSize3.y <= kSize1.y && kSize3.y <= kSize2.y && kSize3.y <= kSize4.y && kSize3.y <= kSize5.y && kSize3.y < 0)
		kMinValues.y = kSize3.y;
	else if ( kSize4.y <= kSize1.y && kSize4.y <= kSize3.y && kSize4.y <= kSize2.y && kSize4.y <= kSize5.y && kSize4.y < 0)
		kMinValues.y = kSize4.y;
	else if ( kSize5.y <= kSize1.y && kSize5.y <= kSize3.y && kSize5.y <= kSize2.y && kSize5.y <= kSize4.y && kSize5.y < 0)
		kMinValues.y = kSize5.y;

	// Z-Min
	if ( kSize1.z <= kSize2.z && kSize1.z <= kSize3.z && kSize1.z <= kSize4.z && kSize1.z <= kSize5.z && kSize1.z < 0)
		kMinValues.z = kSize1.z;
	else if ( kSize2.z <= kSize1.z && kSize2.z <= kSize3.z && kSize2.z <= kSize4.z && kSize2.z <= kSize5.z && kSize2.z < 0)
		kMinValues.z = kSize2.z;
	else if ( kSize3.z <= kSize1.z && kSize3.z <= kSize2.z && kSize3.z <= kSize4.z && kSize3.z <= kSize5.z && kSize3.z < 0)
		kMinValues.z = kSize3.z;
	else if ( kSize4.z <= kSize1.z && kSize4.z <= kSize3.z && kSize4.z <= kSize2.z && kSize4.z <= kSize5.z && kSize4.z < 0)
		kMinValues.z = kSize4.z;
	else if ( kSize5.z <= kSize1.z && kSize5.z <= kSize3.z && kSize5.z <= kSize2.z && kSize5.z <= kSize4.z && kSize5.z < 0)
		kMinValues.z = kSize5.z;

   // the height of the particle
   kMinValues.y -= pkPSType->m_kParticleBehaviour.m_kStartSize.y;

	pkPSType->m_kPSystemBehaviour.m_kCullPosOffset += (kMaxValues + kMinValues) / 2.f;
	pkPSType->m_kPSystemBehaviour.m_kCullPosOffset.y -= fHeight;

	pkPSType->m_kPSystemBehaviour.m_kMaxSize = (kMaxValues - kMinValues) / 2.f;

   pkPSType->m_kPSystemBehaviour.m_kCullPosOffset.y += pkPSType->m_kParticleBehaviour.m_kStartSize.y /2.f;

   pkPSType->m_kPSystemBehaviour.m_kCullPosOffset += ( Vector3 (pkPSType->m_kParticleBehaviour.m_kStartSize.x, 
                                                                        pkPSType->m_kParticleBehaviour.m_kStartSize.y, 
                                                                        pkPSType->m_kParticleBehaviour.m_kStartSize.x )) ;
}

// ------------------------------------------------------------------------------------------
