function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/harry.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
			InitParameter("team",4)
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");

end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),4);
	SetEntityVar(SIGetSelfID(), "g_HarryLife", Life)
end

function Init()
	SetMoveSpeed (SIGetSelfID(), 2.4);
	SetTeam (SIGetSelfID(), 4);
	SetState(SIGetSelfID(), 0);

	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/help.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/have you lost your mind, dude.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/god damn! stop that, mr. military guy!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/do you want this jacket, take it2!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/listen crazy man, I can give you money!!!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/oh my god!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/you can't talk with me like that!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/citizen_man/defensive/damn.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_man/death/god!.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_man/death/nooo2.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_man/death/nooo.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/citizen_man/death/I'm not afraid! I'm going to a world where anything is possible!.wav");

	AddItem(SIGetSelfID(), "data/script/objects/dm/t_shotgun.lua", 1);
	PlayAnim(SIGetSelfID(), "idle");
end


function HeartBeat()

	if ( IsDead(SIGetSelfID()) == 1) then
		AddToEntityVar (SIGetSelfID(), "deadtime", 1);

		if GetEntityVar(SIGetSelfID(), "deadtime") > 9 then
			Delete(SIGetSelfID());
		end
	
		return
	end

	local Life = GetCharStats(SIGetSelfID(), 0)
	local prev_life = GetEntityVar(SIGetSelfID(), "g_HarryLife")

	if Life < prev_life and IsDead(SIGetSelfID()) == 0 then
		if GetState(SIGetSelfID()) == 0 then
			SetState (SIGetSelfID(), 4); --agro
		end
	end

	-- Registrera nuvarande liv
	SetEntityVar(SIGetSelfID(), "g_HarryLife", Life)

	if HavePath(SIGetSelfID()) == 1 then
		return
	end	

	State = GetState(SIGetSelfID());


	if State == 4 then
		Aggro();
	end

	if State == 0 then
		Idle();
	end
	


end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	SetEntityVar(SIGetSelfID(), "deadtime", 0);

	PanicArea (SIGetSelfID(), 13);

	RunScript ("data/script/objects/dm/t_money.lua", SIGetSelfID());
	RunScript ("data/script/objects/dm/t_money.lua", SIGetSelfID());
	RunScript ("data/script/objects/dm/t_money.lua", SIGetSelfID());

end

function Aggro()
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


function Idle()
	Print("IDLE");

	local pos = {};
	if DistanceTo(SIGetSelfID(), GetVar("HarryHouseID")) > 10 then
		pos = GetObjectPos(GetVar("HarryHouseID"));
		pos[1] = pos[1] + Random(8)-4;
		pos[3] = pos[3] + Random(8)-4;
	else
		pos = GetObjectPos(SIGetSelfID());
		pos[1] = pos[1] + Random(10)-5;
		pos[3] = pos[3] + Random(10)-5;
	end


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

function Panic()
	SetMoveSpeed (SIGetSelfID(), 7);
	SetRunAnim (SIGetSelfID(), "panic");
	SetIdleAnim (SIGetSelfID(), "panic_idle");	
	PlayAnim(SIGetSelfID(), "panic_idle");
end
