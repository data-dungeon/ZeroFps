

function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/spider.mad");			
			InitParameter("m_fScale","1.0");
			InitParameter("m_iShadowGroup","2");

		InitProperty("P_PfPath");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
			InitParameter("team",1)
		InitProperty("P_ScriptInterface");

--		InitProperty("P_Tcs");		
--			InitParameter("polygontest","false");
--			InitParameter("gravity","true");
--			InitParameter("character","true");			
--			InitParameter("radius",-1);			
--			InitParameter("group",2);
--			InitParameter("groupflag",0);
--			InitParameter("groupflag",1);			
--			InitParameter("leglength",1.1);			
--			InitParameter("walkablegroupflag",0);
--			InitParameter("airfriction",0.1);
--			InitParameter("mass",1);
--			InitParameter("inertia",1);
--			InitParameter("bounce",0);
--			InitParameter("friction",5);			
--			InitParameter("activemoment","false");
--			InitParameter("cantsleep","true");		
		
end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),4);
end

function Init()
	SetMoveSpeed (SIGetSelfID(), 1.9);
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/mechanic/defensive/daq_ahh_that_fuckin_hurt.wav")
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/mechanic/defensive/daq_careful.wav")
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/mechanic/defensive/daq_damn.wav")
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/mechanic/defensive/daq_didnt_do_nothin.wav")
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/mechanic/defensive/daq_dont_shoot.wav")
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/mechanic/defensive/daq_fuck.wav")
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/mechanic/defensive/daq_please_dont_shoot_me.wav")
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/mechanic/defensive/daq_shit.wav")
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/mechanic/defensive/daq_you_shootin_at_me_for.wav")
	AddDeathSound(SIGetSelfID(), "data/sound/mechanic/death/death1.wav")
	AddDeathSound(SIGetSelfID(), "data/sound/mechanic/death/death2.wav")

	--set life
	SetCharStats(SIGetSelfID(), 0, 20);
	SetCharStats(SIGetSelfID(), 1, 20);

	PlayAnim(SIGetSelfID(), "idle");
	SetTeam (SIGetSelfID(), 1);

	SetEntityVar(SIGetSelfID(), "g_MechLife", Life)
end

function HeartBeat()

	if HavePath(SIGetSelfID()) == 1 then
		return
	end	

	if IsDead(SIGetSelfID()) == 1 then
		AddToEntityVar (SIGetSelfID(), "deadtime", 1);

		if GetEntityVar(SIGetSelfID(), "deadtime") > 9 then
			Delete(SIGetSelfID());
		end
	
		return
	end


	local pos = GetObjectPos(SIGetSelfID());
	pos[1] = pos[1] + Random(12)-6;
	pos[3] = pos[3] + Random(12)-6;

	MakePathFind(SIGetSelfID(),pos);

end

function Dead()
--	Print("DIIIIIIIIE");
	
--	PlaySound(SIGetSelfID(),"deaths/trolldie.wav");
	
	p = GetObjectPos(SIGetSelfID());
	p[2] = p[2] + 0.5;
	
	CreateEntity("data/script/objects/spiderflesh.lua",p);	
	CreateEntity("data/script/objects/spiderflesh.lua",p);	
	CreateEntity("data/script/objects/spiderflesh.lua",p);	
	CreateEntity("data/script/objects/spiderflesh.lua",p);	
	

	Delete(SIGetSelfID());

	
	
end
