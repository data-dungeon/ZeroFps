#include "console.h"
#include "../engine/zerofps.h"
#include <cstdio>
#include <cstdarg>
#include "../basic/zfsystem.h"
#include "inputhandle.h" 
 
using namespace std;

Console::Console()
  : BasicConsole("Console"), MAX_CMD_HISTRORY_LENGTH(50) {
   	

	GetSystem().Log_Create("console"); 
	
	// Register Variables
	RegisterVariable("conlog",			&m_bLog,			CSYS_BOOL);	
}

bool Console::StartUp()	
{ 
	m_pkEngine	= static_cast<ZeroFps*>(GetSystem().GetObjectPtr("ZeroFps"));
	m_pkInput	= static_cast<Input*>(GetSystem().GetObjectPtr("Input"));	
	m_pkRender	= m_pkEngine->m_pkRender;
	m_pkTexMan  = m_pkEngine->m_pkTexMan;

	m_iBufferSize			= 100;
	m_kText.resize(m_iBufferSize);
   m_bShift					=	false;
	strcpy(m_aCommand,"");
	m_iInputPos				= 0;
	
	m_nStartLine			= 0;
	m_iLastCommand			= -1;
	m_nStartLine			= 0;

	m_fToggleTime			= 0;
	m_bActive				= false;
	m_bModeInsert			= true;

	m_bShowMarker			= true;
	m_fMarkerToggleTime	= 0;

	m_iNextAutoComplete  = 0;
	m_bAutoCompleteOn    = false;

	m_pkInputHandle = new InputHandle("Console");

	return true; 
}

bool Console::ShutDown() 
{ 
	delete m_pkInputHandle;

	return true; 
}

bool Console::IsValid()	{ return true; }


void Console::AutoComplete()
{
	vector<string> kFoundCommands;

	// Run acomplete on text from start to input caret
	char szAutoComplete[256];
	strncpy(szAutoComplete,m_aCommand, m_iInputPos);
	szAutoComplete[m_iInputPos] = 0;
	
	if( m_pkSystem->AutoComplete(szAutoComplete,&kFoundCommands ,0) ) 
	{
		if(kFoundCommands.size() == 0)
			return;

		if(kFoundCommands.size() == 1) 
		{
			strcpy(m_aCommand, kFoundCommands[0].c_str());
			m_iInputPos = strlen(m_aCommand);
			m_aCommand[m_iInputPos++] = 32;
			m_aCommand[m_iInputPos] = 0;
		}
		else 
		{
			if(m_bAutoCompleteOn)
			{
				if(m_iNextAutoComplete >= kFoundCommands.size())
					m_iNextAutoComplete = 0;
				strcpy(m_aCommand, kFoundCommands[ m_iNextAutoComplete ].c_str());
				m_iNextAutoComplete++;
				if(m_iNextAutoComplete >= kFoundCommands.size())
					m_iNextAutoComplete = 0;
			}
			else
			{
				m_bAutoCompleteOn		= true;
				m_iNextAutoComplete	= 0;
				for(int i=0; i<kFoundCommands.size(); i++)
					Printf("    %s", kFoundCommands[i].c_str());
			}
		}
	}
}

void Console::InsertKey(unsigned char ucKey)
{
	if(ucKey == KEY_BACKSPACE) {
		RemoveKey(false);
		return;	
		}

	if(ucKey == KEY_DELETE) {
		RemoveKey(true);
		return;	
		}

	if(m_bModeInsert){
		if( m_iInputPos >= strlen(m_aCommand )) {
			strncat(m_aCommand,(char*)&(ucKey),1);
			m_iInputPos++;
			}
		else {
			string kEnd = &m_aCommand[m_iInputPos];
			m_aCommand[m_iInputPos] = ucKey;
			m_iInputPos++;
			strcpy(&m_aCommand[m_iInputPos], kEnd.c_str());			
			}
		}
	else {
		if( m_iInputPos >= strlen(m_aCommand ))
			strncat(m_aCommand,(char*)&(ucKey),1);
		else 
			m_aCommand[m_iInputPos] = ucKey;

		m_iInputPos++;
		}
}
	
/*	Delete at current input position. If bDelete == true it will act like Delete and remove at the postion and if
false it will act like backspace and remove before the current input position.*/
void Console::RemoveKey(bool bDelete)
{
	if(bDelete) {
		if( m_iInputPos >= strlen(m_aCommand ))	// Delete at end of line does nothing.
			return;
		
		string kEnd = &m_aCommand[m_iInputPos + 1];
		strcpy(&m_aCommand[m_iInputPos], kEnd.c_str());			
		}

	else {
		if( m_iInputPos == 0)							// Backspace at start of line does nothing.
			return;

		string kEnd = &m_aCommand[m_iInputPos];
		m_iInputPos--;
		strcpy(&m_aCommand[m_iInputPos], kEnd.c_str());	
		}

}

