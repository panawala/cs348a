#include "utils.h"

Vec3f vector3dToVec3f(Vector3d vec)
{
    return Vec3f(vec[0],vec[1],vec[2]);
}

Vector3d vec3fToVector3d(Vec3f vec)
{
    return Vector3d(vec[0],vec[1],vec[2]);
}
