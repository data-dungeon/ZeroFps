
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Prevent mob to enter location x,y,z
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Crowd controll", difficulty=1, xp=1250, cash=1500, success=0 }

GotoPos = {0,0}; --the place all mobs try to run to

MissionText = 
{ 
	short = "Crowd control - Help the police to stop a riot of people not happy with their lives.",
	long = "Find the evil maffia boss XXX and kill him. He is somewhere in the park at night and is very angry!. You will get 1000 xp and some money if you kill him"
}

function OnMissionStart()
	SetVar("MobX", GotoPos[1]);
	SetVar("MobZ", GotoPos[2]);

	spawnPos = {10,0,10};

	--create mob-spawners
	SpawnID = RunScript("data/script/objects/dm/t_crowd_spawner.lua",SIGetSelfID());
	SetObjectPos(SpawnID, spawnPos);
	SpawnID = RunScript("data/script/objects/dm/t_crowd_spawner.lua",SIGetSelfID());
	SetObjectPos(SpawnID, spawnPos);
	SpawnID = RunScript("data/script/objects/dm/t_crowd_spawner.lua",SIGetSelfID());
	SetObjectPos(SpawnID, spawnPos);

	SetVar("MobTimer", 20);
	SetVar("MobSuccess", 0); -- number of mobs reached goal
end

function IsMissionDone()
	AddToVar("MobTimer", -1);
	
	if GetVar("MobTimer") < 1 then
		OnMissionSuccess();
	end

	if GetVar("MobSuccess") > 9 then
		OnMissionFailed();
	end


end

function OnMissionSuccess()

	MissionInfo.success = 1

end

function OnMissionFailed()

	MissionInfo.success = -1
	Print ("Mission failed!!!")

end


function IsMissionFailed()


end