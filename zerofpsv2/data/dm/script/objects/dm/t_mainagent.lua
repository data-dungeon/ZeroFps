function Create()		
	InitObject();		
	InitProperty("P_Mad");
		InitParameter("m_kMadFile","/data/mad/member.mad");
		InitParameter("m_fScale","1.0");
		InitParameter("m_iShadowGroup","2");
	InitProperty("P_DMGun");
	InitProperty("P_Track");
	InitProperty("P_Sound");

	InitProperty("P_ScriptInterface");
	InitProperty("P_DMCharacter");
		InitParameter("team",0)
	InitProperty("P_Tcs");
		InitParameter("polygontest","false");
		InitParameter("gravity","true");
		InitParameter("character","true");
		InitParameter("radius",0.5);
		InitParameter("group",2);
		InitParameter("groupflag",0);
		InitParameter("groupflag",1);
		InitParameter("leglength",0.5);
		InitParameter("walkablegroupflag",0);
		InitParameter("airfriction",10.0);
		InitParameter("mass",1);
		InitParameter("inertia",1);
		InitParameter("bounce",0);
		InitParameter("friction",10);
		InitParameter("activemoment","false");
		InitParameter("cantsleep","true");
	InitProperty("P_Controller");

end

function Init()
	SetLeaderOfTeam(SIGetSelfID(), 0);
	AddItem(SIGetSelfID(), "data/script/objects/dm/t_rifle.lua", 1);
end