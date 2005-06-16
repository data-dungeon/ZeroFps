#define _DONT_MAIN

#include "mistserver.h"

#include "../mcommon/ml_netmessages.h"
#include "../zerofpsv2/engine/zerofps.h"
#include "../mcommon/si_mistland.h"
#include "../mcommon/ml_netmessages.h"
#include "../mcommon/zssmltime.h"
#include "../mcommon/zssenviroment.h"
#include "../mcommon/p_characterproperty.h"
#include "../mcommon/p_container.h"
#include "../mcommon/p_item.h"

void MistServer::OnNetworkMessage(NetPacket *PkNetMessage)
{
	unsigned char ucType;

	// Read Type of Message.
	PkNetMessage->Read(ucType);

	switch(ucType)
	{
		// ----------- LOGIN STUFF -------------	
		case MLNM_CS_CHARADD:
		{
			string strChar;
			PkNetMessage->Read_Str(strChar);
			string strMod;
			PkNetMessage->Read_Str(strMod);
			cout << "Plz Add: " << strChar << " with modell " << strMod << endl;

			string strLogin = m_pkZeroFps->m_kClient[PkNetMessage->m_iClientID].m_strLogin;
			m_pkPlayerDB->CreateNewCharacter(strLogin, strChar, strMod);
			SendCharacterList(PkNetMessage->m_iClientID);

			break;
		}

		case MLNM_CS_CHARDEL:
		{
			string strChar;
			PkNetMessage->Read_Str(strChar);

			string strLogin = m_pkZeroFps->m_kClient[PkNetMessage->m_iClientID].m_strLogin;
			m_pkPlayerDB->DeleteCharacter(strLogin, strChar);
			SendCharacterList(PkNetMessage->m_iClientID);
			break;
		}

		case MLNM_CS_REQPLAY:
		{
			string strChar;
			PkNetMessage->Read_Str(strChar);
			cout << "Play with character: " << strChar << endl;

			m_pkZeroFps->m_kClient[PkNetMessage->m_iClientID].m_strCharacter = strChar;
			
			//create character, 
			if(!SpawnPlayer( PkNetMessage->m_iClientID ))
			{
				cout<<"WARNING: could not create character "<<strChar<<" for client "<<PkNetMessage->m_iClientID<<endl;
				break;
			}
			
			//player is now no longer in login screen
			m_pkZeroFps->m_kClient[PkNetMessage->m_iClientID].m_bLogin = false;			
			
			//send character id to clients
			SendClientCharacterID(PkNetMessage->m_iClientID);			
			
			break;
		}

		case MLNM_CS_REQ_CHARACTERID:
		{
			SendClientCharacterID(PkNetMessage->m_iClientID);		
			
			break;
		}
		
		// -------------------------------------------
		
		case MLNM_CS_SETDEFAULTATTACK:
		{
			string strSkill;
			PkNetMessage->Read_Str(strSkill);
			
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
					pkCP->SetDefaultAttackSkill(strSkill);

			break;
		}

		case MLNM_CS_ADDSTATPOINT:
		{
			int iStat;
		
			PkNetMessage->Read(iStat);

			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
					pkCP->AddStatPoint(iStat);
			
			break;
		}		
		
		case MLNM_CS_ADDSKILLTOSKILLBAR:
		{
			string strSkill;
			int iPos;
		
			PkNetMessage->Read_Str(strSkill);
			PkNetMessage->Read(iPos);

			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
					pkCP->AddSkillToSkillbar(strSkill,iPos);
			
			break;
		}
		
		case MLNM_CS_REMOVEITEMFROMSKILLBAR:
		{
			int iPos;
			PkNetMessage->Read(iPos);

			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
					pkCP->RemoveItemFromSkillbar(iPos);

			break;
		}				
		
		case MLNM_CS_SIT:
		{
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(P_CharacterControl* pkCC = (P_CharacterControl*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterControl"))
					pkCC->Sit();			
		
			break;
		}
		
		case MLNM_CS_ADDSKILLTOQUEUE:
		{		
			string strSkill;
			int iTargetID;
			PkNetMessage->Read_Str(strSkill);
			PkNetMessage->Read(iTargetID);
			
			cout<<"got add skill to queue "<<strSkill<<"  target "<<iTargetID<<endl;
			
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
					pkCP->AddSkillToQueue(strSkill,iTargetID);
					
			break;
		}		
		
		case MLNM_CS_COMBATMODE:
		{		
			bool bCombatMode;
			PkNetMessage->Read(bCombatMode);
			
			cout<<"go combat mode "<<bCombatMode<<endl;
			
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
					pkCP->SetCombatMode(bCombatMode);
					
			break;
		}				
		
		case MLNM_CS_SET_TARGET:
		{		
			int iTarget;
			PkNetMessage->Read(iTarget);
			
			cout<<"got target "<<iTarget<<endl;
			
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
					pkCP->SetTarget(iTarget);
					
			break;
		}
				
		case MLNM_CS_RESPAWN_IN_TOWN:
		{
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{
				cout<<"Player "<<pkData->m_strPlayerName<<" requested to be respawned in town"<<endl;

				//get character property, revive player and send him to world start pos			
				if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
					if(P_CharacterProperty* pkCP = (P_CharacterProperty*)pkCharacter->GetProperty("P_CharacterProperty"))
						if(pkCP->IsDead())
						{
							pkCP->MakeAlive();
							pkCharacter->SetWorldPosV(GetPlayerStartPos());
							
						}
			}
			
			break;
		}		
		
		case MLNM_CS_REQ_KILLME:
		{
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{
				cout<<"Player "<<pkData->m_strPlayerName<<" requested to be killed, BYE BYE =D"<<endl;
				
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
				{
					pkCP->m_kCharacterStats.SetStat("Health",-100);
				}
/*				
				//move player to start
				if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
				{
					m_kCharacterStats
					pkCharacter->SetWorldPosV(GetPlayerStartPos());
				}	*/			
			}
					
			break;
		}		
		
		case MLNM_CS_CONTROLS:
		{
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{
				bitset<6> kControls;
				float	fYAngle;
				float fPAngle;
								
				PkNetMessage->Read(kControls);
				PkNetMessage->Read(fYAngle);
				PkNetMessage->Read(fPAngle);
				
				
				if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
				{
					if(P_CharacterControl* pkCC = (P_CharacterControl*)pkCharacter->GetProperty("P_CharacterControl"))
					{
						pkCC->SetKeys(&kControls);
						pkCC->SetRotation(fYAngle,fPAngle);
					}
					else
						cout<<"character has no controls ...WHYYYYYYYY!!!!!!!!"<<endl;
				}
				else
					cout<<"missing player character entity:"<<pkData->m_iCharacterID<<endl;
			}
			else
				cout<<"missting playerdata in plyerDB"<<endl;
			
			break;
		}

		case MLNM_CS_ANIM:
		{
			int iEmoteID;
			PkNetMessage->Read(iEmoteID);

			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{
				if(Entity* pkCharacter = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
				{
					if(P_CharacterControl* pkCC = (P_CharacterControl*)pkCharacter->GetProperty("P_CharacterControl"))
					{
						pkCC->DoEmote(iEmoteID);
					}
				}
			}
			break;
		}
		
		case MLNM_CS_SAY:
		{
			string strMsg;
			PkNetMessage->Read_Str(strMsg);
			
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{				
				SayToClients(strMsg,pkData->m_strPlayerName,pkData->m_iCharacterID);
				
				// --- for server output only , not reqired
				m_pkConsole->Printf("Msg> %s: %s",pkData->m_strPlayerName.c_str(),strMsg.c_str());								
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
					pkCP->AddChatMsg(strMsg);
				// ---
				
			}			
			
			break;
		}			
			
		case MLNM_CS_REQ_PLAYERLIST:
		{
			SendPlayerListToClient(PkNetMessage->m_iClientID);
						
			break;
		}
		
		case MLNM_CS_REQ_EQIPMENT:
		{
			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{				
				SendCharacterEquipment(pkData->m_iCharacterID, PkNetMessage->m_iClientID);
				
			}
						
			break;
		}
		
		
		case MLNM_CS_USESKILL:
		{
			string 	strSkill;
			int 		iTargetID;
			Vector3 	kPos;
			Vector3 	kDir;
		
			PkNetMessage->Read_Str(strSkill);
			PkNetMessage->Read(iTargetID);
			PkNetMessage->Read(kPos);
			PkNetMessage->Read(kDir);				

			if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
			{		
				if(P_CharacterProperty* pkCP = (P_CharacterProperty*)m_pkEntityManager->GetPropertyFromEntityID(pkData->m_iCharacterID,"P_CharacterProperty"))
				{
					int iRes = pkCP->UseSkill(strSkill,iTargetID,kPos,kDir);
					
					switch(iRes)
					{
						case -1:
							SayToClients("You don't have that skill","Server",-1,PkNetMessage->m_iClientID);break;
						case -2:
							SayToClients("You are dead","Server",-1,PkNetMessage->m_iClientID);break;						
// 						case 1:
// 							SayToClients("Skill not reloaded yet","Server",-1,PkNetMessage->m_iClientID);break;
						case 2:
							SayToClients("You need at least level 1 to use a skill","Server",-1,PkNetMessage->m_iClientID);break;
						case 3:
							SayToClients("You need to target an enemy","Server",-1,PkNetMessage->m_iClientID);break;
						case 4:
							SayToClients("You need to look at the target","Server",-1,PkNetMessage->m_iClientID);break;							
						case 5:
							SayToClients("You need to target an item","Server",-1,PkNetMessage->m_iClientID);break;										
						case 6:
							SayToClients("You are too far away","Server",-1,PkNetMessage->m_iClientID);break;										
						case 7:
							SayToClients("You need to equip a certain item","Server",-1,PkNetMessage->m_iClientID);break;										
// 						case 8:
// 							SayToClients("Still using another skill","Server",-1,PkNetMessage->m_iClientID);break;										
 						case 9:
 							SayToClients("You can't use that skill when not in combat mode","Server",-1,PkNetMessage->m_iClientID);break;										
							
							
					}
					
				}
			}			
			
			break;
		}		
		
		case MLNM_CS_ACTION:
		{
			int iEntity;
			string strAction;
			PkNetMessage->Read(iEntity);
			PkNetMessage->Read_Str(strAction);
			
			if(Entity* pkObj = m_pkEntityManager->GetEntityByID(iEntity))
			{
				// Get EntityID for client doing stuff.
            if(PlayerData* pkData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				{
					//distance check
					if(Entity* pkChar = m_pkEntityManager->GetEntityByID(pkData->m_iCharacterID))
						if(pkObj->GetWorldPosV().DistanceTo(pkChar->GetWorldPosV()) > 2.0)
						{
							SayToClients("You are to far away","Server",-1,PkNetMessage->m_iClientID);
							break;
						}
				
					if(P_Item* pkItem = (P_Item*)pkObj->GetProperty("P_Item"))
					{
						if(pkItem->GetInContainerID() != -1)
						{
							SayToClients("This item is in a container","Server",-1,PkNetMessage->m_iClientID);
							break;
						}					
					}
					
					//cout<<"character: "<<	pkData->m_strCharacterName<<" doint action: "<<strAction<<" on "<<iEntity<<endl;
				
					vector<ScriptFuncArg> args(2);
					args[0].m_kType.m_eType = tINT;
					args[0].m_pData = &(pkData->m_iCharacterID);
					args[1].m_kType.m_eType = tSTLSTRING;
					args[1].m_pData = &strAction;
					
					m_pkEntityManager->CallFunction(pkObj, "Useit",&args);	
				}
			}
			break;
		}
		
		case MLNM_CS_REQ_ITEMINFO:
		{
			int 				iItemID 	= -1;
			P_Item* 			pkItem = NULL;
			P_Container* 	pkInContainer = NULL;			
			Entity*			pkCharacter;
			
			//get item id
			PkNetMessage->Read(iItemID);
		
			cout<<"client requested info on item "<<iItemID<<endl;
			

			//Get item
			if(Entity* pkContainerEnt = m_pkEntityManager->GetEntityByID(iItemID))
				if(!(pkItem = (P_Item*)pkContainerEnt->GetProperty("P_Item")))			
				{
					cout<<"WARNING: MLNM_CS_REQ_ITEMINFO  could not find item "<<iItemID<<endl;
					break;
				}

			//get in container
			if(Entity* pkContainerEnt = m_pkEntityManager->GetEntityByID(pkItem->GetInContainerID()))
				pkInContainer = (P_Container*)pkContainerEnt->GetProperty("P_Container");
				
			//get players character entity
			if(PlayerData* pkPlayerData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(!(pkCharacter = m_pkEntityManager->GetEntityByID(pkPlayerData->m_iCharacterID)))
				{
					cout<<"WARNING: MLNM_CS_MOVE_ITEM could not find any character"<<endl;
					break;
				}			
			
				
			//is item in a container?
			if(pkInContainer)
			{
				//do we own the container?
				if(pkInContainer->GetOwnerID() != pkCharacter->GetEntityID())
				{
					SayToClients("That container is not yours","Server",-1,PkNetMessage->m_iClientID);
					break;					
				}
			
				SendItemInfo(iItemID,PkNetMessage->m_iClientID);
				break;
			}
			else
			{
				//do a distance check
				if(pkItem->GetEntity()->GetWorldPosV().DistanceTo(pkCharacter->GetWorldPosV()) > 2.0)
				{
					SayToClients("You are to far away","Server",-1,PkNetMessage->m_iClientID);
					break;
				}			
			
				SendItemInfo(iItemID,PkNetMessage->m_iClientID);
				break;
			}
				
		
			break;
		}		
		
		case MLNM_CS_MOVE_ITEM:
		{
			int iItemID;
			int iTargetContainer;
			int iPosX;
			int iPosY;
			int iCount;
			
			PkNetMessage->Read(iItemID);
			PkNetMessage->Read(iTargetContainer);
			PkNetMessage->Read(iPosX);
			PkNetMessage->Read(iPosY);			
			PkNetMessage->Read(iCount);			
		
			P_Container*	pkTargetContainer = 	NULL;
			P_Container*	pkInContainer = 		NULL;
			P_Item* 			pkItem = 				NULL;
			Entity*			pkCharacter = 			NULL;
			
			cout<<"got move package  item:"<<iItemID<<" targetcontainer:"<<iTargetContainer<<" pos:"<<iPosX<<"x"<<iPosY<<" count:"<<iCount<<endl;
			
			//get container
			if(Entity* pkContainerEnt = m_pkEntityManager->GetEntityByID(iTargetContainer))
				pkTargetContainer = (P_Container*)pkContainerEnt->GetProperty("P_Container");

			//Get item
			if(Entity* pkContainerEnt = m_pkEntityManager->GetEntityByID(iItemID))
				if(!(pkItem = (P_Item*)pkContainerEnt->GetProperty("P_Item")))			
				{
					cout<<"WARNING: MLNM_CS_MOVE_ITEM  could not find item "<<iItemID<<endl;
					break;
				}

			//get in container
			if(Entity* pkContainerEnt = m_pkEntityManager->GetEntityByID(pkItem->GetInContainerID()))
				pkInContainer = (P_Container*)pkContainerEnt->GetProperty("P_Container");
				
			//get players character entity
			if(PlayerData* pkPlayerData = m_pkPlayerDB->GetPlayerData(PkNetMessage->m_iClientID))
				if(!(pkCharacter = m_pkEntityManager->GetEntityByID(pkPlayerData->m_iCharacterID)))
				{
					cout<<"WARNING: MLNM_CS_MOVE_ITEM could not find any character"<<endl;
					break;
				}
							
				
			//item is not in any container	
			if(!pkInContainer)
			{
				//do a distance check
				if(pkItem->GetEntity()->GetWorldPosV().DistanceTo(pkCharacter->GetWorldPosV()) > 2.0)
				{
					SayToClients("You are to far away","Server",-1,PkNetMessage->m_iClientID);
					break;
				}						
			
				//do we have a target container , if so try to put item in it
				if(pkTargetContainer)
				{
					//check if target container is ours
					if(pkTargetContainer->GetOwnerID() != pkCharacter->GetEntityID())
					{
						SayToClients("target container is not yours","Server",-1,PkNetMessage->m_iClientID);
						break;					
					}								
							
					cout<<"picking up item from ground"<<endl;
					
					//try adding item from gound									
					if(!pkTargetContainer->AddMove(iItemID,iPosX,iPosY,iCount))
						SayToClients("You could not pick that up","Server",-1,PkNetMessage->m_iClientID);					
					
					SendContainer(iTargetContainer,PkNetMessage->m_iClientID,false);											
						
					break;

				}
				else
				{
					cout<<"WARNING: item not in any container, nor does it have ny target, noting to do :("<<endl;
					break;
				}		
					
			}
			//item is in a container
			else
			{
				//ware we owner of this container?
				if(pkInContainer->GetOwnerID() != pkCharacter->GetEntityID())
				{
					SayToClients("That container is not yours","Server",-1,PkNetMessage->m_iClientID);
					break;					
				}
			

				//do we have a target container 
				if(!pkTargetContainer)
				{
				
					//if we have a position, try moving there
					if(iPosX != -1)
					{
						cout<<"no target container, moving within this container"<<endl;
					
						if(!pkInContainer->AddMove(iItemID,iPosX,iPosY,iCount))
							SayToClients("Could not move item","Server",-1,PkNetMessage->m_iClientID);				
														
						SendContainer(pkInContainer->GetEntity()->GetEntityID(),PkNetMessage->m_iClientID,false);																		
							
						break;		
					}
					//no position, try drop
					else
					{
						cout<<"got no position, droping item"<<endl;
						
						
						Vector3 kOffset(0,0,0);
						
						//findout where the ground is
						if(P_Tcs* pkTcs = (P_Tcs*)pkCharacter->GetProperty("P_Tcs"))
						{
							kOffset.y = -(pkTcs->GetLegLength() -0.1);
						}
						
						
						
						if(!pkInContainer->DropItem(iItemID,pkCharacter->GetWorldPosV()+kOffset))
							SayToClients("Could not drop item","Server",-1,PkNetMessage->m_iClientID);																				
						
						SendContainer(pkInContainer->GetEntity()->GetEntityID(),PkNetMessage->m_iClientID,false);											
							
						break;
					}

				}
				else
				{
					//first do we own this container?						
					if(pkTargetContainer->GetOwnerID() != pkCharacter->GetEntityID())
					{
						SayToClients("Target container is not yours","Server",-1,PkNetMessage->m_iClientID);
						break;					
					}				
				
					cout<<"moving item to another container"<<endl;
					if(!pkTargetContainer->AddMove(iItemID,iPosX,iPosY,iCount))
						SayToClients("Could not move item to that container","Server",-1,PkNetMessage->m_iClientID);	
					
					SendContainer(pkInContainer->GetEntity()->GetEntityID(),PkNetMessage->m_iClientID,false);
					SendContainer(iTargetContainer,PkNetMessage->m_iClientID,false);						
					
					break;	
		
				}
			}
			

			cout<<"WARNING: bad item movement"<<endl;
			SayToClients("Bad item movement","Server",-1,PkNetMessage->m_iClientID);
			
			break;	
		}		
		
		case MLNM_CS_REQ_CONTAINER:
		{
			int iContainerID;
			PkNetMessage->Read(iContainerID);
			
			OpenContainer(iContainerID,PkNetMessage->m_iClientID);	
					
			break;
		}
		

		default:
			cout << "Error in game packet : " << (int) ucType << endl;
			PkNetMessage->SetError(true);
			return;
	}
}