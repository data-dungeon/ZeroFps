
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Cops and c4
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Cops and c4", difficulty=1, xp=900, cash=500, success=0 } -- difficulty = Level

MissionText = 
{ 
	short = "Cops and c4",
	long = [[Cops and c4\n\nThe Bomb Squad wants you to deliver a bomb to the police station in 
	the north district as a revenge for not doing a good work. But watch out! 
	They expect a visit...\n\nThe bomb will be delivered to your headquarter.\n\nYou will get 500 
	cash when this thing is over.]]
}

-- Bomben i spelet
BombEntID = -1
BombTicks = -1
PoliceStationID = -1
HQPos = {}

function OnMissionStart()

	-- Lägg till Da bomb till spelarens hq
	local hq = GetDMObject(0)
	AddItem(hq, "data/script/objects/dm/t_bomb.lua", 0)

	BombEntID = GetItemByName("police_bomb")
	Print( "BombEntID = ", BombEntID)

	PoliceStationID = GetDMObject(2)
	Print( "PoliceStationID = ", PoliceStationID)

	HQPos = GetEntityPos(hq)

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

	RunScript("data/script/objects/dm/t_explosion.lua",BombEntID);
	Delete(BombEntID)
	Delete(PoliceStationID)

end

function IsMissionDone()

	-----------------------------------------------------------------------
	-- Kolla så att alla objekt finns.
	-----------------------------------------------------------------------

	if PoliceStationID < 0 or BombEntID < 0 then

		Print( "Mission Cops and c4 are mission some objects" )
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