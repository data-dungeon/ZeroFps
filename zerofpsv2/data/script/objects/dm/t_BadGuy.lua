deadtime = {};

function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/goblin.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_PfPath");
		InitProperty("P_Track");
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
	SetMoveSpeed (SIGetSelfID(), 1.9);
	SetTeam (SIGetSelfID(), 1);
end

function HeartBeat()

	if HavePath(SIGetSelfID()) then
		return
	end	

	if ( IsDead(SIGetSelfID()) == 1) then
		deadtime[SIGetSelfID()] = deadtime[SIGetSelfID()] + 1;


		if deadtime[SIGetSelfID()] > 9 then
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
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	PlaySound (SIGetSelfID(), "death/DEATH1.WAV");
	SetVar ("BadGuyDead", 1);
end
