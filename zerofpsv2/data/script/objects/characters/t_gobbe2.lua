bla = {};

function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/goblin.mad");
		SetParentObject();
		SetReturnObject();

	InitObject("/data/script/objects/weapons/t_sword.lua");
		HaveRelativOri();
		AttachToParent();
			InitProperty("P_LinkToJoint");
			InitParameter("m_strToJoint","jointlh");

end

function Init()
--Print("im a ball");
end

function Destroy()
--	Print("im dieing");
	bla[GetSelfID()] = nil;
end


function Use(action)
	SetVelTo(GetCurrentPCID(),GetSelfID(),10);
	
	Print("U R A HORA");
--	id = GetSelfID();	
--	if bla[id] == nil then
--		bla[id] = 1;
--		SetPSystem(GetSelfID());
--		--Print("Disabling SmokE");
--	else	
--		bla[id] = nil;
--		SetPSystem(GetSelfID(),"Smoke");
--		--Print("Enabling SmokE");
--	end

end

function Collission()
	id  = GetLastCollidedObject();

--	Print("i collided whit object:",id);
--	Print("Its of type:",GetObjectType(id));
	
	SetPSystem(id,"Smoke");
	
end
