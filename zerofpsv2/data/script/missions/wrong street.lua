
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Wrong street
--//
--///////////////////////////////////////////////////////////////////////////////

NumOfBaldies = 2; -- max number of baldies, not numbers left
HarryLastLife = 0;
HarryID = 0; -- Bad guyen man ska döda
Baldies = {}; -- Harrys goons
HarryHouseID = 0; -- harrys hus, dör alla hans goons springer han in hit och hämtar fler
Reinforcments = 0; -- om harry hämtat fler goons, gör det bara en gång

MissionInfo = { name="Wrong street", difficulty=1, xp=900, cash=500, success=0 } -- difficulty = Level

MissionText = 
{ 
	short = "Wrong street\n\nSome petty gansters have started selling drugs in the wrong hood and need some demotivation.",
	long = [[Wrong street\n\nLarge Leonard, the local drugdealer and maffia boss is pissed off. Someone has begun selling drugs in his neighborhood, and not just anyone - it's Harry Hair and his gang of baldies, a gang of lowly and petty thieves! They sure need to be taught a lesson or two.\nLeonard usually finishes his own buissnises, but thinks to highly of himself to do such a small work. That´s where you come in.\n\nHe wants you to make Harry and his gang a visit and teach them how hard it is to run your own buissness. Harry is recognized by his large afro hair, and all his gang members are bald.\n\nReward: 500 cash\n\nObjectives: Kill Harry and return to base.]]
}

Skin1 = { tex1="map_icons/harry.bmp", tex2="0", tex3="0", tex4="0", 
 tex1a="0", tex2a="0", tex3a="0", tex4a="0", bkR=255, bkG=255,
 bkB=255, borderR=255, borderG=0, borderB=0, bd_size=0, tile=0, trans=0 }

function OnMissionStart()
	-- create harry and his gangmembers

	--position modifier for the whole gang
	local pos = {44.16,1.700,95.97};
	local HarryPos = {44.16,1.70,95.97};

	HarryID = RunScript("data/script/objects/dm/t_harry.lua", -1, HarryPos);

	for i = 0, NumOfBaldies, 1
	do
		local newpos = {0,0,0};
		newpos[1] = Random(6) - 3 + pos[1];
		newpos[2] = pos[2];
		newpos[3] = Random(6) - 3 + pos[3];
		Baldies[i] = RunScript("data/script/objects/dm/t_baldie.lua", -1, newpos);
	end
	
	HarryHouseID = GetDMObject (3);

	SetVar ("HarryWounded", 0);
	SetVar ("HarryID", HarryID);
	SetVar ("HarryHouseID", -1);

	HideAllMapIcons()
	CreateWnd(4,"AMapLabel","MapWnd","",383,325,32,32,0)
	ChangeSkin("AMapLabel","Skin1","Label")

end

function OnMissionSuccess()



end

function OnMissionFailed()



end

function IsMissionDone()
	if GetVar("HarryHouseID") == -1 then
		HarryHouseID = GetDMObject (3);	
		SetVar ("HarryHouseID", HarryHouseID);
	end

	if IsDead(HarryID) == 1 then
		MissionInfo.success = 1;
		return
	end

	-- kolla om harry är på väg till sitt hus
	if GetState(HarryID) == 3 then
		--kolla om han nått fram till huset
		if DistanceTo(HarryID, HarryHouseID) < 1 then
			SetState(HarryID, 4); --agro
			SwallowPlayer(HarryHouseID, HarryID, 3);
			PlayAnim(HarryHouseID, "open");
			SetNextAnim(HarryHouseID, "idle");
			Reinforcments = 1;
		end


		return		
	end


	-- kolla om harry blivit skadad, isåfall, gå berserk med alla goons
	if GetCharStats(HarryID, 0) < HarryLastLife then
		SetState (HarryID, 4);
		SetVar ("HarryWounded", 1);
	end

	HarryLastLife = GetCharStats(HarryID, 0);

	if Reinforcments == 0 then
		-- kolla hur många baldies som lever, är alla döda, får harry panic och springer och hämtar fler
		-- i sitt hus
		BaldLive = NumOfBaldies;
	
		for i = 0, NumOfBaldies, 1
		do
			if IsDead(Baldies[i]) == 1 then
				BaldLive = BaldLive - 1;
			end
		end
	
		if BaldLive < 0 then
			SetState (HarryID, 3); -- panic
			housepos = GetEntityPos(GetVar("HarryHouseID"));
			Print("GOTOHOUSE");
			Print(MakePathFind(HarryID, housepos));
		end
	end	

end

function IsMissionFailed()



end