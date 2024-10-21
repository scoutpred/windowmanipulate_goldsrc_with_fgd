#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "Windows.h"
#include <thread>
#include <chrono>
#include <string>

class CWindowMover : public CBaseEntity
{
public:
	bool KeyValue(KeyValueData* pkvd) override; // To handle properties from the FGD
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;

private:
	void MoveWindowToPosition(int targetX, int targetY, int duration, int easingType, const std::string& gameTitle);
	static void MoveWindowThread(int targetX, int targetY, int duration, int easingType, const std::string& gameTitle);

	// Easing functions
	float Linear(float t);
	float EaseIn(float t);
	float EaseOut(float t);
	float EaseInOut(float t);

	std::string GetCurrentGameTitle(); // Declaration for the title retrieval method

	int m_iTargetX;	   // Target X position
	int m_iTargetY;	   // Target Y position
	int m_iDuration;   // Duration in milliseconds
	int m_iEasingType; // Easing type
};

LINK_ENTITY_TO_CLASS(window_mover, CWindowMover);

bool CWindowMover::KeyValue(KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "targetX"))
	{
		m_iTargetX = atoi(pkvd->szValue); // Convert string to int
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "targetY"))
	{
		m_iTargetY = atoi(pkvd->szValue); // Convert string to int
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "duration"))
	{
		m_iDuration = atoi(pkvd->szValue); // Convert string to int
		return true;
	}
	else if (FStrEq(pkvd->szKeyName, "easingType"))
	{
		m_iEasingType = atoi(pkvd->szValue); // Convert string to int
		return true;
	}
	return CBaseEntity::KeyValue(pkvd); // Pass to base class if not handled
}

void CWindowMover::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	// Get the current window title dynamically
	std::string gameTitle = GetCurrentGameTitle();

	// Start a new thread to move the window with dynamic values from the map
	std::thread(&CWindowMover::MoveWindowThread, m_iTargetX, m_iTargetY, m_iDuration, m_iEasingType, gameTitle).detach();
}

std::string CWindowMover::GetCurrentGameTitle()
{
	// Get the current foreground window
	HWND hwnd = GetForegroundWindow();
	char title[256];
	GetWindowTextA(hwnd, title, sizeof(title)); // Retrieve the window title
	return std::string(title);					// Convert to std::string
}

void CWindowMover::MoveWindowToPosition(int targetX, int targetY, int duration, int easingType, const std::string& gameTitle)
{
	HWND hwnd = FindWindow(NULL, gameTitle.c_str());
	if (!hwnd)
	{
		ALERT(at_console, "Failed to find game window: %s\n", gameTitle.c_str());
		return;
	}

	RECT rect;
	GetWindowRect(hwnd, &rect);
	int startX = rect.left;
	int startY = rect.top;

	int deltaX = targetX - startX;
	int deltaY = targetY - startY;
	int steps = 200;										   // Increased number of steps for smoother movement
	float stepDuration = static_cast<float>(duration) / steps; // Duration of each step

	for (int i = 0; i <= steps; i++)
	{
		float t = static_cast<float>(i) / steps; // Normalized time [0, 1]

		// Call the appropriate easing function
		float easedT = 0.0f;
		switch (easingType)
		{
		case 0: // Linear
			easedT = Linear(t);
			break;
		case 1: // Ease In
			easedT = EaseIn(t);
			break;
		case 2: // Ease Out
			easedT = EaseOut(t);
			break;
		case 3: // Ease In and Out
			easedT = EaseInOut(t);
			break;
		default: // Fallback to Linear
			easedT = Linear(t);
			break;
		}

		int newX = startX + static_cast<int>(deltaX * easedT);
		int newY = startY + static_cast<int>(deltaY * easedT);

		SetWindowPos(hwnd, NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(stepDuration)));
	}

	ALERT(at_console, "Window moved to position: (%d, %d)\n", targetX, targetY);
}

void CWindowMover::MoveWindowThread(int targetX, int targetY, int duration, int easingType, const std::string& gameTitle)
{
	CWindowMover mover; // Create a mover object
	mover.MoveWindowToPosition(targetX, targetY, duration, easingType, gameTitle);
}

// Easing functions
float CWindowMover::Linear(float t)
{
	return t; // No easing
}

float CWindowMover::EaseIn(float t)
{
	return t * t; // Quadratic ease-in
}

float CWindowMover::EaseOut(float t)
{
	return t * (2 - t); // Quadratic ease-out
}

float CWindowMover::EaseInOut(float t)
{
	if (t < 0.5f)
	{
		return 2 * t * t; // Ease-in for the first half
	}
	else
	{
		return -1 + (4 - 2 * t) * t; // Ease-out for the second half
	}
}