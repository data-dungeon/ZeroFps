#include "console.h"
#include "../engine/zerofps.h"
#include <cstdio>
#include <cstdarg>
#include "../basic/zfobjectmanger.h"
 
using namespace std;

Console::Console()
  : BasicConsole("Console"), MAX_CMD_HISTRORY_LENGTH(50) {
   	

	GetSystem().Log_Create("console");
}

bool Console::StartUp()	
{ 
	m_pkEngine	= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));
	m_pkInput	= static_cast<Input*>(GetSystem().GetObjectPtr("Input"));	
	m_pkRender	= m_pkEngine->m_pkRender;
	m_pkTexMan  = m_pkEngine->m_pkTexMan;

	m_iBufferSize=100;
	m_kText.resize(m_iBufferSize);
	m_bShift=false;
	strcpy(m_aCommand,"");
	
	Print("ZeroFps engine started");
	Print("ZeroFps (C) Dvoid & Vim");
	Print("-----------------------");
	Print("");

	m_nStartLine = 0;
	m_nLastCommand = 0;
	m_nStartLine = 0;

	m_fToggleTime = 0;
	m_bActive = false;

	return true; 
}

bool Console::ShutDown() 
{ 
	return true; 
}

bool Console::IsValid()	{ return true; }

/*void Console::Update(void) {
	m_pkRender->DrawConsole(m_aCommand,&m_kText,m_nStartLine);	

	// Scroll console text
	static float PREVTIME = m_pkEngine->GetGameTime();
	static float TIME = 0.10f;
	float fCurrTime = m_pkEngine->GetGameTime();

	if(m_pkEngine->m_bConsoleMode)
	{
		bool bUpdate = ((fCurrTime-PREVTIME) > TIME);

		if(m_pkInput->Pressed(KEY_PAGEUP))
		{
			if(m_nStartLine < m_kText.size() && bUpdate)
			{
				m_nStartLine++;
				PREVTIME = fCurrTime;
			}
			m_pkInput->GetQueuedKey(); // remove latest
			return;
		}
		if(m_pkInput->Pressed(KEY_PAGEDOWN) && bUpdate)
		{
			if(m_nStartLine > 0)
			{
				m_nStartLine--;
				PREVTIME = fCurrTime;
			}
			m_pkInput->GetQueuedKey(); // remove latest
			return;
		}
	}
	
	int iKey;
	while(m_pkInput->SizeOfQueue() > 0) {
		iKey = m_pkInput->GetQueuedKey();
		
		//press keys
		if(iKey == KEY_TAB) {
			glPopAttrib();
			
			m_pkEngine->m_bConsoleMode=false;
			m_pkInput->Reset();
				
			return;
		}


		if(iKey==SDLK_RETURN){
			Execute(m_aCommand);
			for(int i=0;i<TEXT_MAX_LENGHT;i++)					//wipe the command buffer
				m_aCommand[i]=' ';				
			strcpy(m_aCommand,"");
			continue;
		}
		if(iKey==SDLK_BACKSPACE){
			m_aCommand[strlen(m_aCommand)-1]='\0';
			continue;
		}
					
		if(m_pkInput->Pressed(KEY_RSHIFT) || m_pkInput->Pressed(KEY_LSHIFT)){
			m_bShift=true;
		}else{
			m_bShift=false;
		}

		if(iKey==KEY_DOWN)
		{
			if(m_nLastCommand > 0)
			{
				m_nLastCommand--;
				strcpy(m_aCommand, m_kCommandHistory[m_nLastCommand].c_str());	
			}
			continue;
		}
		if(iKey==KEY_UP)
		{
			if(m_nLastCommand+1 < m_kCommandHistory.size())
			{
				m_nLastCommand++;
				strcpy(m_aCommand, m_kCommandHistory[m_nLastCommand].c_str());		
			}
			else
			{
				int last = m_kCommandHistory.size()-1;
				if(last >= 0)
					strcpy(m_aCommand, m_kCommandHistory[last].c_str());		
			}
			continue;
		}
		
//		if(iKey==RSHIFT || iKey==LSHIFT)
//			continue;
		
		
		//type text
		if(strlen(m_aCommand)<COMMAND_LENGHT) {
			int code=iKey;
			
			//shift?
			if(m_bShift) {
				if(code>96 && code<123){
					code-=32;
					strncat(m_aCommand,(char*)&(code),1);
					break;
				}
				if(code=='-'){
					code='_';
					strncat(m_aCommand,(char*)&(code),1);
					break;
				}
				if(code=='.'){
					code=':';
					strncat(m_aCommand,(char*)&(code),1);
					break;
				}					
				if(code=='7'){
					code='/';
					strncat(m_aCommand,(char*)&(code),1);
					break;
				}					
			}else
				strncat(m_aCommand,(char*)&(code),1);
		}
	}		
}*/

