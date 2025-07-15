#include <Core/Core.h>
#define MAP_EDITOR 0
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

Fay::MapEditor editor(layer, 32, 18, 32.0f);
Fay::Camera camera(920, 540);


shader.enable();
shader.setUniform2f("light_pos", Fay::Vec2(4.0f, 1.5f)); // lighting

GLint texIds[]
{
	0, 1, 2, 3, 4, 5, 6, 7, 9, 9
};
shader.enable();
shader.setUniform1iv("textures", texIds, 10);
shader.setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f)); // change to use camera->getProjection();
shader.setUniformMat4("vw_matrix", camera.getViewMatrix());
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
	window.clear();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	editor.renderImGui();
	editor.handleInput();
	editor.render();
	layer->render();
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

	camera.setPosition(Fay::Vec3(16.0f, 9.0f, 0.0f));
	shader.setUniformMat4("vw_matrix", camera.getViewMatrix());

	window.update();
}
#endif
	return 0;
}