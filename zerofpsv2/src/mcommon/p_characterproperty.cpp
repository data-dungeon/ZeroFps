#include "p_characterproperty.h"
#include "p_charactercontrol.h"
#include "p_container.h"
#include "p_item.h"
#include "p_buff.h"
#include "ml_netmessages.h"

#include "../zerofpsv2/engine_systems/script_interfaces/si_objectmanager.h" 


P_CharacterProperty::P_CharacterProperty()
{
	m_pkAudioSystem = 	static_cast<ZFAudioSystem*>(g_ZFObjSys.GetObjectPtr("ZFAudioSystem"));			
	m_pkRender=				static_cast<Render*>(g_ZFObjSys.GetObjectPtr("Render"));			
	m_pkZShaderSystem=	static_cast<ZShaderSystem*>(g_ZFObjSys.GetObjectPtr("ZShaderSystem"));			
	m_pkEntityMan=			static_cast<EntityManager*>(g_ZFObjSys.GetObjectPtr("EntityManager"));			
	m_pkApp=					static_cast<Application*>(g_ZFObjSys.GetObjectPtr("Application"));			
	
	strcpy(m_acName,"P_CharacterProperty");
	m_iType=PROPERTY_TYPE_NORMAL|PROPERTY_TYPE_RENDER;
	m_iSide=PROPERTY_SIDE_SERVER|PROPERTY_SIDE_CLIENT;

	m_bNetwork = 	true;
	m_iVersion = 	1;
	
	
	
	m_kCurrentCharacterStates.reset();
	
	m_iConID					=	-1;
	m_strName 				=	"NoName";
	m_strOwnedByPlayer 	=	"NoPlayer";
	m_bIsPlayerCharacter =	false;
	m_bOverHeadText		=	true;	
	m_bFirstUpdate			=	true;
	m_fChatTime				=	0;
	m_strChatMsg			=	"";
	
	m_iInventory	= -1;		
	m_iHead			= -1;
	m_iGloves		= -1;
	m_iCape			= -1;		
	m_iNecklace		= -1;				
	m_iBody			= -1;
	m_iLeftHand		= -1;
	m_iRightHand	= -1;		
	m_iBracers		= -1;		
	m_iRing1			= -1;
	m_iRing2			= -1;
	m_iRing3			= -1;
	m_iRing4			= -1;
	m_iBelt			= -1;		
	m_iFeets			= -1;
		
		
	//basic sounds
	m_strWalkSound			=	"data/sound/footstep_forest.wav";
	m_strRunSound			=	"data/sound/footstep_forest_run.wav";
	m_strJumpSound			=	"data/sound/jump.wav";
	m_strSwimSound			=	"swim.wav";	
	m_iWalkSoundID 		= 	-1;
	m_iRunSoundID 			= 	-1;
	m_iSwimSoundID 		= 	-1;
	
	
	//setup material
	m_pkTextMaterial = new ZMaterial;
	m_pkTextMaterial->GetPass(0)->m_kTUs[0]->SetRes("data/textures/text/fetfont.tga");
	m_pkTextMaterial->GetPass(0)->m_iPolygonModeFront = 	FILL_POLYGON;
	m_pkTextMaterial->GetPass(0)->m_iCullFace = 				CULL_FACE_BACK;		
	m_pkTextMaterial->GetPass(0)->m_bLighting = 				false;		
	m_pkTextMaterial->GetPass(0)->m_bColorMaterial = 		true;
	m_pkTextMaterial->GetPass(0)->m_kVertexColor =			Vector3(0,1,0);
	m_pkTextMaterial->GetPass(0)->m_bFog = 					true;		
	m_pkTextMaterial->GetPass(0)->m_bAlphaTest =				true;		
	m_pkTextMaterial->GetPass(0)->m_bDepthTest = 			true;

	//setup font
	m_pkFont = new ZGuiFont("CharacterFont");
	m_pkFont->Create("/data/textures/text/fetfont.fnt",-1);	
}

void P_CharacterProperty::Init()
{

}

vector<PropertyValues> P_CharacterProperty::GetPropertyValues()
{
	vector<PropertyValues> kReturn(0);


	return kReturn;	
}

