#include <games/rt.h>

#include <console.h>
#include <debug.h>
#include <memory.h>

#define SKY_COLOR { 0xFF, 0xBF, 0x00, 0xFF }

Vector3 add(Vector3 u, Vector3 v)
{
    Vector3 res = { u.x + v.x, u.y + v.y, u.z + v.z };
    return res;
}

Vector3 minus(Vector3 u, Vector3 v)
{
    Vector3 res = { u.x - v.x, u.y - v.y, u.z - v.z };
    return res;
}

Vector3 multiply(Vector3 u, Vector3 v)
{
    Vector3 res = { u.x * v.x, u.y * v.y, u.z * v.z };
    return res;
}

Vector3 multiply2(Vector3 u, float t)
{
    Vector3 res = { u.x * t, u.y * t, u.z * t };
    return res;
}

Vector3 divide(Vector3 u, float t)
{
    Vector3 res = { u.x / t, u.y / t, u.z / t };
    return res;
}

float dot(Vector3 u, Vector3 v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vector3 cross(Vector3 u, Vector3 v)
{
    Vector3 res = {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
    return res;
}

float length(Vector3 vec)
{
    return sqrt(dot(vec, vec));
}

Vector3 normalize(Vector3 vec)
{
    return divide(vec, length(vec));
}

Vector3 at(Ray ray, float t)
{
    return add(ray.origin, multiply2(ray.direction, t));
}

bool sphereIntersect(void* object, Vector3 origin, Vector3 direction, float tMin, Vector3* normal, float* t)
{
    Object* sphere = (Object*)object;
    SphereData* data = (SphereData*)sphere->data;
    Vector3 originToCenter = minus(origin, sphere->position);

    float a = dot(direction, direction);
    float b = 2 * dot(originToCenter, direction);
    float c = dot(originToCenter, originToCenter) - (data->radius * data->radius);

    float discriminant = (b * b) - (4.f * (a * c));

    if (discriminant < 0)
    {
        return false;
    }

    float t1 = (-b + sqrt(discriminant)) / (2 * a);
    float t2 = (-b - sqrt(discriminant)) / (2 * a);

    if (t1 > tMin) *t = t1;
    if (t2 < t1 && t2 > tMin) *t = t2;

    *normal = normalize(minus(add(origin, multiply2(direction, *t)), sphere->position));
    return *t > tMin;
}

Object* initSphere(Vector3 position, Color color, float radius)
{
    SphereData* data = (SphereData*)malloc(sizeof(SphereData));
    Object* object = (Object*)malloc(sizeof(Object));

    data->radius = radius;
    object->position = position;
    object->color = color;
    object->data = data;
    object->intersection = sphereIntersect;
    return object;
}

Color traceRay(Vector3 origin, Vector3 direction);
bool minObjectIntersect(Vector3 origin, Vector3 direction, float tMin, Vector3* normal, float* t, Object** object);

Object** objects = NULL;
int objectsLength = 0;

Vector3 lightPosition = { 5.f, 5.f, -1.f };
float lightIntensity = 0.45f;
float ambientIntensity = 0.15f;

void addObject(Object* object)
{
    static int length = 0;
    length++;
    Object** newArray = malloc(sizeof(Object) * length);
    if (!newArray)
    {
        debugPrintString("NOT ALLOCATED!\n");
		return;
    }

    if (objects != NULL)
    {
        for (int i = 0; i < length - 1; i++)
        {
            newArray[i] = objects[i];
        }
        free(objects);
    }
	
	newArray[length - 1] = object;

    objects = newArray;
    objectsLength = length;
}

void rtInit(void)
{
	addObject(initSphere((Vector3) { 0.f, 0.f, 5.f }, (Color) { 0xFF, 0xFF, 0xFF, 0xFF }, 1.f));
    addObject(initSphere((Vector3) { -0.5f, 2.f, 5.f }, (Color) { 0x00, 0xFF, 0x00, 0xFF }, 1.f));
    addObject(initSphere((Vector3) { 4.f, 1.f, 10.f }, (Color) { 0x00, 0xFF, 0xFF, 0xFF }, 2.f));
    addObject(initSphere((Vector3) { 0.f, -6001.f, 10.f }, (Color) {0x00, 0x00, 0xFF, 0xFF }, 6000.f));
	
	String rtName = stringNewFromOld("RT");
	
	consoleAddCommand(rtName, rtCommand);
}

Color colorMultiply(Color color, float t)
{
	if(t <= 0) return (Color){ 0, 0, 0, 0 };
	if(t > 1) t = 1;
	return (Color){ color.blue * t, color.green * t, color.red * t, 0xFF };
}

void rtCommand(void)
{
    const float aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;

    const float viewportHeight = 2.f;
    const float viewportWidth = viewportHeight * aspectRatio;
    const float focalLength = 1.f;

    Vector3 origin = { 0, 0, 0 };
	
	//Color* buffer = malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Color));
	
	//static uint16_t barWidth = SCREEN_WIDTH - 100;
	//static uint16_t barHeight = 50;
	//
	//uint16_t barX = SCREEN_WIDTH / 2 - barWidth / 2;
	//uint16_t barY = 500;
	//
	//static Color barColor = { .red = 0x7F, .green = 0x7F, .blue = 0x7F, .alpha = 0xFF };
	
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            Vector3 direction = { (x - SCREEN_WIDTH / 2) * viewportWidth / SCREEN_WIDTH, -(y - SCREEN_HEIGHT / 2) * viewportHeight / SCREEN_HEIGHT, focalLength };
            direction = normalize(direction);
            Color color = traceRay(origin, direction);
            vgaSetPixel(x, y, color);
			//buffer[y * SCREEN_WIDTH + x] = color;
        }
		//vgaDrawProgressBar(barX, barY, barWidth, barHeight, 5, (y * 100) / SCREEN_HEIGHT, barColor);
		vgaFlipBuffer();
    }
	
	//vgaDrawBuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, buffer);
}

Color traceRay(Vector3 origin, Vector3 direction)
{
    float t = 0.f;
    Object* object = NULL;
    Vector3 normal = { 0, 0, 0 };

    if (minObjectIntersect(origin, direction, 0.001f, &normal, &t, &object))
    {
        Vector3 intersection = add(origin, multiply2(direction, t));
		Vector3 lightDirection = normalize(minus(lightPosition, intersection));
        return colorMultiply(object->color, 
			(dot(normal, lightDirection) / (length(normal) * length(lightDirection))) * lightIntensity 
			+ ambientIntensity);
    }
    Color sky = SKY_COLOR;
    return sky;
}

bool minObjectIntersect(Vector3 origin, Vector3 direction, float tMin, Vector3* normal, float* t, Object** intersectedObject)
{
    float tTemp = 0.f;
    *t = POSITIVE_INFINITY;
    Vector3 normalTemp = { 0,0,0 };
    for (int i = 0; i < objectsLength; i++)
    {
        Object* object = *(objects + i);
        if (object->intersection(object, origin, direction, tMin, &normalTemp, &tTemp))
        {
            if (tTemp < *t && tTemp > tMin)
            {
                *t = tTemp;
                *normal = normalTemp;
                *intersectedObject = object;
            }
        }
    }

    return *t != POSITIVE_INFINITY;
}
