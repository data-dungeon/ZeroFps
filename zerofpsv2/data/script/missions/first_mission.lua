
function OnMissionSuccess()
	SetNewMission("data/script/missions/second_mission.lua", 2)
end

function IsMissionDone()

	object = GetDMCharacterByName("adolf hitler");

	-----------------------------------------------------------------------
	-- Kolla om Adolf hitler inte finns. I s� fall antar vi att han d�d och 
	-- uppdraget �r slutf�rt.
	-----------------------------------------------------------------------
	if object == -1 then
		CurrentMission = CurrentMission + 1
	end

end

function IsMissionFailed()
--	Print("you have failed!, no cash this time. return tro base!");
end