#include "FrameBuffer.h"

#include <string>
#include <memory>
#include <algorithm>
#include <glad/glad.h>
#include "shader.hpp"


FrameBuffer::FrameBuffer(int _w, int _h)
	:m_width(_w), m_height(_h)
{
	m_color_buffer = new uint8_t[m_width * m_height * m_bytesPerPixel];
	m_depth_buffor = new float[m_width * m_height];
	for (int i = 0; i < m_width * m_height; i++) {
		m_depth_buffor[i] = 2.0f;
	}
}

FrameBuffer::~FrameBuffer()
{
	if (m_color_buffer)
		delete[] m_color_buffer;
	if (m_depth_buffor)
		delete[] m_depth_buffor;
}

void FrameBuffer::InitGL()
{
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_color_buffer);

	shader = Shader::Create("shader", R"(
		#version 330 core

		out vec2 texCoord;
	
		void main()
		{
			float x = -1.0 + float((gl_VertexID & 1) << 2);
			float y = -1.0 + float((gl_VertexID & 2) << 1);
			texCoord.x = (x+1.0)*0.5;
			texCoord.y = (y+1.0)*0.5;
			gl_Position = vec4(x, y, 0, 1);
		})", R"(
		#version 330 core

		layout (location = 0) out vec4 color;
		uniform sampler2D myTexture;
		in vec2 texCoord;
	
		void main()
		{
			color = texture2D(myTexture, texCoord);
			//color = vec4(texCoord.x, texCoord.y, 0, 1);
		}
	)");
}

void FrameBuffer::Resize(int _w, int _h)
{
	if (m_color_buffer)
		delete[] m_color_buffer;
	if (m_depth_buffor)
		delete[] m_depth_buffor;
	m_width = _w;
	m_height = _h;
	m_color_buffer = new uint8_t[m_width * m_height * m_bytesPerPixel];
	m_depth_buffor = new float[m_width * m_height];
	for (int i = 0; i < m_width * m_height; i++) {
		m_depth_buffor[i] = 2.0f;
	}
}

void FrameBuffer::ClearColor(float red, float green, float blue)
{
	uint8_t r = (uint8_t)(std::clamp<float>(red, 0.f, 1.f) * 255);
	uint8_t g = (uint8_t)(std::clamp<float>(green, 0.f, 1.f) * 255);
	uint8_t b = (uint8_t)(std::clamp<float>(blue, 0.f, 1.f) * 255);

	for (auto i = 0; i < m_height * m_width * m_bytesPerPixel; i += m_bytesPerPixel)
	{
		m_color_buffer[i] = r;
		m_color_buffer[i + 1] = g;
		m_color_buffer[i + 2] = b;
		m_color_buffer[i + 3] = 255;
	}
}

void FrameBuffer::RenderGL()
{
	//if (m_depth_buffor)
	//	delete[] m_depth_buffor;
	//
	if (m_depth_buffor) {
		for (int i = 0; i < m_width * m_height; i++) {
			m_depth_buffor[i] = 2.0f;
		}
	}
	else {
		m_depth_buffor = new float[m_width * m_height];
		for (int i = 0; i < m_width * m_height; i++) {
			m_depth_buffor[i] = 2.0f;
		}
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_color_buffer);

	shader->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	shader->Unbind();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::Save(std::string file_name)
{
	glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_color_buffer);
}

void FrameBuffer::SetPixel(int x, int y, int color)
{
	//ASSERT(x >= 0 || y < 0 || x >= m_width || y >= m_height, "SetPixel out of range!")
	if (x < 0 || y < 0 || x >= m_width || y >= m_height)
		return;
	int idx = m_bytesPerPixel * (y * m_width + x);
	m_color_buffer[idx] = RED(color);
	m_color_buffer[idx + 1] = GREEN(color);
	m_color_buffer[idx + 2] = BLUE(color);
	m_color_buffer[idx + 3] = ALPHA(color);
}

void FrameBuffer::SetPixel(int x, int y, float z, int color)
{
	//ASSERT(x >= 0 || y < 0 || x >= m_width || y >= m_height, "SetPixel out of range!")
	if (x < 0 || y < 0 || x >= m_width || y >= m_height)
		return;

	int dep = y * m_width + x;
	int idx = m_bytesPerPixel * (y * m_width + x);
	//z *= -1;
	if (z < m_depth_buffor[dep]) {
		m_depth_buffor[dep] = z;
		m_color_buffer[idx] = RED(color);
		m_color_buffer[idx + 1] = GREEN(color);
		m_color_buffer[idx + 2] = BLUE(color);
		m_color_buffer[idx + 3] = ALPHA(color);
	}
}

