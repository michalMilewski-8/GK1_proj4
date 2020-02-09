#include <stdio.h>
#include <glad/glad.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include "FrameBuffer.h"
#include "Camera.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include "../Figure.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../Cone.h"
#include "../Cube.h"
#include "../Sphere.h"
#include "../Cylinder.h"
#include "../Helper.h"
#include "bitmap_image.hpp"

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
#define EPS 0.0001

glm::vec3 cameraPos, cameraFront, cameraUp, lookAt;
glm::vec3 def_cameraPos, def_cameraFront, def_cameraUp, def_lookAt;
float fov = 30.f;
glm::vec2 mousePosOld;
glm::vec2 angle = glm::vec2(-M_PI / 2, 0);
glm::vec2 scroolPosOld;
Camera* camera = nullptr;
bool show_demo_window = false;
bool show_figures_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
int active_camera_index = -1;
int current_width = DEFAULT_WIDTH;
int current_height = DEFAULT_HEIGHT;
bool paint_triangles = true;
bool draw_edges = true;
bool draw_color = true;
bool perspective_correction = false;
bool backface_culling = true;
bool z_bufferng = true;
Camera* cam;
FrameBuffer* fram;
bitmap_image texture;
bitmap_image normal_texture;


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos.z -= 0.01;
		lookAt.z -= 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos.z += 0.01;
		lookAt.z += 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos.x += 0.01;
		lookAt.x += 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos.x -= 0.01;
		lookAt.x -= 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		cameraPos.y -= 0.01;
		lookAt.y -= 0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		cameraPos.y += 0.01;
		lookAt.y += 0.01;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec2 mousePos = { xpos,ypos };
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glm::vec2 diff = mousePos - mousePosOld;
		diff.x /= 100;
		diff.y /= 100;
		angle += diff;
		if (angle.y > M_PI_2) angle.y = M_PI_2-EPS;
		if (angle.y < -M_PI_2) angle.y = -M_PI_2+EPS;
		cameraPos.x = lookAt.x + std::cos(angle.y) * std::cos(angle.x);
		cameraPos.z = lookAt.z + -std::cos(angle.y) * std::sin(angle.x);
		cameraPos.y = lookAt.y + std::sin(-angle.y);
		auto tmp = glm::normalize(lookAt - cameraPos );
		//if (tmp.z * cameraFront.z < 0) cameraUp *= -1;
	//	if (tmp.x * cameraFront.x < 0) cameraUp *= -1;
		//if (tmp.y * cameraFront.y < 0) cameraUp *= -1;
		cameraFront = tmp ;
	}

	mousePosOld = mousePos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 0.0f || yoffset >= 0)
		fov += yoffset / 10;
}

void timeMeasurement(GLFWwindow* win, double& deltaTime, double& currentTime)
{
	static int nbFrames = 0;
	static double lastTime = 0.0;
	static double lastFrame = 0.0;
	currentTime = glfwGetTime();
	nbFrames++;
	deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;

	if (currentTime - lastTime >= 1.0) {
		char title[256];
		title[255] = '\0';

		snprintf(title, 255, "CPU Render Pipeline - [FPS: %3.2f][%3.2f ms]", (float)nbFrames, 1000.f / (float)nbFrames);

		glfwSetWindowTitle(win, title);

		nbFrames = 0;
		lastTime += 1.0;
	}
}

void ListFigures(std::vector<Figure*>* figures) {
	int counter = 0;

	Figure* to_delete = nullptr;
	for (auto fig : *figures) {
		fig->ModMenu(counter, to_delete);
		counter++;
	}
	if (to_delete != nullptr) {
		auto it = std::find(figures->begin(), figures->end(), to_delete);
		figures->erase(it);
		//delete to_delete;
	}
}

void ListCameras(std::vector<Camera*>* cameras) {
	char buff[500];
	int cam_to_delete = -1;
	for (int i = 0; i < cameras->size(); i++) {
		sprintf_s(buff, 500, "Camera #%d", i);
		if (ImGui::TreeNode(buff)) {
			if (ImGui::Button("Activate")) active_camera_index = i;
			if (ImGui::Button("Delete")) cam_to_delete = i;
			ImGui::TreePop();
			ImGui::Separator();
		}
	}
	if (cam_to_delete > 0) {
		//delete (*cameras)[cam_to_delete];
		cameras->erase(cameras->begin() + cam_to_delete);
		if (active_camera_index == cam_to_delete) active_camera_index = 0;
	}
}

