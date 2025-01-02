#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Windows.h"
#include <thread>
#include <mutex>

class CTriggerMsgBox : public CBaseEntity
{
public:
    bool KeyValue(KeyValueData* pkvd) override;
    void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;

private:
    char m_szWinTitle[128];    // Window title
    char m_szButton1Text[64];  // Text for button 1
    char m_szButton2Text[64];  // Text for button 2
    char m_szTrigger1[64];     // Target entity for button 1
    char m_szTrigger2[64];     // Target entity for button 2
    int m_iChoiceType;         // 0 = Single Choice, 1 = Two Choices

    static void ShowPlaceholderDialog(const char* message, const char* title, const char* button1, const char* button2,
        int choiceType, const char* trigger1, const char* trigger2);
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
    else if (FStrEq(pkvd->szKeyName, "button1Text"))
    {
        strncpy(m_szButton1Text, pkvd->szValue, sizeof(m_szButton1Text) - 1);
        m_szButton1Text[sizeof(m_szButton1Text) - 1] = '\0';
        return true;
    }
    else if (FStrEq(pkvd->szKeyName, "button2Text"))
    {
        strncpy(m_szButton2Text, pkvd->szValue, sizeof(m_szButton2Text) - 1);
        m_szButton2Text[sizeof(m_szButton2Text) - 1] = '\0';
        return true;
    }
    else if (FStrEq(pkvd->szKeyName, "trigger1"))
    {
        strncpy(m_szTrigger1, pkvd->szValue, sizeof(m_szTrigger1) - 1);
        m_szTrigger1[sizeof(m_szTrigger1) - 1] = '\0';
        return true;
    }
    else if (FStrEq(pkvd->szKeyName, "trigger2"))
    {
        strncpy(m_szTrigger2, pkvd->szValue, sizeof(m_szTrigger2) - 1);
        m_szTrigger2[sizeof(m_szTrigger2) - 1] = '\0';
        return true;
    }
    else if (FStrEq(pkvd->szKeyName, "choiceType"))
    {
        m_iChoiceType = atoi(pkvd->szValue);
        return true;
    }
    return CBaseEntity::KeyValue(pkvd);
}

void CTriggerMsgBox::ShowPlaceholderDialog(const char* message, const char* title, const char* button1, const char* button2,
    int choiceType, const char* trigger1, const char* trigger2)
{
    ClipCursor(NULL); // Release the cursor
    ShowCursor(TRUE); // Make cursor visible

    int userChoice = MessageBoxA(NULL, message, title, choiceType == 0 ? MB_OK : MB_YESNO);

    if (choiceType == 0) // Single choice
    {
        if (trigger1 && *trigger1)
        {
            ALERT(at_console, "Triggering single target: %s\n", trigger1);
            FireTargets(trigger1, NULL, NULL, USE_TOGGLE, 0);
        }
    }
    else if (choiceType == 1) // Two choices
    {
        if (userChoice == IDYES && trigger1 && *trigger1)
        {
            ALERT(at_console, "Triggering 'Yes' target: %s\n", trigger1);
            FireTargets(trigger1, NULL, NULL, USE_TOGGLE, 0);
        }
        else if (userChoice == IDNO && trigger2 && *trigger2)
        {
            ALERT(at_console, "Triggering 'No' target: %s\n", trigger2);
            FireTargets(trigger2, NULL, NULL, USE_TOGGLE, 0);
        }
    }

    ShowCursor(FALSE); // Hide cursor after the dialog box
}

void CTriggerMsgBox::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
    if (FStringNull(pev->message))
    {
        ALERT(at_error, "trigger_msgbox: No message specified!\n");
        return;
    }

    // Launch the placeholder dialog in a separate thread
    std::thread dialogThread(ShowPlaceholderDialog, STRING(pev->message), m_szWinTitle, m_szButton1Text, m_szButton2Text,
        m_iChoiceType, m_szTrigger1, m_szTrigger2);
    dialogThread.detach();
}
