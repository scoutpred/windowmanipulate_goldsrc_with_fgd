#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include <windows.h>
#include <string>  // For std::string
#include <cstring> // For std::strlen if needed


class CTriggerWallpaper : public CBaseEntity
{
public:
	bool KeyValue(KeyValueData* pkvd) override; // Handle properties from the FGD
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value) override;
	void RestorePreviousWallpaper(); // Method to restore the previous wallpaper
	void CaptureOriginalWallpaper(); // Method to capture the original wallpaper settings
	~CTriggerWallpaper();			 // Destructor to ensure cleanup

	void Think() override; // Override Think to check for level changes

private:
	void ChangeDesktopWallpaper(const std::string& wallpaperPath);
	std::string GetGameDir();		   // Method to get the game directory
	std::string GetExecutableFolder(); // Method to get the executable folder

	static std::string m_OriginalWallpaperPath;	 // Store the original wallpaper path statically
	static std::string m_OriginalWallpaperStyle; // Store the original wallpaper style statically
	std::string m_WallpaperFileName;			 // To store the wallpaper filename
	std::string m_pCurrentMapName;				 // To store the current map name
};

// Initialize static members
std::string CTriggerWallpaper::m_OriginalWallpaperPath;
std::string CTriggerWallpaper::m_OriginalWallpaperStyle;

LINK_ENTITY_TO_CLASS(trigger_wallpaper, CTriggerWallpaper);

// Store the original wallpaper and style only once
void CTriggerWallpaper::CaptureOriginalWallpaper()
{
	// Check if the original wallpaper has already been captured
	if (m_OriginalWallpaperPath.empty())
	{
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			char originalWallpaper[256];
			DWORD size = sizeof(originalWallpaper);
			RegQueryValueEx(hKey, "Wallpaper", NULL, NULL, (LPBYTE)originalWallpaper, &size);
			m_OriginalWallpaperPath = originalWallpaper; // Store original wallpaper path

			char originalStyle[10];
			size = sizeof(originalStyle);
			RegQueryValueEx(hKey, "WallpaperStyle", NULL, NULL, (LPBYTE)originalStyle, &size);
			m_OriginalWallpaperStyle = originalStyle; // Store original wallpaper style

			RegCloseKey(hKey);
		}
		else
		{
			ALERT(at_console, "Failed to open registry key to capture original wallpaper.\n");
		}
	}
}

bool CTriggerWallpaper::KeyValue(KeyValueData* pkvd)
{
	// Check for the wallpaper filename key
	if (FStrEq(pkvd->szKeyName, "wallpaper"))
	{
		// Store the wallpaper filename
		m_WallpaperFileName = pkvd->szValue;
		return true; // Key handled
	}
	return CBaseEntity::KeyValue(pkvd); // Pass to base class if not handled
}

std::string CTriggerWallpaper::GetGameDir()
{
	char gameDir[256];				   // Buffer to store the game directory
	g_engfuncs.pfnGetGameDir(gameDir); // Get the game directory
	ALERT(at_console, "Mod Folder: %s\n", gameDir);
	return std::string(gameDir);
}

std::string CTriggerWallpaper::GetExecutableFolder()
{
	char executablePath[MAX_PATH];						// Buffer to store the executable path
	GetModuleFileNameA(NULL, executablePath, MAX_PATH); // Get the full path of the executable

	// Find the last backslash to separate the folder
	std::string pathStr(executablePath);
	size_t pos = pathStr.find_last_of("\\/");

	// If found, return the path up to the last backslash
	if (pos != std::string::npos)
	{
		pathStr = pathStr.substr(0, pos);
		ALERT(at_console, "Executable Folder: %s\n", pathStr.c_str());
		return pathStr;
	}

	return ""; // Return empty string if no path is found
}