void P_CharacterProperty::SetupContainers()
{
	vector<Entity*>	kEntitys;
	m_pkEntity->GetChilds(&kEntitys);
	
	//cout<<"seting up character containers"<<endl;
	for(int i = 0;i<kEntitys.size();i++)
	{
		if(P_Container* pkContainer = (P_Container*)kEntitys[i]->GetProperty("P_Container"))
		{
			//set owner
			pkContainer->SetOwnerID(GetEntity()->GetEntityID());
			
			//setup container id's
			switch(pkContainer->GetContainerType())
			{
				case eInventory:
					cout<<"found inventory"<<endl;
					m_iInventory = kEntitys[i]->GetEntityID();
					break;
				case eBody:
					cout<<"found body"<<endl;
					m_iBody = kEntitys[i]->GetEntityID();
					break;
				case eHead:
					cout<<"found head"<<endl;
					m_iHead= kEntitys[i]->GetEntityID();
					break;
				case eLeftHand:
					cout<<"found left hand"<<endl;
					m_iLeftHand = kEntitys[i]->GetEntityID();
					break;
				case eRightHand:
					cout<<"found right hand"<<endl;
					m_iRightHand = kEntitys[i]->GetEntityID();
					break;			
				case eGloves:
					cout<<"found gloves"<<endl;
					m_iGloves = kEntitys[i]->GetEntityID();
					break;
				case eCape:
					cout<<"found Cape"<<endl;
					m_iCape = kEntitys[i]->GetEntityID();
					break;										
				case eNecklace:
					cout<<"found Necklace"<<endl;
					m_iNecklace = kEntitys[i]->GetEntityID();
					break;										
				case eBracers:
					cout<<"found Bracers"<<endl;
					m_iBracers = kEntitys[i]->GetEntityID();
					break;										
				case eBelt:
					cout<<"found Belt"<<endl;
					m_iBelt = kEntitys[i]->GetEntityID();
					break;										
				case eFeets:
					cout<<"found feets"<<endl;
					m_iFeets = kEntitys[i]->GetEntityID();
					break;										
				case eRing1:
					cout<<"found ring1"<<endl;
					m_iRing1= kEntitys[i]->GetEntityID();
					break;										
				case eRing2:
					cout<<"found ring2"<<endl;
					m_iRing2 = kEntitys[i]->GetEntityID();
					break;										
				case eRing3:
					cout<<"found ring3"<<endl;
					m_iRing3 = kEntitys[i]->GetEntityID();
					break;										
				case eRing4:
					cout<<"found ring4"<<endl;
					m_iRing4 = kEntitys[i]->GetEntityID();
					break;										
					
			}
		}
	}

	if(m_iInventory == -1)
	{
		P_Container* pkCon;
		Entity* pkContainer;
		
		//cout<<"no containers found, creating new ones"<<endl;
		
		//inventory
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iInventory = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());			
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(6,9);
				pkCon->SetContainerType(eInventory);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);			
				
		//body	
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iBody = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());			
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");
				pkCon->SetSize(3,4);
				pkCon->SetContainerType(eBody);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_BODY);
				pkCon->SetEqip(true);
				
		//head
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iHead = pkContainer->GetEntityID();		
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");
				pkCon->SetSize(2,2);
				pkCon->SetContainerType(eHead);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_HEAD);
				pkCon->SetEqip(true);
				
		//left hand
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iLeftHand = pkContainer->GetEntityID();		
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");
				pkCon->SetSize(2,4);
				pkCon->SetContainerType(eLeftHand);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_HAND);
				pkCon->SetJoint("hand_left");
				pkCon->SetEqip(true);
				
		//right hand
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRightHand = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,4);
				pkCon->SetContainerType(eRightHand);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_HAND);
				pkCon->SetJoint("hand_right");
				pkCon->SetEqip(true);
				
		//eGloves
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iGloves = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,2);
				pkCon->SetContainerType(eGloves);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_GLOVES);
				pkCon->SetEqip(true);
				
		//eCape
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iCape = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,2);
				pkCon->SetContainerType(eCape);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_CAPE);																					
				pkCon->SetEqip(true);
		
		//eNecklace
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iNecklace = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eNecklace);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_NECKLACE);																					
				pkCon->SetEqip(true);
		
		//eBracers
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iBracers = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,2);
				pkCon->SetContainerType(eBracers);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_BRACERS);					
				pkCon->SetEqip(true);

		//eBelt
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iBelt = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,1);
				pkCon->SetContainerType(eBelt);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_BELT);					
				pkCon->SetEqip(true);
								
		//eFeets
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iFeets = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(2,2);
				pkCon->SetContainerType(eFeets);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_FEETS);									
				pkCon->SetEqip(true);
				
		//eRing1
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRing1 = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eRing1);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_RING);					
				pkCon->SetEqip(true);

		//eRing2
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRing2 = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eRing2);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_RING);	
				pkCon->SetEqip(true);
				
		//eRing3
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRing3 = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eRing3);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_RING);		
				pkCon->SetEqip(true);

		//eRing4
		pkContainer = m_pkEntityMan->CreateEntity();
			m_iRing4 = pkContainer->GetEntityID();
			pkContainer->SetParent(GetEntity());
			pkCon = (P_Container*)pkContainer->AddProperty("P_Container");				
				pkCon->SetSize(1,1);
				pkCon->SetContainerType(eRing4);
				pkCon->SetOwnerID(GetEntity()->GetEntityID());
				pkCon->SetStaticOwner(true);
				pkCon->SetMaxItems(1);
				pkCon->AddItemType(MLITEM_RING);	
				pkCon->SetEqip(true);
				
																															
		//cout<<"done"<<endl;
	}

	//update network
	SetNetUpdateFlag(true);
}