void ListLights(std::vector<Light*>* lights) {
	char buff[500];
	int to_delete = -1;
	for (int i = 0; i < lights->size(); i++) {
		sprintf_s(buff, 500, "Light #%d", i);
		if (ImGui::TreeNode(buff)) {
			Light* li = (*lights)[i];
			if (ImGui::Button("Delete")) to_delete = i;
			ImGui::Separator();
			ImGui::Text("Position");
			ImGui::SliderFloat("X position",&(li->position.x),-5.0f,5.0f);
			ImGui::SliderFloat("Y position",&(li->position.y),-5.0f,5.0f);
			ImGui::SliderFloat("Z position",&(li->position.z),-5.0f,5.0f);
			ImGui::Separator();
			ImGui::Text("Attenuation");
			ImGui::SliderFloat("Ac", &(li->Ac), 0.0f, 1.0f);
			ImGui::SliderFloat("Al", &(li->Al), 0.0f, 1.0f);
			ImGui::SliderFloat("Aq", &(li->Aq), 0.0f, 1.0f);
			ImGui::Separator();
			ImGui::Text("Colors");
			float colord[3];
			float colors[3];
			colord[0] = li->Id.x;
			colord[1] = li->Id.y;
			colord[2] = li->Id.z;
			colors[0] = li->Is.x;
			colors[1] = li->Is.y;
			colors[2] = li->Is.z;
			ImGui::ColorEdit3("Id", colord);
			li->Id = { colord[0],colord[1],colord[2] };
			ImGui::ColorEdit3("Is", colors);
			li->Is = { colors[0],colors[1],colors[2] };
			ImGui::TreePop();
			ImGui::Separator();
		}
	}
	if (to_delete >= 0) {
		//delete (*cameras)[cam_to_delete];
		lights->erase(lights->begin() + to_delete);
	}
}



