# usbipwinsvc
USB/IP for Windows Service

# Dev Environment setup

Open PowerShell in elevated prompt
```PowerShell
# !!!Security!!! Allow execution of any code
Set-ExecutionPolicy Unrestricted
# Install chocolatey
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
# Install Git for Windows
choco install git -y
```
Exit from the running shell to update environment variables this is needed to make the git commands available. Then open a new elevated PowerShell prompt and continue
```PowerShell
# Get vcpkg and bootstrap
mkdir c:\g
cd c:\g
git clone https://github.com/microsoft/vcpkg
cd .\vcpkg\
.\bootstrap-vcpkg.bat

```

# Related projects
[USB/IP for Windows](https://github.com/cezanne/usbip-win)  
[Interactive CLI](https://github.com/daniele77/cli)  
[VISUAL C++ SERVICE TEMPLATE](https://github.com/kazaamjt/Win32_Service-CPP-Template)
