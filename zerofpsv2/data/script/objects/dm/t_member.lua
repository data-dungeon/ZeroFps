function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/member.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_DMCharacter");
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
end

function FirstRun()

--	SISetHeartRate(SIGetSelfID(),1);
--	PlayAnim(GetSelfID(), "idle");

end

function HeartBeat()

end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
end

