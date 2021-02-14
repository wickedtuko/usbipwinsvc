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
Exit from the running shell to update environment variables, need to make the git command available. Open a new elevated PowerShell prompt and continue
```PowerShell
mkdir c:\g
cd c:\g
git clone https://github.com/microsoft/vcpkg
```

# Related projects
[USB/IP for Windows](https://github.com/cezanne/usbip-win)  
[Interactive CLI](https://github.com/daniele77/cli)  
[VISUAL C++ SERVICE TEMPLATE](https://github.com/kazaamjt/Win32_Service-CPP-Template)
