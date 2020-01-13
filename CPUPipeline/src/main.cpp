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

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
#define PI 3.14

glm::vec3 cameraPos, cameraFront, cameraUp;
glm::vec3 def_cameraPos, def_cameraFront, def_cameraUp;
float fov = 30.f;
glm::vec2 mousePosOld;
glm::vec2 angle = glm::vec2(0, 0);
glm::vec2 scroolPosOld;
Camera* camera = nullptr;
bool show_demo_window = false;
bool show_figures_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
int active_camera_index = -1;
int current_width = DEFAULT_WIDTH;
int current_height = DEFAULT_HEIGHT;
bool paint_triangles = true;
bool perspective_correction = false;
bool backface_culling = true;
bool z_bufferng = true;

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos.z -= 0.01;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos.z += 0.01;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos.x += 0.01;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos.x -= 0.01;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos.y -= 0.01;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos.y += 0.01;


	//TODO: Write camera input control here
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	//TODO: change camera viewport
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec2 mousePos = { xpos,ypos };

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glm::vec2 diff = mousePos - mousePosOld;
		diff.x /= 1000;
		diff.y /= 1000;
		angle += diff;
		cameraFront.x = std::cos(-angle.y) * std::cos(-angle.x);
		cameraFront.z = std::cos(-angle.y) * std::sin(-angle.x) - 1;
		cameraFront.y = std::sin(-angle.y);
	}

	mousePosOld = mousePos;
	//TODO: write mouse camera control
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//glm::vec2 scroolPos = { xoffset,yoffset };

	//glm::vec2 diff = scroolPos - scroolPosOld;
	if(fov >=0.0f || yoffset >=0)
	fov += yoffset / 10;
	//cameraUp.y += diff.y / 100;

	//scroolPosOld = scroolPos;
	//TODO: mouse scroll
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

glm::mat4 createRotationMatrix(float x_angle, float y_angle, float z_angle, Figure* fig) {
	glm::mat4 x_rotate = glm::mat4(1.0f);
	x_rotate[1][1] = std::cos(x_angle);
	x_rotate[2][1] = -std::sin(x_angle);
	x_rotate[1][2] = std::sin(x_angle);
	x_rotate[2][2] = std::cos(x_angle);

	glm::mat4 y_rotate = glm::mat4(1.0f);
	y_rotate[0][0] = std::cos(y_angle);
	y_rotate[2][0] = -std::sin(y_angle);
	y_rotate[0][2] = std::sin(y_angle);
	y_rotate[2][2] = std::cos(y_angle);

	glm::mat4 z_rotate = glm::mat4(1.0f);
	z_rotate[0][0] = std::cos(z_angle);
	z_rotate[1][0] = -std::sin(z_angle);
	z_rotate[0][1] = std::sin(z_angle);
	z_rotate[1][1] = std::cos(z_angle);
	fig->x_angle = x_angle;
	fig->y_angle = y_angle;
	fig->z_angle = z_angle;

	return x_rotate * y_rotate * z_rotate;
}

void getRotateValue(float& x, float& y, float& z, Figure* fig) {
	x = fig->x_angle;
	y = fig->y_angle;
	z = fig->z_angle;
}

glm::mat4 createTranslationMatrix(float x, float y, float z) {
	glm::mat4 translate = glm::mat4(1.0f);
	translate[3][0] = x;
	translate[3][1] = y;
	translate[3][2] = z;

	return translate;
}

void getTranslationValues(float& x, float& y, float& z, glm::mat4& translate) {
	x = translate[3][0];
	y = translate[3][1];
	z = translate[3][2];
}

glm::mat4 createScaleMatrix(float x, float y, float z) {
	glm::mat4 scale = glm::mat4(1.0f);
	scale[0][0] = x;
	scale[1][1] = y;
	scale[2][2] = z;

	return scale;
}

void getScaleValue(float& x, float& y, float& z, glm::mat4& scale) {
	x = scale[0][0];
	y = scale[1][1];
	z = scale[2][2];
}

