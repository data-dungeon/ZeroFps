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

	SISetHeartRate(SIGetSelfID(), 2);

	SetEntityVar(SIGetSelfID(), "g_WomenLife", life)

	SetEntityVar(SIGetSelfID(), "Waypoint", 0);

end



function Init()

	SetCharStats(SIGetSelfID(), 0, 20);

	SetCharStats(SIGetSelfID(), 1, 20);



	SetMoveSpeed (SIGetSelfID(), 6.5);

	SetTeam (SIGetSelfID(), 4);

	SetRunAnim (SIGetSelfID(), "panic");

	PlayAnim(SIGetSelfID(), "idle");

	

	local pos = {GetVar("MobX"), 0, GetVar("MobZ")};

	pos[1] = pos[1] + Random(6)-3;

	pos[3] = pos[3] + Random(6)-3;



	MakePathFind(SIGetSelfID(),GetNextWaypoint());

end



function HeartBeat()



	SelfID = SIGetSelfID();



	if HavePath(SelfID) == 1 then

		return

	end





	if ( IsDead(SelfID) == 1) then

		AddToEntityVar (SelfID, "deadtime", 1);



		if GetEntityVar(SelfID, "deadtime") > 9 then

			Delete(SelfID);

		end

	

		return

	end





	AddToEntityVar(SelfID, "Waypoint", 1);





	if GetEntityVar(SelfID, "Waypoint") == 4 then

		AddToVar("MobSuccess", 1);

		RunScript("data/script/objects/particles/explosion.lua",SIGetSelfID());

		Delete (SelfID);	

	else

		MakePathFind(SelfID, GetNextWaypoint());

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



function GetNextWaypoint()

	local pos = {0,0,0};

	local waypoint = GetEntityVar(SIGetSelfID(), "Waypoint");





	if waypoint == 0 then

		pos[1] = GetVar("Mob_WP1_X");

		pos[2] = GetVar("Mob_WP1_Y");

		pos[3] = GetVar("Mob_WP1_Z");

	elseif waypoint == 1 then

		pos[1] = GetVar("Mob_WP2_X");

		pos[2] = GetVar("Mob_WP2_Y");

		pos[3] = GetVar("Mob_WP2_Z");

	elseif waypoint == 2 then

		pos[1] = GetVar("Mob_WP3_X");

		pos[2] = GetVar("Mob_WP3_Y");

		pos[3] = GetVar("Mob_WP3_Z");

	elseif waypoint == 3 then

		pos[1] = GetVar("Mob_WP4_X");

		pos[2] = GetVar("Mob_WP4_Y");

		pos[3] = GetVar("Mob_WP4_Z");

	end



	pos[1] = pos[1] + Random(2) - 1;

	pos[3] = pos[3] + Random(1) - 1;



	return pos;

end