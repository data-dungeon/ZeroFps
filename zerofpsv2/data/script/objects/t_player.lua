function Create()
	
	InitObject();
		InitProperty("MadProperty");	
			InitParameter("m_kMadFile","/data/mad/orc.mad");
			InitParameter("m_fScale","1");
		InitProperty("CameraProperty");
		InitProperty("P_Event");
		SetParentObject();
		SetReturnObject();
	
	for x = 0,360,20
	do
		InitObject();
			InitProperty("PSystemProperty");
				InitParameter("PSType","Smoke");				
			SetLocalPos(Sin(x)*5,5,Cos(x)*5);	
			AttachToParent();			
			HaveRelativOri();
	end

	InitObject();
		InitProperty("PSystemProperty");
		InitParameter("PSType","Rain");				
		SetLocalPos(0,25,0);	
		AttachToParent();
		HaveRelativOri();
	
end

function Init()
	k = Sin(90);

	Print("Im the big bad ORC");

end
