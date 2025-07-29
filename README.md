# Fay Engine

### Current release
 - [Alpha 1.0](https://github.com/Roberto341/Fay-Engine/tree/a.1.0)
- Alpha 1.0.1 will release soon just cleaning a bit up you can ignore the new_renderer it doesnt do anything was testing it out for the map editor that will be removed once it is ready for release
# Setup
- Download the Fay-Engine-<current_version>.zip folder ex: (Alpha 1.0)
- Navigate to the Fay-Engine-<current_version>.zip and go into the Fay-Engine-<current_version> folder inside you'll see
    - Deps folder
    - Fay folder
    - .gitignore
    - Changelog.md
    - Fay.sln
- If you already have visual stuido 2022 or 2019 just launch the sln file
- If not go to [Visual Studio](https://visualstudio.microsoft.com/) and download and install Visual Studio 2019 or 2022 make sure to add the desktop c++ package

# Explore
    Once inside Visual Studio navigate to the main.cpp file and open it there is already an example in there but I'll run you through how to setup a game
   
    Have a look around 

    I also suggest before tinkering around to build the executable and you'll need to navigate to the Fay folder and copy the Res folder over to the desired configuration ex: x64 and place it inside the folder
    then you can run the program and it should work all just fine
# Making a game
    Now it's time to put the engine to use
    
    - Open your solution explorer and navigate to the folder view go to the Source folder and add a new folder name it Game add a Game.h and .cpp file

    - Once thats done go back to the solution view and go to the source fiter add a new filter called Game and add existing item go to your game folder you created and add all 4 files

    - Now you'll want to change the properties of the solution
    open the propeties tab and navigate to VC++ Directories then go to Include directories and add your Game folder to the path should look like this <C:\Users\User\source\repos\Fay\Fay\Source\Game> click ok and go to the Game.h file

    - Here we'll make a Game class that will handle everything main.cpp already does

    - Go to the Game.h and insert the following

    - You can choose if you want to use a namespace or not for the game it's optional

```cpp
    #pragma once
    #include <Core/Core.h>
    namespace Game
    {
        class Game
        {
        public:
	        Game(const char* title, int width, int height);
	        ~Game();

	        void update();
	        void clean();

	        void renderBackground();

	        void setShader(Fay::Shader* shader);
	        void setShader2(Fay::Shader* shader);

	        Fay::Window* getWindow() const { return window; }

	        Fay::TileLayer* getTileLayer() const { return tileLayer; }
	        Fay::TileLayer* getTileLayer2() const { return tileLayer2; }

	        Fay::Shader* getShader() const { return m_shader; }
	        Fay::Shader* getShader2() const { return m_shader2; }

	        Fay::Label* getFpsLabel() const { return fpsLabel; }
            
            Fay::Sprite* getGroupBack() const { return  groupBackground; }
	        
            Fay::Group* getGroup() const { return group; }
	        Fay::Group* getGroup2() const { return group2; }

        private:
	        Fay::Window* window;

	        Fay::TileLayer* tileLayer;
	        Fay::TileLayer* tileLayer2;

	        Fay::Shader* m_shader;
	        Fay::Shader* m_shader2;

	        Fay::Timer* timer;

	        Fay::Label* fpsLabel;

	        Fay::Group* group;
	        Fay::Group* group2;

	        Fay::Sprite* groupBackground;
        };
    }
```
    
    - Now the Game.cpp file

```cpp
#include <Game.h>
#define USING_FPS 0
namespace Game
{
	Game::Game(const char* title, int width, int height)
	{
		// Init
		Fay::FontManager::add(new Fay::Font("Cam", "Res/Fonts/cambria.ttc", 32));
		Fay::FontManager::add(new Fay::Font("Arial", "Res/Fonts/arial.ttf", 32));
		window = new Fay::Window(title, width, height);	
		m_shader = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag");	
		m_shader2 = new Fay::Shader("Res/Shaders/basic.vert", "Res/Shaders/basic.frag");
		setShader(m_shader);
		setShader2(m_shader2);

		group = new Fay::Group(Fay::Mat4::translation(Fay::Vec3(-15.8f, 7.0f, 0.0f)));
		group2 = new Fay::Group(Fay::Mat4::translation(Fay::Vec3(0, 0, 0.0f)));
		groupBackground = new Fay::Sprite(0, 0, 5, 1.5f, Fay::Vec4(0.3f, 0.3f, 0.3f, 0.9f));

#if USING_FPS
		fpsLabel = new Fay::Label("", 0.4f, 0.4f, Fay::FontManager::getFont("Cam"), Fay::Vec4(1, 1, 1, 1));
#endif
		timer = new Fay::Timer;

		tileLayer = new Fay::TileLayer(getShader());
		tileLayer2 = new Fay::TileLayer(getShader2());
		renderBackground();
	}

	Game::~Game()
	{

	}

	void Game::clean()
	{
		Fay::FontManager::clean();
	}
	void Game::renderBackground()
	{
		
		for (float y = -9.0f; y < 9.0f; y++)
		{
			for (float x = -16.0f; x < 16.0f; x++)
			{
				tileLayer->add(new Fay::Sprite(x, y, 0.9f, 0.9f, Fay::Vec4(0.2f, 0.5f, 0, 1)));
			}
		}
	}

	void Game::setShader(Fay::Shader* shader)
	{
		shader->enable();
		shader->setUniform2f("light_pos", Fay::Vec2(4.0f, 1.5f)); // lighting

		GLint texIds[]
		{
			0, 1, 2, 3, 4, 5, 6, 7, 9, 9
		};
		shader->enable();
		shader->setUniform1iv("textures", texIds, 10);
		shader->setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f));
	}
	void Game::setShader2(Fay::Shader* shader)
	{
		GLint texIds[]
		{
			0, 1, 2, 3, 4, 5, 6, 7, 9, 9
		};
		shader->enable();
		shader->setUniform1iv("textures", texIds, 10);
		shader->setUniformMat4("pr_matrix", Fay::Mat4::orthographic(-16.0f, 16.0f, -9.0f, 9.0f, -1.0f, 1.0f));
	}
	void Game::update()
	{
#if USING_FPS
		float dt = timer->elapsed();
		fpsLabel->text = std::to_string(timer->getFPS()) + " FPS";
#endif
	}
}
```

- Now go back to the main.cpp file and add the changes

- We will start by including

- We will now make our main instance and create our game variable replace <-title>, <-width>, <-height> with the actaul setting you want 

```cpp
#include <Game.h>
int main()
{
    Game::Game* game = new Game(<-title>, <-width>, <-height>)

    game->getGroup()->add(game->getGroupBack());
    game->getTileLayer()->add(game->getGroup()); //fps

    // update window
    auto win = game->getWindow();

    while(!win->closed())
    {
        // update
        win->clear();
        game->getTileLayer()->render();
        win->update();
    }

    game->clean();
    return 0;
}
```
    - Now run the program and you should see a blank window with your title, width, and height
    
    - That's all for now if you would like to see more on how to create a game visit my youtube channle where I'll have a game making series called In the Dark (it's currently in devolpment)
# How to use the FayEditor