int FrameBuffer::GetPixel(int x, int y)
{
	//ASSERT(x < 0 || y < 0 || x >= m_width || y >= m_height, "GetPixel out of range!")
	if (x < 0 || y < 0 || x >= m_width || y >= m_height)
		return 0;
	int idx = m_bytesPerPixel * (y * m_width + x);
	return RGBA(m_color_buffer[idx], m_color_buffer[idx + 1], m_color_buffer[idx + 2], m_color_buffer[idx + 3]);
}

float AproxZValue(int x0, int y0, float z0, int x1, int y1, float z1, int x2, int y2, float z2, int x, int y) {
	float div = ((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
	if (div != 0) {
		float w0 = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2)) / div;
		float w1 = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2)) / div;
		float w2 = 1 - w0 - w1;
		return w0 * z0 + w1 * z1 + w2 * z2;
	}
	return z2;
}

float AproxZValue(float z0, float z1, float z2, float w0, float w1, float w2) {
	/*if (w1 == w2 == w0 == 0.0f)
		return 10.0f;*/
	return w0 * z0 + w1 * z1 + w2 * z2;
}
void AproxwVals(int x0, int y0, int x1, int y1, int x2, int y2, int x, int y, float& w0, float& w1, float& w2) {
	float div = ((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
	if (div != 0) {
		w0 = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2)) / div;
		w1 = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2)) / div;
		w2 = 1 - w0 - w1;
	}
	else
	{
		w0 = 0;
		w1 = 0;
		w2 = 0;
	}
}
glm::vec3 AproxNormValue(glm::vec3 N0, glm::vec3 N1, glm::vec3 N2, float w0, float w1, float w2) {
	return w0 * N0 + w1 * N1 + w2 * N2;
}

glm::vec3 AproxNormValue(int x0, int y0, glm::vec3 N0, int x1, int y1, glm::vec3 N1, int x2, int y2, glm::vec3 N2, int x, int y) {
	float div = ((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
	if (div != 0) {
		float w0 = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2)) / div;
		float w1 = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2)) / div;
		float w2 = 1 - w0 - w1;
		return w0 * N0 + w1 * N1 + w2 * N2;
	}
	/*else if (x0 == x2) {
		if (x2 == x1) {
			return { 0,0,0 };
		}
		else {
			float len = std::sqrt(((y1 - y2)* (y1 - y2)+ (x1 - x2)* (x1 - x2)));
			return (N2 - N1 )/ len;
		}
	}
	else {
		if (x2 == x1) {
			float len = std::sqrt(((y0 - y2) * (y0 - y2) + (x0 - x2) * (x0 - x2)));
			return (N2 - N0) / len;
		}
		else {
			return { 0,0,0 };
		}
d	}*/
	return { 0,0,0 };
}



void FrameBuffer::DrawLine(int x0, int y0, int x1, int y1, int color)
{
	int dx = abs(x1 - x0);
	int dy = -abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;  /* error value e_xy */
	while (true) {
		if (x0 == x1 && y0 == y1) break;
		SetPixel(x0, y0, color);
		int e2 = 2 * err;
		if (e2 >= dy) {
			err += dy; /* e_xy+e_x > 0 */
			x0 += sx;
		}
		if (e2 <= dx) /* e_xy+e_y < 0 */
		{
			err += dx;
			y0 += sy;
		}
	}
}

void FrameBuffer::DrawLine(int x0, int y0, float z0, int x1, int y1, float z1, int color)
{
	int dx = abs(x1 - x0);
	int dy = -abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	float len = std::sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
	float zval = z0;
	float zdiff = (z1 - z0) / len;
	int err = dx + dy;  /* error value e_xy */
	while (true) {
		if (x0 == x1 && y0 == y1) break;
		SetPixel(x0, y0, zval, color);
		zval += zdiff;
		int e2 = 2 * err;
		if (e2 >= dy) {
			err += dy; /* e_xy+e_x > 0 */
			x0 += sx;
		}
		if (e2 <= dx) /* e_xy+e_y < 0 */
		{
			err += dx;
			y0 += sy;
		}
	}
}


void FrameBuffer::DrawRect(int x0, int y0, int x1, int y1, int color)
{
	int dx = abs(x1 - x0);
	int dy = -abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	for (int x = x0; x != x1; x += sx) {
		for (int y = y0; y != y1; y += sy) {
			SetPixel(x, y, color);
		}
	}
}

