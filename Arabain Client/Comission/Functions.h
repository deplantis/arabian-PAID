#pragma once

#include <iostream>
#include <Windows.h>
#include <strsafe.h>
#include <random>
#include <string>
#include "mem.h"
#include <stdio.h> 

#include <cmath>

#define _USE_MATH_DEFINES

float min_reach = 3.1;
float max_reach = 3.7;
float last_min = min_reach;
float last_max = max_reach;
float hitbox_w = 0.6;
float hitbox_h = 1.8;
float min_timer = 20;
float max_timer = 22;
float last_mint = min_timer;
float last_maxt = max_timer;

int velocity_h = 100;
int velocity_v = 100;
int cps_min = 3;
int cps_max = 8;
int cps = 1;

float last_cpsmin = cps_min;
float last_cpsmax = cps_max;

bool timer_enabled = false;
bool reach_enabled = false;
bool hitbox_enabled = false;
bool velocity_enabled = false;
bool auto_enabled = false;
bool autoclicker_on = false;
bool right_clicker;
bool mc_only = false;
bool enabled;
bool penabled = 0;



bool hook_first = false;
bool hook_mouse_down = false;
bool* hook_running;
bool mRAN = false;

static HHOOK hook_mouse;
static DWORD oldprotect = 0;

std::string getActiveWindow() {
	wchar_t wnd_title[256];
	HWND hwnd = GetForegroundWindow();

	GetWindowTextW(hwnd, wnd_title, sizeof(wnd_title));

	std::wstring wstr = std::wstring(wnd_title);
	std::string str(wstr.begin(), wstr.end());

	return str;
}

int random(int i_start, int i_end) {
	std::random_device rd; //min is higher than max, that's ur problem
	std::mt19937 really_nigga(rd());
	const std::uniform_int_distribution<int> uni(i_start, i_end);

	return static_cast<int>(uni(really_nigga));
}

void Checks() {
	//do the min>max check on the cheat thread thing, not
	if (auto_enabled) {
		if (cps_min > cps_max) {
			if (last_cpsmin != cps_min) {
				last_cpsmin = cps_min;
				cps_max = cps_min;
			}
			else if (last_cpsmax != cps_max) {
				last_cpsmax = cps_max;

				cps_min = cps_max;
			}
			else cps_max = cps_min;
		}
	}
}

void spamReach(bool& ran) {

	while (ran) {

		if (reach_enabled) { 

			if (min_reach > max_reach) {
				if (last_min != min_reach) {
					last_min = min_reach;
					max_reach = min_reach;
				}
				else if (last_max != max_reach) {
					last_max = max_reach;

					min_reach = max_reach;
				}
				else max_reach = min_reach;
			}

			float total_reach = (float)random(min_reach * 1000, max_reach * 1000) / 1000.f;
			uintptr_t reachPtr = mem::FindAddr(mem::hProcess, mem::moduleBase + 0x35CDBA8, { });
			VirtualProtectEx(mem::hProcess, (PBYTE)reachPtr, sizeof(float), PAGE_EXECUTE_READWRITE, &oldprotect);
			WriteProcessMemory(mem::hProcess, (BYTE*)reachPtr, &total_reach, sizeof(total_reach), nullptr);

		}
		Sleep(800);
	}
}

void spamTimer(bool& ran) {

	while (ran) {
		

		if (timer_enabled) {

			if (min_timer > max_timer) {
				if (last_mint != min_timer) {
					last_mint = min_timer;
					max_timer = min_timer;
				}
				else if (last_maxt != max_timer) {
					last_maxt = max_timer;

					min_timer = max_timer;
				}
				else max_timer = min_timer;
			}

			float total_timer = (float)random(min_timer * 1000, max_timer * 1000) / 1000.f;
			uintptr_t TimerPtr = mem::FindAddr(mem::hProcess, mem::moduleBase + 0x040A4B88, { 0x10, 0x128, 0x0, 0xA8, 0xD0, 0x0 });
		
			WriteProcessMemory(mem::hProcess, (BYTE*)TimerPtr, &total_timer, sizeof(total_timer), nullptr);

		}
		Sleep(800);
	}
}

