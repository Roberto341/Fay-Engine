# Version Change
- [Version Alpha 1.0](https://github.com/Roberto341/Fay-Engine/tree/release-a1)

# Updates
- ### July 11th 2025 - Added ImGui
- ### July 12th 2025 - Implmented ImGui - Sorta- and added a Map Editor
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
- ### Update July 16th 2025
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
- ### Update July 18th 2025
  - MapEditor class now has the collision tile all setup so you can define your world bounds and/or lakes, other things you dont wanna run into, other npcs, the main player still working on a few things but it's mostly there.
  - The WorldLoader util class will be released with version Alpha 1.01 on Monday July 21st 2025
  - Other news the main engine is under construction right now and will be ready at a later given date I don't exactly have one as I just sketched out the process for it the main engine will allow you to make new projects 2d or 3d if it's 2d it will load the simple MapEditor and go from there the 3D one will be a bit different both will be part of the main engine it self 
  - Starting work now on the objects palette for the MapEditor 
- ### Update July 28th 2025
  - Objects have been added in to the tile map editor
  - A new 3D renderer has been added working on loading objects and meshs so it wont be ready for awhile yet
  - FayEngine is being renamed to FayEditor I dont know why I named it that when I started putting it together but I did :')
  - ###ImageLoad.h recived a pretty good overhaul. The old load method use to only give a 24 bit format (RGB). 
    It now supports 32 bit's which means (RGBA) alpha is recognized instead of being ignored so if you have a tree texture for eample that has a tree and has a opaque background you will see the color of the tile under it instead of seeing just white
    Just make sure your using layer or layer1 for drawing your floor tiles and layer 2 for objects and layer 3 for characters, npcs, .etc
  - ### Also as noted in an earlier update I recomend you get [Paint.net](https://www.getpaint.net/download.html) to edit textures you can load in any bmp, png, jpg that is 32 bits (RGBA) just make sure when you save you use 32 bits otherwise it wont work
  - ### Mat4.h and .cpp recived a update for the new 3d renderer
    - it now has a lookAt method for the 3d camera which is also new
  - Currently finishing work on the FayEditor the release of the new version should be availble pretty soon Sorry for the delay commit should be tommorow July 29th
- ### Update July 29th 2025
  - #### New Features
    - Scene Class
    - Cube
    - 3D rendering
    - Full 2D and 3D Editor
    - [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
  - #### Updates
    - 2D renderer system has been slighlty tweaked IndexBuffer has also been tweaked with nothing bad just useless code that wasnt needed
    - ImageLoad.h
    - The 2D tile MapEditor now has objects so it's fully working but not the best
  - #### Scene class that's right 2D and 3D scenes are now a thing
     and I'm really glad it made it into the engine still working on much of the implementation of it but it's basically here.
     The way the scene class works is, it gets loaded into the editor, you can then choose if you want to work with 2d or 3d, all you gotta do is add a sprite or a cube(this will change later to be a object of some time).
     Once you have your entities in you can use the entity properties panel to edit color, size, and position.
     There is also a Properties panel where you can toggle wireframe on or off pretty neat to see if everything is working good
     Once your ready to save just hit the save button and give it a name **!!NOTE!!** when saving make sure to choose what scene it is if it's 2d use scene_2d 3d use scene_3d make sure to select it in the file dialog other wise it will not work and the file will become corrupt
     **Also** the scenes are saved as binary 
- ###Update October 25th 2025
    - ####New Features
      - C# Scripting
    - ####Updates
      - Added a Entity class 
      - Removed ImGui::Selectable() from entities tab replaced with mouse picking 
      - Added components 
      - Removed the old tile map editor
      - Removed Scene2D and 3D classes
      - Added Scene class
      - Added delete button to delete entites that are picked
      - Update the rendering system (renderables, renderer and batch renderer) now supports 2d and 3d rendering in one
      - Removed StaticSprite and a few other things as well
    - #### Feature Updates
       - Editor will recive multiple updates in the coming months including new tools (tile map editor specifically)
    - #### C# Scripting
       The C# Scripting is still in the works and is still being built it has a internal calls class that allows you to create new methods and then you go to script glue and tie it in and it will work theres a few calls to get entites and move entities and what not but over time will be revamped and soon you will be able to build with the editor and build a fully functional scene
    - ### Scenes
       I went through and removed both 2d and 3d scene classes, 
       and added a Scene class instead just to clean things up alot and get rid of a bunch of bulk code it allows you to create 2d or 3d scenes without conflicting if you have 2d entites you cannot switch the renderMode see **RenderMode** down below to see the details
       - also when creating scenes I removed the is3d checkbox as it was no longer needed
    - ### RenderMode
      I went through and updated the render mode to accomdate the new Scene class if you have a scene your working on and it has 2d entites you cannot swtich to 3d same for 3d you must first delete the entites to go back 
    - ### ImGuizmo 
      fixed a few bugs and got it to where the gizmo will show up when a entity is selected works in both 2d and 3d 

