function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/member.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_DMCharacter");
			InitParameter("team",0)
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");


end


function Init()
	SetMoveSpeed (SIGetSelfID(), 6.5);
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/I give him my best.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/If it bleeds, we can kill it.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/he's already dead.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/are you ready to be fucked, man.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/well done, junior.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/what.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/cyborg/death1.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/cyborg/slaving for you.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/cyborg/my bags are packed.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/cyborg/yes my lord.wav");
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
end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	SetEntityVar (SIGetSelfID(), "deadtime", 0);
end

