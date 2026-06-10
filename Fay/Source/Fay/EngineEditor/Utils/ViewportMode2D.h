#pragma once
#include <EngineEditor/Utils/ViewportMode.h>
namespace Fay
{
	class EditorUtils;
	class ViewportMode2D : public ViewportMode
	{
		void Setup(EditorUtils* utils, const ImVec2& viewportSize, ViewportContext& ctx) override;
		void HandleGizmos(EditorUtils* utils, ViewportContext& ctx) override;
		void HandleSelection(EditorUtils* utils, ViewportContext& ctx) override;
	};
}