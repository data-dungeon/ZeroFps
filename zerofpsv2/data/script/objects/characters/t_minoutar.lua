function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/minoutar.mad");
			InitParameter("m_fScale",1);
		InitProperty("P_Event");
		InitProperty("P_CharStats");
		SetParentObject();
		SetReturnObject();
end

function Init()
	SetAttribute("str", 200);
	SetDefence ("fire", 5);
	SetSkill ("ata", 20);
	SetData("Name", "mu");
	SetAttack("hej", 20);
	SetAttack("svej", 10);

end