void FigureModMenu(Figure* cube) {
	float x_trans, y_trans, z_trans;
	getTranslationValues(x_trans, y_trans, z_trans, cube->translate);
	ImGui::SliderFloat("Move X axis", &x_trans, -5.0f, 5.0f);
	ImGui::SliderFloat("Move Y axis", &y_trans, -5.0f, 5.0f);
	ImGui::SliderFloat("Move Z axis", &z_trans, -5.0f, 5.0f);
	cube->translate = createTranslationMatrix(x_trans, y_trans, z_trans);
	ImGui::Spacing();
	float x_scale, y_scale, z_scale;
	getScaleValue(x_scale, y_scale, z_scale, cube->scale);
	ImGui::SliderFloat("Scale X axis", &x_scale, 0.0f, 5.0f);
	ImGui::SliderFloat("Scale Y axis", &y_scale, 0.0f, 5.0f);
	ImGui::SliderFloat("Scale Z axis", &z_scale, 0.0f, 5.0f);
	cube->scale = createScaleMatrix(x_scale, y_scale, z_scale);
	ImGui::Spacing();
	float x_rot, y_rot, z_rot;
	getRotateValue(x_rot, y_rot, z_rot, cube);
	ImGui::SliderFloat("Rotate X axis", &x_rot, 0.0f, 6.5f);
	ImGui::SliderFloat("Rotate Y axis", &y_rot, 0.0f, 6.5f);
	ImGui::SliderFloat("Rotate Z axis", &z_rot, 0.0f, 6.5f);
	cube->rotate = createRotationMatrix(x_rot, y_rot, z_rot, cube);
	ImGui::Spacing();
}

void ModMenu(Cube* cube) {
	FigureModMenu(cube);
	float lengths[12];
	char buff[100];
	for (int i = 0; i < 12; i++) {
		lengths[i] = cube->edges_lengts[i];
		sprintf_s(buff, 100, "Length %d", i);
		ImGui::SliderFloat(buff, &lengths[i], 0.0f, 5.0f);
	}
	bool difference = false;
	int diff_index = 0;
	for (int i = 0; i < 12; i++) {
		if (cube->edges_lengts[i] != lengths[i]) {
			difference = true;
			diff_index = i;
		}
	}
	if (difference) {
		cube->ChangeEdgeLength(diff_index, lengths[diff_index]);
	}
}

void ModMenu(Cone* cone) {
	FigureModMenu(cone);
	int longitude;
	longitude = cone->longitude;

	ImGui::Text("Number of divistions");
	ImGui::SliderInt("Longitude", &longitude, 1, 100);
	ImGui::Spacing();
	float R, H;
	R = cone->r;
	H = cone->height;
	ImGui::SliderFloat("Radius", &R, 0.01f, 10.0f);
	ImGui::SliderFloat("Height", &H, 0.01f, 10.0f);

	if (cone->height != H || cone->longitude != longitude || cone->r != R) {
		cone->height = H;
		cone->longitude = longitude;
		cone->r = R;
		cone->CalculateVerticesAndTriangles();
	}
}

void ModMenu(Sphere* sphere) {
	FigureModMenu(sphere);
	int longitude, lattitude;
	longitude = sphere->longitude;
	lattitude = sphere->lattitude;

	ImGui::Text("Number of divistions");
	ImGui::SliderInt("Longitude", &longitude, 1, 100);
	ImGui::SliderInt("Lattitude", &lattitude, 1, 100);
	ImGui::Spacing();
	float R;
	R = sphere->r;
	ImGui::SliderFloat("R", &R, 0.01f, 10.0f);

	if (sphere->lattitude != lattitude || sphere->longitude != longitude || sphere->r != R) {
		sphere->lattitude = lattitude;
		sphere->longitude = longitude;
		sphere->r = R;
		sphere->CalculateVerticesAndTriangles();
	}
}

void ModMenu(Cylinder* cone) {
	FigureModMenu(cone);
	int longitude;
	longitude = cone->longitude;

	ImGui::Text("Number of divistions");
	ImGui::SliderInt("Longitude", &longitude, 1, 100);
	ImGui::Spacing();
	float R, H;
	R = cone->r;
	H = cone->height;
	ImGui::SliderFloat("Radius", &R, 0.01f, 10.0f);
	ImGui::SliderFloat("Height", &H, 0.01f, 10.0f);

	if (cone->height != H || cone->longitude != longitude || cone->r != R) {
		cone->height = H;
		cone->longitude = longitude;
		cone->r = R;
		cone->CalculateVerticesAndTriangles();
	}
}

