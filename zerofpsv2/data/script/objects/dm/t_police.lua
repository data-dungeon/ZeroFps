
g_HelpingCharacterID = -1

function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/police.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_DMCharacter");
			InitParameter("team",2);
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");


end


function Init()
	SetMoveSpeed (SIGetSelfID(), 6.5);
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/I give him my best.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/If it bleeds, we can kill it.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/he's already dead.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/are you ready to be fucked, man.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/well done, junior.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/cyborg/what.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/cyborg/death1.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/cyborg/slaving for you.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/cyborg/my bags are packed.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/cyborg/yes my lord.wav");
--	SISetHeartRate(SIGetSelfID(),4);
	Equip(SIGetSelfID(), "data/script/objects/dm/t_gun.lua", 1); -- snuten skall naturligvis ha en picka i b�rjan... (tr�kigt spel annars)
	SetTeam (SIGetSelfID(), 2);
end

function FirstRun()

	SISetHeartRate(SIGetSelfID(),4);
end

function HeartBeat()
	if ( IsDead(SIGetSelfID()) == 1) then
		AddToEntityVar (SIGetSelfID(), "deadtime", 1);

		if GetEntityVar(SIGetSelfID(), "deadtime") > 9 then
			Delete(SIGetSelfID());
		end
	
		return
	end

	-- Lyssna om n�gon ropar p� hj�lp
	-- TODO: l�gg in en avst�ndscheck
	person_calling = GetClosestCaller(SIGetSelfID())

	if person_calling > 0 then
		g_HelpingCharacterID = person_calling
		Print( "Hearing person calling : ", g_HelpingCharacterID)
	end

	-- S�k reda p� personen i n�d.
	if g_HelpingCharacterID > 0 then

		civilian_pos = GetEntityPos(g_HelpingCharacterID)
		police_pos = GetEntityPos(SIGetSelfID())

		range = GetRangeBetween(civilian_pos, police_pos)

		if range > 10 then -- Inte framme hos den som ropar?

			Print( "Moving to person calling : ", g_HelpingCharacterID)

			if HavePath(SIGetSelfID()) == 1 then
				return
			else
				MakePathFind(SIGetSelfID(),civilian_pos);
			end

		else -- �r framme hos den som ropar?

			CallForHelp(g_HelpingCharacterID, -1) -- S�g till personen att sluta skrika (annars springer han till personen i n�d hela tiden)

			SetState(SIGetSelfID(),4) -- Get Aggro
			g_HelpingCharacterID = -1 -- Don't run to that person anymore
		end
	end

	local State = GetState(SIGetSelfID())

	if State == 4 then -- aggresive

		local closest_agent = GetDMCharacterClosest(SIGetSelfID())
	
		-- Close enough?
		agent_pos = GetEntityPos(closest_agent)
		police_pos = GetEntityPos(SIGetSelfID())

		range = GetRangeBetween(agent_pos, police_pos)

		if range < 10 then
			FireAtObject(SIGetSelfID(), closest_agent)
		else

			if HavePath(SIGetSelfID()) == 1 then
				return
			else
				
				agent_pos[1] = agent_pos[1] + Random(12)-6;
				agent_pos[3] = agent_pos[3] + Random(12)-6;

				MakePathFind(SIGetSelfID(),agent_pos);
			end
		end
	end	

	
end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	SetEntityVar (SIGetSelfID(), "deadtime", 0);
end

function GetRangeBetween(a, b) -- tar 2 tabeller med 3 double vardera

	local xdif = a[1] - b[1]
	local zdif = a[3] - b[3]

	-- vi �r intresserade av avst�nd
	if xdif < 0 then xdif = -xdif end
	if zdif < 0 then zdif = -zdif end

	range = xdif + zdif

	return range
end