void simpleSort(int x0, int y0, int x1, int y1, int x2, int y2, int& max_y,
	int& second_y, int& smallest_y, float& max_y_x, float& second_y_x,
	float& smallest_y_x, float& max_m, float& smallest_m, float& second_m) {
	if (y0 < y1) {
		if (y0 < y2) {
			smallest_y = y0;
			smallest_y_x = x0;
			if (y1 < y2) {
				second_y = y1;
				second_y_x = x1;
				max_y = y2;
				max_y_x = x2;
			}
			else {
				second_y = y2;
				second_y_x = x2;
				max_y = y1;
				max_y_x = x1;
			}
		}
		else {
			smallest_y = y2;
			smallest_y_x = x2;
			second_y = y0;
			second_y_x = x0;
			max_y = y1;
			max_y_x = x1;
		}
	}
	else if (y1 < y2) {
		smallest_y = y1;
		smallest_y_x = x1;
		if (y0 < y2) {
			second_y = y0;
			second_y_x = x0;
			max_y = y2;
			max_y_x = x2;
		}
		else {
			second_y = y2;
			second_y_x = x2;
			max_y = y0;
			max_y_x = x0;
		}
	}
	else {
		smallest_y = y2;
		smallest_y_x = x2;
		second_y = y1;
		second_y_x = x1;
		max_y = y0;
		max_y_x = x0;
	}



	if (smallest_y != second_y)
		smallest_m = (smallest_y_x - second_y_x) / (float)(smallest_y - second_y);
	if (second_y != max_y)
		second_m = (second_y_x - max_y_x) / (float)(second_y - max_y);
	if (smallest_y != max_y)
		max_m = (smallest_y_x - max_y_x) / (float)(smallest_y - max_y);

	max_y_x = smallest_y_x;
}

void FrameBuffer::FillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, int color) {
	int max_y = 0;
	int second_y = 0;
	int smallest_y = 0;
	float max_y_x = 0, second_y_x = 0, smallest_y_x = 0;
	float max_m = 0, smallest_m = 0, second_m = 0;

	simpleSort(x0, y0, x1, y1, x2, y2, max_y,
		second_y, smallest_y, max_y_x, second_y_x,
		smallest_y_x, max_m, smallest_m, second_m);

	int from, to;
	for (int i = smallest_y; i < second_y; i++) {
		//DrawLine((int)round(smallest_y_x), i, (int)round( max_y_x),i,color);
		from = (int)round(smallest_y_x);
		to = (int)round(max_y_x);
		if (to < from)
		{
			std::swap(to, from);
		}
		for (from; from <= to; from++) {
			SetPixel(from, i, color);
		}
		smallest_y_x = smallest_y_x + smallest_m;
		max_y_x = max_y_x + max_m;
	}
	for (int i = second_y; i < max_y; i++) {
		//DrawLine((int)round(second_y_x), i, (int)round(max_y_x), i, color);
		from = (int)round(second_y_x);
		to = (int)round(max_y_x);
		if (to < from)
		{
			std::swap(to, from);
		}
		for (from; from < to; from++) {
			SetPixel(from, i, color);
		}
		second_y_x = (second_y_x + second_m);
		max_y_x = (max_y_x + max_m);
	}
}

void FrameBuffer::FillTriangle(int x0, int y0, float z0, int x1, int y1, float z1,
	int x2, int y2, float z2, int color)
{
	int max_y = 0;
	int second_y = 0;
	int smallest_y = 0;
	float max_y_x = 0, second_y_x = 0, smallest_y_x = 0;
	float max_m = 0, smallest_m = 0, second_m = 0;

	simpleSort(x0, y0, x1, y1, x2, y2, max_y,
		second_y, smallest_y, max_y_x, second_y_x,
		smallest_y_x, max_m, smallest_m, second_m);
	int from, to;
	for (int i = smallest_y; i < second_y; i++) {
		from = (int)round(smallest_y_x);
		to = (int)round(max_y_x);
		if (to < from)
		{
			std::swap(to, from);
		}
		for (from; from <= to; from++) {
			SetPixel(from, i, AproxZValue(x0, y0, z0, x1, y1, z1, x2, y2, z2, from, i), color);
		}
		smallest_y_x = smallest_y_x + smallest_m;
		max_y_x = max_y_x + max_m;
	}
	for (int i = second_y; i < max_y; i++) {
		from = (int)round(second_y_x);
		to = (int)round(max_y_x);
		if (to < from)
		{
			std::swap(to, from);
		}
		for (from; from < to; from++) {
			SetPixel(from, i, AproxZValue(x0, y0, z0, x1, y1, z1, x2, y2, z2, from, i), color);
		}
		second_y_x = (second_y_x + second_m);
		max_y_x = (max_y_x + max_m);
	}
}

