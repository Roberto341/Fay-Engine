#pragma once
#include <Graphics/Camera3D.h>
#include <Graphics/Camera2D.h>
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
	struct EditorContext
	{
		Scene* scene = nullptr;
		Camera3D* camera3D = nullptr;
		Camera2D* camera2D = nullptr;
		Shader* shader = nullptr;
		TileLayer* layer = nullptr;
		RenderMode renderMode{};
		BatchRenderer* batchRenderer = nullptr;
		TextureManager textureManager{};
	};
	class EditorCore
	{
	public:
		EditorCore();
		~EditorCore();

		void Init();

		void rebuildRuntime() const;
		void handleScriptExecution();

		void SetUtils(EditorUtils* utils) { m_utils = utils; }
		void SetContext(EditorContext& context);
		const EditorContext& getContext() const { return *m_context; }
	private:
		EditorContext* m_context = nullptr;
		EditorUtils* m_utils = nullptr;
	};
}