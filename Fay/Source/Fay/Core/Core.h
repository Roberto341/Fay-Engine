#pragma once

// Include all files helps keep main.cpp cleaner
#include <Renderer/BatchRenderer.h>
#include <Renderer/Renderable.h>
#include <Renderer/Sprite.h>
#include <Renderer/3DRenderer/Cube.h>
#include <Renderer/Scene.h>

#include <Graphics/Window.h>
#include <Graphics/Shader.h>
#include <Graphics/Layers/TileLayer.h>
#include <Graphics/Layers/Layer.h>
#include <Graphics/Label.h>
#include <Graphics/Layers/Group.h>
#include <Graphics/Font/Font.h>
#include <Graphics/Font/FontManager.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureManager.h>
#include <Graphics/Buffers/FrameBuffer.h>
#include <Graphics/Camera3D.h>
#include <Graphics/Camera.h>

#include <EngineEditor/Editor.h>
#include <EngineEditor/EditorCore.h>
#include <EngineEditor/EditorUtils.h>
#include <EngineEditor/EditorViewport.h>

#include <Entity/Components.h>
#include <Entity/ComponentManager.h>
#include <Entity/Entity.h>

#include <Scripting/ScriptEngine.h>
#include <Scripting/ScriptGlue.h>

#include <Utils/Timer.h>
#include <Utils/FileUtils.h>

#include <Math/Math.h>
#include <Core/Logger.h>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>

#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>

#include <memory>
#include <cmath>
