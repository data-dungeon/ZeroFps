

function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/baldie.mad");			
			InitParameter("m_fScale","1.1");		

		InitProperty("P_PfPath");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
			InitParameter("team",1)	
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");

end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),4);
	local life = GetCharStats(SIGetSelfID(), 0)
	SetEntityVar(SIGetSelfID(), "g_BaldieLife", life)
end

function Init()
	SetState(SIGetSelfID(), 0);
	SetMoveSpeed (SIGetSelfID(), 2.5);
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

	AddItem(SIGetSelfID(), "data/script/objects/dm/t_iron_bar.lua", 1);
	SetTeam (SIGetSelfID(), 4);

	SISetHeartRate(SIGetSelfID(),2);
end

function HeartBeat()

	local Life = GetCharStats(SIGetSelfID(), 0)
	local prev_life = GetEntityVar(SIGetSelfID(), "g_BaldieLife")

	-- om skadad, bli aggro
	if Life < prev_life and IsDead(SIGetSelfID()) == 0 then
		CallForHelp(SIGetSelfID(), 4) -- aggro
	end

	SetEntityVar(SIGetSelfID(), "g_BaldieLife", Life)

	if HavePath(SIGetSelfID()) == 1 then
		return
	end	

	if ( IsDead(SIGetSelfID()) == 1) then
		AddToEntityVar (SIGetSelfID(), "deadtime", 1);

		if GetEntityVar(SIGetSelfID(), "deadtime") > 9 then
			Delete(SIGetSelfID());
		end
	
		return
	end

	local State = GetState(SIGetSelfID());

	if State == 4 then
		Aggro();
	end

	if State == 0 then
		SellDrugs();
	end
	

end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());

	if Random(2) == 1 then
		PlaySound (SIGetSelfID(), "mechanic/death/DEATH1.WAV");
	else
		PlaySound (SIGetSelfID(), "mechanic/death/DEATH2.WAV");
	end

	SetEntityVar(SIGetSelfID(), "deadtime", 0);

	PanicArea(SIGetSelfID(), 12);

	if Random(10) < 3 then
		RunScript ("data/script/objects/dm/t_money.lua", SIGetSelfID());
	end

	SISetHeartRate(SIGetSelfID(),-1);
end

function Aggro() -- attacks player

	local closest_agent = GetDMCharacterClosest(SIGetSelfID())

	if closest_agent == -1 then
		SetState(SIGetSelfID(),0) -- set Idle
		return
	end

	-- Close enough?
	range = DistanceTo(closest_agent, SIGetSelfID());

	if range < GetWeaponRange(SIGetSelfID()) then
		FireAtObject(SIGetSelfID(), closest_agent)
	else
		agent_pos = GetEntityPos(closest_agent);
		agent_pos[1] = agent_pos[1] + Random(4)-2;
		agent_pos[3] = agent_pos[3] + Random(4)-2;
		MakePathFind(SIGetSelfID(),agent_pos);
	end

end

function SellDrugs() --walk around some
	local pos = GetObjectPos(SIGetSelfID());
	pos[1] = pos[1] + Random(12)-6;
	pos[3] = pos[3] + Random(12)-6;

	MakePathFind(SIGetSelfID(),pos);
end
