using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace FayRuntime
{
    public struct Vector2
    {
        public float X, Y;

        public Vector2(float x, float y)
        {
            X = x;
            Y = y;
        }
    }
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }
    }
    public struct Vector4
    {
        public float X, Y, Z, W;

        public Vector4(float x, float y, float z, float w)
        {
            X = x;
            Y = y;
            Z = z;
            W = w;
        }
    }
    public enum SceneType
    {
        None = 0,
        Scene2D = 1,
        Scene3D = 2
    }
    public class Entity
    {
        internal uint _entityID;

        public void SetPosition(Vector3 position)
        {
            InternalCalls.InternalCalls_SetEntityPosition(this, position.X, position.Y, position.Z);
        }

        public Vector3 GetPosition()
        {
            return InternalCalls.InternalCalls_GetEntityPosition((uint)_entityID);
        }
        public void SetEntityId(uint id)
        {
            InternalCalls.InternalCalls_SetEntityId(this, id);
        }
        public int GetEnityId()
        {
            return InternalCalls.InternalCalls_GetEntityId(this);
        }
        public static Entity GetSelected()
        {
            var entity = new Entity();
            entity.SetEntityId(InternalCalls.InternalCalls_GetSelectedEntity());
            return entity;
        }
        public bool HasComponent(Type componentType)
        {
            //bool hasTransform = InternalCalls.InternalCalls_Entity_HasComponent(entity, typeof(FayRuntime.TransformComponent));
            return InternalCalls.InternalCalls_Entity_HasComponent(this, componentType);
        }
        public void Move(float speed)
        {
            //if (Input.GetKey(KeyCode.W) && entity.HasComponent(typeof(FayRuntime.TransformComponent)))
            //{
            //    Vector3 currentPos = entity.GetPosition();      // Get current position
            //    float newY = currentPos.Y + 1.0f;               // Increase Y by 1
            //    entity.SetPosition(new Vector3(currentPos.X, newY, currentPos.Z));  // Set new position
            //}
            if(Input.GetKey(KeyCode.W) && this.HasComponent(typeof(FayRuntime.TransformComponent)))
            {
                Vector3 currentPos = this.GetPosition();
                float newY = currentPos.Y + speed;
                this.SetPosition(new Vector3(currentPos.X, newY, currentPos.Z));
            }
            if (Input.GetKey(KeyCode.S) && this.HasComponent(typeof(FayRuntime.TransformComponent)))
            {
                Vector3 currentPos = this.GetPosition();
                float newY = currentPos.Y - speed;
                this.SetPosition(new Vector3(currentPos.X, newY, currentPos.Z));
            }
            if (Input.GetKey(KeyCode.A) && this.HasComponent(typeof(FayRuntime.TransformComponent)))
            {
                Vector3 currentPos = this.GetPosition();
                float newX = currentPos.X - speed;
                this.SetPosition(new Vector3(newX, currentPos.Y, currentPos.Z));
            }
            if (Input.GetKey(KeyCode.D) && this.HasComponent(typeof(FayRuntime.TransformComponent)))
            {
                Vector3 currentPos = this.GetPosition();
                float newX = currentPos.X + speed;
                this.SetPosition(new Vector3(newX, currentPos.Y, currentPos.Z));
            }
        }
        public Entity() { }
        public Entity(uint id)
        {
            _entityID = id;
        }
    }
    public class TransformComponent
    {
        public Vector3 position;
        public Vector3 translation;
        public Vector3 rotation;
        public Vector3 scale;
    }
    public class Sprite
    {
        private IntPtr nativePtr; // Holds the native Sprite* pointer
        private bool hasColision;
        public Sprite(IntPtr ptr)
        {
            nativePtr = ptr;
        }

        public Vector3 GetPosition()
        {
            return InternalCalls.InternalCalls_Sprite_GetPosition(nativePtr);
        }

        public void SetPosition(Vector3 position)
        {
            InternalCalls.InternalCalls_Sprite_SetPosition(nativePtr, position);
        }

        //public void SetCollision(IntPtr entity, bool condition)
        //{

        //}
    }

    public class Cube
    {
        private IntPtr nativePtr; // Native pointer that points to the actual Cube* pointer must be void* in c++

        public Cube(IntPtr ptr)
        {
            nativePtr = ptr;
        }

        public Vector3 GetPosition()
        {
            return InternalCalls.InternalCalls_Cube_GetPosition(nativePtr);
        }

        public void SetPosition(Vector3 position)
        {
            InternalCalls.InternalCalls_Cube_SetPosition(nativePtr, position);
        }
    }

    public class SpriteComponent
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
    public class CollisionSpriteComponent
    {
        public Vector3 position;
        public Vector2 size;
    }
}
