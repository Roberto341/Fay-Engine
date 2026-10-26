using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FayRuntime;
namespace FayCore
{
    public class PlayerMovement
    {
								private static Node node;
								private static float pSpeed = 25.0f;
        public static void OnStart() 
								{ 
										  node = new Node(1);		
								} 
        public static void OnUpdate() 
								{ 
												if(node == null)
															return;

												foreach(Entity entity in node.GetChildren())
												{
														bool hasPlayerTag = entity.HasTag("Player");

														if(hasPlayerTag)
														{
															entity.Move(pSpeed, false);
														}		
												}
									} 
    }
}
