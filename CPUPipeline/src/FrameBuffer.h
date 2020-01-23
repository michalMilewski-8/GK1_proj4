#pragma once
#include <stdint.h>
#include "Shader.hpp"
#include "../Light.h"
#include "../Helper.h"

#undef RGB
#define RGB(r, g, b) ( ((r) & 0xff) + (((g) & 0xff)<<8) + ( ( (b) & 0xff) <<16) + (0xff<<24) )
#define RGBA(r, g, b, a) ( (r) + (g<<8) + (b<<16) + (a<<24) )
#define RED(color) ((color) & 0xff)
#define GREEN(color) (((color) >> 8) & 0xff)
#define BLUE(color) (((color) >> 16) & 0xff)
#define ALPHA(color) (((color) >> 24) & 0xff)

class Light;
class Triangle;
class Figure;
class Camera;
struct TriangleParams;

class FrameBuffer
{
private:
	
	const int m_bytesPerPixel = 4;

	uint8_t* m_color_buffer = nullptr;
	float* m_depth_buffor = nullptr;



	unsigned int tex = 0;

	Ref<Shader> shader;

public:
	int m_width;
	int m_height;
	float ka = 0.2f;
	glm::vec3 Ia = { 1,1,1 };
	std::vector<Light*>* lights;
	FrameBuffer(int _w, int _h);
	~FrameBuffer();

	void InitGL();
	void Resize(int _w, int _h);
	void ClearColor(float red, float green, float blue);
	void RenderGL();
	void Save(std::string file_name);

	void SetPixel(int x, int y, int color);
	void SetPixel(int x, int y, float z, int color);
	int GetPixel(int x, int y);

	void DrawLine(int x0, int y0, int x1, int y1, int color);
	void DrawLine(int x0, int y0, float z0, int x1, int y1, float z1, int color);
	//void DrawLine(int x0, int y0,float z0, int x1, int y1,float z1, glm::vec3 N1,glm::vec3 N2, std::vector<Light> lights, float ka, glm::vec3 Ia);
	void DrawRect(int x0, int y0, int x1, int y1, int color);

	void FillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color);
	void FillTriangle(int x0, int y0, float z0, int x1, int y1, float z1, int x2,
		int y2, float z2, int color);
	void FillTriangle(int x0, int y0, float z0, int x1, int y1, float z1, int x2,
		int y2, float z2, glm::vec3 N1, glm::vec3 N2, glm::vec3 N3, glm::vec3 A0,
		glm::vec3 A1, glm::vec3 A2, Figure* fig, Camera* cam, Triangle* tri);

	void FillTriangle(int x0, int y0, float z0, int x1, int y1, float z1, int x2,
		int y2, float z2, TriangleParams params, Figure* fig, Camera* cam, Triangle* tri);

};