void CTriggerWallpaper::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	// Capture the original wallpaper when the game starts
	CaptureOriginalWallpaper(); // Capture original wallpaper on use

	// Get game directory and executable folder
	std::string gameDir = GetGameDir();
	std::string execFolder = GetExecutableFolder();

	// Construct full paths dynamically without hardcoding
	const std::string fullModFolderPath = execFolder + "\\" + gameDir;				 // Full mod folder path
	const std::string wallpaperFolderPath = fullModFolderPath + "\\gfx\\wallpapers"; // Full wallpaper folder path

	// Log full paths
	ALERT(at_console, "Full Mod Folder Path: %s\n", fullModFolderPath.c_str());
	ALERT(at_console, "Full Wallpaper Folder Path: %s\n", wallpaperFolderPath.c_str());

	// Construct the full wallpaper path
	if (!m_WallpaperFileName.empty())
	{
		const std::string fullWallpaperPath = wallpaperFolderPath + "\\" + m_WallpaperFileName; // Full wallpaper path
		ALERT(at_console, "Full Wallpaper Path: %s\n", fullWallpaperPath.c_str());				// Output the full wallpaper path

		ChangeDesktopWallpaper(fullWallpaperPath); // Change wallpaper
	}
	else
	{
		ALERT(at_console, "Wallpaper filename is empty. Cannot retrieve wallpaper.\n");
	}
}

void CTriggerWallpaper::ChangeDesktopWallpaper(const std::string& wallpaperPath)
{
	// Log the wallpaper path for debugging
	ALERT(at_console, "Changing wallpaper to: %s\n", wallpaperPath.c_str());

	// Change the registry value for the desktop wallpaper
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
	{
		// Change the wallpaper in the registry
		RegSetValueEx(hKey, "Wallpaper", 0, REG_SZ, (const BYTE*)wallpaperPath.c_str(), wallpaperPath.length() + 1);
		RegSetValueEx(hKey, "WallpaperStyle", 0, REG_SZ, (const BYTE*)"2", sizeof("2")); // 2 for stretched
		RegSetValueEx(hKey, "TileWallpaper", 0, REG_SZ, (const BYTE*)"0", sizeof("0"));	 // 0 to disable tiling
		RegCloseKey(hKey);

		// Notify Windows to refresh the wallpaper
		if (SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)wallpaperPath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE))
		{
			// Force a refresh of the desktop
			HWND hwndDesktop = GetDesktopWindow();
			UpdateWindow(hwndDesktop);
			ALERT(at_console, "Wallpaper set successfully.\n");
		}
		else
		{
			// Log the failure to set the wallpaper
			DWORD error = GetLastError(); // Get the last error code
			ALERT(at_console, "Failed to set wallpaper using SystemParametersInfo. Error: %d\n", error);
		}
	}
	else
	{
		ALERT(at_console, "Failed to open registry key for wallpaper.\n");
	}
}

void CTriggerWallpaper::RestorePreviousWallpaper()
{
	// Restore the original wallpaper settings
	if (!m_OriginalWallpaperPath.empty())
	{
		ALERT(at_console, "Restoring original wallpaper to: %s\n", m_OriginalWallpaperPath.c_str());

		HKEY hKey;
		if (RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
		{
			// Restore the original wallpaper path
			RegSetValueEx(hKey, "Wallpaper", 0, REG_SZ, (const BYTE*)m_OriginalWallpaperPath.c_str(), m_OriginalWallpaperPath.length() + 1);
			// Restore the original wallpaper style
			RegSetValueEx(hKey, "WallpaperStyle", 0, REG_SZ, (const BYTE*)m_OriginalWallpaperStyle.c_str(), m_OriginalWallpaperStyle.length() + 1);
			RegCloseKey(hKey);

			// Notify Windows to refresh the wallpaper
			SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)m_OriginalWallpaperPath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
			ALERT(at_console, "Original wallpaper restored successfully.\n");
		}
		else
		{
			ALERT(at_console, "Failed to open registry key for restoring wallpaper.\n");
		}
	}
}

void CTriggerWallpaper::Think()
{
	// Assuming gpGlobals->mapname is defined as a char array
	std::string currentMapName(gpGlobals->mapname, sizeof(gpGlobals->mapname)); // Create std::string from char array

	// Check if the map has changed
	if (m_pCurrentMapName != currentMapName)
	{
		RestorePreviousWallpaper();			// Restore on level change
		m_pCurrentMapName = currentMapName; // Update current map name
	}

	// Call the base class Think method
	CBaseEntity::Think();
}


CTriggerWallpaper::~CTriggerWallpaper()
{
	RestorePreviousWallpaper(); // Call this method on destruction to revert wallpaper
}
