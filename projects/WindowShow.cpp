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
    char m_szWinTitle[128];    // Variable to store the window title
    char m_szButton1Text[64];  // Text for button 1
    char m_szButton2Text[64];  // Text for button 2
    int m_iChoiceType;         // 0 = Single Choice, 1 = Two Choices

    static void ShowPlaceholderDialog(const char* message, const char* title, const char* button1, const char* button2, int choiceType);
    static LRESULT CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static bool m_bClassRegistered;  // Flag to track if the class is registered
};

LINK_ENTITY_TO_CLASS(trigger_msgbox, CTriggerMsgBox);

// Static variable to check if the dialog class has already been registered
bool CTriggerMsgBox::m_bClassRegistered = false;

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
    else if (FStrEq(pkvd->szKeyName, "choiceType"))
    {
        m_iChoiceType = atoi(pkvd->szValue);
        return true;
    }
    return CBaseEntity::KeyValue(pkvd);
}

void CTriggerMsgBox::ShowPlaceholderDialog(const char* message, const char* title, const char* button1, const char* button2, int choiceType)
{
    ClipCursor(NULL); // Release the cursor
    ShowCursor(TRUE); // Make cursor visible

    // Register the class only if it hasn't been registered yet
    if (!m_bClassRegistered)
    {
        WNDCLASS wc = {};
        wc.lpfnWndProc = DialogProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "UniquePlaceholderDialogClass";  // Use a unique class name

        // Ensure GetModuleHandle returns a valid handle
        HINSTANCE hInstance = GetModuleHandle(NULL);
        if (!hInstance || !RegisterClass(&wc))
        {
            ALERT(at_error, "Failed to register dialog class.\n");
            return;
        }
        m_bClassRegistered = true;  // Set flag to true after successful registration
    }

    // Get the game window handle
    HWND hwndGame = GetForegroundWindow();
    if (!hwndGame)
    {
        ALERT(at_error, "Failed to get game window handle.\n");
        return;
    }

    // Get the game window's position and size
    RECT gameRect;
    if (!GetWindowRect(hwndGame, &gameRect))
    {
        ALERT(at_error, "Failed to get game window position.\n");
        return;
    }

    // Calculate dialog position just below the game window
    int x = gameRect.left + (gameRect.right - gameRect.left) / 2 - 200;  // Center horizontally
    int y = gameRect.bottom;  // Just below the game window

    // Create the dialog window with a more modern style
    HWND hwnd = CreateWindowEx(
        0, "UniquePlaceholderDialogClass", title, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU | WS_CLIPCHILDREN,
        x, y, 400, 200, NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!hwnd)
    {
        ALERT(at_error, "Failed to create dialog window.\n");
        return;
    }

    // Create the message static text
    HWND hMessage = CreateWindow("STATIC", message, WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY,
        20, 20, 360, 60, hwnd, NULL, GetModuleHandle(NULL), NULL);

    // Set the font for the message text (optional)
    HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    SendMessage(hMessage, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Create buttons based on choiceType
    int buttonWidth = 120;
    int buttonHeight = 30;
    if (choiceType == 0)
    {
        // Single choice
        CreateWindow("BUTTON", button1, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            140, 120, buttonWidth, buttonHeight, hwnd, (HMENU)IDOK, GetModuleHandle(NULL), NULL);
    }
    else if (choiceType == 1)
    {
        // Two choices
        CreateWindow("BUTTON", button1, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            50, 120, buttonWidth, buttonHeight, hwnd, (HMENU)IDOK, GetModuleHandle(NULL), NULL);
        CreateWindow("BUTTON", button2, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            230, 120, buttonWidth, buttonHeight, hwnd, (HMENU)IDCANCEL, GetModuleHandle(NULL), NULL);
    }

    // Show the dialog and process messages
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_QUIT || msg.message == WM_DESTROY)
            break;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ShowCursor(FALSE);
}

LRESULT CALLBACK CTriggerMsgBox::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CTriggerMsgBox::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
    // Validate message
    if (FStringNull(pev->message))
    {
        ALERT(at_error, "trigger_msgbox: No message specified!\n");
        return;
    }

    // Launch the placeholder dialog in a separate thread
    std::thread dialogThread(ShowPlaceholderDialog, STRING(pev->message), m_szWinTitle, m_szButton1Text, m_szButton2Text, m_iChoiceType);
    dialogThread.detach();
}
