#pragma once
#include <unordered_map>
#include <cstdint>

namespace Fay {
	using EntityID = uint32_t;

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
			m_Components[entity] = component;
			m_entites.push_back(entity);
		}
		void removeComponent(EntityID entity)
		{
			if (m_Components.find(entity) != m_Components.end())
			{
				m_Components.erase(entity);

				auto it = std::find(m_entites.begin(), m_entites.end(), entity);
				if (it != m_entites.end())
					m_entites.erase(it);
			}
		}
		bool hasComponent(EntityID entity) const
		{
			return m_Components.find(entity) != m_Components.end();
		}
		T* getComponent(EntityID entity)
		{
			auto it = m_Components.find(entity);
			if (it != m_Components.end())
				return &it->second;
			return nullptr;
		}
		void clear()
		{
			m_Components.clear();
			m_entites.clear();
		}
		std::unordered_map<EntityID, T>& getAllComponents() { return m_Components; }
		const std::vector<EntityID>& getEntities() const { return m_entites; }
	private:
		std::unordered_map<EntityID, T> m_Components;
		std::vector<EntityID> m_entites;
	};
}