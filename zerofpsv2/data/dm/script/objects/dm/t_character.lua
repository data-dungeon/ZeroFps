function Create()
		InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/member.mad");
		InitProperty("P_DMGun");
		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_DMCharacter");
			InitParameter("team",0)
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");
end

function FirstRun()
	SISetHeartRate(SIGetSelfID(),0.6);
end

function HeartBeat()
	if HaveOrders(SIGetSelfID()) == 1 then
		return;
	end

	--keep near mainAgent
	mainAgentID = GetVar("MainAgent");
	selfID = SIGetSelfID();

	if DistanceTo(selfID, mainAgentID) > 3 then
		Print("Tjoff");
		agent_pos = GetEntityPos(mainAgentID);
		agent_pos[1] = agent_pos[1] + Random(4)-2;
		agent_pos[3] = agent_pos[3] + Random(4)-2;
		MakePathFind(selfID,agent_pos);
	end
	
	-- if ammo low, reload

	-- if life low, and has healthpack in belt, use it

	-- if mainAgent shoots, imitate (5 = shooting)
	if GetState(mainAgentID) == 5 then
		FireAtLocation(  SIGetSelfID(), GetTarget( mainAgentID )  );
	end

	-- if is under attack, shoot back

	-- if enemy is near, shoot back
end

function Dead()
	Print("IM DEAD!!!");
	Delete(SIGetSelfID());
end