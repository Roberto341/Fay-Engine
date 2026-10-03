#pragma once
#include <unordered_map>
#include <cstdint>

namespace Fay {
	using EntityID = uint32_t;
	using NodeID = uint32_t;

	template<typename T>
	class ComponentManager
	{
	public:
		static ComponentManager<T>& Get()
		{
			static ComponentManager<T> instance;
			return instance;
		}
		void addComponent(EntityID entity, const T& component)
		{
			m_components[entity] = component;
			m_entities.push_back(entity);
		}
		void addNodeComponent(NodeID node, const T& component)
		{
			m_nodeComponents[node] = component;
			m_nodes.push_back(node);
		}
		void removeComponent(EntityID entity)
		{
			if (m_components.find(entity) != m_components.end())
			{
				m_components.erase(entity);

				auto it = std::find(m_entities.begin(), m_entities.end(), entity);
				if (it != m_entities.end())
					m_entities.erase(it);
			}
		}
		void removeNodeComponent(NodeID node)
		{
			if (m_nodeComponents.find(node) != m_nodeComponents.end())
			{
				m_nodeComponents.erase(node);
				auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
				if (it != m_nodes.end())
					m_nodes.erase(it);
			}
		}
		bool hasComponent(EntityID entity) const
		{
			return m_components.find(entity) != m_components.end();
		}
		bool hasNodeComponent(NodeID node) const
		{
			return m_nodeComponents.find(node) != m_nodeComponents.end();
		}
		T* getComponent(EntityID entity)
		{
			auto it = m_components.find(entity);
			if (it != m_components.end())
				return &it->second;
			return nullptr;
		}
		T* getNodeComponent(NodeID node)
		{
			auto it = m_nodeComponents.find(node);
			if(it != m_nodeComponents.end())
				return &it->second;
			return nullptr;
		}
		bool getNodeHasEntity(EntityID entity)
		{
			auto it = std::find_if(m_entities.begin(), m_entities.end(), entity);

			if (it != m_entities.end())
				return true;
			return false;
		}
		void clear()
		{
			m_components.clear();
			m_entities.clear();
		}
		void clearNodeComponents()
		{
			m_nodeComponents.clear();
			m_nodes.clear();
		}
		std::unordered_map<EntityID, T>& getAllComponents() { return m_components; }
		std::unordered_map<NodeID, T>& getAllNodeComponents() { return m_nodeComponents; }
		const std::vector<EntityID>& getEntities() const { return m_entities; }
		const std::vector<NodeID>& getNodes() const { return m_nodes; }
	private:
		std::unordered_map<EntityID, T> m_components;
		std::unordered_map<NodeID, T> m_nodeComponents;
		std::vector<EntityID> m_entities;
		std::vector<NodeID> m_nodes;
	};
}