using FayRuntime;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
namespace FayCore
{
    public class AiScript
    {
        private static Vector3 dir = Vector3.Zero;
        private static Random rng = new Random();
        private static float speed = 0.5f;
        private static Entity entity;
        private static void MoveAi(Entity entity)
        {
            if(dir.X == 0 && dir.Y == 0)
            {
                Vector3[] dirs = new Vector3[]
                {
                    new Vector3(1, 0, 0),
                    new Vector3(-1, 0, 0),
                    new Vector3(0, 1, 0),
                    new Vector3(0, -1, 0)
                };

                dir = dirs[rng.Next(dirs.Length)];
            }

            Vector3 pos = entity.GetPosition();
            Vector3 oldPos = pos;

            if(dir.X != 0)
            {
                Vector3 attempt = new Vector3(pos.X + dir.X * speed, pos.Y, pos.Z);
                entity.SetPosition(attempt);
                if (entity.CheckCollision())
                    entity.SetPosition(pos);
                else
                    pos = attempt;
            }

            if (dir.Y != 0)
            {
                Vector3 attempt = new Vector3(pos.X, pos.Y + dir.Y * speed, pos.Z);
                entity.SetPosition(attempt);
                if (entity.CheckCollision())
                    entity.SetPosition(pos);
                else
                    pos = attempt;
            }

            Vector3 newPos = entity.GetPosition();
            if(newPos.X == oldPos.X && newPos.Y == oldPos.Y)
            {
                Vector3[] dirs = new Vector3[]
                {
                    new Vector3( 1, 0, 0),
                    new Vector3(-1, 0, 0),
                    new Vector3( 0, 1, 0),
                    new Vector3( 0,-1, 0)
                };
                dir = dirs[rng.Next(dirs.Length)];
            }

        }
        public static void OnStart() 
        {
            entity = new Entity(3);
            bool hasScript = entity.HasComponent(typeof(FayRuntime.ScriptComponent));

            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine("Entity has ScriptComponent? " + hasScript);
            Console.ResetColor();
        }
        public static void OnUpdate() 
        {
            if (entity == null)
                return;

            MoveAi(entity); // ccauses the break as well as PlayerScript Move() see API.cs Move method 
        }
    }
}
