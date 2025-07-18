# Version Change
- [Version Alpha 1.0](https://github.com/Roberto341/Fay-Engine/tree/release-a1)

# Updates
- July 11th 2025 - Added ImGui
- July 12th 2025 - Implmented ImGui - Sorta- and added a Map Editor
- Starting on July 12th I will be over hauling the map editor and relasing a full application for making maps and what not that will be available to the public
- Update July 13th todays commit has been pushed back to tomorrow because the map editor experienced a major bug dont worry I fixed it and the new version will be released in the morning 
 I also went and overhauled the map editor significantly it now features 
   - Configuration file loading to load assests in 
   - Textured tile display and normal Vec4 display
   - Fixed a issue with the imgui grid not matching the main window 
   - small tweaks to the save and load .world files 
- Also I've been contemplating this one so I'll break it down for ya 
- The game engine doesn't have a world loader its only in the game I'm currently making however I can implement it into the engine with a few things 
  - 1 only .world files will be supported 
  - 2 you wont be able to render any npcs 
  - multiple sprites wont render either 
  - Just so you know I'm working on a fix for it but it might take awhile I'm a one man band right now so please be patient 
- Update July 16th 2025
    - MapEditor class now has the ability to add textures and color tiles you can also make new ones directly in the editor (textures must follow below guidelines)
        - Textures must be 24 bit to read otherwise it wont work
        - Textures can be any file type .png, .jpg, .bmp, .dds
        - As long as it's saved as 24 bit it will work
        - I recomend using [Paint.net](https://www.getpaint.net/download.html) 
    - Also MapEditor now has the ability to add spawn points for players, npcs, and enemey
    - I've started working on adding Objects as well
    - At a latter date I will add collision directly in the map editor 
    - Other features will be added as well as time goes by
    - Current projection for the MapEditor release will be later in the begining of 2026 or mid 2026 
    - Other news I will be rebuilding FayEngine as a library it will have 2D and 3D support as well as the 3D Map Editor which will come at a latter date 
- Update July 18th 2025
  - MapEditor class now has the collision tile all setup so you can define your world bounds and/or lakes, other things you dont wanna run into, other npcs, the main player still working on a few things but it's mostly there.
  - The WorldLoader util class will be released with version Alpha 1.01 on Monday July 21st 2025
  - Other news the main engine is under construction right now and will be ready at a later given date I don't exactly have one as I just sketched out the process for it the main engine will allow you to make new projects 2d or 3d if it's 2d it will load the simple MapEditor and go from there the 3D one will be a bit different both will be part of the main engine it self 
  - Starting work now on the objects palette for the MapEditor 