P_CharacterProperty::~P_CharacterProperty()
{
	//make sure all sounds are stoped
	m_pkAudioSystem->StopAudio(m_iWalkSoundID);
	m_pkAudioSystem->StopAudio(m_iRunSoundID);
	m_pkAudioSystem->StopAudio(m_iSwimSoundID);

	delete m_pkTextMaterial;
	delete m_pkFont;	
}


void P_CharacterProperty::Update()
{
	if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_NORMAL))
	{
		//SERVER
		if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_SERVER))
		{
			//try to find items on load
			if(m_bFirstUpdate)
			{
				m_bFirstUpdate = false;
				SetupContainers();
			}
		
			//UpdateAnimation();
		}
			
		//CLIENT
		if(m_pkEntityManager->IsUpdate(PROPERTY_SIDE_CLIENT))
			PlayCharacterMovementSounds();					
		
		//CLIENT AND SERVER
		//reset chat msg
		if(m_pkZeroFps->GetTicks() > m_fChatTime + 10.0)
			m_strChatMsg = "";						
		
	}
	else if(m_pkEntityManager->IsUpdate(PROPERTY_TYPE_RENDER))
	{
		if(GetIsPlayerCharacter())
		{
			if(m_bOverHeadText)
			{
				string strText = GetName()+string(" <")+GetOwnedByPlayer()+string("> ") + m_strChatMsg;		
				float fDistance = m_pkZeroFps->GetCam()->GetRenderPos().DistanceTo(GetEntity()->GetWorldPosV());
				if(fDistance < 20)
				{
					float fScale = 0.05 * fDistance;
					
					m_pkRender->PrintBillboard(m_pkZeroFps->GetCam()->GetRotM(),GetEntity()->GetIWorldPosV()+
									Vector3(0,1.0,0),fScale,strText,m_pkTextMaterial,m_pkFont,true);													
				}
			}
		}	
	}		
}

void P_CharacterProperty::AddChatMsg(const string& strChatMsg)
{
	m_strChatMsg = strChatMsg;
	m_fChatTime = m_pkZeroFps->GetTicks();	
}


