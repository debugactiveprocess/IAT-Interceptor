# IAT Hook for File Download

This project demonstrates the use of an **Import Address Table (IAT) hook** to intercept calls to the `MessageBoxW` function and replace it with custom logic that downloads a file from a given URL using PowerShell. The file is saved to the user's **Documents** folder, specifically under the **Downloads** subdirectory.

## Key Features:
- **IAT Hooking**: Intercepts and modifies function calls at runtime using the Import Address Table.
- **File Download**: Uses PowerShell’s `Invoke-WebRequest` to download a file (`yourFile`) from GitHub releases.
- **Directory Management**: Creates the **Downloads** directory in the **Documents** folder if it doesn’t already exist.
- **C++17 Compatibility**: Uses C++17 features like `std::filesystem` to handle directory creation.

## How It Works:
1. The program hooks into `MessageBoxW` using the IAT hook technique.
2. When `MessageBoxW` is called, it triggers a PowerShell command to download the `yourFile` file from the specified GitHub URL.
3. The downloaded file is saved to the `Documents\Downloads` directory.
4. If the **Downloads** directory doesn't exist, it is created dynamically before starting the download.

## Setup:
1. Ensure your project is set to use **C++17** or later (required for `std::filesystem`).
2. Build and run the program. When `MessageBoxW` is called, the download starts automatically.

## Requirements:
- Visual Studio 2017 or later (for C++17 support)
- A working internet connection for downloading the file
- PowerShell installed on the system (typically installed by default on Windows)

## Usage:
1. The hook is activated by calling `MessageBoxW`.
2. Once activated, the file will be downloaded to `C:\Users\<UserName>\Documents\Downloads\yourFile`.

## Notes:
- This project is intended for educational purposes, demonstrating basic IAT hooking techniques and using PowerShell from C++.
- The project requires administrative privileges if the system’s security settings prevent PowerShell scripts from being executed.
