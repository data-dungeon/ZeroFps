function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/citizen_mechanic.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");

end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),4);
end

function Init()
	SetMoveSpeed (SIGetSelfID(), 1.9);
end

function HeartBeat()


	if HavePath(SIGetSelfID()) then
		return;
	end	

	if ( IsDead(SIGetSelfID()) == 1) then
		AddToEntityVar (SIGetSelfID(), "deadtime", 1);

		if GetEntityVar(SIGetSelfID(), "deadtime") > 9 then
			Delete(SIGetSelfID());
		end
		
		return;
	end

	local pos = GetObjectPos(SIGetSelfID());
	pos[1] = pos[1] + Random(12)-6;
	pos[3] = pos[3] + Random(12)-6;
	
	MakePathFind(SIGetSelfID(),pos);

end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	PlaySound (SIGetSelfID(), "death/DEATH6.WAV");
	SetEntityVar(SIGetSelfID, "deadtime", 0);

	if Random(10) < 3 then
		RunScript ("data/script/objects/dm/t_money.lua", SIGetSelfID());
	end
end