void P_CharacterProperty::PlayCharacterMovementSounds()
{
	if(P_CharacterControl* pkCC = (P_CharacterControl*)GetEntity()->GetProperty("P_CharacterControl"))
	{
		//check states and play sounds
		
		//walk sound
		if(pkCC->GetCharacterState(eWALKING))
		{
			if(!m_kCurrentCharacterStates[eWALKING])
			{
				//m_pkAudioSystem->StopAudio(m_iWalkSoundID);
				m_iWalkSoundID = m_pkAudioSystem->PlayAudio(m_strWalkSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),ZFAUDIO_LOOP);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iWalkSoundID, GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eWALKING])
				m_pkAudioSystem->StopAudio(m_iWalkSoundID);
		}	
		
		//run sound
		if(pkCC->GetCharacterState(eRUNNING))
		{
			if(!m_kCurrentCharacterStates[eRUNNING])
			{
				//m_pkAudioSystem->StopSound(m_iRunSoundID);
				m_iRunSoundID = m_pkAudioSystem->PlayAudio(m_strRunSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),ZFAUDIO_LOOP);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iRunSoundID, GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eRUNNING])
				m_pkAudioSystem->StopAudio(m_iRunSoundID);
		}	
		
				
		
		//swim sound
		if(pkCC->GetCharacterState(eSWIMMING))
		{
			if(!m_kCurrentCharacterStates[eSWIMMING])
			{	
				//m_pkAudioSystem->StopAudio(m_iSwimSoundID);
				m_iSwimSoundID = m_pkAudioSystem->PlayAudio(m_strSwimSound,GetEntity()->GetIWorldPosV(),Vector3(0,0,0),ZFAUDIO_LOOP);
			}
			else
				m_pkAudioSystem->MoveAudio(m_iSwimSoundID,GetEntity()->GetIWorldPosV());
		}
		else
		{
			if(m_kCurrentCharacterStates[eSWIMMING])
				m_pkAudioSystem->StopAudio(m_iSwimSoundID);
		}			

		//jump sound
		if(pkCC->GetCharacterState(eJUMPING))
			if(!m_kCurrentCharacterStates[eJUMPING])
				m_pkAudioSystem->PlayAudio(m_strJumpSound,GetEntity()->GetIWorldPosV());
		
														
		//update staes
		m_kCurrentCharacterStates[eRUNNING] =	pkCC->GetCharacterState(eRUNNING);
		m_kCurrentCharacterStates[eWALKING] =	pkCC->GetCharacterState(eWALKING);
		m_kCurrentCharacterStates[eJUMPING] =	pkCC->GetCharacterState(eJUMPING);
		m_kCurrentCharacterStates[eSWIMMING] =	pkCC->GetCharacterState(eSWIMMING);
	}
}

void P_CharacterProperty::SendBuffList()
{
	if(m_iConID == -1)
		return;

		
	NetPacket kNp;
	kNp.Write((char) MLNM_SC_BUFFLIST);	
	
	vector<Property*> kProps;
	GetEntity()->GetAllPropertys(&kProps,PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_SERVER);

	for(int i = 0;i<kProps.size();i++)
	{
		if(P_Buff* pkBuff = dynamic_cast<P_Buff*>(kProps[i]))
		{
			if(pkBuff->m_bShow)
			{
				kNp.Write_Str(pkBuff->m_strName);
				kNp.Write_Str(pkBuff->m_strIcon);
				kNp.Write(pkBuff->m_fTimeOut);
				kNp.Write(pkBuff->m_cType);
			}
		}
	}
	kNp.Write_Str(string(""));
	
	
	//send package
	kNp.TargetSetClient(m_iConID);
	m_pkApp->SendAppMessage(&kNp);		
}



P_Buff* P_CharacterProperty::AddBuff(const string& strBuffName)
{
	const string strBuffDir = "data/script/objects/buffs/";

	if(Entity* pkEnt = m_pkEntityManager->CreateEntityFromScript((strBuffDir+strBuffName).c_str()))	
	{	
		pkEnt->SetParent(GetEntity());
		
		cout<<"added buff "<<strBuffName<<endl;
		
		SendBuffList();
		return (P_Buff*)pkEnt->GetProperty("P_Buff");
	}
		
		
	cout<<"WARNING: could not add buff "<<strBuffName<<endl;
	return NULL;
}

void P_CharacterProperty::RemoveBuff(const string& strBuffName)
{
	vector<Property*> kProps;
	GetEntity()->GetAllPropertys(&kProps,PROPERTY_TYPE_NORMAL,PROPERTY_SIDE_SERVER);

	for(int i = 0;i<kProps.size();i++)
	{
		if(P_Buff* pkBuff = dynamic_cast<P_Buff*>(kProps[i]))
		{
			if(pkBuff->m_strName == strBuffName)
			{
				cout<<"removing buffs by name: "<<strBuffName<<endl;
				RemoveBuff(pkBuff);
			}
		}
	}
}

void P_CharacterProperty::RemoveBuff(P_Buff* pkBuff)
{
	if(pkBuff->GetCharacter() == this)
	{
		m_pkEntityManager->Delete(pkBuff->GetEntity());
		pkBuff->m_bShow = false;
		
		SendBuffList();
	}
}


