#include "GuiLoader.h"
#include "GuiLoader.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"

#include "Functions.h"
#include "font_definitions.hpp"


#include <d3d9.h>

#define DIRECTINPUAT_VERSION 0x0900
#pragma comment(lib,"d3d9.lib")

#include <atomic>
#include <tchar.h>
#include <thread>
#include "fontawesome.hpp"
#include "Menu.h"

bool switchTabs = 0;

static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

bool CreateDeviceD3D(HWND hWnd);

void CleanupDeviceD3D();
void ResetDevice();


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

GuiLoader::GuiLoader() {

	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("Arrow.EXE"), nullptr };
	::RegisterClassEx(&wc);
	static HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Arrow.EXE"), WS_POPUP, 150, 150, 1, 1, nullptr, nullptr, wc.hInstance, nullptr);
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClass(wc.lpszClassName, wc.hInstance);

	}
	//::ShowWindow(::GetConsoleWindow(), SW_SHOW);
	::ShowWindow(hwnd, SW_HIDE);
	::UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	std::atomic_bool atomicenable(true);
	bool ran(true);
	std::thread t1(spamReach, std::ref(ran));
	std::thread t2(Hitboxgg, std::ref(ran));
	std::thread t3(spamTimer, std::ref(ran));
	std::thread t4(hook, std::ref(ran));
	std::thread t5(Clicker, std::ref(ran));


	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}



	ImFont* font1 = io.Fonts->AddFontDefault();
	static ImFont* font2 = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\arial.ttf)", 18.0f);
	static ImFont* font3 = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\BRLNSR.ttf)", 40.0f);

	ImFontConfig config;
	//config.MergeMode = true;
	config.PixelSnapH = true;

	static const ImWchar ranges[] =
	{
			ICON_MIN_FA,
			ICON_MAX_FA,
			0
	};

	static ImFont* font5 = io.Fonts->AddFontFromMemoryCompressedTTF(fa_compressed_data, fa_compressed_size, 50.0f, &config, ranges);
	static ImFont* font7 = io.Fonts->AddFontFromMemoryCompressedTTF(fa_compressed_data, fa_compressed_size, 30.0f, &config, ranges);





	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);


	ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("##", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);

		ImGui::PushFont(font2);
		ImGui::PushFont(font5);
		LoadStyle();

		ImGui::SetWindowSize(ImVec2(800, 500));
		ImGui::SetCursorPosY(25);
		ImGui::Text("");
		ImGui::SameLine(20.f);
	//	ImGui::Text(ICON_FA_WATER); //L lol wat
		ImGui::PushFont(font3);
		ImGui::SetCursorPosY(15);
		ImGui::Text("");
		ImGui::SameLine(90.f);
		ImGui::Text("Lakeside");
		ImGui::Dummy(ImVec2(0.0f, 10.0f));


		ImGui::AlignTextToFramePadding();
		ImGui::Spacing();

		ImGui::PopFont();
		ImGui::PopFont();

		ImGui::PopFont();
		ImGui::PushFont(font7);
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, .7f));

		ImGui::SetCursorPosY(25);
		ImGui::Text("");
		ImGui::SameLine(400.f);

		if (ImGui::Button((char*)ICON_FA_CROSSHAIRS, ImVec2(100, 40)))
			switchTabs = 0;
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::SetCursorPosY(25);
		ImGui::Text("");
		ImGui::SameLine(530.f);
		if (ImGui::Button((char*)ICON_FA_TACHOMETER_ALT, ImVec2(100, 40)))
			switchTabs = 1;
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::SetCursorPosY(25);
		ImGui::Text("");
		ImGui::SameLine(660.f);
		if (ImGui::Button((char*)ICON_FA_COG, ImVec2(100, 40)))
			switchTabs = 2;
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SetCursorPosY(130);


		ImGui::PopFont();
		switch (switchTabs) {
		case 0:
			ImGui::BeginChild("ReachTab", ImVec2(250, 350), true, ImGuiWindowFlags_NoScrollbar);
			ImGui::PushFont(font2);

			ImGui::Checkbox(" Reach", &reach_enabled);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::SliderFloatFill("Minimum", &min_reach, 3, 7, "%.2f", 1.8f);
			ImGui::SliderFloatFill("Maximum", &max_reach, 3, 7, "%.2f", 1.8f);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Button(("None"), ImVec2(125, 35));

			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::Checkbox(" Hitbox", &hitbox_enabled);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::SliderFloatFill("HitboxWidth", &hitbox_w, 0.6, 6, "%.3f", 1.8f);
			ImGui::SliderFloatFill("HitboxHeight", &hitbox_h, 1.8, 3, "%.3f", 1.8f);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Button(("None"), ImVec2(125, 35));

			ImGui::PopFont();
			ImGui::EndChild();

			ImGui::SameLine(400);
			ImGui::BeginChild("AutoTab", ImVec2(250, 255), true, ImGuiWindowFlags_NoScrollbar);
			ImGui::PushFont(font2);
			ImGui::Checkbox(" Autoclicker", &auto_enabled);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::SliderIntFill("cps_min", &cps_min, 0, 20);
			ImGui::SliderIntFill("cps_max", &cps_max, 0, 20);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Button(("None"), ImVec2(125, 35));
			ImGui::EndChild();

			if (ImGui::Button((char*)ICON_FA_CROSSHAIRS, ImVec2(100, 40)))
				exit();
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::EndChild();

			break;
		case 1:
			ImGui::BeginChild("VelocityTab", ImVec2(250, 350), true, ImGuiWindowFlags_NoScrollbar);
			ImGui::Checkbox(" Velocity", &velocity_enabled);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::SliderIntFill("velocity_h", &velocity_h, 0, 100, "%0.2f %%");
			ImGui::SliderIntFill("velocity_v", &velocity_v, 0, 100, "%0.2f %%");
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Button(("None"), ImVec2(125, 35));

			ImGui::Dummy(ImVec2(0.0f, 20.0f));
			ImGui::Checkbox(" Phase", &penabled);
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
	
			ImGui::Text("F3 to toggle ON/OFF");



			ImGui::EndChild();

	

			//ImGui::SameLine(200);

			ImGui::SameLine(400);
			ImGui::BeginChild("TimerTab", ImVec2(250, 255), true, ImGuiWindowFlags_NoScrollbar);
			ImGui::PushFont(font2);
			ImGui::Checkbox(" Timer", &timer_enabled);
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::SliderFloatFill("mintimer", &min_timer, 20, 30, "%0.2f");
			ImGui::SliderFloatFill("maxtimer", &max_timer, 20, 30, "%0.2f");
			ImGui::Dummy(ImVec2(0.0f, 10.0f));
			ImGui::Button(("None"), ImVec2(125, 35));
			ImGui::EndChild();
			break;
		case 2:
			break;
		}
		ImGui::EndChild();
		ImGui::End();

		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f), (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);


		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}



	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
}
bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
		return false;


	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}



void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	default:;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);


}
