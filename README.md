# usbipwinsvc
USB/IP for Windows Service

# Dev Environment setup

Open PowerShell in evelevated prompt
```
Set-ExecutionPolicy Unrestricted
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

```

# Related projects
[USB/IP for Windows](https://github.com/cezanne/usbip-win)  
[Interactive CLI](https://github.com/daniele77/cli)  
[VISUAL C++ SERVICE TEMPLATE](https://github.com/kazaamjt/Win32_Service-CPP-Template)
