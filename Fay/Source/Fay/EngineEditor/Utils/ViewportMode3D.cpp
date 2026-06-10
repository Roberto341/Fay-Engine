#include <EngineEditor/Utils/ViewportMode3D.h>
#include <EngineEditor/EditorUtils.h>
#include <ImGuizmo/ImGuizmo.h>
#include <Math/Mat4.h>
#include <Math/Vec3.h>
namespace Fay
{
	void ViewportMode3D::Setup(EditorUtils* utils, const ImVec2& viewportSize, ViewportContext& ctx)
	{
		float aspect = viewportSize.x / viewportSize.y;

		ctx.proj = Mat4::perspective(70.0f, aspect, 0.1f, 1000.0f);
		ctx.view = utils->GetCamera3D()->getViewMatrix();

		utils->GetShader()->enable();
		utils->GetShader()->setUniformMat4("pr_matrix", ctx.proj);
		utils->GetShader()->setUniformMat4("vw_matrix", ctx.view);
	}
	void ViewportMode3D::HandleGizmos(EditorUtils* utils, ViewportContext& ctx)
	{
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetRect(ctx.imgPos.x, ctx.imgPos.y, ctx.imgSize.x, ctx.imgSize.y);

		EntityID selected = utils->GetSelectedEntity();
		if (selected == INVALID_ENTITY)
			return;

		auto* comp = ComponentManager<CubeComponent>::Get().getComponent(selected);
		if (!comp || !comp->cube)
			return;

		auto* cube = comp->cube;

		Mat4 model =
			Mat4::translation(cube->getPosition()) *
			Mat4::scale(cube->getSize());

		utils->GetShader()->setUniformMat4("ml_matrix", model);

		bool manipulated = ImGuizmo::Manipulate(
			ctx.view.data(),
			ctx.proj.data(),
			ImGuizmo::TRANSLATE,
			ImGuizmo::WORLD,
			model.data()
		);

		if (manipulated)
		{
			cube->setPosition(Vec3(
				model.elements[12],
				model.elements[13],
				model.elements[14]
			));
		}
	}
	void ViewportMode3D::HandleSelection(EditorUtils* utils, ViewportContext& ctx)
	{
		if (!ctx.clicked)
			return;

		Ray ray = getRayFromMouse(
			Vec2(ctx.mousePos.x, ctx.mousePos.y),
			Vec2(ctx.viewportPos.x, ctx.viewportPos.y),
			Vec2(ctx.viewportSize.x, ctx.viewportSize.y),
			ctx.proj,
			ctx.view
		);

		float closestT = FLT_MAX;
		EntityID selected = INVALID_ENTITY;

		for (auto* obj : utils->GetScene()->getObjects())
		{
			Vec3 min = obj->getPosition() - obj->getSize() * 0.5f;
			Vec3 max = obj->getPosition() + obj->getSize() * 0.5f;

			float t;
			if (intersectRayAABB(ray.origin, ray.dir, min, max, t))
			{
				if (t > 0 && t < closestT)
				{
					closestT = t;
					selected = obj->getId();
				}
			}
		}
		utils->SetSelectedEntity(selected);
	}
}