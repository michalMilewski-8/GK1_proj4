#include "../Figure.h"
#include "imgui.h"
#include "../Helper.h"

void Figure::ModMenu(int counter, Figure*& to_delete)
{
	float x_trans, y_trans, z_trans;
	Helper::getTranslationValues(x_trans, y_trans, z_trans, this->translate);
	ImGui::SliderFloat("Move X axis", &x_trans, -5.0f, 5.0f);
	ImGui::SliderFloat("Move Y axis", &y_trans, -5.0f, 5.0f);
	ImGui::SliderFloat("Move Z axis", &z_trans, -5.0f, 5.0f);
	this->translate = Helper::createTranslationMatrix(x_trans, y_trans, z_trans);
	ImGui::Spacing();
	float x_scale, y_scale, z_scale;
	Helper::getScaleValue(x_scale, y_scale, z_scale, this->scale);
	ImGui::SliderFloat("Scale X axis", &x_scale, 0.0f, 5.0f);
	ImGui::SliderFloat("Scale Y axis", &y_scale, 0.0f, 5.0f);
	ImGui::SliderFloat("Scale Z axis", &z_scale, 0.0f, 5.0f);
	this->scale = Helper::createScaleMatrix(x_scale, y_scale, z_scale);
	ImGui::Spacing();
	float x_rot, y_rot, z_rot;
	Helper::getRotateValue(x_rot, y_rot, z_rot, this);
	ImGui::SliderFloat("Rotate X axis", &x_rot, 0.0f, 6.5f);
	ImGui::SliderFloat("Rotate Y axis", &y_rot, 0.0f, 6.5f);
	ImGui::SliderFloat("Rotate Z axis", &z_rot, 0.0f, 6.5f);
	this->rotate = Helper::createRotationMatrix(x_rot, y_rot, z_rot, this);
	ImGui::Spacing();
}

void Figure::Transform(glm::mat4 proj, glm::mat4 view, glm::mat4 view_port) {
	transformation = view_port * proj * view * scale * translate * rotate * center;
	trans_without_viewport = proj * view * scale * translate * rotate * center;
}
