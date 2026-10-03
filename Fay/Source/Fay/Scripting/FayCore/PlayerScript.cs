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
        private static Node node;

        public static void OnStart()
        {
            node = new Node(1);
        }

        public static void OnUpdate()
        {
            if (node == null)
                return;

            foreach(Entity entity in node.GetChildren())
            {
                bool hasPlayerTag = entity.HasTag("Player");

                if(hasPlayerTag)
                {
                    entity.Move(0.1f, true);
                }
            }
        }
    }
}