void Hitboxgg(bool& ran)
{
	while (ran)
	{
		if (hitbox_enabled) {
			uintptr_t LocalPlayerPtr = mem::FindAddr(mem::hProcess, mem::moduleBase + 0x040A4B88, { 0x10, 0x128, 0x0, 0x138 });
			uintptr_t EntityStartPtr = mem::FindAddr(mem::hProcess, LocalPlayerPtr, { 0x378, 0x58, 0x0 });
			uintptr_t EntityEndPtr = mem::FindAddr(mem::hProcess, LocalPlayerPtr, { 0x378, 0x60, 0x0 });
			if (EntityStartPtr) {
			}
			for (uintptr_t entity = EntityStartPtr; entity < EntityEndPtr; entity += 0x8) {
				if (entity == EntityStartPtr) continue;
				uintptr_t HitboxWidthPtr = mem::FindAddr(mem::hProcess, entity, { 0x4EC });
				uintptr_t HitboxHPtr = mem::FindAddr(mem::hProcess, entity, { 0x4F0 });
				WriteProcessMemory(mem::hProcess, (BYTE*)HitboxWidthPtr, &hitbox_w, sizeof(hitbox_w), 0);
				WriteProcessMemory(mem::hProcess, (BYTE*)HitboxHPtr, &hitbox_h, sizeof(hitbox_h), 0);

			}
		}
		Sleep(800);
	}
}

LRESULT WINAPI mouse_hook(int nCode, WPARAM wParam, LPARAM lParam) {
	auto* hook = reinterpret_cast<MSLLHOOKSTRUCT*> (lParam);

	if ((hook->flags == LLMHF_INJECTED)) {
		hook->flags &= ~LLMHF_INJECTED;
		hook->flags &= ~LLMHF_LOWER_IL_INJECTED;
		return false;
	}

	if (wParam != WM_MOUSEMOVE) {
		switch (wParam) {
		case WM_LBUTTONDOWN:
			hook_first = true;
			hook_mouse_down = true;
			break;
		case WM_LBUTTONUP:
			hook_mouse_down = false;
			break;
		}
	}

	return CallNextHookEx(hook_mouse, nCode, wParam, lParam);
}

DWORD WINAPI hook(bool ran) {
	hook_mouse = SetWindowsHookExA(WH_MOUSE_LL, &mouse_hook, nullptr, 0);

	MSG lpMsg;
	while (GetMessageA(&lpMsg, nullptr, 0, 0)) {
		TranslateMessage(&lpMsg);
		DispatchMessageA(&lpMsg);
	}

	UnhookWindowsHookEx(hook_mouse);

	return EXIT_SUCCESS;
}


void Clicker(bool ran) {
	while (ran) {

		if (cps_min > cps_max) {
			if (last_cpsmin != cps_min) {
				last_cpsmin = cps_min;
				cps_max = cps_min;
			}
			else if (last_cpsmax != cps_max) {
				last_cpsmax = cps_max;

				cps_min = cps_max;
			}
			else cps_max = cps_min;
		}
		//this ooh

		static bool pressed = false;
		if (!GetAsyncKeyState(VK_F8)) {
			if (GetKeyState(VK_F7) & 0x8000 && !pressed) {
				pressed = true;
			}

			else if (!(GetKeyState(VK_F7) & 0x8000) && pressed) {
				auto_enabled = !auto_enabled;
				pressed = false;
			}

		}

		bool pass = auto_enabled and hook_mouse_down and getActiveWindow() == ("Minecraft");

		autoclicker_on = pass;

		if (!pass) {
			Sleep(100);
			continue;

		}

		float cps = (float)random(cps_min, cps_max);
		int delta = (int)round(1000.f / cps);

		if (hook_first) hook_first = false;
		else {

			INPUT input = { 0 };
			input.type = INPUT_MOUSE;
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			SendInput(1, &input, sizeof(INPUT));
		}

		Sleep(delta);

		INPUT input = { 0 };
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, &input, sizeof(INPUT));
	}
	Sleep(200);
}
