-- just a common door without functions

function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/dm/dm_door.mad");			
			InitParameter("m_fScale","1");		
		InitProperty("P_ScriptInterface");
		InitProperty("P_Sound");
end


function Init()
end

function FirstRun()
end
