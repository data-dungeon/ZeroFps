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
	SetCharStats(SIGetSelfID(), 0, 20);
	SetCharStats(SIGetSelfID(), 1, 20);

	SetMoveSpeed (SIGetSelfID(), 6);
	SetTeam (SIGetSelfID(), 4);
	SetRunAnim (SIGetSelfID(), "panic");
	PlayAnim(SIGetSelfID(), "idle");
	
	local pos = {GetVar("MobX"), 0, GetVar("MobZ")};
	pos[1] = pos[1] + Random(6)-3;
	pos[3] = pos[3] + Random(6)-3;

	MakePathFind(SIGetSelfID(),pos);
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

	-- have no pathfind, see if person has arrived at location
	local pos = {GetVar("MobX"), 0, GetVar("MobZ")};

	if 2 == 1 then
		AddToVar("MobSuccess", 1);
		Delete (SIGetSelfID());	
	else
		pos[1] = pos[1] + Random(6)-3;
		pos[3] = pos[3] + Random(6)-3;

		MakePathFind(SIGetSelfID(),pos);
	end
	


end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	SetEntityVar(SIGetSelfID(), "deadtime", 0);
end

function Panic()
end
