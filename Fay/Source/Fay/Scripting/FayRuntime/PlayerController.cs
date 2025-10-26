using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
namespace FayRuntime
{
    public class PlayerController
    {
        //public float speed = 1f;
        public static Entity entity = new Entity();
        public void OnStart()
        {

        }

        public void OnUpdate()
        {
            
            //    var transform = InternalCalls.InternalCalls_GetTransformComponent(entity);

            //    transform.position.X += 1.0f;

            //    InternalCalls.InternalCalls_SetEntityPosition((uint)entity.GetEnityId(), transform.position.X, transform.position.Y, transform.position.Z);
        }
    }
}
