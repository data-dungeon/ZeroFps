function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_grenade.mad");			
			InitParameter("m_fScale","1.0");		
		InitProperty("P_ScriptInterface");

		InitProperty("P_Event");		
		InitProperty("P_Tcs");
			InitParameter("radius",-1);
			InitParameter("gravity","true");
			InitParameter("group",1);
			InitParameter("groupflag",0);		
end

function Init()
end

function FirstRun()
end

function Collission()
	RunScript("data/script/objects/dm/t_explosion.lua",SIGetSelfID());
	Delete (SIGetSelfID());
end