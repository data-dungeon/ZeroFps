function Create()
	
	InitObject();
		InitProperty("P_Spell");
			InitParameter("SpellType", "fireexplosion");
		InitProperty("P_Light");
			InitParameter("Mode",0);
			InitParameter("Quadratic_Atten",0.05);
		SetParentObject();
		SetReturnObject();
end


function Init()

end