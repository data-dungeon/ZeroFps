
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Prevent mob to enter location x,y,z
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Crowd controll", difficulty=1, xp=1250, cash=1500, success=0 }

Trackers = {}; -- trackers so the mob can run
Spawners = {};

pos1 = {-50.77, 1.27, 5.80};
pos2 = {-35.63, 0.81, 5.56};
pos3 = {-17.19, 0.78, 6.0};
pos4 = {4.82, 0.66, 6.12};
pos5 = {20.40, 1.58, 20.92};

MissionText = 
{ 
	short = "Crowd control - Help the police to stop a riot of people not happy with their lives.",
	long = "Find the evil maffia boss XXX and kill him. He is somewhere in the park at night and is very angry!. You will get 1000 xp and some money if you kill him"
}

function OnMissionStart()
	SetVar("MobX", pos5[1]);
	SetVar("MobZ", pos5[3]);


	--create mob-spawners
	Spawners[1] = RunScript("data/script/objects/dm/t_crowd_spawner.lua",-1, {-51.37, 1.10, 9.9});
	Spawners[2] = RunScript("data/script/objects/dm/t_crowd_spawner2.lua",-1, {-51.37, 1.10, 1.7});

	SetVar("MobTimer", 20);
	SetVar("MobSuccess", 0); -- number of mobs reached goal

	--create trackers needed
	Trackers[1] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos1);
	Trackers[2] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos2);
	Trackers[3] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos3);
	Trackers[4] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos4);
	Trackers[5] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos5);

	-- set waypoint vars...gaaaah!
	SetVar("Mob_WP1_X", -35.63)
	SetVar("Mob_WP1_Y", 0.81)
	SetVar("Mob_WP1_Z", 5.56)

	SetVar("Mob_WP2_X", -17.19)
	SetVar("Mob_WP2_Y", 0.78)
	SetVar("Mob_WP2_Z", 6.0)

	SetVar("Mob_WP3_X", 4.82)
	SetVar("Mob_WP3_Y", 0.66)
	SetVar("Mob_WP3_Z", 6.12)

	SetVar("Mob_WP4_X", 20.40)
	SetVar("Mob_WP4_Y", 1.58)
	SetVar("Mob_WP4_Z", 20.92)

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
	DeleteStuff();
	MissionInfo.success = 1

end

function OnMissionFailed()
	DeleteStuff();
	MissionInfo.success = -1
	Print ("Mission failed!!!")

end


function IsMissionFailed()

end

function DeleteStuff()
	--delete trackers
	for i = 0, 1, 6
	do
		Delete (Trackers[i]);
	end

	--delete spawners
	for i = 0, 1, 2
	do
		Delete (Spawners[i]);
	end
end