void FrameBuffer::FillTriangle(int x0, int y0, float z0, int x1, int y1, float z1,
	int x2, int y2, float z2, glm::vec3 N1, glm::vec3 N2, glm::vec3 N3,
	glm::vec3 A0, glm::vec3 A1, glm::vec3 A2, Figure* fig, Camera* cam,
	Triangle* tri)
{
	int max_y = 0;
	int second_y = 0;
	int smallest_y = 0;
	float max_y_x = 0, second_y_x = 0, smallest_y_x = 0;
	float max_m = 0, smallest_m = 0, second_m = 0;

	simpleSort(x0, y0, x1, y1, x2, y2, max_y,
		second_y, smallest_y, max_y_x, second_y_x,
		smallest_y_x, max_m, smallest_m, second_m);
	int from, to;
	int width = fig->textura->width() - 1;
	int height = fig->textura->height() - 1;
	float w0 = 0, w1 = 0, w2 = 0;
	for (int i = smallest_y; i < second_y; i++) {
		from = (int)round(smallest_y_x);
		to = (int)round(max_y_x);
		if (to < from)
		{
			std::swap(to, from);
		}

		for (from; from <= to; from++) {
			AproxwVals(x0, y0, x1, y1, x2, y2, from, i, w0, w1, w2);
			auto N = AproxNormValue(N1, N2, N3, w0, w1, w2);
			glm::vec3 C;
			glm::vec3 to = AproxNormValue(A0, A1, A2, w0, w1, w2);
			C = Ia * ka;
			if (fig->draw_texture) {
				glm::vec3 texture_coord = AproxNormValue({ tri->texture_cord[0],0 }, { tri->texture_cord[1],0 }, { tri->texture_cord[2],0 }, w0, w1, w2);
				texture_coord.x = texture_coord.x > 1 ? 1 : texture_coord.x;
				texture_coord.y = texture_coord.y > 1 ? 1 : texture_coord.y;
				texture_coord.x = texture_coord.x < 0 ? 0 : texture_coord.x;
				texture_coord.y = texture_coord.y < 0 ? 0 : texture_coord.y;
				glm::u8vec3 values;
				fig->textura->get_pixel((unsigned int)round(texture_coord.x * width), (unsigned int)round(texture_coord.y * height), values.r, values.g, values.b);
				glm::vec3 col = glm::vec3(values) / 255.0f;
				for (auto I : *lights) {
					C += I->CalculateValueLightVal(to, N, col, fig->ks, fig->n, cam->pos);
				}
			}
			else
				for (auto I : *lights) {
					C += I->CalculateValueLightVal(to, N, { fig->kd,fig->kd,fig->kd }, fig->ks, fig->n, cam->pos);
				}
			C *= 255;
			C.x = C.x > 255 ? 255 : C.x;
			C.y = C.y > 255 ? 255 : C.y;
			C.z = C.z > 255 ? 255 : C.z;
			C.x = C.x < 0 ? 0 : C.x;
			C.y = C.y < 0 ? 0 : C.y;
			C.z = C.z < 0 ? 0 : C.z;
			SetPixel(from, i, AproxZValue(z0, z1, z2, w0, w1, w2), RGB((int)C.x, (int)C.y, (int)C.z));
		}
		smallest_y_x = smallest_y_x + smallest_m;
		max_y_x = max_y_x + max_m;
	}
	for (int i = second_y; i < max_y; i++) {
		from = (int)round(second_y_x);
		to = (int)round(max_y_x);
		if (to < from)
		{
			std::swap(to, from);
		}
		for (from; from <= to; from++) {
			AproxwVals(x0, y0, x1, y1, x2, y2, from, i, w0, w1, w2);
			auto N = AproxNormValue(N1, N2, N3, w0, w1, w2);
			glm::vec3 C;
			glm::vec3 to = AproxNormValue(A0, A1, A2, w0, w1, w2);
			C = Ia * ka;
			if (fig->draw_texture) {
				glm::vec3 texture_coord = AproxNormValue({ tri->texture_cord[0],0 }, { tri->texture_cord[1],0 }, { tri->texture_cord[2],0 }, w0, w1, w2);
				texture_coord.x = texture_coord.x > 1 ? 1 : texture_coord.x;
				texture_coord.y = texture_coord.y > 1 ? 1 : texture_coord.y;
				texture_coord.x = texture_coord.x < 0 ? 0 : texture_coord.x;
				texture_coord.y = texture_coord.y < 0 ? 0 : texture_coord.y;
				glm::u8vec3 values;
				fig->textura->get_pixel((unsigned int)round(texture_coord.x * width), (unsigned int)round(texture_coord.y * height), values.r, values.g, values.b);
				glm::vec3 col = glm::vec3(values) / 255.0f;
				for (auto I : *lights) {
					C += I->CalculateValueLightVal(to, N, col, fig->ks, fig->n, cam->pos);
				}
			}
			else
				for (auto I : *lights) {
					C += I->CalculateValueLightVal(to, N, { fig->kd,fig->kd,fig->kd }, fig->ks, fig->n, cam->pos);
				}
			C *= 255;
			C.x = C.x > 255 ? 255 : C.x;
			C.y = C.y > 255 ? 255 : C.y;
			C.z = C.z > 255 ? 255 : C.z;
			C.x = C.x < 0 ? 0 : C.x;
			C.y = C.y < 0 ? 0 : C.y;
			C.z = C.z < 0 ? 0 : C.z;
			SetPixel(from, i, AproxZValue(z0, z1, z2, w0, w1, w2), RGB((int)C.x, (int)C.y, (int)C.z));
		}
		second_y_x = (second_y_x + second_m);
		max_y_x = (max_y_x + max_m);
	}
}

