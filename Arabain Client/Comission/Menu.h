#pragma once
#include "imgui/imgui.h"
#include <Windows.h>

void LoadStyle() {
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(7, 14);  // padding location
	style->ItemSpacing = ImVec2(0, 5);
	style->WindowRounding = 0.6f;
	style->ChildBorderSize = 0.6f;
	style->AntiAliasedLines = true;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	style->GrabRounding = 3.f;

	//	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(255.420f, 0.0f, 0.0f, 1.0f);


}

void RenderRainbowBar(float x, float y, float width, float height, float flSpeed, float& flRainbow) {
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImColor colColor(0, 0, 0, 255);
	flRainbow += flSpeed;

	if (flRainbow > 1.f) flRainbow = 0.f;
	for (int i = 0; i < width; i++) {
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;

		if (hue < 0.f) hue += 1.f;
		ImGuiStyle* style = &ImGui::GetStyle();

		ImColor colRainbow = ImColor::HSV(hue, 1.f, 1.f);
		style->Colors[ImGuiCol_Border] = colRainbow;
		style->Colors[ImGuiCol_PlotHistogram] = colRainbow;
		style->Colors[ImGuiCol_CheckMark] = colRainbow;



		drawList->AddRectFilled(ImVec2(x + i, y), ImVec2(x + i + width, y + height), colRainbow);
	}
}

void RenderRainbowBar() {
	RECT dimension;
	HWND hDesktop = GetDesktopWindow();

	GetWindowRect(hDesktop, &dimension);

	auto pos = ImGui::GetWindowPos();
	static float flRainbow;
	float flSpeed = 0.003f;
	float x = pos.x;
	float y = pos.y;
	float width = dimension.right + 24;
	float height = 5;

	RenderRainbowBar(x, y, width, height, flSpeed, flRainbow);
}