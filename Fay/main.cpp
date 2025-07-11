#include <Core/Core.h>
/*
*
* 
*/
int main()
{
	Fay::Window window("Fay Game Engine, Copyright (C) RobCo 2020. All rights reserved.", 960, 540);

	Fay::Mat4 ortho = Fay::Mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f);

	Fay::Shader* s = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag"); //shader's
	Fay::Shader& shader = *s;
	shader.enable();
	shader.setUniform2f("light_pos", Fay::Vec2(4.0f, 1.5f)); // lighting

	Fay::TileLayer layer(&shader);
	Fay::Texture* texture = new Fay::Texture("Res/Textures/tc.png");
	Fay::Sprite* sprite = new Fay::Sprite(0, 0, 3.0f, 3.0f, texture);

	layer.add(sprite);
	GLint texIDs[] =
	{
		0,1,2,3,4,5,6,7,8,9
	};

	shader.enable();
	shader.setUniform1iv("textures", texIDs, 10);
	shader.setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f));

	while (!window.closed())
	{
		window.clear();
		double x, y;
		layer.render();
		window.getMousePos(x, y);
		shader.setUniform2f("light_pos", Fay::Vec2((float)(x * 32.0f / window.getWidth() - 16.0f), (float)(9.0f - y * 18.0f / window.getHeight())));

		window.update();
	}

	delete texture;
	Fay::FontManager::clean();
	return 0;
}