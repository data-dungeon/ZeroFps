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
end

function Init()
	SetMoveSpeed (SIGetSelfID(), 2.6);
	SetTeam (SIGetSelfID(), 1);
end

function Exit()
	PlayAnim(SIGetSelfID(), "open");
	SetNextAnim(SIGetSelfID(), "idle");
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

	local pos = GetObjectPos(SIGetSelfID());
	pos[1] = pos[1] + Random(20)-10;
	pos[3] = pos[3] + Random(20)-10;

	MakePathFind(SIGetSelfID(),pos);

end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	PlaySound (SIGetSelfID(), "death/DEATH6.WAV");
	SetEntityVar(SIGetSelfID, "deadtime", 0);


	PanicArea (SIGetSelfID(), 5);

	if Random(10) < 4 then
		RunScript ("data/script/objects/dm/t_money.lua", SIGetSelfID());
	end
end

function Panic()
	SetMoveSpeed (SIGetSelfID(), 6);
	SetRunAnim (SIGetSelfID(), "panic");
	SetIdleAnim (SIGetSelfID(), "panic_idle");	
	ClearPathFind(SIGetSelfID());
	SISetHeartRate(SIGetSelfID(),2);
end

