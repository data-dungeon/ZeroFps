
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Kill XXX. 
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Kill XXX", difficulty=1, xp=1250, cash=2000, success=0 } -- difficulty = Level

MissionText = 
{ 
	short = "mission 1 - Kill the evil badguy.",
	long = "Find the evil maffia boss XXX and kill him. He is somewhere in the park at night and is very angry!. You will get 1000 xp and some money if you kill him"
}

function OnMissionStart()

	Print ("Starting mission 1 : Kill XXX")

end

function OnMissionSuccess()

	MissionInfo.success = 1
	Print ("MissionDone!!!")

end

function OnMissionFailed()

	MissionInfo.success = -1
	Print ("Mission failed!!!")

end

function IsMissionDone()

	-----------------------------------------------------------------------
	-- Kolla om XXX inte finns. I så fall antar vi att han död och 
	-- uppdraget är slutfört.
	-----------------------------------------------------------------------

	if GetVar("BadGuyDead") == 1 then
		OnMissionSuccess()
	end

end

function IsMissionFailed()

--	local num_of_living_agents = GetNumOfLivingAgents()

	-----------------------------------------------------------------------
	-- Om alla agenter är döda så har uppdraget misslyckats.
	-----------------------------------------------------------------------
--	if num_of_living_agents == 0 then
--		OnMissionFailed()
--	end


end