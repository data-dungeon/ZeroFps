function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/citizen_man.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_PfPath");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
			InitParameter("team",1)
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");

end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),4);
	SetEntityVar(SIGetSelfID(), "g_CitizenManLife", Life)
end

function Init()
	SetMoveSpeed (SIGetSelfID(), 2.6);
	SetTeam (SIGetSelfID(), 1);

	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/help.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/have you lost your mind, dude.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/do you want this jacket, take it2!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/listen crazy man, I can give you money!!!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/oh my god!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/you can't talk with me like that!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/damn.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_man/death/god!.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_man/death/nooo2.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_man/death/nooo.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_man/death/I'm not afraid! I'm going to a world where anything is possible!.wav");

	PlayAnim(SIGetSelfID(), "idle");

	--set life
	SetCharStats(SIGetSelfID(), 0, 20);
	SetCharStats(SIGetSelfID(), 1, 20);
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
	local prev_life = GetEntityVar(SIGetSelfID(), "g_CitizenManLife")

	-- Ropa på hjälp om han har blivit skadad igen.
	if Life < prev_life and IsDead(SIGetSelfID()) == 0 then
		CallForHelp(SIGetSelfID(), 1)
		Panic();
	end

	-- Registrera nuvarande liv
	SetEntityVar(SIGetSelfID(), "g_CitizenManLife", Life)

	local pos = GetObjectPos(SIGetSelfID());
	pos[1] = pos[1] + Random(20)-10;
	pos[3] = pos[3] + Random(20)-10;

	MakePathFind(SIGetSelfID(),pos);

	-- Spela upp en liten kommentar då och då när personen står still och inte har blivit skadad
	if Random(100) > 75 and Life == prev_life then
		if GetState (SIGetSelfID()) == 0 then --if idle
			talk_sound = 
			{
			  "citizen_man/talking/are you drunk.wav",
			  "citizen_man/talking/hello there mr nice guy!.wav",
			  "citizen_man/talking/that gun you have there looks powerful.wav",
			  "citizen_man/talking/you look like a normal person...skratt.wav",
			}
			
			PlaySound (SIGetSelfID(), talk_sound[Random(4)+1] );
		end
	end

end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	SetEntityVar(SIGetSelfID(), "deadtime", 0);

	PanicArea (SIGetSelfID(), 13);

	if Random(10) < 4 then
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