void Console::Draw(void)
{
	int iMarker;

	if(m_bModeInsert)
		iMarker = 22;	
	else
		iMarker = 203;	
	if(m_bShowMarker == false)
		iMarker = -1;

	m_pkRender->DrawConsole(m_aCommand,&m_kText,m_nStartLine, m_iInputPos, iMarker );	
	float fCurrTime = m_pkEngine->GetEngineTime();	//GetGameTime();

	if(fCurrTime > m_fMarkerToggleTime) {
		m_fMarkerToggleTime = fCurrTime + 0.5; 
		m_bShowMarker = !m_bShowMarker;
		}
}

void Console::ConsoleCmd(CON_CMD eCmd)
{
	// Any command turns of autocomplete
	m_bAutoCompleteOn = false;

	switch(eCmd) {
		case CONCMD_TOGGLE:			
			glPopAttrib();
			
			Toggle();

			//m_pkInputHandle->Reset();
			break;

		case CONCMD_RUN:			
			m_bAutoCompleteOn = false;
			Execute(m_aCommand);
			for(int i=0;i<TEXT_MAX_LENGHT;i++)					//wipe the command buffer
				m_aCommand[i]=' ';				
			strcpy(m_aCommand,"");
			m_iInputPos = 0;
			break;

		case CONCMD_SCROLLUP:			
			if(m_nStartLine < m_kText.size()/* && bUpdate*/ )
			{
				m_nStartLine++;
				//PREVTIME = fCurrTime;
			}
			m_pkInputHandle->GetQueuedKey(); // remove latest
			
			break;

		case CONCMD_SCROLLDOWN:	
			if(m_nStartLine > 0)
			{
				m_nStartLine--;
				//PREVTIME = fCurrTime;
			}
			m_pkInputHandle->GetQueuedKey(); // remove latest

			break;

		case CONCMD_HISTORYUP:		
			if( !m_kCommandHistory.empty() ) 
			{
				if((m_iLastCommand + 1) < m_kCommandHistory.size() )
					m_iLastCommand++;
				
				strcpy(m_aCommand, m_kCommandHistory[m_iLastCommand].c_str());
				m_iInputPos = strlen(m_aCommand);
			
				cout<<"upp"<<endl;
			}
			
			break;
		
		case CONCMD_HISTORYDOWN:
			if( !m_kCommandHistory.empty() && m_iLastCommand != -1  ) {
				m_iLastCommand--;
				if(m_iLastCommand >= 0) {
					strcpy(m_aCommand, m_kCommandHistory[m_iLastCommand].c_str());
					m_iInputPos = strlen(m_aCommand);
				}
				else {
					strcpy(m_aCommand, "");
					m_iInputPos = 0;
				
					}

			}
			else 
				m_iLastCommand = -1;
			
			break;

		case CONCMD_MARKERLEFT:			
			m_iInputPos--;
			if(m_iInputPos < 0 ) 
				m_iInputPos = 0;
			break;

		case CONCMD_MARKERRIGHT:		
			m_iInputPos++;
			if(m_iInputPos >= strlen(m_aCommand) ) 
				m_iInputPos = strlen(m_aCommand);
			break;

		case CONCMD_TOGGLEINSERT:	
			m_bModeInsert = !m_bModeInsert;
			break;
		
		default:
			assert(0);
		}
}
	
