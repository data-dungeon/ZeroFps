function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/citizen_dude.mad");			
			InitParameter("m_fScale","1");
			InitParameter("m_iShadowGroup","2");

		InitProperty("P_PfPath");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
		InitProperty("P_ScriptInterface");


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

	if ( IsDead(SIGetSelfID()) == 1) then
		AddToEntityVar (SIGetSelfID(), "deadtime", 1);

		if GetEntityVar(SIGetSelfID(), "deadtime") > 9 then
			Delete(SIGetSelfID());
		end
	
		return
	end

	local Life = GetCharStats(SIGetSelfID(), 0)
	local prev_life = GetEntityVar(SIGetSelfID(), "g_MechLife")

	-- Ropa på hjälp om han har blivit skadad igen.
	if Life < prev_life and IsDead(SIGetSelfID()) == 0 then
		CallForHelp(SIGetSelfID(), 1)
		Panic();
	end

	SetEntityVar(SIGetSelfID(), "g_MechLife", Life)

	local pos = GetObjectPos(SIGetSelfID());
	pos[1] = pos[1] + Random(12)-6;
	pos[3] = pos[3] + Random(12)-6;

	MakePathFind(SIGetSelfID(),pos);

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

end

function Panic()
	SetMoveSpeed (SIGetSelfID(), 5);
	SetRunAnim (SIGetSelfID(), "panic");
	SetIdleAnim (SIGetSelfID(), "panic_idle");
	PlayAnim(SIGetSelfID(), "panic_idle");	
	ClearPathFind(SIGetSelfID());
	SISetHeartRate(SIGetSelfID(),2);
end