void Console::Update(void) {
	m_pkRender->DrawConsole(m_aCommand,&m_kText,m_nStartLine);	

	// Scroll console text
	static float PREVTIME = m_pkEngine->GetEngineTime();	//GetGameTime();
	static float TIME = 0.10f;
	float fCurrTime = m_pkEngine->GetEngineTime();	//GetGameTime();

	bool bUpdate = ((fCurrTime-PREVTIME) > TIME);

	if(m_pkInput->Pressed(KEY_PAGEUP))
	{
		if(m_nStartLine < m_kText.size() && bUpdate)
		{
			m_nStartLine++;
			PREVTIME = fCurrTime;
		}
		m_pkInput->GetQueuedKey(); // remove latest
		return;
	}
	if(m_pkInput->Pressed(KEY_PAGEDOWN) && bUpdate)
	{
		if(m_nStartLine > 0)
		{
			m_nStartLine--;
			PREVTIME = fCurrTime;
		}
		m_pkInput->GetQueuedKey(); // remove latest
		return;
	}
	
	static bool s_bKeyrepeatActivated = false;
	static int s_iLastKeyPressed;
	static float s_fKeyrepeatCheckTime = m_pkEngine->GetEngineTime();		//GetGameTime();
	static float s_fLastRepeatTime = m_pkEngine->GetEngineTime();			//GetGameTime();

	int iKeyPressed = m_pkInput->GetQueuedKey();
		
	//press keys
	if(iKeyPressed == KEY_TAB) {
		glPopAttrib();
		
		Toggle();
		m_pkInput->Reset();
		return;
	}

	if(iKeyPressed==SDLK_RETURN){
		Execute(m_aCommand);
		for(int i=0;i<TEXT_MAX_LENGHT;i++)					//wipe the command buffer
			m_aCommand[i]=' ';				
		strcpy(m_aCommand,"");
		return;
	}
				
	if(m_pkInput->Pressed(KEY_RSHIFT) || m_pkInput->Pressed(KEY_LSHIFT)){
		m_bShift=true;
	}else{
		m_bShift=false;
	}

	if(iKeyPressed==KEY_DOWN)
	{
		if(m_nLastCommand > 0)
		{
			m_nLastCommand--;
			strcpy(m_aCommand, m_kCommandHistory[m_nLastCommand].c_str());
		}
		return;
	}
	if(iKeyPressed==KEY_UP)
	{
		if(m_nLastCommand+1 < m_kCommandHistory.size())
		{
			m_nLastCommand++;
			strcpy(m_aCommand, m_kCommandHistory[m_nLastCommand].c_str());
		}
		else
		{
			int last = m_kCommandHistory.size()-1;
			if(last >= 0)
				strcpy(m_aCommand, m_kCommandHistory[last].c_str());		
		}
		return;
	}
	
	//type text
	if(strlen(m_aCommand)<COMMAND_LENGHT) 
	{
		// Registrera senast knappnedtryck.
		if(iKeyPressed != -1 && 
			!(iKeyPressed == KEY_LSHIFT || iKeyPressed == KEY_RSHIFT))
		{
			// Formatera bokstaven.
			FormatKey(iKeyPressed);

			if(s_iLastKeyPressed != iKeyPressed)
				s_bKeyrepeatActivated = false;

			s_iLastKeyPressed = iKeyPressed; // registrera

			// Har knappen tryckts ner?
			if(s_iLastKeyPressed != SDLK_BACKSPACE)
				strncat(m_aCommand,(char*)&(s_iLastKeyPressed),1);
			else
				m_aCommand[strlen(m_aCommand)-1]='\0';
		}
		
		// Kolla om den sist nedtryckta knappen fortfarande är nedtryckt...
		if(m_pkInput->Pressed(s_iLastKeyPressed))
		{
			float fCurrTime = m_pkEngine->GetEngineTime();	//GetGameTime();

			const float REPEAT_DELAY = 0.50f, REPEAT_RATE = 0.05f;

			if(s_bKeyrepeatActivated == false)
			{
				// Är det dags att aktivera Key Repeat?
				if(fCurrTime - s_fKeyrepeatCheckTime > REPEAT_DELAY)
				{
					s_bKeyrepeatActivated = true;
					s_fKeyrepeatCheckTime = fCurrTime;
					s_fLastRepeatTime = fCurrTime;
				}
			}
			else
			{
				// Är det dags att skriva ett nytt tecken?
				if(fCurrTime - s_fLastRepeatTime > REPEAT_RATE)
				{
					if(s_iLastKeyPressed != SDLK_BACKSPACE)
						strncat(m_aCommand,(char*)&(s_iLastKeyPressed),1);
					else
						m_aCommand[strlen(m_aCommand)-1]='\0';

					s_fLastRepeatTime = fCurrTime;
				}
			}
		}
		// ... i annat fall nollställ statiska variabler.
		else
		{
			s_bKeyrepeatActivated = false;
			s_fKeyrepeatCheckTime = m_pkEngine->GetEngineTime();	//GetGameTime();
			s_fLastRepeatTime = m_pkEngine->GetEngineTime();		//GetGameTime();
		}	
	}	
}
	
