# Fay Engine

### Current Release
 - [Alpha 3.0](https://github.com/Roberto341/Fay-Engine/releases/tag/alpha.3)
### Previous Releaes
- [Alpha 2.1 Bug Fix](https://github.com/Roberto341/Fay-Engine/tree/a.2.1)
- [Alpha 2.0](https://github.com/Roberto341/Fay-Engine/tree/a.2.0)
- [Alpha 1.0](https://github.com/Roberto341/Fay-Engine/tree/a.1.0)
## Installing Mono (Windows)

This project requires **Mono** to run the C# scripting backend.

### Step 1: Download Mono
1. Go to the official Mono website: [https://www.mono-project.com/download/stable/](https://www.mono-project.com/download/stable/)
2. Download the **Windows installer** (`.msi`).

### Step 2: Install Mono
1. Run the installer.
2. Choose an installation directory (by default, it installs to `C:\Program Files\Mono`). **Recomended**
3. Make sure to check **“Add Mono to PATH”** if available.

### Step 3: Verify Installation
1. Open a new command prompt.
2. Run:

```bash
mono --version
```
## Setup
- Download the Fay-Engine-<current_version>.zip folder ex: (Alpha 3.0)
- Navigate to the Fay-Engine-<current_version>.zip and go into the Fay-Engine-<current_version> folder inside you'll see
    - Bin folder
    - Deps folder
    - Fay folder
    - .gitignore
    - Changelog.md
    - Fay.sln
    - README.md
- If you already have visual stuido 2026 or 2022 just launch the sln file

- If not go to [Visual Studio](https://visualstudio.microsoft.com/) and download and install Visual Studio 2026 or 2022 make sure to add the desktop c++ package as well as the c# library also make sure to install net 4.8 for the c# scripting to work with mono

- The default window that opens is the Editor in the main.cpp file 
- ## To start scripting
    - It's pretty simple to get scripting right away you can either use the Editor to add script components and add the class right into the FayCore or you can open the FayCore or FayRuntime and add the class that way just dont forget to build the solution before testing and then relaunch the Editor and add it in using the Add existing button
- ## Notes
    - This project is a 1 man show as of right now (December 3rd 2025) some things have been outdated like this README for example I completetly forgot to update it to add the Mono install instructions so if it's outdated my apologies but dont worry it will eventually be updated again also the Changelog I might give a overhaul to cuz I like the new setup I have and figured I'll go back previous versions to integrate that

    - As of right now I'm finishing up work on the C# Scripting functionallity and adding other C# logic in we will be ready for Alpha 3.0 shortly just cleaning and tidying things up a bit for the up to date code visit the dev branch and pull that to work on after Alpha 3 launches I will be merging over to main again
