bla = {};
timer  = {};

function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/goblin.mad");
			InitParameter("m_fScale",1);	
		SetParentObject();
		SetReturnObject();

		InitProperty("P_PfPath");
		InitProperty("P_CharStats");
		InitProperty("P_Item");
		InitProperty("P_Ml");
		InitProperty("P_Sound");	
		InitProperty("P_AI");
		InitProperty("P_Event");

		InitProperty("P_Tcs");
			InitParameter("radius",-1);
			InitParameter("polygontest","true");
			InitParameter("group",1);
			InitParameter("groupflag",0);
			InitParameter("groupflag",1);			

end

function FirstRun()

	SetMaxHP(10);
	SetHP("Max");
	SetHeartRate(GetSelfID(), 3);
	SetMoveSpeed(1.9)
	SetReloadTime(2);

	SetSkill ("krossvapen", 3);
	SetSkill ("huggvapen", 4);
	SetSkill ("stickvapen", 5);

	SetScriptWhenHit ("/data/script/objects/psystems/t_blood.lua");
	
	EquipFromScript ("/data/script/objects/weapons/t_sword.lua","RightHand");
	EquipFromScript ("/data/script/objects/weapons/t_shield.lua","LeftHand");

	pos = {Random(5)-2.5,0,Random(5)-2.5};
	AIMoveTo(GetSelfID(),pos);
end


function Init()
	AddAction(GetSelfID(),"Attack");
	PlayAnim(GetSelfID(), "idle");
	
end

function Use(action)
	if action == "Attack" then
		AIAttack (GetSelfID(), GetCurrentPCID());
	end
end
	
function HeartBeat()


	if AIHaveTarget() == 1 then
		return
	end
	
	target = GetClosestPlayer(5);
	
	if target == -1 then
		target = GetClosestObjectOfType("t_demotroll.lua",5);
	end

	AIAttack(target);
end

function OnDeath()
	PlaySound(GetSelfID(),"deaths/goblindie.wav");

	RunScript ("data/script/objects/gibb/t_gobhand.lua",GetSelfID());
	RunScript ("data/script/objects/gibb/t_gobhand.lua",GetSelfID());

	RunScript ("data/script/objects/gibb/t_gobleg.lua",GetSelfID());
	RunScript ("data/script/objects/gibb/t_gobleg.lua",GetSelfID());

	RunScript ("data/script/objects/gibb/t_gobhead.lua",GetSelfID());
	RunScript ("data/script/objects/gibb/t_gobbody.lua",GetSelfID());

	Delete(GetSelfID());
end