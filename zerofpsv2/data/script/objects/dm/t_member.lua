function Create()
	
	InitObject();
		InitProperty("P_Mad");	
			InitParameter("m_kMadFile","/data/mad/member.mad");			
			InitParameter("m_fScale","1");		

		InitProperty("P_PfPath");
		InitProperty("P_Track");
		InitProperty("P_Sound");
		InitProperty("P_DMCharacter");
			InitParameter("team",0)
		InitProperty("P_ScriptInterface");
		InitProperty("P_ShadowBlob");



end


function Init()
	SetMoveSpeed (SIGetSelfID(), 6.5);

	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent/offensive/are you ready to be fucked, man.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent/offensive/didn't I promise you fireworks.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent/offensive/how many lives do you got, man.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent/offensive/i give him my best!.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent/offensive/if it bleeds, we can kill it.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent/offensive/oh, sorry.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent/offensive/where are you going.wav");
	AddOffenciveActionQuot(SIGetSelfID(), "data/sound/agent/offensive/you scared, man.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent/defensive/he's already dead.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent/defensive/i'm too old for this shit!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent/defensive/that was a bullet, wasn't it.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent/defensive/well done, junior!.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent/defensive/ohh fuck.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent/defensive/fuck.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent/defensive/fuck2.wav");
	AddDefenciveActionQuot(SIGetSelfID(), "data/sound/agent/defensive/fuck3.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/agent/death/nooo.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/agent/death/nooo2.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/agent/death/nooo3.wav");
	AddDeathSound(SIGetSelfID(), "data/sound/agent/death/nooo4.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent/select/what.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent/select/if we gonna work together, you might try and be polite.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent/select/speek2.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent/select/what2.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent/select/what do you want from me, my soul.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent/select/what.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent/select/yeah, so.wav");
	AddSelectCharSound(SIGetSelfID(), "data/sound/agent/select/yes my lord.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent/move/my bags are packed!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent/move/im already there.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent/move/i'm back in five minutes!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent/move/i'm done here!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent/move/no action here.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent/move/of course2!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent/move/of course!.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent/move/says who.wav");
	AddMoveCharSound(SIGetSelfID(), "data/sound/agent/move/slaving for you!.wav");

	SetTeam (SIGetSelfID(), 0);
	SISetHeartRate(SIGetSelfID(),4);

	PlayAnim(SIGetSelfID(), "idle");

end

function FirstRun()
	--Equip(SIGetSelfID(), "data/script/objects/dm/t_gun.lua", 1);
end

function HeartBeat()

	if ( IsDead(SIGetSelfID()) == 1) then
		AddToEntityVar (SIGetSelfID(), "deadtime", 1);

		if GetEntityVar(SIGetSelfID(), "deadtime") > 9 then
			Delete(SIGetSelfID());
		end

		return
	end

	if HavePath(SIGetSelfID()) == 1 then
		return
	end

	-- Spela upp en liten kommentar då och då när personen står still och inte har blivit skadad
	if Random(100) > 85 then

		talk_sound =
		{
		  "agent/talking/i want to do something for humanity.wav",
		  "agent/talking/one man alone cant fight the future.wav",
		  "agent/talking/i'm extremely nervous right now.wav",
		}
		PlaySound (SIGetSelfID(), talk_sound[Random(3)+1] );

	end

end

function Dead()
	PlayAnim(SIGetSelfID(), "die");
	SetNextAnim(SIGetSelfID(), "dead");
	ClearPathFind(SIGetSelfID());
	SetEntityVar (SIGetSelfID(), "deadtime", 0);
end

