bla = {};

function Create()
	
	InitObject();
		InitProperty("P_Primitives3D");	
			InitParameter("Radius","1");
		InitProperty("PSystemProperty");
			InitParameter("PSType","Smoke");
		InitProperty("P_Event");
		InitProperty("P_Ml");		
		InitProperty("PhysicProperty");		
		AttachToClosestZone();
	
end

function Init()
	Print("im a ball");
end

function Destroy()
	Print("im dieing");
	bla[GetSelfID()] = nil;
end


function Use()
	id = GetSelfID();
	
	if bla[id] == nil then
		bla[id] = 1;
		SetPSystem(GetSelfID());
		--Print("Disabling SmokE");
	else	
		bla[id] = nil;
		SetPSystem(GetSelfID(),"Smoke");
		--Print("Enabling SmokE");
	end

end

function Collission()
	id  = GetLastCollidedObject();

	Print("i collided whit object:",id);
	Print("Its of type:",GetObjectType(id));
	
	SetPSystem(id,"Smoke");
	
end