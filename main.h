#ifndef MAIN_H
#define MAIN_H

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable: 4244)

#include <ctime>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "hook/MinHook.h"
#pragma comment(lib, "hook/libMinHook.x86.lib")

#include "d3drender.h"
#include "keyhook.h"

#include "samp/CChat.h"
#include "samp/CGame.h"
#include "samp/CInput.h"
#include "samp/CNetGame.h"

#include "mineswp.h"

#define VT_FUNC(vt, i)	((void**)(*(void***)(vt))[i])

struct ___globals
{
	WNDPROC				orig_wndproc = NULL;
	IDirect3DDevice9*	&pD3DDevice = *reinterpret_cast<IDirect3DDevice9**>(0xC97C28);
	HWND				&hwnd = *reinterpret_cast<HWND*>(0xC97C1C);
	uint32_t			&gameState = *reinterpret_cast<uint32_t*>(0xC8D4C0);
	bool				&isMenuOpened = *reinterpret_cast<bool*>(0xBA67A4);
};

extern struct ___globals G;

typedef void(__cdecl* hookedMainloop_t)(void);
typedef HRESULT(__stdcall* hookedPresent_t)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef HRESULT(__stdcall* hookedReset_t)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

#endif