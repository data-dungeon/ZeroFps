
--///////////////////////////////////////////////////////////////////////////////
--//
--// Mission: Another mission
--// 
--///////////////////////////////////////////////////////////////////////////////

MissionInfo = { name="Dr Gun", difficulty=1, xp=100, cash=500, success=0 } -- difficulty = Level

NumCivilians = 0
NumCiviliansWounded = 0
CivWounded = {} -- lista på vilka civilians som har blivit skadade
InjureList = {} -- en annan lista som söks igenom för att se om några skadeskjutna civila har dött

MissionText = 
{ 
	short = "Dr Gun",
	long  = [[Dr Gun\n\nMr. Wilhelm Von Leath wont you to work for him for a while.\n\nYou are hired to damage 5 persons. 
	 You can pick anyone, he does't care. When you are done return to base. You will get 1000 dollar in cash
	 when the mission is done]]
}

function OnMissionStart()

	Print( "starting mission Dr Gun" )

	CivWounded = {} -- töm listan

	local team = 1 -- team civilians
	local civ_list = GetCharsByFraction(team) -- hämta en lista på alla civila
	NumCivilians = civ_list[1] -- första arg är vektorns size

	local counter = 0 
	for x = 2, NumCivilians+1, 1
	do
		local obj_id = civ_list[x]
		local life = GetCharStats( obj_id, 0 ) -- 0 = Life
		
		CivWounded[counter] = obj_id
		counter = counter + 1

		CivWounded[counter] = life
		counter = counter + 1
	end

end

function OnMissionSuccess()

		

end

function OnMissionFailed()



end

-- Anropas var 3:e sekund
function IsMissionDone()

	-- Kolla om några civila har blivit skadade
	for x = 0, NumCivilians-1, 1
	do
		local obj = CivWounded[x*2] 

		if obj > -1 then -- hoppa förbi dom som redan har blivit räknade som skadade

			local prev_life = CivWounded[x*2+1] 
			local curr_life = GetCharStats( obj, 0 ) -- 0 = Life
			local dmg = prev_life - curr_life

			if dmg > 0 then
				
				CivWounded[x*2] = -1 -- markera att denna inte skall räknas mer

				InjureList[NumCiviliansWounded] = obj -- lägg till till lista över skadeskjutna gubbar

				NumCiviliansWounded = NumCiviliansWounded + 1 -- öka antalet skadade personer
			end

		end

	end

	-- Kolla om några har blivit skadeskjutna till döds

	if NumCiviliansWounded > 0 then

		for y = 0, NumCiviliansWounded-1, 1 
		do
			local obj_wounded = InjureList[y]

			if obj_wounded > -1 then -- om objektet inte redan är död

				if IsDead(obj_wounded) == 1 then

					Print("--------- YOU ARE NOT SUPPOSED TO KILL PEOPLE! ------------")

					NumCiviliansWounded = NumCiviliansWounded - 1 -- minska antalet skadade personer

					InjureList[y] = -1
				end

			end
		end

	end

	Print("Person wounded: ", NumCiviliansWounded)

	if NumCiviliansWounded == 3 then

		MissionInfo.success = 1 -- Uppdraget har lyckats!

	end

end

function IsMissionFailed()



end