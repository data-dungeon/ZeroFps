
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Cops and c4
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Cops and c4", difficulty=1, xp=900, cash=500, success=0 } -- difficulty = Level

MissionText = 
{ 
	short = "Cops and c4\n\nBlow up a police station.",
	long = [[Cops and c4\n\nThe Bomb Squad wants you to deliver a bomb to the police station in 
	the north district as a revenge for not doing a good work. But watch out! 
	They expect a visit...\n\nThe bomb will be sent to your headquarter, don't forget to bring it with you!\n\nYou will get 500 cash when this thing is over.]]
}

-- Bomben i spelet
BombEntID = -1
BombTicks = -1
PoliceStationID = -1
HQPos = {}
PoliceForce = {}

Skin1 = { tex1="dm/map_icons/policestation.bmp", tex2="0", tex3="0", tex4="0", 
 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

function OnMissionStart()

	-----------------------------------------------------------------------
	-- Lägg till Da bomb till spelarens hq
	-----------------------------------------------------------------------

	local hq = GetDMObject(0)
	AddItem(hq, "data/script/objects/dm/t_bomb.lua", 0)

	BombEntID = GetItemByName("police_bomb")
	Print( "BombEntID = ", BombEntID)

	PoliceStationID = GetDMObject(2)
	Print( "PoliceStationID = ", PoliceStationID)

	HQPos = GetEntityPos(hq)

	-----------------------------------------------------------------------
	-- Skapa 3 arga poliser
	-----------------------------------------------------------------------

	pos1 = { 32.5738,1.40289,26.3275 }
	pos2 = { 31.9065,1.22861,18.5373 }
	pos3 = { 23.8532,1.60644,17.2107 }
	pos4 = { 12.4345,1.42537,16.631 }
	pos5 = { 7.31069,1.32392,20.3325 }
	pos6 = { 7.3235,1.29891,26.9971 }
	pos7 = { 8.69011,1.34066,42.1484 }
	pos8 = { 15.3835,1.48972,42.964 }
	pos9 = { 26.4581,1.42928,42.9286 }
	pos10 = { 32.2395,1.40035,40.4068 }

	-- Skapa polis 2
	local police1 = RunScript("data/script/objects/dm/t_police.lua", -1, pos1)
	PoliceForce[1] = police1
	ClearPatrolPath(police1)
	AddPatrolPoint(police1, pos1)
	AddPatrolPoint(police1, pos2)
	AddPatrolPoint(police1, pos3)
	AddPatrolPoint(police1, pos4)
	AddPatrolPoint(police1, pos5)
	AddPatrolPoint(police1, pos6)
	AddPatrolPoint(police1, pos7)
	AddPatrolPoint(police1, pos8)
	AddPatrolPoint(police1, pos9)
	AddPatrolPoint(police1, pos10)

	-- Skapa polis 2
	local police2 = RunScript("data/script/objects/dm/t_police.lua", -1, pos2)
	PoliceForce[2] = police2
	ClearPatrolPath(police2)
	AddPatrolPoint(police2, pos2)
	AddPatrolPoint(police2, pos3)
	AddPatrolPoint(police2, pos4)
	AddPatrolPoint(police2, pos5)
	AddPatrolPoint(police2, pos6)
	AddPatrolPoint(police2, pos7)
	AddPatrolPoint(police2, pos8)
	AddPatrolPoint(police2, pos9)
	AddPatrolPoint(police2, pos10)
	AddPatrolPoint(police2, pos1)

	-- Skapa polis 3
	local police3 = RunScript("data/script/objects/dm/t_police.lua", -1, pos3)
	PoliceForce[3] = police3
	ClearPatrolPath(police3)
	AddPatrolPoint(police3, pos3)
	AddPatrolPoint(police3, pos4)
	AddPatrolPoint(police3, pos5)
	AddPatrolPoint(police3, pos6)
	AddPatrolPoint(police3, pos7)
	AddPatrolPoint(police3, pos8)
	AddPatrolPoint(police3, pos9)
	AddPatrolPoint(police3, pos10)
	AddPatrolPoint(police3, pos1)
	AddPatrolPoint(police3, pos2)

--	ch = GetDMCharacterClosest(hq)	
--	SetObjectPos(ch, pos1)

	-----------------------------------------------------------------------
	-- Rensa alla gamla mapiconer och lägg till en ny som visar
	-- vart polistationen finns.
	-----------------------------------------------------------------------
	HideAllMapIcons()
	CreateWnd(4,"PoliceStationMapLabel","MapWnd","",330,164,16,16,0)
	ChangeSkin("PoliceStationMapLabel","Skin1","Label")

end

function OnMissionSuccess()



end

function OnMissionFailed()



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

function BlowupPoliceStation()

	for i=1, 50, 1
	do
		Print("BOOOOOM")
	end

	RunScript("data/script/objects/dm/t_bigexposion.lua",BombEntID);
	Delete(BombEntID)
	Delete(PoliceStationID)

	bomb_loc = GetEntityPos(BombEntID)

	zonid = GetZoneIDAtPos(bomb_loc)
	SetZoneModel(zonid, "data/mad/zones/dm/zon_ruin.mad")

	----------------------------------------------------
	-- Nu går poliserna till attack
	----------------------------------------------------

	SetState(PoliceForce[1],4) -- Get Aggro
	SetState(PoliceForce[2],4) -- Get Aggro
	SetState(PoliceForce[3],4) -- Get Aggro

end

function IsMissionDone()

	-----------------------------------------------------------------------
	-- Kolla så att alla objekt finns.
	-----------------------------------------------------------------------

	if PoliceStationID < 0 or BombEntID < 0 then

		Print( "Mission Cops and c4 are mission some objects" )

		-- Fortsätt leta efter polisstationen
		PoliceStationID = GetDMObject(2)
		return
	end

	-----------------------------------------------------------------------
	-- Hämta position på bomb och polisstation
	-----------------------------------------------------------------------
	bomb_loc = GetEntityPos(BombEntID)

	Print( "Da bomp is at point: ")
	Print( bomb_loc[1] )
	Print( bomb_loc[2] )
	Print( bomb_loc[3] )
	Print( "---------------------")

	policestation_loc = GetEntityPos(PoliceStationID)

	Print( "Policestation is at point: ")
	Print( policestation_loc[1] )
	Print( policestation_loc[2] )
	Print( policestation_loc[3] )
	Print( "---------------------")

	-----------------------------------------------------------------------
	-- Smäll inte av bomben i högkvarteret
	-----------------------------------------------------------------------
	if HQPos[1] == bomb_loc[1] and 
	   HQPos[2] == bomb_loc[2] and 
	   HQPos[3] == bomb_loc[3]
	then
		Print("Bomb is in HQ")
		return
	end

	-----------------------------------------------------------------------
	-- Kolla om bomben ligger nära polisstationen.
	-- Det gör den endast när den är droppad på marken eftersom bomben
	-- inte får någon position i världen förränn den har placerats synligt 
	-- i världen.
	-----------------------------------------------------------------------

	if GetRangeBetween( bomb_loc, policestation_loc ) < 5 then

		BombTicks = BombTicks + 1
	end

	-----------------------------------------------------------------------
	-- Vänta 3 * 3 = 9 sek innan bomben sprängs
	-----------------------------------------------------------------------
	if BombTicks > 3 then

		BlowupPoliceStation()
		MissionInfo.success = 1
	end

end

function IsMissionFailed()



end