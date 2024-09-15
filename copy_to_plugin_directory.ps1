# Check if the script is running with administrative privileges
if (!([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator"))
{
    # Restart the script with elevated privileges
    Start-Process PowerShell -ArgumentList "-NoProfile -ExecutionPolicy Bypass -File `"$PSCommandPath`"" -Verb RunAs
    exit
}

# Your original script
cp -r -Force "C:\Users\ola\OneDrive\projekt\Audio-Develop-Plugins\VST3\SimpleSampler.vst3" "C:\Program Files\Common Files\VST3\."
dir "C:\Program Files\Common Files\VST3\SimpleSampler.vst3\Contents\x86_64-win"

# Pause at the end
Read-Host -Prompt "Press Enter to exit"
