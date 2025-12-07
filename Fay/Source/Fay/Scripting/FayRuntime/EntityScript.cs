using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FayRuntime
{
    public class EntityScript
    {
        // Still In progress much to do 
        public static Entity entity = new Entity();
        static bool rightMousePreviouslyDown = false;
        static SceneType prevScene = SceneType.None;
        public static void OnStart()
        {
            // does nothing as of right now 
            Console.WriteLine("Hello From ECS");
        }
        public static void OnUpdate()
        {
            SceneType currentScene = InternalCalls.InternalCalls_Scene_GetActive(); // returns if 2d or 3d scene is active 

            if(currentScene != prevScene)
            {
                Console.WriteLine("SceneType: " + currentScene);
                prevScene = currentScene;
            }
            if (currentScene == SceneType.None)
                return;

            if (currentScene == SceneType.Scene2D)
            {
                Entity entity = Entity.GetSelected();
                entity.Move(entity.GetSpeed(), false);
                if (entity.HasCollisionComp() && entity.CheckCollision())
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("Entity: " + entity.GetId() + " has collided with an object ");
                    Console.ResetColor();
                }
                bool rightMouseCurrentlyDown = Input.GetMouse(MouseButton.Right);

                if (rightMouseCurrentlyDown && !rightMousePreviouslyDown)
                {
                    // Mouse button was just pressed this frame
                    Console.ForegroundColor = ConsoleColor.Blue;
                    Console.WriteLine("Selected Entity is: " + entity.GetId());
                    Console.ResetColor();

                    bool hasTransform = InternalCalls.InternalCalls_Entity_HasComponent(entity, typeof(FayRuntime.TransformComponent));
                    Console.ForegroundColor = ConsoleColor.DarkYellow;
                    Console.WriteLine("Entity has TransformComponent? " + hasTransform);
                    Console.ResetColor();

                    bool hasHitBox = InternalCalls.InternalCalls_Entity_HasComponent(entity, typeof(FayRuntime.CollisionComponent));
                    if (hasHitBox)
                    {
                        Console.ForegroundColor = ConsoleColor.DarkYellow;
                        Console.WriteLine("Entity has CollisionSpriteComponent? " + hasHitBox);
                        Console.ResetColor();
                    }
                }
                // Update the previous state for the next frame
                rightMousePreviouslyDown = rightMouseCurrentlyDown;
            }
            else
            {
                Entity entity = Entity.GetSelected();
                entity.Move(entity.GetSpeed(), true);

                bool rightMouseCurrentlyDown = Input.GetMouse(MouseButton.Right);

                if (rightMouseCurrentlyDown && !rightMousePreviouslyDown)
                {
                    // Mouse button was just pressed this frame
                    Console.ForegroundColor = ConsoleColor.Blue;
                    Console.WriteLine("Selected Entity is: " + entity.GetId());
                    Console.ResetColor();

                    bool hasTransform = InternalCalls.InternalCalls_Entity_HasComponent(entity, typeof(FayRuntime.TransformComponent));
                    Console.ForegroundColor = ConsoleColor.DarkYellow;
                    Console.WriteLine("Entity has TransformComponent? " + hasTransform);
                    Console.ResetColor();

                    bool hasHitBox = InternalCalls.InternalCalls_Entity_HasComponent(entity, typeof(FayRuntime.CollisionComponent));
                    if (hasHitBox)
                    {
                        Console.ForegroundColor = ConsoleColor.DarkYellow;
                        Console.WriteLine("Entity has CollisionSpriteComponent? " + hasHitBox);
                        Console.ResetColor();
                    }
                }
                // Update the previous state for the next frame
                rightMousePreviouslyDown = rightMouseCurrentlyDown;
            }
        }
    }
}