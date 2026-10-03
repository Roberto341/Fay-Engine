#pragma once

#include <Renderer/Mesh.h>
#include <Math/Math.h>
namespace Fay
{
	class MeshRenderer
	{
	public:
		MeshRenderer();

		void draw(const Mesh& mesh, const Mat4& transform);

	private:
		void uploadMesh(Mesh& mesh);
	};
}