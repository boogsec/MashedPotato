# MashedPotato
A self-contained Windows local privilege escalation proof-of-concept that demonstrates SYSTEM impersonation via named pipes. It consists of two C programs:
 * server.c: Creates a named-pipe listener, waits for a SYSTEM-level client, impersonates its token, then spawns a SYSTEM shell.
 * bait.c: A minimal loader that connects to the named pipe as SYSTEM (after you elevate it via scheduled task/UAC trick) and sends the handshake byte.

# Prerequisites
Make sure you have a Linux environment with the mingw-w64 cross-compiler installed:
```
sudo apt update
sudo apt install mingw-w64
```

# Compiling
```
# Build the pipe server
x86_64-w64-mingw32-gcc -o server.exe server.c

# Build the bait file
x86_64-w64-mingw32-gcc -o bait.exe bait.c

# Build the loader
x86_64-w64-mingw32-gcc -o loader.exe loader.c 
```

# Usage
To use MashedPotato, you want to make sure that you first launch the `server.exe` file as Admin. This can be done manually or with a UAC bypass too. Then run the `loader.exe` binary and after a few seconds you can observe that you now have a `cmd.exe` session as `NT AUTHORITY\SYSTEM`

You can also use the `stage1.ps1` script to launch everything automatically from a elevated powershell session. `stage.ps1` uses the `FodHelper` UAC Bypass to execute everything.