void P_CharacterProperty::Save(ZFIoInterface* pkPackage)
{
	pkPackage->Write_Str(m_strName);
	pkPackage->Write_Str(m_strOwnedByPlayer);
	pkPackage->Write(m_bIsPlayerCharacter);
		
/*	//save container settings
	m_pkInventory->Save(pkPackage);
	m_pkHead->Save(pkPackage);
	m_pkBody->Save(pkPackage);
	m_pkLeftHand->Save(pkPackage);
	m_pkRightHand->Save(pkPackage);
*/	
}

void P_CharacterProperty::Load(ZFIoInterface* pkPackage,int iVersion)
{
	pkPackage->Read_Str(m_strName);	
	pkPackage->Read_Str(m_strOwnedByPlayer);	
	pkPackage->Read(m_bIsPlayerCharacter); 
	
/*	//load container settings
	m_pkInventory->Load(pkPackage);
	m_pkHead->Load(pkPackage);
	m_pkBody->Load(pkPackage);
	m_pkLeftHand->Load(pkPackage);
	m_pkRightHand->Load(pkPackage);
*/	
}

void P_CharacterProperty::PackTo( NetPacket* pkNetPacket, int iConnectionID ) 
{
	pkNetPacket->Write(m_iInventory);
	pkNetPacket->Write(m_iHead);
	pkNetPacket->Write(m_iBody);
	pkNetPacket->Write(m_iLeftHand);
	pkNetPacket->Write(m_iRightHand);	
	pkNetPacket->Write(m_iGloves);
	pkNetPacket->Write(m_iCape);
	pkNetPacket->Write(m_iNecklace);
	pkNetPacket->Write(m_iBracers);
	pkNetPacket->Write(m_iRing1);
	pkNetPacket->Write(m_iRing2);
	pkNetPacket->Write(m_iRing3);
	pkNetPacket->Write(m_iRing4);
	pkNetPacket->Write(m_iBelt);
	pkNetPacket->Write(m_iFeets);
	
	

	pkNetPacket->Write_Str(m_strName);
	pkNetPacket->Write_Str(m_strOwnedByPlayer);
	pkNetPacket->Write(m_bIsPlayerCharacter);
	
	pkNetPacket->Write_Str(m_strWalkSound);
	pkNetPacket->Write_Str(m_strRunSound);
	pkNetPacket->Write_Str(m_strJumpSound);
	pkNetPacket->Write_Str(m_strSwimSound);
	
	SetNetUpdateFlag(iConnectionID,false);
	
}

void P_CharacterProperty::PackFrom( NetPacket* pkNetPacket, int iConnectionID  ) 
{
	pkNetPacket->Read(m_iInventory);
	pkNetPacket->Read(m_iHead);
	pkNetPacket->Read(m_iBody);
	pkNetPacket->Read(m_iLeftHand);
	pkNetPacket->Read(m_iRightHand);	
	pkNetPacket->Read(m_iGloves);
	pkNetPacket->Read(m_iCape);
	pkNetPacket->Read(m_iNecklace);
	pkNetPacket->Read(m_iBracers);
	pkNetPacket->Read(m_iRing1);
	pkNetPacket->Read(m_iRing2);
	pkNetPacket->Read(m_iRing3);
	pkNetPacket->Read(m_iRing4);
	pkNetPacket->Read(m_iBelt);
	pkNetPacket->Read(m_iFeets);


	pkNetPacket->Read_Str(m_strName);
	pkNetPacket->Read_Str(m_strOwnedByPlayer);
	pkNetPacket->Read(m_bIsPlayerCharacter);

	pkNetPacket->Read_Str(m_strWalkSound);
	pkNetPacket->Read_Str(m_strRunSound);
	pkNetPacket->Read_Str(m_strJumpSound);
	pkNetPacket->Read_Str(m_strSwimSound);		

}


// SCRIPT INTERFACE FOR P_CharacterProperty
using namespace ObjectManagerLua;

namespace SI_P_CharacterProperty
{
	//stat modification
	int ChangeStatLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
		{
			cout<<"WARNING: ChangeStat - wrong number of arguments"<<endl;
			return 0;		
		}
			
