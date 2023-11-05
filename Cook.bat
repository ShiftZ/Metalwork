set UE_PATH=D:\iProject\Epic Games\UE_5.3
set PLATFORM=Windows
set CONFIGURATION=Development
set PROJECT_PATH=%~dp0

"%UE_PATH%\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" "%PROJECT_PATH%MetalworkArena.uproject" -run=cook -targetplatform=%PLATFORM% -config=%CONFIGURATION%