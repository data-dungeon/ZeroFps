function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/citizen_female.mad");			
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
	SetEntityVar(SIGetSelfID(), "g_WomenLife", life)
end

function Init()
	SetMoveSpeed (SIGetSelfID(), 2.5);
	SetTeam (SIGetSelfID(), 1);

	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/eee! you’re crazy!2.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/eee! you’re crazy!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/eee!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/fuck your sick2.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/fuck your sick3.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/fuck your sick.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/help2.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/help3.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/help.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/i'm not your ex-girlfriend little boy!!!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/oh my god2.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/oh my god3.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/ooo! you’re crazy!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/ooo! you’re crazy!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_woman/defensive/what.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_woman/death/nooo.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_woman/death/nooo2.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_woman/death/nooo3.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_woman/death/nooo4.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_woman/death/nooo5.wav");

	-- set life
	SetCharStats(SIGetSelfID(), 0, 20);
	SetCharStats(SIGetSelfID(), 1, 20);

	PlayAnim(SIGetSelfID(), "idle");
end

function HeartBeat()

	local Life = GetCharStats(SIGetSelfID(), 0)
	local prev_life = GetEntityVar(SIGetSelfID(), "g_WomenLife")

	-- Ropa på hjälp om han har blivit skadad igen.
	if Life < prev_life and IsDead(SIGetSelfID()) == 0 then
		CallForHelp(SIGetSelfID(), 1)
		Panic();
	end

	-- Registrera nuvarande liv
	SetEntityVar(SIGetSelfID(), "g_WomenLife", Life)

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

	local pos = GetObjectPos(SIGetSelfID());
	pos[1] = pos[1] + Random(20)-10;
	pos[3] = pos[3] + Random(20)-10;

	MakePathFind(SIGetSelfID(),pos);

	-- Spela upp en liten kommentar då och då när personen står still och inte har blivit skadad
	if Random(100) > 75 and Life == prev_life then
		if GetState (SIGetSelfID()) == 0 then
			talk_sound = 
			{
			  "citizen_woman/talking/forgive me your highness, I did not see you2.wav",
			  "citizen_woman/talking/forgive me your highness, I did not see you3.wav",
			  "citizen_woman/talking/forgive me your highness, I did not see you.wav",
			  "citizen_woman/talking/hello there mr nice guy!.wav",
	
			  "citizen_woman/talking/i'm a dancer2.wav",
			  "citizen_woman/talking/i'm a dancer3.wav",
			  "citizen_woman/talking/i'm a dancer.wav",
			  "citizen_woman/talking/so what do you like to do2.wav",
			  
			  "citizen_woman/talking/so what do you like to do.wav",
			  "citizen_woman/talking/well hello there mr nice guy.wav",
			  "citizen_woman/talking/well, you know2.wav",
			  "citizen_woman/talking/well, you know.wav",
			  
			  "citizen_woman/talking/what do you do for a living2.wav",
			  "citizen_woman/talking/what do you do for a living3.wav",
			  "citizen_woman/talking/what do you do for a living.wav",
			  "citizen_woman/talking/you married2.wav",
		  
			  "citizen_woman/talking/you married.wav",
			}
			
			PlaySound (SIGetSelfID(), talk_sound[Random(17)+1] );
		end
	end

end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	SetEntityVar(SIGetSelfID(), "deadtime", 0);

	PanicArea(SIGetSelfID(), 12);

	if Random(10) < 6 then
		RunScript ("data/script/objects/dm/t_money.lua", SIGetSelfID());
	end

end

function Panic()
	SetMoveSpeed (SIGetSelfID(), 6);
	SetRunAnim (SIGetSelfID(), "panic");
	SetIdleAnim (SIGetSelfID(), "panic_idle");
	PlayAnim(SIGetSelfID(), "panic_idle");	
	ClearPathFind(SIGetSelfID());
	SISetHeartRate(SIGetSelfID(),2);

end
