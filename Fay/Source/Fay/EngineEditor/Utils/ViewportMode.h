#pragma once 
#include <Math/Mat4.h>
#include <Math/Vec2.h>
#include <Math/Vec3.h>
#include <ImGui/imgui.h>

namespace Fay
{
	class EditorUtils; // Forward declaration

	struct ViewportContext
	{
		ImVec2 mousePos;
		ImVec2 viewportPos;
		ImVec2 viewportSize;
		ImVec2 imgPos;
		ImVec2 imgSize;

		Mat4 view;
		Mat4 proj;

		bool clicked;
	};

	class ViewportMode
	{
	public:
		virtual ~ViewportMode() = default;

		virtual void Setup(EditorUtils* utils, const ImVec2& viewportSize, ViewportContext& ctx) = 0;
		virtual void HandleGizmos(EditorUtils* utils, ViewportContext& ctx) = 0;
		virtual void HandleSelection(EditorUtils* utils, ViewportContext& ctx) = 0;
	};
}