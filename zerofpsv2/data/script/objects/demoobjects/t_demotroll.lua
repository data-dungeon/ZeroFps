function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/cavetroll.mad");
			InitParameter("m_fScale",0.85);
		InitProperty("P_Event");
		InitProperty("P_CharStats");
		InitProperty("P_Ml");
		InitProperty("P_PfPath");
		InitProperty("P_Sound");		
		InitProperty("P_AI");
		
		InitProperty("P_Tcs");
			InitParameter("radius",-1);
			InitParameter("polygontest","true");
			InitParameter("group",1);
			InitParameter("groupflag",0);
			InitParameter("groupflag",1);			
		
	
		SetParentObject();
		SetReturnObject();
end

function Init()
	PlayAnim(GetSelfID(), "idle");
	AddAction(GetSelfID(),"Attack");

	SetScriptWhenHit ("/data/script/objects/psystems/t_blood.lua");
end

function FirstRun()
	SetMaxHP(200);
	SetHP("Max");

	SetSkill ("krossvapen", 10);
	SetSkill ("huggvapen", 7);
	SetSkill ("stickvapen", 5);

	SetAttack ("mörda", 100);

	SetHeartRate(GetSelfID(), 3);
	SetMoveSpeed(2.6);
	SetReloadTime(3);

	EquipFromScript ("/data/script/objects/weapons/t_cleaver.lua","righthand");

	pos = {Random(5)-2.5,0,Random(5)-2.5};
	AIMoveTo(GetSelfID(),pos);

end

function Use(action)
	if action == "Attack" then
		AIAttack (GetSelfID(), GetCurrentPCID());
	end
end
	
function HeartBeat()
	if HavePath(GetSelfID()) then
		return
	end

	if AIHaveTarget() == 1 then
		return
	end
	
	target = GetClosestPlayer(5);
	
	if target == -1 then
		target = GetClosestObjectOfType("t_demogoblin.lua",5);
	end
	
	AIAttack (target);
	
	
	if target == -1 then
		pos = GetObjectPos(GetSelfID());
		pos[1] = pos[1] + Random(32)-16;
		pos[3] = pos[3] + Random(32)-16;
	
		AIMoveTo(GetSelfID(),pos);	
	end
end

function OnDeath()
	PlaySound(GetSelfID(),"deaths/trolldie.wav");
	
	RunScript ("data/script/objects/gibb/t_trollhead.lua",GetSelfID());

	RunScript ("data/script/objects/gibb/t_trollmeat.lua",GetSelfID());	
	RunScript ("data/script/objects/gibb/t_trollmeat.lua",GetSelfID());	
	RunScript ("data/script/objects/gibb/t_trollmeat.lua",GetSelfID());	
	RunScript ("data/script/objects/gibb/t_trollmeat.lua",GetSelfID());	
	RunScript ("data/script/objects/gibb/t_trollmeat.lua",GetSelfID());	



	Delete(GetSelfID());
end