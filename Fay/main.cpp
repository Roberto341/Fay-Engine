#define MAP_EDITOR 0
#define NEW_EDITOR 1
#define USING_3D 0
#include <Core/Core.h>
int main()
{
#if USING_3D
	Fay::Window window("My Game", 1920, 1080);
	Fay::Shader* s = new Fay::Shader("Res/Shaders/basic_3d.vert", "Res/Shaders/basic_3d.frag");
	Fay::Shader& shader = *s;
	Fay::TileLayer* layer = new Fay::TileLayer(&shader, new Fay::BatchRenderer3D());

	Fay::Camera3D camera(Fay::Vec3(0, 0, 5), Fay::Vec3(0, 0, -1), Fay::Vec3(0, 1.0f, 0));
	
	Fay::Cube* newCube = new Fay::Cube(0, 0, 0, 1, 1, 1, Fay::Vec4(0, 1, 0, 1));
	Fay::Cube* cube2 = new Fay::Cube(0, 0, 0, 1, 1, 1, Fay::Vec4(1, 0, 0, 1));
	Fay::Cube* cube3 = new Fay::Cube(0, -2, 0, 1, 1, 1, Fay::Vec4(0, 0, 1, 1));

	layer->add(newCube);
	layer->add(cube2);
	layer->add(cube3);

	
	float angle = 0.0f;
	float lastTime = glfwGetTime();
	while (!window.closed())
	{
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		angle += 50.0f * deltaTime;
		if (angle > 360.0f) angle -= 360.0f;
		float speed = 5.0f;
		glEnable(GL_DEPTH_TEST);
		window.clear();

		float time = glfwGetTime(); // however you're measuring time
		Fay::Mat4 model = Fay::Mat4::translation(Fay::Vec3(0, 0, 0));// *Fay::Mat4::rotation(angle, Fay::Vec3(0, 1, 0));
		
		Fay::Mat4 proj = Fay::Mat4::perspective(70.0f, (float)window.getWidth() / (float)window.getHeight(), 0.1f, 100.0f);
		
		if (window.isKeyPressed(GLFW_KEY_UP))
		{
			auto delta = camera.getForward() * speed * deltaTime;
			camera.move(delta);
		}
		if (window.isKeyPressed(GLFW_KEY_DOWN))
		{
			auto delta = camera.getForward() * -speed * deltaTime;
			camera.move(delta);
		}


		Fay::Vec3 right = camera.getForward().cross(Fay::Vec3(0, 1, 0)).normalized();

		if (window.isKeyPressed(GLFW_KEY_RIGHT))
			camera.move(right * speed * deltaTime);
		if (window.isKeyPressed(GLFW_KEY_LEFT))
			camera.move(right * -speed * deltaTime);


		if (window.isKeyPressed(GLFW_KEY_D))
			newCube->pos.x += speed * deltaTime;
		if (window.isKeyPressed(GLFW_KEY_A))
			newCube->pos.x -= speed * deltaTime;
		if (window.isKeyPressed(GLFW_KEY_W))
			newCube->pos.y += speed * deltaTime;
		if (window.isKeyPressed(GLFW_KEY_S))
			newCube->pos.y -= speed * deltaTime;

		Fay::Mat4 view = camera.getViewMatrix();// Mat4::translate(Vec3(0, 0, -5));
		
		shader.enable();
		shader.setUniformMat4("pr_matrix", proj);
		shader.setUniformMat4("vw_matrix", camera.getViewMatrix());
		shader.setUniformMat4("ml_matrix", model);

		//layer->getRenderer()->push(model);
		layer->render3d();
		//layer->getRenderer()->pop();

		window.update();
	}
#endif
#if NEW_EDITOR
	Fay::Window window("New Editor", 1920, 1080);
	if (!window.getWindow()) return -1;
	Fay::Editor editor(window);

	editor.run();
#endif
#if MAP_EDITOR
	Fay::Window window("Map Editor", 920, 540);

	Fay::Shader* s = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag");
	Fay::Shader& shader = *s;
	
	Fay::TileLayer* layer = new Fay::TileLayer(&shader);
	Fay::TileLayer* layer2 = new Fay::TileLayer(&shader);
	Fay::TileLayer* layer3 = new Fay::TileLayer(&shader);
	Fay::TileLayer* layer4 = new Fay::TileLayer(&shader);
	
	std::vector<Fay::TileLayer*> layers = { layer, layer2, layer3, layer4 };
	Fay::MapEditor editor(layers, 32, 18, 32.0f);
	Fay::Camera camera(920, 540);
	
	shader.enable();
	shader.setUniform2f("light_pos", Fay::Vec2(4.0f, 1.5f)); // lighting
	
	GLint texIds[]
	{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9
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
	
	I	mGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");
	
	while (!window.closed())
	{
	
		camera.setPosition(Fay::Vec3(16.0f, 9.0f, 0.0f));
	
		shader.enable();
		shader.setUniformMat4("vw_matrix", camera.getViewMatrix());
	
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