function Create()
	Print("Running player create");
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/fwarrior.mad");
			InitParameter("m_fScale","0.165");
--			InitParameter("m_kMadFile","/data/mad/wolf.mad");
--		InitParameter("m_kMadFile","/data/mad/cube.mad");							
		InitProperty("P_CharStats");
		InitProperty("P_Event");
		InitProperty("P_Ml");
			InitParameter("showtext","true");
		InitProperty("P_Container");
		InitProperty("P_AI");	
		InitProperty("P_Tcs");
			InitParameter("radius",0.4);
			InitParameter("gravity", "true");
			InitParameter("character", "true");
			InitParameter("group",2);
			InitParameter("leglength", 0.7);
			InitParameter("groupflag",0);							
			InitParameter("groupflag",4);										
			InitParameter("walkablegroupflag",0);				
			InitParameter("walkablegroupflag",4);							
			
			
		InitProperty("P_Enviroment");			
		InitProperty("P_Track");
--		InitProperty("P_PfPath");
end

function Init()
	AddMesh(GetSelfID(), "1");
	AddMesh(GetSelfID(), "2");
	
	AddAction(GetSelfID(),"Fire");
	
	SetAIIsPlayer(1);
	SetScriptWhenHit ("/data/script/objects/psystems/t_blood.lua");

	--MakePathFind(GetSelfID(),GetObjectPos(GetSelfID()))
	
end

function FirstRun()
	SetContainerSize(50);

	SetData("Name", "MeHuman");
	SetData("Race", "Human");
	SetData("Sex", "Male");


	SetAttribute("str", 18);
	SetAttribute("con", 18);
	SetAttribute("dex", 18);
	SetAttribute("int", 18);
	SetAttribute("pie", 3);

	SetSkill ("hide", 10);
	SetSkill ("picklock", 5);
	SetSkill ("jump", 11);
	SetMaxHP (50);
	SetHP(50);
	SetAttack("karate",40);
	SetMoveSpeed(6);
	SetReloadTime(1);

end

function Use(action)
	Print("im:",GetSelfID());

	if action == "Fire" then
		Print("firing");
		fbid = CastSpell("/data/script/objects/spells/t_fireball(tm).lua",GetCurrentPCID(), GetSelfID());
		Print("FireBAll id is:",fbid);
		SetVelTo(fbid,GetObjectPos(GetSelfID()),4);
	end
end

function GroundClick(action,x,y,z)
	pos = {};
	
	pos[1] = x;
	pos[2] = y;
	pos[3] = z;

--	if action == "G_Move" then	
--		AIMoveTo (GetSelfID(), pos);
--	end
	
--	if action == "G_Attack" then	
--		fbid = CastSpell("/data/script/objects/spells/t_fireball(tm).lua",GetCurrentPCID(), --GetSelfID());				
--		b = GetObjectPos(fbid);
--		b[2] = b[2] + 1;
--		SetObjectPos(fbid,b);	
--		
--		SetVelTo(fbid,pos,4);
--	end

end

function Collission()
--	Print("Collided whit something");

end