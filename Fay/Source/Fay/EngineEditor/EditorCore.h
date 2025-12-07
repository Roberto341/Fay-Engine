#pragma once
#include <Graphics/Camera3D.h>
#include <Graphics/Shader.h>
#include <Graphics/Layers/TileLayer.h>
#include <Renderer/Scene.h>
#include <Graphics/Buffers/FrameBuffer.h>
#include <Graphics/TextureManager.h>
#include <Renderer/BatchRenderer.h>
#include <EngineEditor/EditorUtils.h>

#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>
#include <memory>
namespace Fay
{
	class EditorCore
	{
	private:
	public:
		EditorCore();
		~EditorCore();

		void Init(Scene* scene, Camera3D* camera, Shader* shader, TileLayer* layer, RenderMode mode, BatchRenderer* batch, TextureManager texMan);

		void rebuildRuntime() const;
		void handleScriptExecution();

		void SetUtils(EditorUtils* utils) { m_utils = utils; }

	private:
		EditorUtils* m_utils = nullptr;
	};
}