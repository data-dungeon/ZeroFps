
function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/police.mad");			
			InitParameter("m_fScale","1");
			InitParameter("m_iShadowGroup","2");		

		InitProperty("P_PfPath");
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
--	SISetHeartRate(SIGetSelfID(),4);
	SetTeam (SIGetSelfID(), 2);
	AddItem(SIGetSelfID(), "data/script/objects/weapons/rifle.lua", 1); -- snuten skall naturligvis ha en picka i början... (tråkigt spel annars)

	PlayAnim(SIGetSelfID(), "idle");

end

function FirstRun()

	SISetHeartRate(SIGetSelfID(),1);
	SetEntityVar(SIGetSelfID(), "g_HelpingCharacterID", -1)

end

function InitPoliceData()

	local civilian_list = GetCharsByFraction(1)
	local num_civilians = civilian_list[1]

	Print ( "Antal civila : ", num_civilians)

	for x = 2, num_civilians+1, 1
	do
		local civilian_pos = GetEntityPos(civilian_list[x])
		civilian_pos[1] = civilian_pos[1] + Random(3)
		civilian_pos[3] = civilian_pos[3] + Random(3)
		AddPatrolPoint(SIGetSelfID(), civilian_pos)
	end

	AddPatrolPoint(SIGetSelfID(), GetEntityPos(SIGetSelfID()))
	SetEntityVar(SIGetSelfID(), "CreateCounter", 10)
end

function HeartBeat()



	-----------------------------------------------------------------------
	-- Vänta på att alla objekt skall ha skapats så att vi kan 
	-- skapa en lista över alla civila.
	-----------------------------------------------------------------------
	local cc = GetEntityVar(SIGetSelfID(), "CreateCounter")
	if cc == 2 then
		InitPoliceData()
	end
	if cc < 10 then
		cc = cc + 1
		SetEntityVar(SIGetSelfID(), "CreateCounter", cc)
		return
	end
	-----------------------------------------------------------------------



	if ( IsDead(SIGetSelfID()) == 1) then
		AddToEntityVar (SIGetSelfID(), "deadtime", 1);

		if GetEntityVar(SIGetSelfID(), "deadtime") > 9 then
			Delete(SIGetSelfID());
		end
	
		return
	end

	-- Lyssna om någon ropar på hjälp
	person_calling = GetClosestCaller(SIGetSelfID())

	HelpingCharacterID = GetEntityVar(SIGetSelfID(), "g_HelpingCharacterID")

	if person_calling > 0 and HelpingCharacterID == -1 then

		civilian_pos = GetEntityPos(person_calling)
		police_pos = GetEntityPos(SIGetSelfID())
		range = GetRangeBetween(civilian_pos, police_pos)

		if range < 30 then
			SetEntityVar(SIGetSelfID(), "g_HelpingCharacterID", person_calling)
			Print( "Hearing person calling : ", person_calling)
		else
			--Print( "A person is calling for help, but thed istance to far, police dont hear call from person, range is: ", range)
		end
	end

	-- Sök reda på personen i nöd.
	if HelpingCharacterID > 0 then

		civilian_pos = GetEntityPos(HelpingCharacterID)
		police_pos = GetEntityPos(SIGetSelfID())
		range = GetRangeBetween(civilian_pos, police_pos)

		if range > 10 then -- Inte framme hos den som ropar?

			Print( "Moving to person calling : ", HelpingCharacterID)

			if HavePath(SIGetSelfID()) == 1 then
				return
			else
				MakePathFind(SIGetSelfID(),civilian_pos);
			end

		else -- Är framme hos den som ropar?

			CallForHelp(HelpingCharacterID, -1) -- Säg till personen att sluta skrika (annars springer polisen till personen i nöd hela tiden)

			SetState(SIGetSelfID(),4) -- Get Aggro
			HelpingCharacterID = -1 -- Don't run to that person anymore
			SetEntityVar(SIGetSelfID(), "g_HelpingCharacterID", HelpingCharacterID)
		end
	end

	local State = GetState(SIGetSelfID())

	if State == 4 then -- aggresive

		local closest_agent = GetDMCharacterClosest(SIGetSelfID())

		if closest_agent == -1 then
			SetState(SIGetSelfID(),0) -- Get Idle
			Patrol(SIGetSelfID())
			return
		end
	
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
	else
		Patrol(SIGetSelfID());


		if HavePath(SIGetSelfID()) == 0 then
			pos = GetEntityPos(SIGetSelfID());
			pos[1] = pos[1] + Random(20)-10;
			pos[3] = pos[3] + Random(20)-10;
			MakePathFind(SIGetSelfID(),pos);
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

	-- vi är intresserade av avstånd
	if xdif < 0 then xdif = -xdif end
	if zdif < 0 then zdif = -zdif end

	range = xdif + zdif

	return range
end

function OnTakingDmgFrom(EnemyShooting)

	Print("Enemy is: ", EnemyShooting)
	SetState(SIGetSelfID(),4) -- Get Aggro

end
