#ifndef _ENGINE_MADPROPERTY_H_
#define _ENGINE_MADPROPERTY_H_

#include "../mad/mad_modell.h"
#include "../../engine/zerofps.h"
#include "../../engine/property.h"
#include <string>
#include "../engine_systems_x.h"
using namespace std;

//#define NO_ANIMATION_ID	-1	// Index used for non existing anim.

/// Property to add a Modell (MAD) to a object.
class ENGINE_SYSTEMS_API P_Mad : public Property, public Mad_Modell {
	private:
		vector<PropertyValues> GetPropertyValues();
		bool HandleSetValue( string kValueName ,string kValue );
	
		Light*			m_pkLight;
		Render*			m_pkRender;		// Ptr to render sys.
		ZeroFps*			m_pkZeroFps;	// Ptr to zerofps sys.
		ZShaderSystem* m_pkZShaderSystem;
		
		float		m_fLod;
		bool		m_bIsVisible;
		int		m_iShadowGroup;
		int		m_iCollisionMeshID;
		
		float		m_fLastAnimationUpdateTime;
		int		m_iLastAnimationUpdateFrame;
		
		Vector3	m_kOffset;
		
		LightProfile	m_kLightProfile;				//used for lighting
		
		//linetest stuff
		Matrix4		m_kModelMatrix;
		Vector3		m_kColPos;
		int			m_iColFace;

		//line test
		bool LineVSSphere(const Vector3& kPos,Vector3 &kDir,bool bIgnoreY = false);
		bool LineVSMesh(const Vector3& kPos,const Vector3& kDir);		
		void GenerateModelMatrix();		
		bool TestSides(Vector3* kVerts,Vector3* pkNormal,Vector3 kPos);		
		bool TestPolygon(Vector3* kVerts,Vector3 kPos,Vector3 kDir);

		void DoAnimationUpdate();
		
	public:
		bool	m_bCanBeInvisible;	// True if this Mad fades away if it gets between the cam and the player.		
		
		//base funktions
		P_Mad();
		void Update();
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		void PackTo(NetPacket* pkNetPacket, int iConnectionID );
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID );

		//model handling
		void SetBase(const char* acName);
		void SetScale(float fScale);
		bool AddMesh(int iSId);

		int GetCollisionMeshID()				{	return m_iCollisionMeshID; }
		
		//animation
		void SetAnimation(const char* szName, float fStartTime);
		void SetNextAnimation(const char* szName);		
		void SetNextAnimation(int iAnimID);
		int GetCurrentAnimation();
		string GetCurrentAnimationName();
		
		void SetVisible(bool bVisible);
		Vector3 GetOffset()						{	return m_kOffset;				};
		
		//shadow groups
		void SetShadowGroup(int iGroup)		{	m_iShadowGroup = iGroup;	};
		int GetShadowGroup() 					{	return m_iShadowGroup;		};	

		//get joint position
		Vector3 GetJointPosition(const char* szJointName);
		Matrix4 GetJointRotation(const char* szJointName);

		//line test
		bool TestLine(Vector3 kPos,Vector3 kDir,bool bSphereOnly = false,bool bIgnoreY = false);
		Vector3 GetLastColPos() 				{return m_kColPos;};
		int GetLastColFace() 					{ return m_iColFace;};		
				
		//vertex data	
		Mad_TextureCoo* GetTextureCooPtr()	{return Mad_Modell::GetTextureCooPtr();};
		Vector3* GetNormalsPtr()				{return Mad_Modell::GetNormalsPtr();};
		int* GetFacesPtr()						{return Mad_Modell::GetFacesPtr();};
		Vector3* GetVerticesPtr()				{return Mad_Modell::GetVerticesPtr();};
		int GetNumVertices()						{return Mad_Modell::GetNumVertices();};
		int GetNumFaces()							{return Mad_Modell::GetNumFaces();};

		
		bool operator<(Property& kOther);
};

namespace SI_PMad
{
	int ENGINE_SYSTEMS_API SetNextAnim(lua_State* pkLua);
	int ENGINE_SYSTEMS_API PlayAnim(lua_State* pkLua);
	int ENGINE_SYSTEMS_API AddMesh(lua_State* pkLua);
	
}


Property* Create_MadProperty();
void ENGINE_SYSTEMS_API Register_MadProperty(ZeroFps* pkZeroFps);

#endif



