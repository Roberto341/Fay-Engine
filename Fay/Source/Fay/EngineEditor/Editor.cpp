#include <EngineEditor/Editor.h>
namespace Fay
{
	Editor::Editor() 
		: m_window(std::make_unique<Window>("Fay Editor", 1920, 1080))
		, m_framebuffer(m_window->getWidth(), m_window->getHeight())
	{
		initImgui();
		setupEditor();
		TextureManager::add(new Texture("Folder", "Res/Assets/Content-Browser/Icons/Folder_Icon.png"));
		TextureManager::add(new Texture("Blank", "Res/Assets/Content-Browser/Icons/Blank_Icon.png"));
		TextureManager::add(new Texture("Json", "Res/Assets/Content-Browser/Icons/Json_Icon.png"));
		TextureManager::add(new Texture("Script", "Res/Assets/Content-Browser/Icons/Script_Icon.png"));
		TextureManager::add(new Texture("Txt", "Res/Assets/Content-Browser/Icons/Txt_Icon.png"));
		TextureManager::add(new Texture("Wav", "Res/Assets/Content-Browser/Icons/WAV_Icon.png"));
		TextureManager::add(new Texture("Mp3", "Res/Assets/Content-Browser/Icons/MP3_Icon.png"));
		TextureManager::add(new Texture("Scene", "Res/Assets/Content-Browser/Icons/Scene_Icon.png"));

	}

	Editor::~Editor()
	{
		ScriptEngine::Shutdown();
		m_viewport->Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		m_window->shutdown();
	}

	void Editor::setupEditor()
	{
		// Create all needed utilities then pass to the EditorContext
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

		EditorContext ctx;
		ctx.scene = &m_Scene;
		ctx.batchRenderer = m_batchRenderer;
		ctx.camera3D = m_camera3D;
		ctx.shader = &shader;
		ctx.layer = m_renderLayer;
		ctx.renderMode = m_renderMode;
		ctx.batchRenderer = m_batchRenderer;
		ctx.textureManager = m_textureManager;

		m_core->SetContext(ctx);
		m_core->Init();

		m_viewport = std::make_unique<EditorViewport>();
		m_viewport->SetUtils(m_utils.get());
		m_viewport->Init(m_window->getWidth(), m_window->getHeight());
		m_viewport->SetViewportMode(std::make_unique<ViewportMode2D>());
		m_ui = std::make_unique<EditorUI>();
		m_ui->SetCore(m_core.get());
		m_ui->SetUtils(m_utils.get());
		m_ui->SetViewport(m_viewport.get());
	
		// Entity Factory
		m_factory = std::make_unique<EntityFactory>(
			&m_Scene,
			m_renderLayer, 
			m_utils.get()
		);
		m_utils->SetFactory(m_factory.get());
	}

	void Editor::initImgui()
	{
		// Init ImGui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(m_window->getWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330 core");
	}

	void Editor::runEditor()
	{
		while (!m_window->closed())
		{
			//m_window.clear();
			glfwPollEvents();
			Window& windRef = *m_window;
			ScriptGlue::SetWindow(windRef); // for input

			// Start ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			ImGuizmo::BeginFrame();

			m_utils->applyPendingMode(m_viewport.get());

			// Render scene into framebuffer
			m_viewport->RenderBegin();
			// new render mode
			if (!m_utils->GetSkipNextFrame())
			{
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
			m_ui->DrawContentBrowser();
			m_ui->DrawCodeEditor();
			if (m_utils->GetIsPlaying())
			{
				m_core->handleScriptExecution();
			}
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
			m_window->update();
		}
	}
}