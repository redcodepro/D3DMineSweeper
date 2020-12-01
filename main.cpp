#include "main.h"

___globals G;

hookedMainloop_t	orig_mainloop;
hookedPresent_t		orig_Present;
hookedReset_t		orig_Reset;

CD3DRender *render = new CD3DRender(128);
CD3DFont *pGameFont = new CD3DFont("Arial", 16, FCR_BOLD | FCR_BORDER);
CD3DFont *pD3DFont2 = new CD3DFont("Tahoma", 10, FCR_NONE);

int ps = 22;
bool Menu = false;
MineSweeper *game = new MineSweeper(10, 10, 10);
float wSizeX = game->GetXSize()*ps + 10;
float wSizeY = game->GetYSize()*ps + 105;
float wPosX, wPosY;

void cmd_mswp(const char* param)
{
	if (!strlen(param))
	{
		Menu ^= true;
		return;
	}
	int temp[3] = { 0,0,0 };
	char *p;

	if (p = strtok((char*)param, " "))
		temp[0] = atoi(p);

	if (p && (p = strtok(NULL, " ")))
		temp[1] = atoi(p);

	if (p && (p = strtok(NULL, " ")))
		temp[2] = atoi(p);

	if (temp[0] < 10 || temp[1] < 10 || temp[2] < 10)
	{
		SAMP::pChat->Print(SAMP::pChat, 0xFF0000FF, "[ERR] {FFFFFF}Минимальный размер поля: 10x10. Минимум мин: 10");
		return;
	}

	int maxx = min((*(int*)0xC9C040 - 10) / ps, MAX_SIZE_X);
	int maxy = min((*(int*)0xC9C044 - 130) / ps, MAX_SIZE_Y);
	if (temp[0] > maxx || temp[1] > maxy)
	{
		SAMP::pChat->Print(SAMP::pChat, 0xFF0000FF, "[ERR] {FFFFFF}Максимальный размер поля %dx%d", maxx, maxy);
		return;
	}
	delete game;
	game = new MineSweeper(temp[0], temp[1], temp[2]);
	wSizeX = game->GetXSize()*ps + 10;
	wSizeY = game->GetYSize()*ps + 105;
	wPosX = floor(*(int*)0xC9C040 / 2 - wSizeX / 2);
	wPosY = floor(*(int*)0xC9C044 / 2 - wSizeY / 2 + 30);

	Menu = true;
}

