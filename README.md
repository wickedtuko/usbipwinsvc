# usbipwinsvc
USB/IP for Windows Service

# Dev Environment setup

Open PowerShell in elevated prompt
```PowerShell
# Install chocolatey
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
# Install Git for Windows
choco install git -y

# Force reload environment variables
$env:ChocolateyInstall = Convert-Path "$((Get-Command choco).Path)\..\.."
Import-Module "$env:ChocolateyInstall\helpers\chocolateyProfile.psm1"
Update-SessionEnvironment

# Install Visual Studio 2019 Build Tools
# Workload IDs - https://docs.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools?view=vs-2019&preserve-view=true#c-build-tools
choco install visualstudio2019buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --includeRecommended" -y

# Get vcpkg and bootstrap
mkdir c:\g
cd c:\g
git clone https://github.com/microsoft/vcpkg
cd .\vcpkg\
.\bootstrap-vcpkg.bat
# Install CLI library
.\vcpkg.exe install cli
# Integrate wiht Visual Studio
.\vcpkg.exe integrate install
# Install editors
choco install neovim -y
choco install vscode -y
choco install notepadplusplus -y
```
# Building the project
```PowerShell
msbuild usbipwinsvc.vcxproj /p:configuration=Debug /p:platform=win32
```
# Related projects
[USB/IP for Windows](https://github.com/cezanne/usbip-win)  
[Interactive CLI](https://github.com/daniele77/cli)  
[VISUAL C++ SERVICE TEMPLATE](https://github.com/kazaamjt/Win32_Service-CPP-Template)
