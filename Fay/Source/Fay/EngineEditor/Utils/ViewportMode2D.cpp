#include <EngineEditor/Utils/ViewportMode2D.h>
#include <EngineEditor/EditorUtils.h>
#include <ImGuizmo/ImGuizmo.h>
#include <Math/Mat4.h>
#include <Math/Vec3.h>
namespace Fay
{
	void ViewportMode2D::Setup(EditorUtils* utils, const ImVec2& viewportSize, ViewportContext& ctx)
	{
		float halfW = viewportSize.x * 0.5f;
		float halfH = viewportSize.y * 0.5f;

		ctx.proj = Mat4::orthographic(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
		ctx.view = Mat4::identity();

		utils->GetShader()->enable();
		utils->GetShader()->setUniformMat4("pr_matrix", ctx.proj);
		utils->GetShader()->setUniformMat4("vw_matrix", ctx.view);
	}
	void ViewportMode2D::HandleGizmos(EditorUtils* utils, ViewportContext& ctx)
	{
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetOrthographic(true);
		ImGuizmo::SetRect(ctx.imgPos.x, ctx.imgPos.y, ctx.imgSize.x, ctx.imgSize.y);

		EntityID selected = utils->GetSelectedEntity();
		if (selected == INVALID_ENTITY)
			return;

		auto* comp = ComponentManager<SpriteComponent>::Get().getComponent(selected);
		if (!comp || !comp->sprite)
			return;

		auto* sprite = comp->sprite;

		Vec3 pos = sprite->getPosition();
		Vec3 size = sprite->getSize();

		Mat4 model =
			Mat4::translation(pos) *
			Mat4::scale(Vec3(size.x, size.y, 1.0f));

		bool manipulated = ImGuizmo::Manipulate(
			ctx.view.data(),
			ctx.proj.data(),
			ImGuizmo::TRANSLATE,
			ImGuizmo::LOCAL,
			model.data()
		);

		if (manipulated)
		{
			sprite->setPosition(Vec3(
				model.elements[12],
				model.elements[13],
				model.elements[14]
			));
		}
	}
	void ViewportMode2D::HandleSelection(EditorUtils* utils, ViewportContext& ctx)
	{
		if (!ctx.clicked)
			return;

		// --- screen -> world ---
		Vec2 relative =
		{
			ctx.mousePos.x - ctx.imgPos.x,
			(ctx.imgPos.y + ctx.viewportSize.y) - ctx.mousePos.y
		};

		Vec2 world =
		{
			relative.x - (ctx.viewportSize.x / 2.0f),
			relative.y - (ctx.viewportSize.y / 2.0f)
		};

		EntityID hit = INVALID_ENTITY;

		for (auto* obj : utils->GetScene()->getObjects())
		{
			if (obj == nullptr)
				continue;

			Vec3 pos = obj->getPosition();
			Vec3 size = obj->getSize();

			if (world.x >= pos.x &&
				world.x <= pos.x + size.x &&
				world.y >= pos.y &&
				world.y <= pos.y + size.y)
			{
				hit = obj->getId();
				break;
			}
		}
		utils->SetSelectedEntity(hit);
	}
}