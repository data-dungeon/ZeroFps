function Create()
	
	InitObject();
		InitProperty("P_Spell");
			InitParameter("SpellType", "fireball");
		InitProperty("P_Light");
			InitParameter("Mode",0);
			InitParameter("Quadratic_Atten",0.1);
		InitProperty("P_Event");
		InitProperty("P_Tcs");
			InitParameter("radius",0.1);
			InitParameter("group",2);
			InitParameter("groupflag",2);	

		SetParentObject();
		SetReturnObject();
end


function Init()

end

function Collission()
	Print("BOM");

	RunScript("data/script/objects/spells/t_fireexplosion.lua",GetSelfID());
	
	Delete(GetSelfID());
end