/*	
	while(SDL_PollEvent(&m_kEvent)) {
		
		//press keys
		if(m_kEvent.type==SDL_KEYDOWN){

			if(m_kEvent.key.keysym.sym==SDLK_TAB) {
				glPopAttrib();
//				m_pkEngine->m_iState=state_normal;
//				m_pkEngine->m_bClientMode=false;
				m_pkEngine->m_bConsoleMode=false;

				return;
			}

			if(m_kEvent.key.keysym.sym==SDLK_RETURN){
				Execute(m_aCommand);
				for(int i=0;i<TEXT_MAX_LENGHT;i++)					//wipe the command buffer
					m_aCommand[i]=' ';				
				strcpy(m_aCommand,"");
				break;
			}
			if(m_kEvent.key.keysym.sym==SDLK_BACKSPACE){
				m_aCommand[strlen(m_aCommand)-1]='\0';
				break;
			}
						
			if(m_kEvent.key.keysym.sym==SDLK_LSHIFT || m_kEvent.key.keysym.sym==SDLK_RSHIFT){
				m_bShift=true;
				break;
			}

			
			//type text
			if(strlen(m_aCommand)<COMMAND_LENGHT) {
				int code=m_kEvent.key.keysym.sym;
				
				//shift?
				if(m_bShift) {
					if(code>96 && code<123){
						code-=32;
						strncat(m_aCommand,(char*)&(code),1);
						break;
					}
					if(code=='-'){
						code='_';
						strncat(m_aCommand,(char*)&(code),1);
						break;
					}
					if(code=='.'){
						code=':';
						strncat(m_aCommand,(char*)&(code),1);
						break;
					}					
					if(code=='7'){
						code='/';
						strncat(m_aCommand,(char*)&(code),1);
						break;
					}					
				}
				strncat(m_aCommand,(char*)&(code),1);
			}
		}
		
		//release keys
		if(m_kEvent.type==SDL_KEYUP){									
			if(m_kEvent.key.keysym.sym==SDLK_LSHIFT || m_kEvent.key.keysym.sym==SDLK_RSHIFT){
				m_bShift=false;
				break;
			}		
		}
	}
}

*/


bool Console::Execute(char* aText) {
	if(strlen(aText)==0){
		Print("");
		return false;
	}
	
	Printf("> %s", aText);				// Print command to screen.
	if(!GetSystem().RunCommand(aText))
	{
		Printf("No Command was Found for '%s'", aText);
		return false;
	}
	else
	{
		m_kCommandHistory.push_back(string(aText));

		if(m_kCommandHistory.size() > MAX_CMD_HISTRORY_LENGTH)
		{
			m_kCommandHistory.pop_front();
		}
	}

	return true;
}

void Console::FormatKey(int& r_iKey)
{
	if(m_pkInput->Pressed(KEY_RSHIFT) || m_pkInput->Pressed(KEY_LSHIFT)) 
	{
		if(r_iKey>96 && r_iKey<123){
			r_iKey-=32;
		}
		if(r_iKey=='-'){
			r_iKey='_';
		}
		if(r_iKey=='.'){
			r_iKey=':';
		}					
		if(r_iKey=='7'){
			r_iKey='/';
		}
	}
}

void Console::Toggle()
{
	float fTime = m_pkEngine->GetEngineTime();	//GetGameTime();
	if(fTime < m_fToggleTime)
		return;

	m_bActive = !m_bActive;
	m_fToggleTime = fTime += 0.2;
}


