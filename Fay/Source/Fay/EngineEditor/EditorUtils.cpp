#include <EngineEditor/EditorUtils.h>
#include <EngineEditor/EditorViewport.h>
namespace Fay
{
	EditorUtils::EditorUtils() = default;
	EditorUtils::~EditorUtils() = default;

	EntityID EditorUtils::m_selectedEntity = INVALID_ENTITY;
	SceneType EditorUtils::s_ActiveScene = SceneType::Scene2D;
	float EditorUtils::s_entitySpeed = 1.0f;

	Scene* EditorUtils::s_Scene = nullptr;
	
	void EditorUtils::SaveScene()
	{
		if (m_currentScene.empty())
		{
			FAY_LOG_WARN("No Scene currently loaded to save");
			return;
		}

		if (!m_currentScene.ends_with(".fayScene"))
		{
			FAY_LOG_ERROR("Failed to save FayScene: " << m_currentScene);
			return;
		}

		if (!m_scene->saveScene(m_currentScene))
		{
			FAY_LOG_ERROR("Failed to save FayScene: " << m_currentScene);
			return;
		}
		FAY_LOG_INFO("FayScene Saved: " << m_currentScene);

	}
	void EditorUtils::CreateScene(const std::string& path)
	{
		m_scene->clear();
		m_renderLayer->clear();
		m_scene->saveScene(path);
		m_scene->setSceneType((m_renderMode == RenderMode::MODE_2D) ? SceneType::Scene2D : SceneType::Scene3D);
		FAY_LOG_INFO("New Scene Created: " << path);
		m_currentScene = path;
	}
	void EditorUtils::LoadScene(const std::string& path)
	{
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

		FAY_LOG_INFO("FayScene Loaded: " << path);
		
		if (m_scene->has2DEntities())
			m_pendingMode = RenderMode::MODE_2D;
		else
			m_pendingMode = RenderMode::MODE_3D;

		m_pendingModeUpdate = true;

		//SetStaticScene();

		m_currentScene = path;
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
