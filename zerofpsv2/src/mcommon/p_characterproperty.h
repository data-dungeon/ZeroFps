#ifndef _P_CHARACTERPROPERTY_H_
#define _P_CHARACTERPROPERTY_H_

#include "mcommon_x.h"
#include "../zerofpsv2/engine/property.h"


using namespace std;

class P_CharacterProperty: public Property
{
	private:
		string	m_strName;			
		
		bool		m_bIsPlayerCharacter;
		string	m_strOwnedByPlayer;

	public:
	
		P_CharacterProperty();
		~P_CharacterProperty();

		void SetName(const string& strName)						{	m_strName = strName;						SetNetUpdateFlag(true);}
		void SetOwnedByPlayer(const string& strPlayer)		{	m_strOwnedByPlayer = strPlayer;		SetNetUpdateFlag(true);}
		void SetIsPlayerCharacter(bool bIsPlayer)				{	m_bIsPlayerCharacter = bIsPlayer;	SetNetUpdateFlag(true);}
		
		string GetName()												{	return m_strName;					}
		string GetOwnedByPlayer()									{	return m_strOwnedByPlayer;		}
		bool	 GetIsPlayerCharacter()								{	return m_bIsPlayerCharacter;	}
				
		void Save(ZFIoInterface* pkPackage);
		void Load(ZFIoInterface* pkPackage,int iVersion);
		void PackTo(NetPacket* pkNetPacket, int iConnectionID ) ;
		void PackFrom(NetPacket* pkNetPacket, int iConnectionID ) ;		
};

MCOMMON_API Property* Create_P_CharacterProperty();

#endif








