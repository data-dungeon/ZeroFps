function Create()
	
	InitObject();
		InitProperty("P_Mad");
			InitParameter("m_kMadFile","/data/mad/gob_arm.mad");
			InitParameter("m_fScale",1);

		InitProperty("P_ScriptInterface");
		InitProperty("P_PSystem");
			InitParameter("PSType", "gibb");
		
		InitProperty("P_Tcs");
			InitParameter("radius",0.3);
			InitParameter("gravity","true");
			InitParameter("group",4);
--			InitParameter("groupflag",0);
			InitParameter("airfriction",0.5);
			InitParameter("mass",1);			
			InitParameter("inertia",2);
			InitParameter("bounce",0.0);
			InitParameter("friction",0.8);
			InitParameter("removeonsleep","true");
--			InitParameter("activemoment","false");
		
		
		SetParentObject();
		SetReturnObject();
end

function Init()
	--p = GetObjectPos(SIGetSelfID());
	local p = {};
	p[1] = Random(8)-4;
	p[2] = Random(8)+1;	
	p[3] = Random(8)-4;

	ApplyImpuls(SIGetSelfID(),p);
--SetVelTo(SIGetSelfID(),p,20);

--	p[1] = Random(100);
--	p[2] = Random(100);
--	p[3] = Random(100);

--	SetRotVel (SIGetSelfID(),p);

	--p = GetObjectPos(SIGetSelfID());	
	--Print("spiderflesh Pos:",p[1],"  ",p[2],"  ",p[3]);
	
end

function Collission()
--	Bounce(GetSelfID());
end