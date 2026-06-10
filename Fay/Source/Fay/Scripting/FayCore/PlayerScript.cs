using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FayRuntime;
namespace FayCore
{
    public class PlayerScript
    {
        private static bool waitForMouseUp = false;
        public static void OnStart()
        {
        }

        public static void OnUpdate()
        {
            Entity entity = new Entity(1);
            entity.Move(2.0f, false);
            if (Input.GetMouse(MouseButton.Right))
            {
                if (!waitForMouseUp)
                {
                    Console.WriteLine($"PlayerScript attached to entity {entity.GetId()}");
                    waitForMouseUp = true;
                }
            }

            if (Input.GetMouseUp(MouseButton.Right))
            {
                waitForMouseUp = false;
            }
        }
    }
}
