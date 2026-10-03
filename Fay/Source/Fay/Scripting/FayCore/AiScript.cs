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
        private static bool using3D = false;
        private static Dictionary<int, Vector3> directions = new Dictionary<int, Vector3>();
        private static Random rng = new Random();
        private static float speed = using3D ? 0.01f : 2.0f;
        private static Node node;
        private static void MoveAi(Entity entity)
        {
            int id = entity.GetId();
            if(!directions.TryGetValue(id, out Vector3 dir))
            {
                Vector3[] dirs = new Vector3[]
                {
                    new Vector3(1, 0, 0),
                    new Vector3(-1, 0, 0),
                    new Vector3(0, 1, 0),
                    new Vector3(0, -1, 0)
                };

                dir = dirs[rng.Next(dirs.Length)];
                directions[id] = dir;
            }
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

                directions[id] = dir;
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

                directions[id] = dir;
            }

        }
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
                bool hasAiTag = entity.HasTag("Ai");

                if (hasAiTag)
                {
                    MoveAi(entity);
                }
            }

        }
    }
}
