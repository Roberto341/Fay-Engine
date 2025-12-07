using System;
using System.Collections.Generic;
using System.Linq;
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
        public Vector2 Add(Vector2 other)
        {
            X += other.X;
            Y += other.Y;

            return this;
        }

        public Vector2 Sub(Vector2 other)
        {
            X -= other.X;
            Y -= other.Y;

            return this;
        }

        public Vector2 Mul(Vector2 other)
        {
            X *= other.X;
            Y *= other.Y;

            return this;
        }

        public Vector2 Div(Vector2 other)
        {
            X /= other.X;   
            Y /= other.Y;

            return this;
        }
    }
    public struct Vector3
    {
        public float X, Y, Z;
        public static Vector3 Zero => new Vector3(0, 0, 0);
        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector3 Add(Vector3 other)
        {
            X += other.X;  
            Y += other.Y;   
            Z += other.Z;

            return this;
        }

        public Vector3 Sub(Vector3 other)
        {
            X -= other.X;
            Y -= other.Y;
            Z -= other.Z;

            return this;
        }
        public Vector3 Mul(Vector3 other)
        {
            X *= other.X;
            Y *= other.Y;
            Z *= other.Z;

            return this;
        }
        public Vector3 Div(Vector3 other)
        {
            X /= other.X;
            Y /= other.Y;
            Z /= other.Z;

            return this;
        }
        public Vector3 Normalize()
        {
            float len = Length();
            if (len == 0) return new Vector3(0, 0, 0);
            return new Vector3(X / len, Y / len, Z / len);
        }
        public float Dot(Vector3 other)
        {
            return X * other.X + Y * other.Y + Z * other.Z;
        }
        public float Length()
        {
            return (float)Math.Sqrt(X * X + Y * Y + Z * Z);
        }

        public Vector3 Cross(Vector3 other)
        {
            return new Vector3(
                 Y * other.Z - Z * other.Y,
                 Z * other.X - X * other.Z,
                 X * other.Y - Y * other.X
            );
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

        public Vector4 Add(Vector4 other)
        {
            X += other.X;
            Y += other.Y;
            Z += other.Z;
            W += other.W; 

            return this;
        }

        public Vector4 Sub(Vector4 other)
        {
            X -= other.X;
            Y -= other.Y;
            Z -= other.Z;
            W -= other.W;

            return this;
        }
        public Vector4 Mul(Vector4 other)
        {
            X *= other.X;
            Y *= other.Y;
            Z *= other.Z;
            W *= other.W;

            return this;
        }
        public Vector4 Div(Vector4 other)
        {
            X /= other.X;
            Y /= other.Y;
            Z /= other.Z;
            W /= other.W;

            return this;
        }
    }
}