void FrameBuffer::FillTriangle(int x0, int y0, float z0, int x1, int y1, float z1, int x2, int y2, float z2, TriangleParams params, Figure* fig, Camera* cam, Triangle* tri)
{
	int max_y = 0;
	int second_y = 0;
	int smallest_y = 0;
	float max_y_x = 0, second_y_x = 0, smallest_y_x = 0;
	float max_m = 0, smallest_m = 0, second_m = 0;

	glm::vec3 N1 = params.N[0];
	glm::vec3 N2 = params.N[1];
	glm::vec3 N3 = params.N[2];

	glm::vec3 A0 = params.A[0];
	glm::vec3 A1 = params.A[1];
	glm::vec3 A2 = params.A[2];

	glm::vec3 B0 = params.B[0];
	glm::vec3 B1 = params.B[1];
	glm::vec3 B2 = params.B[2];

	glm::vec3 T0 = params.T[0];
	glm::vec3 T1 = params.T[1];
	glm::vec3 T2 = params.T[2];


	simpleSort(x0, y0, x1, y1, x2, y2, max_y,
		second_y, smallest_y, max_y_x, second_y_x,
		smallest_y_x, max_m, smallest_m, second_m);
	int from, to;
	int width = fig->textura->width() - 1;
	int height = fig->textura->height() - 1;
	int norm_width = fig->normal_textura->width() -1;
	int norm_height = fig->normal_textura->height() - 1;
	float w0 = 0, w1 = 0, w2 = 0;
	for (int i = smallest_y; i < second_y; i++) {
		from = (int)round(smallest_y_x);
		to = (int)round(max_y_x);
		if (to < from)
		{
			std::swap(to, from);
		}

		for (from; from <= to; from++) {
			AproxwVals(x0, y0, x1, y1, x2, y2, from, i, w0, w1, w2);
			auto N = AproxNormValue(N1, N2, N3, w0, w1, w2);
			auto B = AproxNormValue(B0, B1, B2, w0, w1, w2);
			auto T = AproxNormValue(T0, T1, T2, w0, w1, w2);
			glm::mat3 TBN(T, B, N);
			glm::vec3 texture_coord = AproxNormValue({ tri->texture_cord[0],0 }, { tri->texture_cord[1],0 }, { tri->texture_cord[2],0 }, w0, w1, w2);
			texture_coord.x = texture_coord.x > 1 ? 1 : texture_coord.x;
			texture_coord.y = texture_coord.y > 1 ? 1 : texture_coord.y;
			texture_coord.x = texture_coord.x < 0 ? 0 : texture_coord.x;
			texture_coord.y = texture_coord.y < 0 ? 0 : texture_coord.y;
			glm::vec3 M;
			glm::u8vec3  val;
			fig->normal_textura->get_pixel((unsigned int)round(texture_coord.x * norm_width), (unsigned int)round(texture_coord.y * norm_height), val.r, val.g, val.b);
			M = val;
			M /= 255;
			M *= 2;
			M -= 1;
			M = glm::normalize(M);
			N = glm::normalize(TBN * M);
			glm::vec3 C;
			glm::vec3 to = AproxNormValue(A0, A1, A2, w0, w1, w2);
			C = Ia * ka;
			if (fig->draw_texture) {
				glm::u8vec3 values;
				fig->textura->get_pixel((unsigned int)round(texture_coord.x * width), (unsigned int)round(texture_coord.y * height), values.r, values.g, values.b);
				glm::vec3 col = glm::vec3(values) / 255.0f;
				for (auto I : *lights) {
					C += I->CalculateValueLightVal(to, N, col, fig->ks, fig->n, cam->pos);
				}
			}
			else
				for (auto I : *lights) {
					C += I->CalculateValueLightVal(to, N, { fig->kd,fig->kd,fig->kd }, fig->ks, fig->n, cam->pos);
				}
			C *= 255;
			C.x = C.x > 255 ? 255 : C.x;
			C.y = C.y > 255 ? 255 : C.y;
			C.z = C.z > 255 ? 255 : C.z;
			C.x = C.x < 0 ? 0 : C.x;
			C.y = C.y < 0 ? 0 : C.y;
			C.z = C.z < 0 ? 0 : C.z;
			SetPixel(from, i, AproxZValue(z0, z1, z2, w0, w1, w2), RGB((int)C.x, (int)C.y, (int)C.z));
		}
		smallest_y_x = smallest_y_x + smallest_m;
		max_y_x = max_y_x + max_m;
	}
	for (int i = second_y; i < max_y; i++) {
		from = (int)round(second_y_x);
		to = (int)round(max_y_x);
		if (to < from)
		{
			std::swap(to, from);
		}
		for (from; from <= to; from++) {
			AproxwVals(x0, y0, x1, y1, x2, y2, from, i, w0, w1, w2);
			auto N = AproxNormValue(N1, N2, N3, w0, w1, w2);
			auto B = AproxNormValue(B0, B1, B2, w0, w1, w2);
			auto T = AproxNormValue(T0, T1, T2, w0, w1, w2);
			glm::mat3 TBN(T, B, N);
			glm::vec3 texture_coord = AproxNormValue({ tri->texture_cord[0],0 }, { tri->texture_cord[1],0 }, { tri->texture_cord[2],0 }, w0, w1, w2);
			texture_coord.x = texture_coord.x > 1 ? 1 : texture_coord.x;
			texture_coord.y = texture_coord.y > 1 ? 1 : texture_coord.y;
			texture_coord.x = texture_coord.x < 0 ? 0 : texture_coord.x;
			texture_coord.y = texture_coord.y < 0 ? 0 : texture_coord.y;
			glm::vec3 M;
			glm::u8vec3  val;
			fig->normal_textura->get_pixel((unsigned int)round(texture_coord.x * norm_width), (unsigned int)round(texture_coord.y * norm_height), val.r, val.g, val.b);
			M = val;
			M /= 255;
			M *= 2;
			M -= 1;
			M = glm::normalize(M);
			N = glm::normalize(TBN * M);
			glm::vec3 C;
			glm::vec3 to = AproxNormValue(A0, A1, A2, w0, w1, w2);
			C = Ia * ka;
			if (fig->draw_texture) {
				glm::u8vec3 values;
				fig->textura->get_pixel((unsigned int)round(texture_coord.x * width), (unsigned int)round(texture_coord.y * height), values.r, values.g, values.b);
				glm::vec3 col = glm::vec3(values) / 255.0f;
				for (auto I : *lights) {
					C += I->CalculateValueLightVal(to, N, col, fig->ks, fig->n, cam->pos);
				}
			}
			else
				for (auto I : *lights) {
					C += I->CalculateValueLightVal(to, N, { fig->kd,fig->kd,fig->kd }, fig->ks, fig->n, cam->pos);
				}
			C *= 255;
			C.x = C.x > 255 ? 255 : C.x;
			C.y = C.y > 255 ? 255 : C.y;
			C.z = C.z > 255 ? 255 : C.z;
			C.x = C.x < 0 ? 0 : C.x;
			C.y = C.y < 0 ? 0 : C.y;
			C.z = C.z < 0 ? 0 : C.z;
			SetPixel(from, i, AproxZValue(z0, z1, z2, w0, w1, w2), RGB((int)C.x, (int)C.y, (int)C.z));
		}
		second_y_x = (second_y_x + second_m);
		max_y_x = (max_y_x + max_m);
	}
}
