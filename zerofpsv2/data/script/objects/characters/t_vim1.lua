function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/goblin.mad");
			InitParameter("m_fScale",1);	
		InitProperty("P_Event");
		SetParentObject();
		SetReturnObject();

	InitObject("/data/script/objects/misc/t_torch.lua");
		HaveRelativOri();
		AttachToParent();
			InitProperty("P_LinkToJoint");
			InitParameter("m_strToJoint","maNus_r_u_finger_end");


end

function Init()
	PlayAnim(GetSelfID(), "run");
end





