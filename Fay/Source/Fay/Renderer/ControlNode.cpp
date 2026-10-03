#include <Renderer/ControlNode.h>

namespace Fay
{
	ControlNode::ControlNode(uint32_t& id, const std::string& controlName)
		: Node(id, controlName), id(id), name(controlName)
	{
		m_id = id;
		m_name = controlName;
	}
}