		int iCharcterID;
		int iValue;
		char czStat[128];
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,czStat);
		g_pkScript->GetArgInt(pkLua, 2, &iValue);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			pkCP->m_kCharacterStats.ChangeStat(czStat,iValue);		
			
			cout<<"changed stat "<<czStat<<" with "<<iValue<<endl;
		}
	
		return 0;				
	}
		
	int ChangeStatModLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 3)
		{
			cout<<"WARNING: ChangeStatMod - wrong number of arguments"<<endl;
			return 0;		
		}		

			
		int iCharcterID;
		int iValue;
		char czStat[128];
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,czStat);
		g_pkScript->GetArgInt(pkLua, 2, &iValue);
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
		{
			pkCP->m_kCharacterStats.ChangeMod(czStat,iValue);		
			
			cout<<"changed stat mod "<<czStat<<" with "<<iValue<<endl;
		}
	
		return 0;			
	}
	
	int AddBuffLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		

			
		int iCharcterID;
		char czBuff[128];
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,czBuff);
		
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			pkCP->AddBuff(czBuff);		
	
		return 0;			
	}

	int RemoveBuffLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		

			
		int iCharcterID;
		char czBuff[128];
		
		g_pkScript->GetArgInt(pkLua, 0, &iCharcterID);
		g_pkScript->GetArgString(pkLua, 1,czBuff);
		
		
		if(P_CharacterProperty* pkCP = (P_CharacterProperty*)g_pkObjMan->GetPropertyFromEntityID(iCharcterID,"P_CharacterProperty"))
			pkCP->RemoveBuff(czBuff);		
	
		return 0;			
	}
		
	
	int PickupItemLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		
	
		int iCharcterID;
		int iItemID;
		double dTemp;
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iCharcterID = (int)dTemp;
		g_pkScript->GetArgNumber(pkLua, 1, &dTemp);
		iItemID = (int)dTemp;
		
		cout<<iCharcterID<<"trying to pickup "<<iItemID<<endl;
		
		if(Entity* pkEnt = g_pkObjMan->GetEntityByID(iCharcterID))
			if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))		
			{
				if(Entity* pkContainerEnt = g_pkObjMan->GetEntityByID(pkCP->m_iInventory))
					if(P_Container* pkContainer = (P_Container*)pkContainerEnt->GetProperty("P_Container"))
					{
						if(pkContainer->AddMove(iItemID,-1,-1,-1))
						{
							cout<<"sucessfully picked up item"<<endl;
							return 0;
						}
						else
							cout<<"could not pick that up"<<endl;
					}
			}
			else
				cout<<"WARNING: entity without P_CharacterProperty tried to pickup an item"<<endl;
				
		return 0;
	}
	
	int HaveItemLua(lua_State* pkLua)
	{
		if(g_pkScript->GetNumArgs(pkLua) != 2)
			return 0;		
		
		int iCharcterID;
		double dTemp;
		double dItemID = -1;
		char czItemName[64];
		
		g_pkScript->GetArgNumber(pkLua, 0, &dTemp);
		iCharcterID = (int)dTemp;
		
		g_pkScript->GetArgString(pkLua, 1,czItemName);
		
		if(Entity* pkEnt = g_pkObjMan->GetEntityByID(iCharcterID))
			if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkEnt->GetProperty("P_CharacterProperty"))
				if(Entity* pkContainerEnt = g_pkObjMan->GetEntityByID(pkCP->m_iInventory))
					if(P_Container* pkContainer = (P_Container*)pkContainerEnt->GetProperty("P_Container"))
						dItemID = pkContainer->HaveItem(czItemName);
			

		g_pkScript->AddReturnValue(pkLua, dItemID);							
		return 1;		
	}	
}




Property* Create_P_CharacterProperty()
{
	return new P_CharacterProperty;
}


void Register_P_CharacterProperty(ZeroFps* pkZeroFps)
{
	// Register Property
	pkZeroFps->m_pkPropertyFactory->Register("P_CharacterProperty", Create_P_CharacterProperty);					

	// Register Property Script Interface
	g_pkScript->ExposeFunction("PickupItem",		SI_P_CharacterProperty::PickupItemLua);
	g_pkScript->ExposeFunction("HaveItem",			SI_P_CharacterProperty::HaveItemLua);

	g_pkScript->ExposeFunction("ChangeStat",		SI_P_CharacterProperty::ChangeStatLua);
	g_pkScript->ExposeFunction("ChangeStatMod",	SI_P_CharacterProperty::ChangeStatModLua);
	
	g_pkScript->ExposeFunction("AddBuff",			SI_P_CharacterProperty::AddBuffLua);
	g_pkScript->ExposeFunction("RemoveBuff",		SI_P_CharacterProperty::RemoveBuffLua);
	
}


