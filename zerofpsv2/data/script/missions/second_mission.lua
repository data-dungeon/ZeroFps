
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Another mission
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Another mission", difficulty=1, xp=100, cash=500 } -- difficulty = Level

MissionText = 
{ 
	short = "Another mission",
	long = "Walk around in the town and collect XX number of XXX, when you have collected all XXX return to base."
}

function OnMissionSuccess()
--	Print("victory!, mission complete.");	
end

function IsMissionDone()

--	Print("Mission number 2\n");

end

function IsMissionFailed()
--	Print("you have failed!, no cash this time. return tro base!");
end