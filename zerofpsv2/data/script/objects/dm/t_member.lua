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
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/robot/I give him my best.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/robot/If it bleeds, we can kill it.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/robot/he's already dead.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/robot/are you ready to be fucked, man.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/robot/well done, junior.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/robot/what.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/robot/death1.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/robot/slaving for you.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/robot/yes my lord.wav");
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

