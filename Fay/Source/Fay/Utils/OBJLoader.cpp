#include <Utils/OBJLoader.h>

namespace Fay
{
	bool OBJLoader::LoadOBJ(const std::string& filepath, Mesh& mesh)
	{
		std::ifstream file(filepath);

		if (!file.is_open())
			return false;

		std::vector<Vec3> positions;
		std::vector<Vec2> texCoords;
		std::vector<Vec3> normals;

		std::vector<MeshVertex> vertices;
		std::vector<uint32_t> indicies;

		std::unordered_map<OBJIndex, uint32_t, OBJIndexHash> uniqueVertices;

		std::string line;

		while (std::getline(file, line))
		{
			std::stringstream ss(line);

			std::string type;

			ss >> type;

			if (type == "v")
			{
				float x, y, z;

				ss >> x >> y >> z;

				positions.push_back(Vec3(x, y, z));
			}

			else if (type == "vt")
			{
				float u, v;
				ss >> u >> v;

				texCoords.push_back(Vec2(u, v));
			}

			else if (type == "vn")
			{
				float x, y, z;

				ss >> x >> y >> z;

				normals.push_back(Vec3(x, y, z));
			}

			else if (type == "f")
			{
				std::string face;

				while (ss >> face)
				{
					OBJIndex index{ 0, 0, 0 };

					sscanf(
						face.c_str(),
						"%d/%d/%d",
						&index.vertex,
						&index.uv,
						&index.normal
					);

					index.vertex--;
					index.uv--;
					index.normal--;

					auto found = uniqueVertices.find(index);

					if (found == uniqueVertices.end())
					{
						MeshVertex vertex;

						vertex.position = positions[index.vertex];

						if (index.uv >= 0)
							vertex.uv = texCoords[index.uv];
						else
							vertex.uv = Vec2(0, 0);

						if (index.normal >= 0)
							vertex.normal = normals[index.normal];
						else
							vertex.normal = Vec3(0, 1, 0);

						uint32_t id = vertices.size();

						vertices.push_back(vertex);

						uniqueVertices[index] = id;

						indicies.push_back(id);
					}
					else
					{
						indicies.push_back(found->second);
					}
				}
			}
		}
		mesh.setVertices(vertices);
		mesh.setIndicies(indicies);

		return true;
	}
}