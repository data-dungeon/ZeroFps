
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

function OnMissionStart()

	-- Lägg till Da bomb till spelarens hq
	local hq = GetDMObject(0)
	AddItem(hq, "data/script/objects/dm/t_bomb.lua", 0)

end

function OnMissionSuccess()



end

function OnMissionFailed()



end

function IsMissionDone()


end

function IsMissionFailed()



end