HRESULT CALLBACK hooked_Present(IDirect3DDevice9* pDevice, const RECT* r1, const RECT* r2, HWND hwnd, const RGNDATA* rngd)
{
	static bool init = false;
	if (!init)
	{
		render->Initialize(pDevice);
		pGameFont->Initialize(pDevice);
		pD3DFont2->Initialize(pDevice);

		wPosX = floor(*(int*)0xC9C040 / 2 - wSizeX / 2);
		wPosY = floor(*(int*)0xC9C044 / 2 - wSizeY / 2 + 30);

		init = true;
	}

	if (Menu && SUCCEEDED(render->BeginRender()))
	{
		static bool move = false;
		static int offset[2] = { 0, 0 };

		POINT &curPos = keyhook_get_mouse_position();

		if (move)
		{
			if (KEY_DOWN(VK_LBUTTON))
			{
				wPosX = curPos.x - offset[0];
				wPosY = curPos.y - offset[1];

				if (wPosX < 0) wPosX = 0;
				if (wPosY < 0) wPosY = 0;
				if (wPosX + 32 > *(int*)0xC9C040) wPosX = ((*(int*)0xC9C040) - 32);
				if (wPosY + 32 > *(int*)0xC9C044) wPosY = ((*(int*)0xC9C044) - 32);
			}
			else move = false;
		}
		else
		{
			if (KEY_PRESSED(VK_LBUTTON) && MOUSE_HOVERED(wPosX, wPosY - 30, game->GetXSize() * ps - 35, 30))
			{
				offset[0] = curPos.x - wPosX;
				offset[1] = curPos.y - wPosY;
				move = true;
			}
		}

		bool isHovered;
		float gfX = wPosX + 5;
		float gfY = wPosY + 70;

		// Окошко
		render->D3DBoxBorder(wPosX, wPosY - 30, wSizeX, wSizeY, 0xFF00BECC, 0xFFF5F6F7);
		render->D3DBox(wPosX, wPosY - 30, wSizeX - 2, 30, 0xFFFFFFFF);
		render->D3DMSMine(wPosX + 6, wPosY - 24, 0xFF000000);
		pD3DFont2->Print("D3DMineSweeper", 0xFF000000, wPosX + 28, wPosY - 23);

		// Крестик
		isHovered = MOUSE_HOVERED(wPosX + wSizeX - 45, wPosY - 30, 45, 30);
		render->D3DBox(wPosX + wSizeX - 45, wPosY - 30, 45, 30, isHovered ? 0xFFE81123 : 0xFFFFFFFF);
		pD3DFont2->Print("x", isHovered ? 0xFFFFFFFF : 0xFF000000, wPosX + wSizeX - 25, wPosY - 25);
		if (isHovered && KEY_PRESSED(VK_LBUTTON))
		{
			Menu = false;
		}

		// Время
		render->D3DMSSBox(game->GetTime(), wPosX + 10, wPosY + 10, 0xFF000000, 0xFFFF0000);

		// Мины/флаги
		render->D3DMSSBox(game->GetFlagNum(), wPosX + wSizeX - 86, wPosY + 10, 0xFF000000, 0xFFFF0000);

		// Рожа
		isHovered = MOUSE_HOVERED(wPosX + wSizeX / 2 - 20, wPosY + 14, 40, 40);
		render->D3DBoxBorder(wPosX + wSizeX / 2 - 20, wPosY + 14, 40, 40, 0xFFDADBDC, isHovered ? 0xFFFFFFFF : 0xFFDADBDC);
		render->D3DMSRozha(wPosX + wSizeX / 2 - 15, wPosY + 18, KEY_DOWN(VK_LBUTTON) ? 4 : game->GetGameState());
		if (isHovered && KEY_PRESSED(VK_LBUTTON))
		{
			game->StartGame();
		}

		// Игровое поле
		render->D3DBoxBorder(gfX, gfY, game->GetXSize() * ps, game->GetYSize()*ps, 0xFFDADBDC, 0xFFC2C2C2);

		isHovered = MOUSE_HOVERED(gfX, gfY, game->GetXSize() * ps, game->GetYSize() * ps);
		if (isHovered)
			game->SetPos((keyhook_get_mouse_position().x - gfX) / ps, (keyhook_get_mouse_position().y - gfY) / ps);
		else
			game->SetPos(-1, -1);

		bool one = false;
		for (int y = 0; y < game->GetYSize(); y++)
		{
			int offset = 0;
			bool two = one;
			for (int x = 0; x < game->GetXSize(); x++)
			{
				short block = game->getBlock(x, y);
				if (block & STATE_OPEN)
				{
					if (two) render->D3DBox(gfX + offset, gfY + y * ps, ps, ps, 0xFFA6A6A6);

					if (LOBYTE(block) != stNone)
						if (LOBYTE(block) != stMine)
						{
							pGameFont->PrintMSNum(LOBYTE(block), gfX + offset, gfY + y * ps);
						}
						else
						{
							if (block & STATE_BOOM)
								render->D3DBox(gfX + offset, gfY + y * ps, ps, ps, 0xFFFF0000);
							render->D3DMSMine(gfX + offset + 2, gfY + y * ps + 2, 0xFF000000);
						}
				}
				else
				{
					render->D3DBox(gfX + offset, gfY + y * ps, ps, ps, two ? 0xFF229c30 : 0xFF2ad43e);
					if (block & STATE_FLAG)
					{
						if (game->GetGameState() == gstLose && block != stMine)
							render->D3DBox(gfX + offset, gfY + y * ps, ps, ps, 0xFFFFF700);
						render->D3DMSFlag(gfX + offset + 2, gfY + y * ps + 2, 0xFFFF0000);
					}
				}
				offset += ps;
				two ^= true;
			}
			one ^= true;
		}
		if (isHovered)
			render->D3DBox(game->GetXPos()*ps + gfX, game->GetYPos()*ps + gfY, ps, ps, 0xA0FFFFFF);
		render->EndRender();
	}
	return orig_Present(pDevice, r1, r2, hwnd, rngd);
}

