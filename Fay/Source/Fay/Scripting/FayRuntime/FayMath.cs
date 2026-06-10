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
        private const float eps = 0.0001f;
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

        public static bool operator ==(Vector3 a, Vector3 b)
        {
            return Math.Abs(a.X - b.X) < eps &&
                Math.Abs(a.Y - b.Y) < eps &&
                Math.Abs(a.Z - b.Z) < eps;
        }
        public static bool operator !=(Vector3 a, Vector3 b)
        {
            return !(a == b);
        }

        public override bool Equals(object obj)
        {
            if (!(obj is Vector3))
                return false;

            return this == (Vector3)obj;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                int hx = (int)Math.Round(X / eps);
                int hy = (int)Math.Round(Y / eps);
                int hz = (int)Math.Round(Z / eps);

                int hash = 17;

                hash = hash * 23 + hx;
                hash = hash * 23 + hy;
                hash = hash * 23 + hz;
                return hash;
            }
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
