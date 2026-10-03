#pragma once
#include <Renderer/Node.h>
#include <Entity/Entity.h>

namespace Fay {
	class ControlNode : public Node
	{
	public: 
		uint32_t& id;
		const std::string& name;
		ControlNode(uint32_t& id, const std::string& controlName);
	};
}