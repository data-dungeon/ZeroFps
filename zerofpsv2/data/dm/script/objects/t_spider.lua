function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/spider.mad");			
			InitParameter("m_fScale","1");
			InitParameter("m_iShadowGroup","2");

		InitProperty("P_PfPath");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
			InitParameter("team",1)
		InitProperty("P_ScriptInterface");

end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),4);
end

function Init()
	SetMoveSpeed (SIGetSelfID(), 3);
	SetTeam (SIGetSelfID(), 1);

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


	local pos = GetObjectPos(SIGetSelfID());
	pos[1] = pos[1] + Random(20)-10;
	pos[3] = pos[3] + Random(20)-10;

	MakePathFind(SIGetSelfID(),pos);


end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	SetEntityVar(SIGetSelfID(), "deadtime", 0);
end

