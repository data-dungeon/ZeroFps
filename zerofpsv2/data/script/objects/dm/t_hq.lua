function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/cube.mad");
			InitParameter("m_fScale","1");		

		InitProperty("P_Track");
		InitProperty("P_DMHQ");
			InitParameter("active","true");					
		InitProperty("P_ScriptInterface");
		InitProperty("P_DMShop");

end

function Init()
	AddItemToShop(SIGetSelfID(), "/data/script/objects/dm/t_gun.lua", 400);
end

function FirstRun()

end

function HeartBeat()

end