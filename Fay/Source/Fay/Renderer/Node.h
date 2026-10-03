#pragma once
#include <Math/Math.h>
namespace Fay
{
	class Node
	{
	protected:
		uint32_t m_id;
		std::string m_name;
	public:
		Node(uint32_t id, const std::string name)
			: m_id(id), m_name(name)
		{

		}
		virtual ~Node() {}
		inline const uint32_t& getId() const { return m_id; }
		inline const std::string& getName() const { return m_name; }
	};
}