#define MAP_EDITOR 0
#include <Core/Core.h>
int main()
{
#if !MAP_EDITOR
	Fay::Window window("RobCo", 920, 540);
	Fay::Camera camera(920, 540);
	Fay::Shader* s = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag");
	Fay::Shader* s2 = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag");

	Fay::Shader& shader = *s;
	Fay::Shader& shader2 = *s2;

	Fay::TileLayer layer(&shader);
	Fay::TileLayer layer2(&shader2);

	Fay::Sprite* sprite = new Fay::Sprite(0, 0, 1, 1, Fay::Vec4(0, 0, 1, 1));

	layer2.add(sprite);
	GLint texIDs[] =
	{
		0,1,2,3,4,5,6,7,8,9
	};

	shader.enable();
	shader.setUniform1iv("textures", texIDs, 10);
	shader.setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f));
	shader.setUniformMat4("vw_matrix", camera.getViewMatrix());

	shader2.enable();
	shader2.setUniform1iv("textures", texIDs, 10);
	shader2.setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f));
	shader2.setUniformMat4("vw_matrix", camera.getViewMatrix());

	for (float y = -32; y < 32; y++) 
	{
		for (float x = -32; x < 32; x++)
		{
			layer.add(new Fay::Sprite(x, y, 0.9f, 0.9f, Fay::Vec4(0.2f, 0.5f, 0, 1)));
		}
	}

	while (!window.closed())
	{
		camera.setPosition(sprite->getPosition());

		shader.enable();
		shader.setUniformMat4("vw_matrix", camera.getViewMatrix());

		shader2.enable();
		shader2.setUniformMat4("vw_matrix", camera.getViewMatrix());

		window.bindRenderTarget();
		window.clear();
		layer.render();
		layer2.render();
		if (window.isKeyPressed(GLFW_KEY_A))
			sprite->pos.x -= 0.1f;
		else if(window.isKeyPressed(GLFW_KEY_D))
			sprite->pos.x += 0.1f;
		if (window.isKeyPressed(GLFW_KEY_S))
			sprite->pos.y -= 0.1f;
		else if (window.isKeyPressed(GLFW_KEY_W))
			sprite->pos.y += 0.1f;

		window.unbindRenderTarget();
		window.update();
	}
#else
Fay::Window window("Map Editor", 920, 540);

Fay::Shader* s = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag");
Fay::Shader& shader = *s;
Fay::TileLayer* layer = new Fay::TileLayer(&shader);

Fay::Shader* s2 = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag");
Fay::Shader& shader2 = *s2;
Fay::TileLayer* layer2 = new Fay::TileLayer(&shader2);


Fay::Shader* s3 = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag");
Fay::Shader& shader3 = *s3;
Fay::TileLayer* layer3 = new Fay::TileLayer(&shader3);

Fay::Shader* s4 = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag");
Fay::Shader& shader4 = *s4;
Fay::TileLayer* layer4 = new Fay::TileLayer(&shader4);
//Fay::TileLayer& layer = *lay;


std::vector<Fay::TileLayer*> layers = { layer, layer2, layer3, layer4 };
Fay::MapEditor editor(layers, 32, 18, 32.0f);
Fay::Camera camera(920, 540);
Fay::Camera camera2(920, 540);



shader.enable();
shader.setUniform2f("light_pos", Fay::Vec2(4.0f, 1.5f)); // lighting

shader2.enable();
shader2.setUniform2f("light_pos", Fay::Vec2(4.0f, 1.5f)); // lighting

shader3.enable();
shader3.setUniform2f("light_pos", Fay::Vec2(4.0f, 1.5f)); // lighting

shader4.enable();
shader4.setUniform2f("light_pos", Fay::Vec2(4.0f, 1.5f)); // lighting


GLint texIds[]
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9
};
shader.enable();
shader.setUniform1iv("textures", texIds, 10);
shader.setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f)); // change to use camera->getProjection();
shader.setUniformMat4("vw_matrix", camera.getViewMatrix());

shader2.enable();
shader2.setUniform1iv("textures", texIds, 10);
shader2.setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f)); // change to use camera->getProjection();
shader2.setUniformMat4("vw_matrix", camera2.getViewMatrix());

shader3.enable();
shader3.setUniform1iv("textures", texIds, 10);
shader3.setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f)); // change to use camera->getProjection();
shader3.setUniformMat4("vw_matrix", camera.getViewMatrix());

shader4.enable();
shader4.setUniform1iv("textures", texIds, 10);
shader4.setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f)); // change to use camera->getProjection();
shader4.setUniformMat4("vw_matrix", camera2.getViewMatrix());

Fay::Vec3 camPos;
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImGuiIO& io = ImGui::GetIO();
io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

ImGuiStyle& style = ImGui::GetStyle();

if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
{
	style.WindowRounding = 0.0f;
	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
}

ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
ImGui_ImplOpenGL3_Init("#version 330");

while (!window.closed())
{

	camera.setPosition(Fay::Vec3(16.0f, 9.0f, 0.0f));

	shader.enable();
	shader.setUniformMat4("vw_matrix", camera.getViewMatrix());

	shader2.enable();
	shader2.setUniformMat4("vw_matrix", camera.getViewMatrix());

	shader3.enable();
	shader3.setUniformMat4("vw_matrix", camera.getViewMatrix());

	shader4.enable();
	shader4.setUniformMat4("vw_matrix", camera.getViewMatrix());
	window.clear();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	editor.renderImGui();
	editor.handleInput();
	editor.render();
	for (auto* layer : layers)
	{
		layer->render();
	}
	editor.update();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	window.update();
}
#endif

return 0;
}