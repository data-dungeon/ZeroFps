function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/member.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
			InitParameter("team",0)
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");



end


function Init()
	SetMoveSpeed (SIGetSelfID(), 6.5);

	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent2/offensive/are you ready to be fucked, man.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent2/offensive/didn't I promise you fireworks.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent2/offensive/how many lives do you got, man.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent2/offensive/i give him my best!.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent2/offensive/if it bleeds, we can kill it.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent2/offensive/oh, sorry.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent2/offensive/where are you going.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent2/offensive/you scared, man.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent2/defensive/he's already dead.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent2/defensive/i'm too old for this shit!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent2/defensive/that was a bullet, wasn't it.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent2/defensive/well done, junior!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent2/defensive/ohh fuck.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent2/defensive/fuck.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent2/defensive/fuck2.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent2/defensive/fuck3.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/agent2/death/nooo.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/agent2/death/nooo2.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/agent2/death/nooo3.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/agent2/death/nooo4.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent2/select/what.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent2/select/if we gonna work together, you might try and be polite.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent2/select/speek2.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent2/select/what2.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent2/select/what do you want from me, my soul.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent2/select/what.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent2/select/yeah, so.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent2/select/yes my lord.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent2/move/my bags are packed!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent2/move/im already there.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent2/move/i'm back in five minutes!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent2/move/i'm done here!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent2/move/no action here.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent2/move/of course2!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent2/move/of course!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent2/move/says who.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent2/move/slaving for you!.wav");

	SetTeam (SIGetSelfID(), 0);
	SISetHeartRate(SIGetSelfID(),4);

--	Equip(SIGetSelfID(), "data/script/objects/dm/t_gun.lua", 1);
end

function FirstRun()
end

function HeartBeat()
	if ( IsDead(SIGetSelfID()) == 1) then
		AddToEntityVar (SIGetSelfID(), "deadtime", 1);

		if GetEntityVar(SIGetSelfID(), "deadtime") > 9 then
			Delete(SIGetSelfID());
		end
	
		return
	end

	if HavePath(SIGetSelfID()) == 1 then
		return
	end	

	-- Spela upp en liten kommentar då och då när personen står still och inte har blivit skadad
	if Random(100) > 85 then
			
		talk_sound = 
		{
		  "agent/talking/i want to do something for humanity.wav",
		  "agent/talking/one man alone cant fight the future.wav",
		  "agent/talking/i'm extremely nervous right now.wav",
		}		
		PlaySound (SIGetSelfID(), talk_sound[Random(3)+1] );
		
	end

	
end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	SetEntityVar (SIGetSelfID(), "deadtime", 0);
end

