function Create()
	
	InitObject();
--		InitProperty("P_Mad");
--			InitParameter("m_kMadFile","/data/mad/physicsball.mad");
--			InitParameter("m_fScale",0.5);
--			InitParameter("m_iShadowGroup","2");
--		InitProperty("P_Primitives3D");
		InitProperty("P_BillBoardRender");
			InitParameter("texture","data/textures/rainsplash.tga");
			
		InitProperty("P_ScriptInterface");


		InitProperty("P_Tcs");
--			InitParameter("polygontest","true");
			InitParameter("radius",0.3);
			InitParameter("gravity","false");
			InitParameter("group",3);
			InitParameter("groupflag",0);
			InitParameter("groupflag",1);			
--			InitParameter("groupflag",2);			
			InitParameter("airfriction",0.1);
			InitParameter("mass",1);			
			InitParameter("inertia",0);
			InitParameter("bounce",0.5);
			InitParameter("friction",0.5);
--			InitParameter("disableonsleep","true");
			InitParameter("removeonsleep","true");
			InitParameter("activemoment","false");
			
end

function Collission(bah)
	Delete(SIGetSelfID());
end
