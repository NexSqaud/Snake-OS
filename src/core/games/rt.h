#include <graphics.h>
#include <math.h>
#include <stddef.h>

#ifndef __RT_H
#define __RT_H

typedef struct
{
    float x;
    float y;
    float z;
} Vector3;

typedef struct
{
    Vector3 origin;
    Vector3 direction;
} Ray;


typedef struct
{
    Vector3 position;
    Color color;
    void* data;

    bool(*intersection)(void*, Vector3, Vector3, float, Vector3*, float*);
} Object;

typedef struct
{
    float radius;
} SphereData;

Vector3 add(Vector3 u, Vector3 v);
Vector3 minus(Vector3 u, Vector3 v);
Vector3 multiply(Vector3 u, Vector3 v);
Vector3 multiply2(Vector3 u, float t);
Vector3 divide(Vector3 u, float t);
float dot(Vector3 u, Vector3 v);
Vector3 cross(Vector3 u, Vector3 v);
float length(Vector3 vec);
Vector3 normalize(Vector3 vec);
Vector3 at(Ray ray, float t);

void rtInit(void);
void rtCommand(void);

#endif