HRESULT CALLBACK hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	render->Invalidate();
	pGameFont->Invalidate();
	pD3DFont2->Invalidate();

	HRESULT hr = orig_Reset(pDevice, pPresentationParameters);
	if (SUCCEEDED(hr))
	{
		render->Initialize(pDevice);
		pGameFont->Initialize(pDevice);
		pD3DFont2->Initialize(pDevice);
	}
	return hr;
}

LRESULT CALLBACK WndProc(HWND hwd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	wnd_proc(msg, wParam, lParam);

	static bool show_cursor = false;
	if (Menu)
	{
		if (msg == WM_KEYDOWN && wParam == VK_ESCAPE)
			Menu = false;

		if (msg == WM_LBUTTONUP|| msg == WM_RBUTTONUP || msg == WM_LBUTTONDBLCLK)
			game->MouseClick(msg);

		SAMP::pGame->SetCursorMode(3, 1);
		show_cursor = true;
	}
	else
	{
		if (show_cursor)
		{
			SAMP::pGame->SetCursorMode(0, 1);
			SAMP::pGame->ProcessInputEnabling();
			show_cursor = false;
		}
	}
	return CallWindowProc(G.orig_wndproc, hwd, msg, wParam, lParam);
}

void CALLBACK hooked_mainloop()
{
	static bool init = false;
	if (!init)
	{
		if (G.gameState != 9 || SAMP::pNetGame == nullptr || SAMP::pChat == nullptr || SAMP::pInputBox == nullptr)
			return orig_mainloop();

		G.orig_wndproc = (WNDPROC)SetWindowLongA(G.hwnd, GWL_WNDPROC, (LONG)WndProc);

		MH_CreateHook(VT_FUNC(G.pD3DDevice, 17), &hooked_Present, reinterpret_cast<void**>(&orig_Present));
		MH_EnableHook(VT_FUNC(G.pD3DDevice, 17));

		MH_CreateHook(VT_FUNC(G.pD3DDevice, 16), &hooked_Reset, reinterpret_cast<void**>(&orig_Reset));
		MH_EnableHook(VT_FUNC(G.pD3DDevice, 16));

		SAMP::pInputBox->AddCommand("mswp", cmd_mswp);

		init = true;
	}

	if (G.isMenuOpened)
	{
		keyhook_clear_states();
	}
	else
	{
		keyhook_run();
	}

	return orig_mainloop();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hModule);

			MH_Initialize();

			MH_CreateHook((void*)0x748DA3, &hooked_mainloop, (void**)(&orig_mainloop));
			MH_EnableHook((void*)0x748DA3);
		}
		break;
	case DLL_PROCESS_DETACH:
		{
			if (G.orig_wndproc)
			{
				SetWindowLongA(G.hwnd, GWL_WNDPROC, (LONG)G.orig_wndproc);
			}

			MH_RemoveHook((void*)0x748DA3);

			MH_RemoveHook(VT_FUNC(G.pD3DDevice, 17));
			MH_RemoveHook(VT_FUNC(G.pD3DDevice, 16));

			MH_Uninitialize();
		}
		break;
	}
	return TRUE;
}