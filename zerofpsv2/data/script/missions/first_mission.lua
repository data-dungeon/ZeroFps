
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Kill XXX. 
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Kill XXX", difficulty=1, xp=1000, cash=2000 } -- difficulty = Level

MissionText = 
{ 
	short = "Kill XXX",
	long = "Find the evil maffia boss XXX and kill him. He is somewhere in the park at night and is very angry!. You will get 1000 xp and some money if you kill him"
}

function OnMissionSuccess()
	SetNewMission("data/script/missions/second_mission.lua")
end

function IsMissionDone()

	local object = 1; -- vi l�stsas att ha finns  GetDMCharacterByName("XXX");

	-----------------------------------------------------------------------
	-- Kolla om XXX inte finns. I s� fall antar vi att han d�d och 
	-- uppdraget �r slutf�rt.
	-----------------------------------------------------------------------
	if object == -1 then
		g_iMissionStatus = 1; -- success
	end

	Print("APA");

end

function IsMissionFailed()

	local num_of_living_agents = GetNumOfLivingAgents()

	-----------------------------------------------------------------------
	-- Om alla agenter �r d�da s� har uppdraget misslyckats.
	-----------------------------------------------------------------------
	if num_of_living_agents == 0 then
		g_iMissionStatus = -1; -- success
	end


end