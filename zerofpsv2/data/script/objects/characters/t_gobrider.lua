function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/goblin.mad");
			InitParameter("m_fScale",1);	
		SetParentObject();
		SetReturnObject();
	InitProperty("P_Event");
	InitProperty("P_CharStats");

--	InitObject("/data/script/objects/weapons/t_sword.lua");
--		AttachToParent();
--			InitProperty("P_LinkToJoint");
--			InitParameter("m_strToJoint","RightHand");


end

function Init()
	PlayAnim(GetSelfID(), "riding");

end


function FirstRun()
	EquipFromScript ("/data/script/objects/weapons/t_spear.lua","RightHand");


end
