#pragma once

#include <Renderer/Mesh.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
namespace Fay
{
	class Mesh;

	struct OBJIndex
	{
		int vertex;
		int uv;
		int normal;

		bool operator==(const OBJIndex& other) const
		{
			return vertex == other.vertex && uv == other.uv && normal == other.normal;
		}
	};

	struct OBJIndexHash
	{
		size_t operator()(const OBJIndex& index) const
		{
			return index.vertex ^
				(index.uv << 8) ^
				(index.normal << 16);
		}
	};
	class OBJLoader
	{
	public:
		static bool LoadOBJ(const std::string& filepath, Mesh& mesh);
	};
}