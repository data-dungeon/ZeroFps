

function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/baldie.mad");			
			InitParameter("m_fScale","1.1");
			InitParameter("m_iShadowGroup","2");

		InitProperty("P_PfPath");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
			InitParameter("team",1)
		InitProperty("P_ScriptInterface");

end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),4);
	local life = GetCharStats(SIGetSelfID(), 0)
	SetEntityVar(SIGetSelfID(), "g_BaldieLife", life)
end

function Init()
	SetState(SIGetSelfID(), 0);
	SetMoveSpeed (SIGetSelfID(), 3.3);
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

	AddItem(SIGetSelfID(), "data/script/objects/weapons/iron_bar.lua", 1);
	SetTeam (SIGetSelfID(), 4);

	SISetHeartRate(SIGetSelfID(),2);

	PlayAnim(SIGetSelfID(), "idle");
end

function HeartBeat()

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

	local Life = GetCharStats(SIGetSelfID(), 0)
	local prev_life = GetEntityVar(SIGetSelfID(), "g_BaldieLife")

	-- om skadad, bli aggro
	if Life < prev_life and IsDead(SIGetSelfID()) == 0 then
		SetState(SIGetSelfID(), 4) -- aggro
		SetVar("HarryWounded", 1);
	end

	SetEntityVar(SIGetSelfID(), "g_BaldieLife", Life)


	-- check if harry is wounded, if so, get aggro
	if GetVar("HarryWounded") == 1 then
		SetState (SIGetSelfID(), 4);
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
		RunScript ("data/script/objects/pickup gears/money.lua", SIGetSelfID());
	end

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
		agent_pos[1] = agent_pos[1] + Random(3)-1.5;
		agent_pos[3] = agent_pos[3] + Random(3)-1.5;
		MakePathFind(SIGetSelfID(),agent_pos);
	end

end

function SellDrugs() --walk around some

	-- never wander to far away from harry
	if DistanceTo(SIGetSelfID(), GetVar("HarryID")) > 5 then
		local pos = GetObjectPos(GetVar("HarryID"));
		pos[1] = pos[1] + Random(6)-3;
		pos[3] = pos[3] + Random(6)-3;

		MakePathFind(SIGetSelfID(),pos);		
	else
		local pos = GetObjectPos(SIGetSelfID());
		pos[1] = pos[1] + Random(8)-4;
		pos[3] = pos[3] + Random(8)-4;

		MakePathFind(SIGetSelfID(),pos);
	end


end
