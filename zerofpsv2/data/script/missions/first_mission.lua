
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Kill XXX. 
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Kill XXX", difficulty=20 }

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