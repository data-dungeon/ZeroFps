function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/cube.mad");
			InitParameter("m_fScale","1");		

		InitProperty("P_Track");
		InitProperty("P_DMHQ");
			InitParameter("active","true");					
		InitProperty("P_ScriptInterface");

end

function FirstRun()

end

function HeartBeat()

end