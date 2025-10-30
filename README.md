# Fay Engine

### Current Release
 - [Alpha 2.1 Bug Fix](https://github.com/Roberto341/Fay-Engine/tree/a.2.1)
### Previous Releaes
- [Alpha 2.0](https://github.com/Roberto341/Fay-Engine/tree/a.2.0)
- [Alpha 1.0](https://github.com/Roberto341/Fay-Engine/tree/a.1.0)
# Setup
- Download the Fay-Engine-<current_version>.zip folder ex: (Alpha 2.1)
- Navigate to the Fay-Engine-<current_version>.zip and go into the Fay-Engine-<current_version> folder inside you'll see
    - Deps folder
    - Fay folder
    - .gitignore
    - Changelog.md
    - Fay.sln
- If you already have visual stuido 2022 or 2019 just launch the sln file
- If not go to [Visual Studio](https://visualstudio.microsoft.com/) and download and install Visual Studio 2019 or 2022 make sure to add the desktop c++ package

# Notes
I highly recomend you do a build to see if everythingcompiles correctly some operating systems seem to bug outand throw random errors best tested on Windows 10 or 1

# Project History
Back in 2019, this project began with the originalrepository called Game-Engine. Progress was slow — Jr. wasattending high school and spent his free time adding bitsand pieces to the engine. The project was originallyinspired by none other than our dear friend [The Cherno(https://www.youtube.com/@TheCherno)

Jr. was 16 at the time and just getting into C++ development when he came across a playlist called The Sparky Engine — a 2D game engine written in C++ using OpenGL. It was Jr.’s first time working with C++ and graphics. He followed every tutorial, learning how to use C++ and how graphics worked. He bounced back and forth between DirectX and OpenGL, but ultimately found OpenGL easier to work with.

After a two-year break, he returned to the engine and worked on it a bit more, adding FMOD for sound and tinkering around. Then, he went radio silent again — burned out from not fully understanding all the tools and C++. However, he didn’t give up. He kept learning C++ and figured out how to better manage his workload.

Earlier this year, he jumped back into game development, creating a small mock-up of a game world and giving birth to the **Fay Engine** project. After adding ImGui, he was suddenly inspired to take the incomplete game engine and expand it. He started by adding a basic tile map editor that allowed users to place tiles, render them, and build small game worlds.

After a few weeks of work, he scrapped that idea in favor of something bigger — adding 3D rendering. At first, everything worked really well, but there was a problem: too much code and too many files. After a major overhaul, he merged both 2D and 3D rendering, replacing the 2D renderer with one that also supported 3D. This opened the door to new possibilities.

Along came the Fay Editor, which allowed users to create scenes with 2D or 3D entities and work with them in an editor space before exporting them to an actual playable executable. After more tinkering, he added ImGuizmo for entity selection, modified a few systems, added C# scripting, and began refactoring the map editor — now known as the Tile Map Editor.

He has also improved many aspects of the engine and fixed several bugs along the way. It’s still a lightweight engine, but over time, it will continue to evolve — gaining more features and better optimization.

I personally thank The Cherno for his tutorials, which sparked this entire idea. Originally, I was going to abandon the whole project and let the engine sit, but I saw purpose in it and wanted to keep building.

This is the story of **Fay Engine**.