void CreateMenu(std::vector<Figure*>* figures, std::vector<Camera*>* cameras, std::vector<Light*>* lights) {
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Main Menu");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("In this menu you can list, select, move, add figures, cameras and lights");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Paint triangles", &paint_triangles);
		ImGui::Checkbox("Correct perspective", &perspective_correction);
		ImGui::Checkbox("Backface culling", &backface_culling);
		ImGui::Checkbox("Z-buffering", &z_bufferng);
		ImGui::Checkbox("Draw Lights", &draw_color);
		ImGui::Checkbox("Draw Edges", &draw_edges);
		ImGui::Text("Ambient Light");
		ImGui::SliderFloat("ka", &(fram->ka), 0.0f, 1.0f);
		float colora[3];
		colora[0] = fram->Ia.x;
		colora[1] = fram->Ia.y;
		colora[2] = fram->Ia.z;
		ImGui::ColorEdit3("Ia", colora);
		fram->Ia = { colora[0],colora[1],colora[2] };
		if (ImGui::CollapsingHeader("List Figures")) {
			ListFigures(figures);
		}
		if (ImGui::CollapsingHeader("List Cameras")) {
			ListCameras(cameras);
		}
		if (ImGui::CollapsingHeader("List Lights")) {
			ListLights(lights);
		}

		if (ImGui::Button("Add Cube"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Cube* tmp = new Cube();
			tmp->center = Helper::createTranslationMatrix(-0.5f, -0.5f, -0.5f);
			tmp->scale = Helper::createScaleMatrix(0.2f, 0.2f, 0.2f);
			tmp->textura = &texture;
			tmp->normal_textura = &normal_texture;
			figures->push_back(tmp);
		}
		if (ImGui::Button("Add Sphere"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Sphere* tmp = new Sphere();
			tmp->scale = Helper::createScaleMatrix(0.2f, 0.2f, 0.2f);
			tmp->textura = &texture;
			tmp->normal_textura = &normal_texture;
			figures->push_back(tmp);

		}
		if (ImGui::Button("Add Cone"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Cone* tmp = new Cone();
			tmp->scale = Helper::createScaleMatrix(0.2f, 0.2f, 0.2f);
			tmp->textura = &texture;
			tmp->normal_textura = &normal_texture;
			figures->push_back(tmp);
		}
		if (ImGui::Button("Add Cylinder"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Cylinder* tmp = new Cylinder();
			tmp->scale = Helper::createScaleMatrix(0.2f, 0.2f, 0.2f);
			tmp->textura = &texture;
			tmp->normal_textura = &normal_texture;
			figures->push_back(tmp);
		}

		if (ImGui::Button("Add Camera"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Camera* cam = new Camera(def_cameraPos, def_cameraFront, def_cameraUp);
			cam->SetViewport(0, 0, current_width, current_height);
			cam->SetPerspective(fov, ((float)current_height / (float)current_width), 0.1, 100);
			cameras->push_back(cam);
		}

		if (ImGui::Button("Add Light"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Light* light = new Light();
			lights->push_back(light);
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

}


void window_size_callback(GLFWwindow* window, int width, int height) {
	current_width = width;
	current_height = height;
	fram->Resize(width, height);
	fram->InitGL();
	cam->SetViewport(0, 0, current_width, current_height);
}

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	texture = bitmap_image("texture.bmp");
	normal_texture = bitmap_image("cheek-edge.bmp");

	if (!texture || ! normal_texture) {
		int i;
		i = 0;;
	}

	//createMenu();

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "CPU Render Pipeline", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to create window!\n");
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0); // 0 - Disable vsync

	// Initialize OpenGL loader
	if (gladLoadGL() == 0)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	// Set callback for resizing window
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	FrameBuffer fb(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	fram = &fb;
	fb.InitGL();


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	const char* glsl_version = "#version 130";
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	std::vector<Camera*> cameras;

	std::vector<Figure*> figures;
	std::vector<Light*> lights;

	Cube cub = Cube();
	Cube cub2 = Cube();

	Sphere sp = Sphere();
	Cylinder cy = Cylinder();
	Cone co = Cone();

	cub.center = Helper::createTranslationMatrix(-0.5f, -0.5f, -0.5f);
	cub.scale = Helper::createScaleMatrix(0.2f, 0.2f, 0.2f);
	cub.translate = Helper::createTranslationMatrix(1.0f, 0, 0);
	cub2.translate = Helper::createTranslationMatrix(-1.0f, 0, 0);
	cub2.center = Helper::createTranslationMatrix(-0.5f, -0.5f, -0.5f);;
	cub2.scale = Helper::createScaleMatrix(0.2f, 0.2f, 0.2f);

	sp.scale = Helper::createScaleMatrix(0.1f, 0.1f, 0.1f);
	cy.scale = Helper::createScaleMatrix(0.1f, 0.1f, 0.1f);
	co.scale = Helper::createScaleMatrix(0.1f, 0.1f, 0.1f);

	cub2.translate = Helper::createTranslationMatrix(-3.0f, 0, 0);

	sp.textura = &texture;
	sp.normal_textura = &normal_texture;
	//figures.push_back(&cub);
	//figures.push_back(&cub2);
	figures.push_back(&sp);

	def_cameraPos = { 0,0,1 };
	def_cameraFront = { 0,0,-1 };
	def_lookAt = { 0,0,0 };
	def_cameraUp = { 0,1,0 };

	cameraPos = { 0,0,1 };
	cameraFront = { 0,0,-1 };
	lookAt = { 0,0,0 };
	cameraUp = { 0,1,0 };

	//TODO: initialize camera

	double deltaTime = 0.0;
	double currentTime = 0.0;

	cam = new Camera(cameraPos, cameraFront, cameraUp);
	cam->SetViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	cam->SetPerspective(fov, ((float)DEFAULT_HEIGHT / (float)DEFAULT_WIDTH), 0.1, 100);
	cameras.push_back(cam);
	active_camera_index = 0;

	Light* li = new Light();
	li->position = { 0.0f,0.0f,2.0f };
	li->Ac = 1;
	li->Al = 0.09f;
	li->Aq = 0.032f;
	li->Id = { 1,1,1 };
	li->Is = { 1,1,1 };

	lights.push_back(li);

	double alfa = 0.0;
	fb.lights = &lights;
	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		CreateMenu(&figures, &cameras, &lights);
		alfa += 0.03;
		glfwPollEvents();
		timeMeasurement(window, deltaTime, currentTime);
		processInput(window, deltaTime);

		fb.ClearColor(0.1f, 0.15f, 0.15f);

		cam->LookAt(cameraPos, cameraFront, cameraUp);
		cam->SetPerspective(fov, ((float)current_height / (float)current_width), 0.1, 5);
		cam = cameras[active_camera_index];
		cam->SetPosFrontUp(cameraPos, cameraFront, cameraUp);
		//lookAt = cameraPos - cameraFront;
		auto proj = cam->GetProjectionMatrix();
		auto view = cam->GetWorldMatrix();
		auto view_port = cam->GetViewPortMatrix();

		int color = RGB(0, 255, 0);
		for (auto fig : figures) {
			fig->Transform(proj, view, view_port);
			for (auto t : fig->triangles) {
				t.CalculatePointsAfterTransformation();
				t.DrawTriangle(backface_culling, paint_triangles, z_bufferng, perspective_correction,draw_edges,draw_color, fb, color, cam);
			}
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		fb.RenderGL();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	// Cleanup
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
