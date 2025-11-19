using System;
using System.Collections.Generic;
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
        public void Move(float speed, bool useZ)
        {
            if (this.HasComponent(typeof(FayRuntime.TransformComponent)) && Input.GetKey(KeyCode.W))
            {
                Vector3 currentPos = this.GetPosition();
                float newY = currentPos.Y + speed;
                this.SetPosition(new Vector3(currentPos.X, newY, currentPos.Z));
            }
            if (this.HasComponent(typeof(FayRuntime.TransformComponent)) && Input.GetKey(KeyCode.S))
            {
                Vector3 currentPos = this.GetPosition();
                float newY = currentPos.Y - speed;
                this.SetPosition(new Vector3(currentPos.X, newY, currentPos.Z));
            }
            if (this.HasComponent(typeof(FayRuntime.TransformComponent)) && Input.GetKey(KeyCode.A))
            {
                Vector3 currentPos = this.GetPosition();
                float newX = currentPos.X - speed;
                this.SetPosition(new Vector3(newX, currentPos.Y, currentPos.Z));
            }

            if (this.HasComponent(typeof(FayRuntime.TransformComponent)) && Input.GetKey(KeyCode.D))
            {
                Vector3 currentPos = this.GetPosition();
                float newX = currentPos.X + speed;
                this.SetPosition(new Vector3(newX, currentPos.Y, currentPos.Z));
            }
            if (this.HasComponent(typeof(FayRuntime.TransformComponent)) && Input.GetKey(KeyCode.Up) && useZ)
            {
                Vector3 currentPos = this.GetPosition();
                float newZ = currentPos.Z + speed;
                this.SetPosition(new Vector3(currentPos.X, currentPos.Y, newZ));
            }

            if (this.HasComponent(typeof(FayRuntime.TransformComponent)) && Input.GetKey(KeyCode.Down) && useZ)
            {
                Vector3 currentPos = this.GetPosition();
                float newZ = currentPos.Z - speed;
                this.SetPosition(new Vector3(currentPos.X, currentPos.Y, newZ));
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
}
