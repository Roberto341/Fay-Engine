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
        public float GetSpeed()
        {
            return InternalCalls.InternalCalls_Entity_GetSpeed();
        }
        public bool CheckCollision()
        {
            return InternalCalls.InternalCalls_Entity_CheckCollision(this);
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

                if(CheckCollision())
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
}