void ListFigures(std::vector<Figure*>* figures) {
	int counter = 0;
	char buff[500];
	Figure* to_delete = nullptr;
	for (auto fig : *figures) {
		if (fig->type == Type::Cube_) {
			sprintf_s(buff, 500, "Figure #%d : Cube", counter);
			if (ImGui::TreeNode(buff)) {
				ModMenu(dynamic_cast<Cube*>(fig));
				if (ImGui::Button("Delete"))
					to_delete = fig;
				ImGui::TreePop();
				ImGui::Separator();
			}
		}
		if (fig->type == Type::Cone_) {
			sprintf_s(buff, 500, "Figure #%d : Cone", counter);
			if (ImGui::TreeNode(buff)) {
				ModMenu(dynamic_cast<Cone*>(fig));
				if (ImGui::Button("Delete"))
					to_delete = fig;
				ImGui::TreePop();
				ImGui::Separator();
			}
		}
		if (fig->type == Type::Sphere_) {
			sprintf_s(buff, 500, "Figure #%d : Sphere", counter);
			if (ImGui::TreeNode(buff)) {
				ModMenu(dynamic_cast<Sphere*>(fig));
				if (ImGui::Button("Delete"))
					to_delete = fig;
				ImGui::TreePop();
				ImGui::Separator();
			}
		}
		if (fig->type == Type::Cylinder_) {
			sprintf_s(buff, 500, "Figure #%d : Cylinder", counter);
			if (ImGui::TreeNode(buff)) {
				ModMenu(dynamic_cast<Cylinder*>(fig));
				if (ImGui::Button("Delete"))
					to_delete = fig;
				ImGui::TreePop();
				ImGui::Separator();
			}
		}
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



void CreateMenu(std::vector<Figure*>* figures, std::vector<Camera*>* cameras) {
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
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
		if (ImGui::CollapsingHeader("List Figures")) {
			ListFigures(figures);
		}
		if (ImGui::CollapsingHeader("List Cameras")) {
			ListCameras(cameras);
		}
		if (ImGui::CollapsingHeader("List Lights")) {

		}

		//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Add Cube"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Cube* tmp = new Cube();
			tmp->center = createTranslationMatrix(-0.5f, -0.5f, -0.5f);
			tmp->scale = createScaleMatrix(0.2f, 0.2f, 0.2f);
			figures->push_back(tmp);
		}
		if (ImGui::Button("Add Sphere"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Sphere* tmp = new Sphere();
			tmp->scale = createScaleMatrix(0.2f, 0.2f, 0.2f);
			figures->push_back(tmp);

		}
		if (ImGui::Button("Add Cone"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Cone* tmp = new Cone();
			tmp->scale = createScaleMatrix(0.2f, 0.2f, 0.2f);
			figures->push_back(tmp);
		}
		if (ImGui::Button("Add Cylinder"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Cylinder* tmp = new Cylinder();
			tmp->scale = createScaleMatrix(0.2f, 0.2f, 0.2f);
			figures->push_back(tmp);
		}

		if (ImGui::Button("Add Camera"))// Buttons return true when clicked (most widgets return true when edited/activated)
		{
			Camera* cam = new Camera(def_cameraPos, def_cameraFront, def_cameraUp);
			cam->SetViewport(0, 0, current_width, current_height);
			cam->SetPerspective(fov, ((float)current_height / (float)current_width), 10, 12);
			cameras->push_back(cam);
		}
		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

}


int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);



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

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	FrameBuffer fb(DEFAULT_WIDTH, DEFAULT_HEIGHT);
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

	Cube cub = Cube();
	Cube cub2 = Cube();

	Sphere sp = Sphere();
	Cylinder cy = Cylinder();
	Cone co = Cone();

	cub.center = createTranslationMatrix(-0.5f, -0.5f, -0.5f);
	cub.scale = createScaleMatrix(0.2f, 0.2f, 0.2f);
	cub.translate = createTranslationMatrix(1.0f, 0, 0);
	cub2.translate = createTranslationMatrix(-1.0f, 0, 0);
	cub2.center = createTranslationMatrix(-0.5f, -0.5f, -0.5f);;
	cub2.scale = createScaleMatrix(0.2f, 0.2f, 0.2f);

	sp.scale = createScaleMatrix(0.1f, 0.1f, 0.1f);
	cy.scale = createScaleMatrix(0.1f, 0.1f, 0.1f);
	co.scale = createScaleMatrix(0.1f, 0.1f, 0.1f);

	cub2.translate = createTranslationMatrix(-3.0f, 0, 0);


	//figures.push_back(&cub);
	//figures.push_back(&cub2);
	figures.push_back(&sp);

	def_cameraPos = { 0,0,1 };
	def_cameraFront = { 0,0,-1 };
	def_cameraUp = { 0,1,0 };

	cameraPos = { 0,0,1 };
	cameraFront = { 0,0,-1 };
	cameraUp = { 0,1,0 };

	//TODO: initialize camera

	double deltaTime = 0.0;
	double currentTime = 0.0;

	Camera* cam = new Camera(cameraPos, cameraFront, cameraUp);
	cam->SetViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	cam->SetPerspective(fov, ((float)DEFAULT_HEIGHT / (float)DEFAULT_WIDTH), 1, 100);

	cameras.push_back(cam);
	active_camera_index = 0;
	double alfa = 0.0;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		CreateMenu(&figures, &cameras);
		alfa += 0.03;
		glfwPollEvents();
		timeMeasurement(window, deltaTime, currentTime);
		processInput(window, deltaTime);

		// Update scene
		//fb.ClearColor(0.5f, 0.5f, 1.0f);
		fb.ClearColor(0.1f, 0.15f, 0.15f);

		//write your render pipeline here
		cam->LookAt(cameraPos, cameraFront, cameraUp);
		cam->SetPerspective(fov, ((float)current_height / (float)current_width), 1, 5);
		cam = cameras[active_camera_index];
		cam->SetPosFrontUp(cameraPos, cameraFront, cameraUp);
		//TODO: get MVP matrix

		auto proj = cam->GetProjectionMatrix();
		auto view = cam->GetWorldMatrix();

		cub.rotate = createRotationMatrix(0, -alfa, 0, &cub);
		int color;
		for (auto fig : figures) {
			fig->Transform(proj, view);
			color = RGB(0, 255, 0);
			if (fig->type == Type::Cube_) {
				color = RGB(0, 0, 255);
			}
			for (auto t : fig->triangles) {
				//t.CalculateNormalVectors();
				t.CalculatePointsAfterTransformation();
				t.DrawTriangle(backface_culling, paint_triangles, z_bufferng, perspective_correction, fb, color,cam);
				/*auto tmp = t.after_transformations[0];
				auto tmp2 = t.after_transformations[1];
				auto tmp3 = t.after_transformations[2];
				float val = 1.0f;
				if(backface_culling)
					val = t.CalculateSideValue();
				if (val <= 0) {
					if (paint_triangles)
					{
						if(z_bufferng)
						fb.FillTriangle(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, color);
						else
							fb.FillTriangle(tmp.x, tmp.y, tmp2.x, tmp2.y, tmp3.x, tmp3.y, color);

					}
					if (z_bufferng) {
						fb.DrawLine(tmp.x, tmp.y, tmp.z, tmp2.x, tmp2.y, tmp2.z, RGB(255, 0, 0));
						fb.DrawLine(tmp2.x, tmp2.y, tmp2.z, tmp3.x, tmp3.y, tmp3.z, RGB(255, 0, 0));
						fb.DrawLine(tmp3.x, tmp3.y, tmp3.z, tmp.x, tmp.y, tmp.z, RGB(255, 0, 0));
					}
					else
					{
						fb.DrawLine(tmp.x, tmp.y,  tmp2.x, tmp2.y, RGB(255, 0, 0));
						fb.DrawLine(tmp2.x, tmp2.y,  tmp3.x, tmp3.y, RGB(255, 0, 0));
						fb.DrawLine(tmp3.x, tmp3.y,  tmp.x, tmp.y, RGB(255, 0, 0));
					}
				}*/
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
