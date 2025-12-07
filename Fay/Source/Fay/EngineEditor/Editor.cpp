#include <EngineEditor/Editor.h>
namespace Fay
{
	Editor::Editor(Window& window) : m_window(window), m_framebuffer(window.getWidth(), window.getHeight())
	{
		// Init ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(m_window.getWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");

		m_camera3D = new Camera3D(Vec3(0, 0, 5), Vec3(0, 0, -1), Vec3(0, 1.0f, 0));
		m_shader = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
		Shader& shader = *m_shader;

		m_batchRenderer = new BatchRenderer();
		m_renderLayer = new TileLayer(m_batchRenderer);
		m_renderLayer->setShader(m_shader);
		m_Scene.setSceneType(SceneType::Scene2D);

		m_utils = std::make_unique<EditorUtils>();
		
		m_core = std::make_unique<EditorCore>();
		m_core->SetUtils(m_utils.get());
		m_core->Init(&m_Scene, m_camera3D, &shader, m_renderLayer, m_renderMode, m_batchRenderer, m_textureManager);

		m_viewport = std::make_unique<EditorViewport>();
		m_viewport->SetUtils(m_utils.get());
		m_viewport->Init(window.getWidth(), window.getHeight());

		m_ui = std::make_unique<EditorUI>();
		m_ui->SetCore(m_core.get());
		m_ui->SetUtils(m_utils.get());
	}

	Editor::~Editor()
	{
		ScriptEngine::Shutdown();
		m_viewport->Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Editor::run()
	{
		while (!m_window.closed())
		{
			//m_window.clear();
			glfwPollEvents();

			ScriptGlue::SetWindow(m_window); // for input
			// Find a way to invoke this in runtime
			//ScriptEngine::InvokeRootStatic("EntityScript", "OnUpdate");
			
			// Start ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();

			m_utils->applyPendingMode();

			// Render scene into framebuffer
			m_viewport->RenderBegin();
			// new render mode
			if (!m_utils->GetSkipNextFrame())
			{
				//m_Scene.render(m_renderLayer);
				m_utils->GetScene()->render(m_utils->GetRenderLayer());
			}
			else
			{
				m_utils->SetSkipNextFrame(false);
			}
			m_viewport->RenderEnd();
			m_ui->DrawDockspace();
			m_ui->DrawToolsPanel();
			m_viewport->DrawViewport();
			m_ui->DrawEntitiesPanel();

			m_core->handleScriptExecution();
			m_ui->DrawFileMenu();

			// Rendering ImGui
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Multi-viewport
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup);
			}
			m_window.update();
		}
	}
}