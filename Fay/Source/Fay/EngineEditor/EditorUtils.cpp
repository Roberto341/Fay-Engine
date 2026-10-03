#include <EngineEditor/EditorUtils.h>
#include <EngineEditor/EditorViewport.h>
namespace Fay
{
	EditorUtils::EditorUtils() = default;
	EditorUtils::~EditorUtils() = default;

	EntityID EditorUtils::m_selectedEntity = INVALID_ENTITY;
	EntityID EditorUtils::m_selectedNode= INVALID_ENTITY;
	SceneType EditorUtils::s_ActiveScene = SceneType::Scene2D;
	float EditorUtils::s_entitySpeed = 1.0f;

	Scene* EditorUtils::s_Scene = nullptr;
	
	std::string EditorUtils::GetSceneNameFromPath(const std::string& path)
	{
		// get filename
		size_t lastSlash = path.find_last_of("/\\");
		std::string filename = (lastSlash == std::string::npos)
			? path
			: path.substr(lastSlash + 1);

		// remove extension
		size_t dot = filename.find_last_of('.');
		if (dot != std::string::npos)
			filename = filename.substr(0, dot);

		return filename;
	}

	const std::string& EditorUtils::GetCurrentSceneName() const
	{
		static const std::string empty = "Untitled";
		return m_sceneName.empty() ? empty : m_sceneName;
	}

	void EditorUtils::SaveScene()
	{
		if (m_currentScene.empty())
		{
			FAY_LOG_WARN("No Scene currently loaded to save");
			return;
		}

		if (!m_currentScene.ends_with(".fayScene"))
		{
			FAY_LOG_ERROR("Failed to save FayScene invalid extension:" << m_currentScene);
			return;
		}
		auto entities = m_scene->getAllEntities(); // whatever your API is

		FAY_LOG_INFO("Save entity count: " << entities.size());
		if (!m_scene->saveScene(m_currentScene))
		{
			FAY_LOG_ERROR("Failed to save FayScene: " << m_currentScene);
			return;
		}
		FAY_LOG_INFO("FayScene Saved: " << m_currentScene);
	}
	void EditorUtils::SaveSceneAs(const std::string& path)
	{
		if (!path.ends_with(".fayScene"))
		{
			FAY_LOG_ERROR("Failed to save FayScene invalid extension: " << path);
			return;
		}

		if (!m_scene->saveSceneAs(path))
		{
			FAY_LOG_ERROR("Failed to save FayScene: " << path);
			return;
		}

		FAY_LOG_INFO("FayScene Saved: " << path);
	}
	void EditorUtils::CreateScene(const std::string& path)
	{

		// Add a iterator to check if the file already exists
		if (std::filesystem::exists(path))
		{
			FAY_LOG_WARN("Scene already exists, aborting scene creation");
			return;
		}

		m_scene->clear();
		m_renderLayer->clear();
		m_scene->saveScene(path);
		m_scene->setSceneType((m_renderMode == RenderMode::MODE_2D) ? SceneType::Scene2D : SceneType::Scene3D);
		FAY_LOG_INFO("New Scene Created: " << path);
		LoadScene(path);
	}
	void EditorUtils::LoadScene(const std::string& path)
	{
		if (GetCurrentSceneName() == GetSceneNameFromPath(path))
		{
			FAY_LOG_WARN("Scene already loaded: " << path << ", aborting load operation");
			return;
		}
		if (!path.ends_with(".fayScene"))
		{
			FAY_LOG_ERROR("Failed to load FayScene: " << path);
			return;
		}

		if (!m_scene->loadScene(path, m_texManager))
		{
			FAY_LOG_ERROR("Failed to load FayScene: " << path);
			return;
		}

		FAY_LOG_INFO("Scene loaded");

		auto entities = m_scene->getAllEntities(); // whatever your API is

		FAY_LOG_INFO("Loaded entity count: " << entities.size());
		
		if (m_scene->has2DEntities())
			m_pendingMode = RenderMode::MODE_2D;
		else
			m_pendingMode = RenderMode::MODE_3D;

		m_pendingModeUpdate = true;

		//SetStaticScene();
		m_currentScene = path;
		m_sceneName = GetSceneNameFromPath(m_currentScene);
		m_selectedEntity = INVALID_ENTITY;
	}
	
	void EditorUtils::DeleteScene()
	{
		if(!m_currentScene.ends_with(".fayScene"))
		{
			FAY_LOG_ERROR("Failed to delete FayScene: " << m_currentScene);
			return;
		}

		if (!m_scene->deleteSceneFile(m_currentScene))
		{
			FAY_LOG_ERROR("Failed to delete FayScene: " << m_currentScene);
			return;
		}
		FAY_LOG_INFO("FayScene Deleted: " << m_currentScene);

		m_currentScene = "";
		m_selectedEntity = INVALID_ENTITY;
	}
	void EditorUtils::applyPendingMode(EditorViewport* viewport)
	{
		if (!m_pendingModeUpdate)
			return;

		m_pendingModeUpdate = false;

		if(m_renderMode != m_pendingMode)
			m_skipNextFrame = true;

		m_renderMode = m_pendingMode;

		if (m_renderMode == RenderMode::MODE_3D)
		{
			m_batchRenderer->setDimension(RenderDimension::D3);
			m_renderLayer->setProjectionType(ProjectionType::Cube3D);
			SetActiveScene(SceneType::Scene3D);
			m_scene->setSceneType(SceneType::Scene3D);
			m_renderLayer->setShader(m_shader);
		}
		else
		{
			m_batchRenderer->setDimension(RenderDimension::D2);
			m_renderLayer->setProjectionType(ProjectionType::Quad2D);
			SetActiveScene(SceneType::Scene2D);
			m_scene->setSceneType(SceneType::Scene2D);
			m_renderLayer->setShader(m_shader);
		}

		if (m_renderMode == RenderMode::MODE_2D)
		{
			viewport->SetViewportMode(std::make_unique<ViewportMode2D>());
		}
		else
		{
			viewport->SetViewportMode(std::make_unique<ViewportMode3D>());
		}
		m_pendingModeUpdate = false;
	}
}
