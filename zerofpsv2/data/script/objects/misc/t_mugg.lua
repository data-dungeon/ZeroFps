function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/mugg.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_Ml");
		InitProperty("P_Item");
		
--		InitProperty("P_Physic");
--			InitParameter("m_fRadius",0.2);
		
		InitProperty("P_Tcs");
--			InitParameter("polygontest","true");
			InitParameter("radius",-1);
			
		SetParentObject();
		SetReturnObject();
end

function Init()
	AddAction(GetSelfID(),"Look");
	AddAction(GetSelfID(),"PickUp");
	AddAction(GetSelfID(),"Eat");
	SetIcon ("mugg.bmp", "mugg_a.bmp");
end

function Use(action)
	if action == "Look" then
		MessageCaracter(GetCurrentPCID(),"It is full of coke!")
	end

	if action == "PickUp" then
		GetPickedUpBy (GetCurrentPCID());
	end

	if action == "Eat" then
		UseOn (GetCurrentPCID());
		MessageCaracter(GetCurrentPCID(),"The coke tasted fantastic!")
	end
end

function Collission()
	Print("BOM");

	RunScript("data/script/objects/spells/t_fireexplosion.lua",GetSelfID());
	
	Delete(GetSelfID());
end
