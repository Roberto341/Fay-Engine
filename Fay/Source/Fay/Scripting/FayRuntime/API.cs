using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace FayRuntime
{
    public enum SceneType
    {
        None = 0,
        Scene2D = 1,
        Scene3D = 2
    }
    public class Scene
    {

    }
    public class API
    {
        public void OpenSceneFile(string scene)
        {
            var fs = new FileStream(scene, FileMode.Open);
            var len = (int)fs.Length;
            var bits = new byte[len];
            fs.Read(bits, 0, len);
            // Dump 16 bytes per line
            for(int ix = 0; ix < len; ix += 16)
            {
                var cnt = Math.Min(16, len - ix);
                var line = new byte[cnt];
                Array.Copy(bits, ix, line, 0, cnt);

                // Write address + hex + ascii
                Console.Write("{0:X6}  ", ix);
                Console.Write(BitConverter.ToString(line));
                Console.Write("  ");
                // Convert non-ascii chars to dots
                for (int jx = 0; jx < cnt; ++jx)
                    if (line[jx] < 0x20 || line[jx] > 0x7f) line[jx] = (byte)'.';
                Console.WriteLine(Encoding.ASCII.GetString(line));
            }
        }

        public int GetObjectCount(string sceneName)
        {
            return InternalCalls.InternalCalls_Scene_GetChildCount(sceneName);
        }
    }
    public class Node
    {
       internal uint _nodeID;

        public Node() { }
        public Node(uint id)
        {
            _nodeID = id;
        }
        public int ChildCount
        {
            get
            {
                return InternalCalls.InternalCalls_Node_GetChildCount(this);
            }
        }

        public Entity GetChild(uint index)
        {
            uint childId = InternalCalls.InternalCalls_Node_GetChild(this, index);
            return new Entity(childId);
        }

        public Entity[] GetChildren()
        {
            Entity[] children = new Entity[ChildCount];

            for(uint i = 0; i < ChildCount; i++)
            {
                children[i] = GetChild(i);
            }
            return children;
        }
    }
    public class Entity
    {
        internal uint _entityID;

        public void SetPosition(Vector3 position)
        {
            InternalCalls.InternalCalls_Entity_SetPosition(this, position.X, position.Y, position.Z);
        }

        public Vector3 GetPosition()
        {
            return InternalCalls.InternalCalls_Entity_GetPosition((uint)_entityID);
        }
        public void SetId(uint id)
        {
            InternalCalls.InternalCalls_Entity_SetID(this, id);
        }
        public int GetId()
        {
            return InternalCalls.InternalCalls_Entity_GetID(this);
        }
        public float GetSpeed()
        {
            return InternalCalls.InternalCalls_Entity_GetSpeed();
        }
        public bool CheckCollision()
        {
            return InternalCalls.InternalCalls_Entity_CheckCollision(this);
        }
        // Used for getting the currently selected entity in the editor
        public static Entity GetSelected()
        {
            var entity = new Entity();
            entity.SetId(InternalCalls.InternalCalls_Entity_GetSelected());
            return entity;
        }
        public bool HasComponent(Type componentType)
        {
            return InternalCalls.InternalCalls_Entity_HasComponent(this, componentType);
        }
        public bool HasTag(string tag)
        {
            // returns true if given tag is apart of the entities tag list returns false if not
            return InternalCalls.InternalCalls_Entity_HasTag(this, tag);
        }
        public void Move(float speed, bool useZ)
        {
            Vector3 pos = this.GetPosition();
            Vector3 delta = Vector3.Zero;

            if (Input.GetKey(KeyCode.W))
                delta.Y += speed;
            if (Input.GetKey(KeyCode.S))
                delta.Y -= speed;
            if (Input.GetKey(KeyCode.A))
                delta.X -= speed;
            if (Input.GetKey(KeyCode.D))
                delta.X += speed;
            if(useZ)
            {
                if (Input.GetKey(KeyCode.Up))
                    delta.Z += speed;
                if (Input.GetKey(KeyCode.Down))
                    delta.Z -= speed;
            }

            // Try X movement
            if(delta.X != 0f)
            {
                Vector3 attemptX = new Vector3(pos.X + delta.X, pos.Y, pos.Z);
                this.SetPosition(attemptX);
                if (CheckCollision())
                {
                    // Block X movement only
                    this.SetPosition(pos);
                }
                else
                {
                    pos = attemptX;
                }
            }
            // Try Y movement
            if (delta.Y != 0f)
            {
                Vector3 attemptY = new Vector3(pos.X, pos.Y + delta.Y, pos.Z);
                this.SetPosition(attemptY);

                if (CheckCollision())
                {
                    // Block Y movement only
                    this.SetPosition(pos);
                }
                else
                {
                    pos = attemptY;
                }
            }
            // Try Z movement
            if (delta.Z != 0f)
            {
                Vector3 attemptZ = new Vector3(pos.X, pos.Y, pos.Z + delta.Z);
                this.SetPosition(attemptZ);

                if (CheckCollision())
                {
                    // Block Z movement only
                    this.SetPosition(pos);
                }
                else
                {
                    pos = attemptZ;
                }
            }
        }
        public Entity() { }
        public Entity(uint id)
        {
            _entityID = id;
        }
    }
    // Components
    public class TransformComponent
    {
        public Vector3 position;
        public Vector3 translation;
        public Vector3 rotation;
        public Vector3 scale;
    }
   
    public class SpriteComponent
    {
        public Vector3 position;
        public Vector3 scale;
    }
    public class CubeComponent
    {
        public Vector3 position;
        public Vector3 scale;
    }
    public class CameraComponent
    {
        public Vector3 position;
        public Vector3 rotation;
        public Vector3 scale;
    }
    public class CollisionComponent
    {
        public Vector3 position;
        public Vector3 size;
    }
    public class ScriptComponent
    {
    }
}
