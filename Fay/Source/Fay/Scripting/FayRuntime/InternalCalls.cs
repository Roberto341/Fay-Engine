using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace FayRuntime
{
    public static class InternalCalls
    {
        // Component
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern TransformComponent InternalCalls_Entity_GetTransformComponent(Entity entity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool InternalCalls_Entity_HasComponent(object entity, Type componentType);
        // Entity 
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void InternalCalls_Entity_SetPosition(object entity, float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Vector3 InternalCalls_Entity_GetPosition(uint entityID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void InternalCalls_Entity_SetCollision(int entityID, bool condition);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool InternalCalls_Entity_GetCollision(int entityID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern int InternalCalls_Entity_GetID(object entity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern int InternalCalls_Entity_SetID(object entity, uint id);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint InternalCalls_Entity_GetSelected();
        // Input handling
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool InternalCalls_Window_KeyPressed(int keyCode);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool InternalCalls_Window_KeyReleased(int keyCode);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool InternalCalls_Window_MouseDown(int mouseButton);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool InternalCalls_Window_MouseUp(int mouseButton);
        // Scene handling
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern SceneType InternalCalls_GetActiveScene(); // returns true if scene is currently loaded otherwise false
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void InternalCalls_SetActiveScene(); // Set's the current scene that has been loaded
        
    }
}