void Console::Update(void) 
{
	// Scroll console text
	static float PREVTIME = m_pkEngine->GetEngineTime();	//GetGameTime();
	static float TIME = 0.10f;
	float fCurrTime = m_pkEngine->GetEngineTime();	//GetGameTime();
	bool bUpdate = ((fCurrTime-PREVTIME) > TIME);

	static bool s_bKeyrepeatActivated = false;
	static QueuedKeyInfo s_kLastKeyPressed;
	static float s_fKeyrepeatCheckTime = m_pkEngine->GetEngineTime();		//GetGameTime();
	static float s_fLastRepeatTime = m_pkEngine->GetEngineTime();			//GetGameTime();

	
	//loop while theres keys to handle
	while(m_pkInputHandle->SizeOfQueue() > 0)
	{
		//get next queued key
		QueuedKeyInfo kKey = m_pkInputHandle->GetQueuedKey();

		CON_CMD eCmd = CONCMD_NONE;		// We assume we don't need to do any console cmd.

		//check funktion keys
		if(kKey.m_iKey == KEY_PAGEUP)		eCmd = CONCMD_SCROLLUP;
		if(kKey.m_iKey == KEY_PAGEDOWN)	eCmd = CONCMD_SCROLLDOWN;
		if(kKey.m_iKey == KEY_UP)			eCmd = CONCMD_HISTORYUP;
		if(kKey.m_iKey == KEY_DOWN)		eCmd = CONCMD_HISTORYDOWN;
		if(kKey.m_iKey == KEY_LEFT)		eCmd = CONCMD_MARKERLEFT;
		if(kKey.m_iKey == KEY_RIGHT)		eCmd = CONCMD_MARKERRIGHT;
		if(kKey.m_iKey == KEY_INSERT)		eCmd = CONCMD_TOGGLEINSERT;
		if(kKey.m_iKey == KEY_BACKQUOTE)	eCmd = CONCMD_TOGGLE;
		if(kKey.m_iKey == KEY_RETURN)		eCmd = CONCMD_RUN;
		if(kKey.m_iKey == KEY_TAB)			
		{
			AutoComplete();
			kKey.m_iKey = 0;
		}

		//if a function-key was pressed, execute function and return
		if(eCmd != CONCMD_NONE)
		{
			cout << "Running Cmd" << endl;
			if(bUpdate) {
				PREVTIME = fCurrTime;
				ConsoleCmd( eCmd );
				}
			return;
		}

		//is this a valid key?
		if( !( (kKey.m_iKey >= 32 && kKey.m_iKey <= 126) || kKey.m_iKey==8 || kKey.m_iKey==13) )
			continue;

		//type text
		if(strlen(m_aCommand) < COMMAND_LENGHT) 
		{
			//if a new button was pressed, disable keyrepeat and reset repeat timers
			if(s_kLastKeyPressed.m_iKey != kKey.m_iKey)
			{
				s_bKeyrepeatActivated = false;		
				s_fKeyrepeatCheckTime = m_pkEngine->GetEngineTime();	//GetGameTime();
				s_fLastRepeatTime = m_pkEngine->GetEngineTime();		//GetGameTime();				
			}
				
			//set last pressed key
			s_kLastKeyPressed = kKey;

			//add character to typed command
			InsertKey(FormatKey(kKey));
		}
	}

	//fixar keyrepeat
	if(s_kLastKeyPressed.m_iKey != -1)//ingen repeat om ingen knapp tryckts
	{
		if(m_pkInputHandle->Pressed((Buttons)s_kLastKeyPressed.m_iKey))
		{
	
		float fCurrTime = m_pkEngine->GetEngineTime();	//GetGameTime();
		const float REPEAT_DELAY = 0.50f, REPEAT_RATE = 0.1f;
		
		if(s_bKeyrepeatActivated == false)
		{
			// Är det dags att aktivera Key Repeat?
			if(fCurrTime - s_fKeyrepeatCheckTime > REPEAT_DELAY)
			{
				s_bKeyrepeatActivated = true;
				s_fKeyrepeatCheckTime = fCurrTime;
				s_fLastRepeatTime = fCurrTime;
				cout<<"aktiverar keyrepeaet"<<endl;
			}
		}
		else
		{
			// Är det dags att skriva ett nytt tecken?
			if(fCurrTime - s_fLastRepeatTime > REPEAT_RATE)
			{
				InsertKey(FormatKey(s_kLastKeyPressed));
				s_fLastRepeatTime = fCurrTime;
			}
		}		
		}
		else
		{
			s_bKeyrepeatActivated = false;
			s_fKeyrepeatCheckTime = m_pkEngine->GetEngineTime();	//GetGameTime();
			s_fLastRepeatTime = m_pkEngine->GetEngineTime();		//GetGameTime();
		}			
	}
}


bool Console::Execute(char* aText) 
{
	if(strlen(aText)==0){
		Printf("");
		return false;
	}
	
	Printf("] %s", aText);				// Print command to screen.

	// Put into command history. New command are pushed on front and oldest are poped from back of deque
	if(m_kCommandHistory.empty())
	{		
		//if history is empty add command
		m_kCommandHistory.push_front( string(aText) );		
	}
	else
	{
		//check if command was typed lasttime
		if(m_kCommandHistory.front() != string(aText))
			m_kCommandHistory.push_front( string(aText) );		
	}
		
	
	// If deque is full remove last element.
	if(m_kCommandHistory.size() > MAX_CMD_HISTRORY_LENGTH)
		m_kCommandHistory.pop_back();
	
	m_iLastCommand = -1;

	/*	DEBUG Help for printing cmd buffer.
	Printf("Cmd History'", aText);
	for(int i=0; i<m_kCommandHistory.size(); i++)
		Printf(" [%d] '%s'", i, m_kCommandHistory[i].c_str()); */

	if(!GetSystem().RunCommand(aText,CSYS_SRC_CONSOLE))
	{
		Printf("Unknown command '%s'", aText);
		return false;
	}

	return true;
}

char Console::FormatKey(QueuedKeyInfo& kKey)
{
	if(kKey.m_iModifiers & MODIFIER_SHIFT) 
	{
		cout << "Formating Key" << endl;
		if(kKey.m_iKey>96 && kKey.m_iKey<123){
			return kKey.m_iKey-32;
		}
		if(kKey.m_iKey=='-'){
			return '_';
		}
		if(kKey.m_iKey=='.'){
			return ':';
		}					
		if(kKey.m_iKey=='7'){
			return '/';
		}
	}
	return kKey.m_iKey;
}


void Console::Toggle()
{
	float fTime = m_pkEngine->GetEngineTime();	//GetGameTime();
	if(fTime < m_fToggleTime)
		return;

	m_bActive = !m_bActive;
	m_fToggleTime = fTime += 0.2;
	
	if(m_bActive)
	{
		m_pkInput->SetActiveInputHandle("Zerofps");	
		m_pkInput->AddActiveInputHandle("Console");
	}
	else
	{
		m_pkInput->SetActiveInputHandle("Zerofps");
		m_pkInput->AddActiveInputHandle("Gui");	
		m_pkInput->AddActiveInputHandle("Application");			
	}
}