
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Prevent mob to enter location x,y,z
--//
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Crowd controll", difficulty=1, xp=1250, cash=1500, success=0 }

Trackers = {}; -- trackers so the mob can run
Spawners = {};
TempDoor = {};

pos1 = {-50.77, 1.27, 5.80};
pos2 = {-35.63, 0.81, 5.56};
pos3 = {-17.19, 0.78, 6.0};
pos4 = {4.82, 0.66, 6.12};
pos5 = {20.40, 1.58, 20.92};

MissionText = 
{ 
	short = "Crowd control\n\nHelp the police to stop a riot of unhappy people.",
	long = [[Crowd control\n\nThe police has gotten inside information from the underground anarchy group 'I Can Do What I Want' of an planned attack on the police station. The group sees the lawforce as the biggest threat to their freedom and has according to the information decided to take countermeisures in some unknown form.\nThey are going to hold a meeting and then attack the station. The police however, doesn't want to lose face to the citizens by attacking a large group of common people, so they have hired you to do the dirty work.\nYour work is to stop the anarchists from reaching the police station. You are free to do this in any way, the police will overlook any "incidents".\n\nReward: 1500 cash\n\nObjectives: Stop the anarchysts from reaching the police station. If ten or more people reach the station, you have failed. If you can hinder them during in 2 minutes, the mission is a success.\nYou will have a small delay to reach the location before their meeting ends.]]
}

function OnMissionStart()
	SetVar("MobX", pos5[1]);
	SetVar("MobZ", pos5[3]);

	SetVar("MobTimer", 20);
	SetVar("MobWaitTimer", 5);
	SetVar("MobSuccess", 0); -- number of mobs reached goal

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

	TempDoor[1] = RunScript("data/script/objects/dm/t_door.lua",-1, {-51.37, 1.00, 9.9});
	TempDoor[2] = RunScript("data/script/objects/dm/t_door.lua",-1, {-51.37, 1.00, 1.7});

end

function IsMissionDone()
	if GetVar("MobWaitTimer") > 0 then
		AddToVar("MobWaitTimer", -1);
		return
	end

	-- first run after waittime is over
	if GetVar("MobTimer") == 20 then
		CreateStuff();
	end

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

function CreateStuff()
	--create mob-spawners
	Spawners[1] = RunScript("data/script/objects/dm/t_crowd_spawner.lua",-1, {-51.37, 1.00, 9.9});
	Spawners[2] = RunScript("data/script/objects/dm/t_crowd_spawner2.lua",-1, {-51.37, 1.00, 1.7});

	--create trackers needed
	Trackers[1] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos1);
	Trackers[2] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos2);
	Trackers[3] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos3);
	Trackers[4] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos4);
	Trackers[5] = RunScript("data/script/objects/dm/t_tracker.lua", -1, pos5);

	Delete (TempDoor[1]);
	Delete (TempDoor[2]);
end