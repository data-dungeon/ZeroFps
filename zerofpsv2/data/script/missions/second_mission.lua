
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

function OnMissionStart()


end

function OnMissionSuccess()



end

function OnMissionFailed()



end

function IsMissionDone()

	-----------------------------------------------------------------------
	-- Kolla om XXX inte finns. I s� fall antar vi att han d�d och 
	-- uppdraget �r slutf�rt.
	-----------------------------------------------------------------------

end

function IsMissionFailed()


end