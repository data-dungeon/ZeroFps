
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Kill XXX. 
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Kill XXX", difficulty=1, xp=1000, cash=2000 } -- difficulty = Level

MissionText = 
{ 
	short = "mission 1 - Kill the evil badguy.",
	long = "Find the evil maffia boss XXX and kill him. He is somewhere in the park at night and is very angry!. You will get 1000 xp and some money if you kill him"
}

Success = 22

function OnMissionSuccess()
	SetNewMission("data/script/missions/second_mission.lua")
	Print ("Starting mission - 2!");
end

function IsMissionDone()

	-----------------------------------------------------------------------
	-- Kolla om XXX inte finns. I så fall antar vi att han död och 
	-- uppdraget är slutfört.
	-----------------------------------------------------------------------


	if GetVar("BadGuyDead") == 1 then
		Print ("MissionDone!!!");
		OnMissionSuccess();
	end

end

function IsMissionFailed()

	local num_of_living_agents = GetNumOfLivingAgents()

	-----------------------------------------------------------------------
	-- Om alla agenter är döda så har uppdraget misslyckats.
	-----------------------------------------------------------------------
--	if num_of_living_agents == 0 then
--		
--	end


end