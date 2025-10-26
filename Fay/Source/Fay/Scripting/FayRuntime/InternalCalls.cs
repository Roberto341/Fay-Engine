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
        // Postion stuff
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void InternalCalls_SetEntityPosition(object entity, float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Vector3 InternalCalls_GetEntityPosition(uint entityID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void InternalCalls_SetEntityCollision(int entityID, bool condition);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool InternalCalls_GetEntityCollision(int entityID);
        // Misc transform component 
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern TransformComponent InternalCalls_GetTransformComponent(Entity entity);

        // Entity stuff
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool InternalCalls_Entity_HasComponent(object entity, Type componentType);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern int InternalCalls_GetEntityId(object entity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint InternalCalls_GetSelectedEntity();
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void InternalCalls_SetEntityId(object entity, uint id);

        // Sprite 
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Vector3 InternalCalls_Sprite_GetPosition(IntPtr spritePtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void InternalCalls_Sprite_SetPosition(IntPtr spitePtr, Vector3 pos);
        // Implement in c++
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void InternalCalls_Sprite_SetCollision(IntPtr entity, bool condition);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool InternalCalls_Sprite_GetCollision(IntPtr entity);
        // Cube
        [MethodImpl (MethodImplOptions.InternalCall)]
        internal static extern Vector3 InternalCalls_Cube_GetPosition(IntPtr cubePtr);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void InternalCalls_Cube_SetPosition(IntPtr cubePtr, Vector3 pos);

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
