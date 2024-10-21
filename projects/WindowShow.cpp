#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Windows.h"
#include <thread>

class CTriggerMsgBox : public CBaseEntity
{
public:
	bool KeyValue(KeyValueData* pkvd) override;
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;

private:
	char m_szWinTitle[128]; // Variable to store the WinTitle

	// Function to display the message box in a separate thread
	static void ShowMessageBox(const char* message, const char* title);
};

LINK_ENTITY_TO_CLASS(trigger_msgbox, CTriggerMsgBox);

bool CTriggerMsgBox::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "WinTitle"))
	{
		strncpy(m_szWinTitle, pkvd->szValue, sizeof(m_szWinTitle) - 1);
		m_szWinTitle[sizeof(m_szWinTitle) - 1] = '\0';
		return true;
	}
	return CBaseEntity::KeyValue(pkvd);
}

void CTriggerMsgBox::ShowMessageBox(const char* message, const char* title)
{
	ClipCursor(NULL); // Release the cursor
	ShowCursor(TRUE); // Make cursor visible

	// Show the message box (non-blocking because it's in a new thread)
	MessageBox(0, message, title, MB_OK | MB_TASKMODAL);

	ShowCursor(FALSE); // Hide cursor again if necessary
	RECT screenRect;
	GetClientRect(GetForegroundWindow(), &screenRect);
	ClipCursor(&screenRect); // Re-lock the cursor to the game window
}

void CTriggerMsgBox::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	// Launch the message box in a separate thread
	std::thread messageBoxThread(ShowMessageBox, STRING(pev->message), m_szWinTitle);
	messageBoxThread.detach(); // Detach the thread so it runs independently
}
