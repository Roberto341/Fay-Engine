# Version Changes

* [Version Alpha 1.0](https://github.com/Roberto341/Fay-Engine/releases/tag/a.1.0)
* [Version Alpha 2.0](https://github.com/Roberto341/Fay-Engine/releases/tag/a.2.0)
* [Version Alpha 2.1 Bug Fix](https://github.com/Roberto341/Fay-Engine/releases/tag/a.2.1)
* [Version Alpha 3.0](https://github.com/Roberto341/Fay-Engine/releases/tag/alpha.3)
# Updates

## July 11th, 2025

* Added ImGui.

## July 12th, 2025

* Implemented ImGui — sorta — and added a Map Editor.
* Starting on July 12th, I began overhauling the Map Editor with the goal of eventually releasing a full public application for creating maps and other content.

## July 13th, 2025

Today's commit was pushed back to tomorrow because the Map Editor experienced a major bug. Don't worry, I fixed it, and the new version will be released in the morning.

I also significantly overhauled the Map Editor. It now features:

* Configuration file loading for assets.
* Textured tile display and normal `Vec4` color display.
* Fixed an issue where the ImGui grid did not match the main window.
* Small tweaks to saving and loading `.world` files.

### World Loader

I've also been contemplating this one, so I'll break it down:

The game engine itself doesn't currently have a world loader. It only exists in the game I'm currently making. However, I can implement it into the engine with a few limitations:

1. Only `.world` files will be supported.
2. NPCs will not be renderable.
3. Multiple sprites will not render correctly.

I'm currently working on a fix for these limitations, but it might take a while. I'm a one-man band right now, so please be patient!

## July 16th, 2025

### Map Editor

The `MapEditor` class can now add textures and color tiles. You can also create new tiles directly in the editor.

Textures must follow these guidelines:

* Textures must be 24-bit to be read correctly.
* Supported formats include `.png`, `.jpg`, `.bmp`, and `.dds`.
* As long as the texture is saved as 24-bit, it should work.
* I recommend using [Paint.NET](https://www.getpaint.net/download.html) for editing textures.

The Map Editor can now also add spawn points for:

* Players
* NPCs
* Enemies

I've also started working on adding objects.

At a later date, I will add collision directly into the Map Editor.

Other features will be added as development continues.

The current projection for the Map Editor release is sometime in early or mid-2026.

### FayEngine

Another piece of news: I will be rebuilding FayEngine as a library. It will have both 2D and 3D support, with a 3D Map Editor planned for a later date.

## July 18th, 2025

### Map Editor

The `MapEditor` class now has the collision tile fully set up. You can define world boundaries, lakes, areas you don't want the player to enter, NPC boundaries, and other collision areas. The main player collision is still being worked on, but it's mostly there.

The `WorldLoader` utility class will be released with **Alpha 1.01** on Monday, July 21st, 2025.

### Main Engine

The main engine is currently under construction and will be ready at a later date. I don't have an exact release date yet, as I've only recently sketched out the overall process.

The main engine will allow you to create new projects in either 2D or 3D.

* 2D projects will load the simple Map Editor.
* 3D projects will have a different workflow.
* Both will ultimately be part of the main engine itself.

I'm also starting work on the object palette for the Map Editor.

## July 28th, 2025

### Map Editor

* Objects have been added to the tile map editor.
* A new 3D renderer has been added.
* Work has begun on loading objects and meshes into the 3D renderer, so this feature will not be ready for a while.

### FayEditor

FayEngine is being renamed to **FayEditor**. I don't know why I named it that when I started putting it together, but I did. :')

### ImageLoad

`ImageLoad.h` received a pretty significant overhaul.

The old loading method only supported a 24-bit format (RGB). It now supports 32-bit images (RGBA), meaning alpha is recognized instead of being ignored.

For example, if you have a tree texture with a transparent background, you will now see the color of the tile underneath the tree instead of seeing a solid white background.

For drawing:

* Use `layer` or `layer1` for floor tiles.
* Use `layer2` for objects.
* Use `layer3` for characters, NPCs, etc.

As noted in an earlier update, I recommend using [Paint.NET](https://www.getpaint.net/download.html) to edit textures.

You can load `.bmp`, `.png`, and `.jpg` files that are 32-bit (RGBA). Just make sure you save them as 32-bit as well.

### Mat4

`Mat4.h` and `Mat4.cpp` received updates for the new 3D renderer.

* Added a `lookAt` method for the 3D camera.
* Added the new 3D camera implementation.

I'm currently finishing work on FayEditor. The release of the new version should be available pretty soon.

Sorry for the delay. The commit should be tomorrow, July 29th.

## July 29th, 2025

### New Features

* Scene class
* Cube
* 3D rendering
* Full 2D and 3D Editor
* [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)

### Updates

* The 2D renderer system has been slightly tweaked.
* `IndexBuffer` has also been tweaked, removing some unnecessary code.
* `ImageLoad.h` received additional updates.
* The 2D Tile Map Editor now supports objects. It's fully functional, although still a work in progress.

### Scene Class

That's right — 2D and 3D scenes are now a thing!

I'm really glad this made it into the engine. There is still a lot of implementation work to do, but the basic system is now here.

The `Scene` class is loaded into the editor, where you can choose whether you want to work in 2D or 3D.

You can then add either a sprite or a cube. This will eventually change to use a more generalized object system.

Once you have your entities in the scene, you can use the Entity Properties panel to edit:

* Color
* Size
* Position

There is also a Properties panel where you can toggle wireframe mode on or off. It's pretty neat for checking whether everything is working correctly.

Once you're ready to save, simply hit the Save button and give the scene a name.

> **NOTE:** When saving, make sure you choose the correct scene type. Use `scene_2d` for 2D scenes and `scene_3d` for 3D scenes. Make sure you select the correct type in the file dialog; otherwise, the scene will not load correctly and may become corrupted.

Scenes are saved as binary files.

## October 25th, 2025

### New Features

* C# Scripting

### Updates

* Added an `Entity` class.
* Removed `ImGui::Selectable()` from the Entities tab and replaced it with mouse picking.
* Added components.
* Removed the old Tile Map Editor.
* Removed the `Scene2D` and `Scene3D` classes.
* Added the new `Scene` class.
* Added a Delete button for deleting selected entities.
* Updated the rendering system (`Renderable`, `Renderer`, and `BatchRenderer`) to support 2D and 3D rendering in one system.
* Removed `StaticSprite` and several other legacy components.

### Feature Updates

The Editor will receive multiple updates over the coming months, including new tools such as a Tile Map Editor.

### C# Scripting

C# scripting is still under development.

The system includes an `InternalCalls` class that allows new methods to be exposed to C# by implementing them in C++ and connecting them through ScriptGlue.

There are currently several calls for retrieving and moving entities, along with other functionality. Over time, this system will be expanded and revamped.

Eventually, the goal is to allow you to build a fully functional scene directly through the Editor.

### Scenes

I removed both the 2D and 3D scene classes and replaced them with a single `Scene` class.

This was done to clean things up, remove duplicated code, and allow both 2D and 3D scenes to be handled through one system.

If a scene contains 2D entities, you cannot switch its `RenderMode` to 3D. The same applies to 3D scenes.

The `is3D` checkbox was also removed when creating scenes because it was no longer necessary.

### RenderMode

The render mode system was updated to accommodate the new `Scene` class.

If a scene contains 2D entities, you cannot switch to 3D. Likewise, if a scene contains 3D entities, you cannot switch to 2D.

You must first remove the existing entities before switching modes.

### ImGuizmo

* Fixed several bugs.
* Gizmo now appears when an entity is selected.
* Works in both 2D and 3D.

## October 28th, 2025

### Notes

This update is primarily a bug-fix update.

### New Features

* 3D ray casting.
* Updated `Camera3D` to support setting and retrieving perspective.
* Added `intersectAABB` to the Editor.
* Added `getRayFromMouse` for Cube selection and interaction.

### Updates

Fixed several issues:

* Sprite deletion causing a silent crash.
* Cubes not being selected or deleted.
* Replaced the old selection code with the actual `EntityID`.
* Automatically generate the next ID when creating an entity.

## October 30th, 2025

### Notes

* This is a `dev` branch update as features are still in progress.
* This update also includes several bug fixes.

### New Features

* Tile Map Editor

### Updates

The Tile Map Editor has once again made a comeback with a refurbishment.

The `Editor` class was becoming quite large, reaching over 1,000 lines of code as of this update. I've started moving functionality into separate functions for each ImGui display so the code is easier to maintain and edit.

### Tile

`Tile` is now a header-only file containing the `Tile` and `TileInfo` structs used by the Tile Map Editor.

It supports both colored tiles and textured tiles.

### Tile Configuration

As with the previous version of the Tile Map Editor, it loads a configuration file for the tile palette.

The system has been updated to work with both tile formats and can also save the configuration.

### TileLayer

The `TileLayer` class received a much-needed update.

After finally discovering that the layer was logging an error during construction but wasn't crashing because it was being initialized correctly, I moved that logic into the `TileLayer` class and added a 2D default override.

It also works in 3D, and I made sure to thoroughly test everything.

### Tile Map Editor

The Tile Map Editor is now part of the main Editor, allowing you to switch between the two at any time.

It saves using the standard scene format with `SpriteComponent`s. However, there is no Transform component attached to these tiles because they are essentially static sprites.

## November 2nd, 2025

### Notes

First of all, I would like to welcome everyone to a brand-new month as we get closer to Christmas!

### Updates

#### Core Improvements

Some improvements have been made to the new `Configuration` class and will also be added to other classes, such as `Scene`.

The goal is to better manage the ID stack when deleting and creating tiles.

For example, if you delete tile ID `5` and then create a new tile, the new tile will reuse ID `5` instead of jumping to `6`.

I will be overhauling the `Scene` class and related systems to accommodate this as well.

Entities will also no longer start at `0`. Instead, IDs will start at `1`.

I've learned quite a bit while working on the `Configuration` class, and using ID `1` as the starting point provides a better overall system.

Some folders have also been moved and deleted.

We now have an **Assets** folder containing:

* Textures
* Configurations
* Scenes

### Configuration Class

The new `Configuration` class is responsible for the tile palette in the Tile Editor.

It provides functionality similar to the `Scene` class. You can:

* Add tiles.
* Remove tiles.
* Load configuration files.
* Save configuration files.
* Delete configuration files.
* Create new configuration files (`.config`).

Some helper functions have also been added and will eventually be introduced into other classes:

* `getSize` — Returns `m_tiles.size()`.
* `isEmpty` — Returns `m_tiles.empty()`.
* `getBack` — Returns `m_tiles.back()`.
* `getNextId` — Returns the next available ID from `m_tiles`.

### Editor Class

The `Editor` class can now create new tiles, either colored or textured, and automatically add them to the tile palette.

The display has also been refurbished. When working with the Tile Editor, you no longer see everything from the **Viewport**.

A combo box has also been added to toggle between the different views.

There is also now a dedicated Configuration box.

Removed from `Editor`:

* `m_tilePalette`
* `showSaveDialog`
* `showLoadDialog`

Several functions have been renamed or added:

* `createNewScene` → `createScene`
* `loadTilePalette` → `loadPalette`
* `saveTilePalette` → `savePalette`
* `showTilePalette` → `showPalette`
* Added `createPalette`
* `createNewTile` → `createTile`

## November 19th, 2025

### Notes

Moving forward with the refurbishment, the `Editor` class no longer has two shaders.

With the new rendering system, only one shader is required, so I removed the unnecessary code and several related functions.

The `Scene` class was also updated to use the new ID format along with several other fixes.

Tile Map functionality has now been added to the `Scene` class.

The `InternalCalls.cs` class has also been updated. Sprite and Cube-specific calls have been removed because they are no longer necessary with the ECS system.

The Entity now contains everything needed, so functions are now named using the format:

`InternalCalls_Entity_MethodName`

These methods also take the `Entity` object instead of individual sprites or cubes.

### Known Bug Updates

* [Fix framebuffer issue when switching from viewport to tile map #7](https://github.com/Roberto341/Fay-Engine/issues/7)

### Updates

#### Renderable / Sprite / Cube

* `Renderable`, `Sprite`, and `Cube` now take a `uint32_t id`.

#### Scene

The `Scene` class received a small update.

As discussed in the previous update, the new ID generator has been implemented:

* Added `render`.
* `generateEntityID` renamed to `getNextId`.
* IDs now start at `1` instead of `0`.

#### Renderable

* Added `setTexture`.

#### Editor

* Removed `setShader`.
* Removed `setShader3D`.
* Removed `m_shader3D`.
* Added `setupRenderMode`.
* Refactored `setupDockspace` to include a main menu bar.

#### Components

* Renamed `CollisionSpriteComponent` to `CollisionComponent`.

#### C# FayRuntime

* Added `FayMath.cs`.
* Updated `InternalCalls.cs`.
* Updated `EntityScript.cs`.

## November 30th, 2025

### Notes

A lot more refurbishment is going on in this update.

I'm continuing to add C# scripting functionality, adding new pages for Tools, and cleaning up various parts of the Editor.

The Tile Map Editor has been removed.

For now, configurations will remain.

### Known Bug Updates

* [Scene save and load not working correctly #8](https://github.com/Roberto341/Fay-Engine/issues/8) — Resolved with this commit.
* [Fix framebuffer issue when switching from viewport to tile map #7](https://github.com/Roberto341/Fay-Engine/issues/7) — Removed.

### Updates

#### C# Scripting

* Added `InternalCalls_Entity_CheckCollision`.
* Renamed `InternalCalls_SetActiveScene` to `InternalCalls_Scene_SetActive`.
* Renamed `InternalCalls_GetActiveScene` to `InternalCalls_Scene_GetActive`.
* Added `InternalCalls_Entity_GetSpeed`.

#### Renderable.h

* Added `checkCollision`, which checks both 2D and 3D collisions.

#### Editor.h

* Added `static bool` to `shouldRefreshConfigs`.
* Added `s_Scene`.
* Added `SetScene`.
* Added `GetSceneObjects()`.
* Added `s_EntitySpeed`.
* Added `SetEntitySpeed`.
* Added `GetEntitySpeed`.
* Added `setupTools`.

#### Sprite.h

* Removed `checkCollision` and moved it to `Renderable.h`.
* Removed `getSprite` as it is no longer needed.

## December 6th, 2025

### Notes

With **Alpha 3.0** just around the corner, more refactoring and cleanup has been taking place.

I've reworked the entire `Editor` class into several subclasses:

* `EditorUI`
* `EditorViewport`
* `EditorCore`
* `EditorUtils`

Some key improvements have been added.

The `loadScene` method has been refactored to automatically change the rendering mode when loading a scene. It has also been cleaned up for easier reading and safer usage.

A new method called `applyPendingRenderMode` has been added. It checks whether a pending render mode switch exists and, if so, applies it.

If you attempt to switch rendering modes while entities already exist in the viewport, the Editor will remain in the current rendering mode instead of switching or producing unnecessary logs. This provides a cleaner experience.

The `setupRenderMode` method has also been updated to accommodate the new pending system.

It now contains:

`currentMode = (int)m_renderMode;`

This helps the ImGui UI display the correct rendering mode instead of being stuck displaying 2D when the actual mode is 3D.

### C# Scripting

After several days of work and a major rework of the `ScriptEngine` class, Fay now loads two separate C# libraries:

* `FayRuntime.dll`
* `FayCore.dll`

A new `ReloadAssembly` method has also been added for `FayCore.dll`, allowing it to be hot-reloaded while the Editor is running.

I'm still working on `EditorCore::Init()` so that the remaining initialization code can be moved out of the main Editor and into `EditorCore`.

With the new entity-specific scripting system, there are now two DLLs:

* **FayRuntime.dll** — Provides the runtime API and engine functionality.
* **FayCore.dll** — Used for entity scripting.

FayCore is linked with FayRuntime, allowing scripts to continue accessing things such as Input, the engine API, and other functionality as development continues.

### Updates

#### Components

* Updated and integrated `ScriptComponent`.

#### Logger

* Added `FAY_LOG_THROW_ERROR`.

#### ScriptEngine

* Removed `std::cout` and `std::cerr` and replaced them with `FAY_LOG`.
* Renamed `s_domain` to `s_rootDomain`.
* Added `s_scriptDomain`.
* Added `s_oldDomains`.
* Renamed `GetDomain` to `GetRootDomain`.
* Added `GetScriptDomain`.
* Added `s_coreImage`.
* Added `s_coreAssembly`.
* Added `InvokeCoreStatic`.
* Added `InvokeCoreMethod`.
* Removed `CreateObject`.
* Renamed `InvokeStatic` to `InvokeRootStatic`.
* Renamed `InvokeMethod` to `InvokeRootMethod`.
* Added `CreateScriptDomain`.
* Added `GetMonoClass`.
* Added `ReloadAssembly`.
* Added `UnloadScriptDomain`.
* Added `createScriptTemplate`.

#### Scene

* Added `if (m_ActiveScene == type) return;` to `setSceneType()`.
* Made `has2DEntities` and `has3DEntities` public.

#### README

* Updated.

## December 7th, 2025

### Updates

* Pre-release update.
* Removed old files and folders.
* Fixed an issue where there was a semicolon at the end of an `if` block. I'm not sure how it got there, but it has been removed.
* Verified everything works in both Release and Debug x64 modes.
* Added a Delete Scene button to the main menu bar.
* The Delete Scene button deletes whichever scene is currently loaded.
* Added a `clear` method to `Scene::deleteScene()` to prevent deleted entities from continuing to render.

## June 9th, 2026

### Notes

First, I'd like to welcome everyone to the first 2026 update for FayEngine.

A lot of work has been done behind the scenes, and even more is currently in progress.

Several systems have been refactored, cleaned up, or replaced entirely.

This includes the newly refurbished **ViewportMode** system, which replaces a previously messy and fragile implementation.

Alongside this, entity creation and handling have been significantly improved, resulting in better overall readability and maintainability.

A number of C# API classes have also been renamed and refactored for clarity.

The new convention for declaring internal calls is:

```cpp
InternalCalls_{CallType}_{Method}
```

For example:

```cpp
InternalCalls_Entity_HasComponent
```

I'm also currently working on a better Add Script UI workflow so you don't have to click through as many buttons.

### Updates

#### C# Scripting

* Modified script support.
* Moved scene management to be C# invoked.

#### Components

* Changed `ScriptComponent` so it now takes a `ScriptInstance`.

#### EntityFactory

New class:

* `CreateSprite`
* `CreateCube`
* `CreateEntity(RenderMode)`

#### Entities

* Entities can now have multiple scripts attached to them.

#### ViewportMode

New header-only class.

#### ViewportMode2D

New class.

#### ViewportMode3D

New class.

#### EditorUtils

* Updated `applyPendingMode` to take `EditorViewport` as a parameter.
* Handles `RenderMode` similarly to the previous implementation while using the new rendering mode system.

#### EditorUI

* The Entities panel has become the **Hierarchy**.
* Entity Properties and Components have been merged into the **Inspector**, with collapsible sections for components and properties to clean things up.

# Update — October 3rd, 2026

## Notes

Over the last few weeks, some major changes have been made in addition to the previous update pushed to the `dev` branch, which introduced the new `Editor` class system.

The new `Editor` class system has cleaned things up quite a bit. I've also been hard at work on additional UI changes and a number of smaller quality-of-life improvements.

This update introduces a brand-new **ControlNode** system. ControlNodes essentially resemble renderable or Sprite-like classes and inherit from a parent class called `Node`.

The `Node` system will eventually expand to support additional functionality such as lighting, cameras, and other systems. The idea is that you can create a Node such as an `AiControl` and attach as many entities to it as you want.

You can also attach multiple scripts to a ControlNode and have the entities carry tags such as `"ai"` or `"ai_move"`. Those tags can then be used to invoke scripts on the appropriate entities, allowing them to be moved or controlled based on the logic defined by the scripts.

Another feature that has been added is **drag and drop**. It's a pretty nifty feature, honestly, and I've already begun sketching out a design for a **Content Browser** that will eventually allow drag-and-drop functionality as well. More information will become available in a future update.

I also went through the `EditorUI` class and cleaned up the `#pragma` regions so that they are organized around their respective methods. It looks much cleaner and makes the class easier to navigate and read.

Additionally, this update was originally intended to be pushed to the `dev` branch back in July. However, after looking over a few things, I didn't think it would be fair to ship a half-baked potato, so I spent the last few months tying up loose ends.

Some of the larger pieces I worked on during that time include making ControlNodes fully saveable and loadable, including their associated scripts, as well as getting the new tag system saved and loaded correctly. I hit a few roadblocks along the way and had to redo some things, but everything is now back up and working.

If you look through the changes, you'll also notice that I added several new classes:

* `OBJLoader`
* `Mesh`
* `MeshObject`
* `MeshRenderer`

As part of the refactoring and cleanup that's taken place, I also tackled the big elephant in the room: **FayCore**.

FayCore has now been refactored to pull user scripts directly from:

`Res/Assets/Scripts`

It is also no longer invoked when the Editor starts up. Instead, there are now **Play** and **Stop** buttons in the Tools panel that control script execution.

I also fixed a small bug in `BeginListbox()` within the hierarchy that was causing an abort exception. This previously made it impossible to properly access the Tools panel.

The Play/Stop system now rebuilds and loads the Core DLL when necessary, resets the script state between Play sessions, and allows scripts to start fresh each time. This means newly added scripts can be picked up without having to restart the Editor.

I've also begun working on the built-in code editor. It's still a work in progress and isn't fully tied into the scripting pipeline just yet, but more on that in a future update.

## Updates

### C# Scripting

* Added improved script loading and execution
* Added support for scripts stored in `Res/Assets/Scripts`
* Added script state resetting between Play sessions
* Added conditional FayCore rebuilding when scripts have changed

### Changelog
* Cleaned up

### EditorUI

* `DrawFolder`
* `DrawFile`
* `DrawContentBrowser`
* `DrawCodeEditor`
* `SetupCodeEditor`
* `ResetScriptState`

### ScriptGlue

* `InternalCalls_Node_GetChild`
* `InternalCalls_Node_GetChildCount`
* `InternalCalls_Entity_HasTag`

### Scene

* `getNodeByIndex`
* `getNodeById`
* Added save/load support for ControlNodes
* Added save/load support for associated node scripts and tags
* Various smaller improvements and fixes

### EditorUtils

* `SetIsPlaying`
* `GetIsPlaying`

### ScriptEngine

* `createScriptTemplateNode`
* `createTemplate`
* `BuildAndLoadCoreAssembly`
* `ScriptsNeedRebuild`
* Refactored FayCore script loading
* Added Play/Stop script lifecycle support
* Added conditional script rebuilding
