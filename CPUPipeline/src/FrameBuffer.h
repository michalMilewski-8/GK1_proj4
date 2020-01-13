#pragma once
#include <stdint.h>
#include "Shader.hpp"

#undef RGB
#define RGB(r, g, b) ( ((r) & 0xff) + (((g) & 0xff)<<8) + ( ( (b) & 0xff) <<16) + (0xff<<24) )
#define RGBA(r, g, b, a) ( (r) + (g<<8) + (b<<16) + (a<<24) )
#define RED(color) ((color) & 0xff)
#define GREEN(color) (((color) >> 8) & 0xff)
#define BLUE(color) (((color) >> 16) & 0xff)
#define ALPHA(color) (((color) >> 24) & 0xff)

class FrameBuffer
{
private:
	int m_width;
	int m_height;
	const int m_bytesPerPixel = 4;

	uint8_t *m_color_buffer = nullptr;
	float* m_depth_buffor = nullptr;

	unsigned int tex = 0;

	Ref<Shader> shader;

public:
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
	void DrawLine(int x0, int y0,float z0, int x1, int y1,float z1, int color);
	void DrawRect(int x0, int y0, int x1, int y1, int color);

	void FillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color);
	void FillTriangle(int x0, int y0,float z0, int x1, int y1,float z1, int x2, int y2,float z2, int color);

};

