/*

	PROJECT:		mod_sa
	LICENSE:		See LICENSE in the top level directory
	COPYRIGHT:		Copyright we_sux, BlastHack

	mod_sa is available from https://github.com/BlastHackNet/mod_s0beit_sa/

	mod_sa is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	mod_sa is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with mod_sa.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "main.h"

struct key_state	key_table[256];
int					keys_cleared;
POINT				mouse_position;

static void process_key(int down, int vkey)
{
	if ( down && KEY_DOWN(vkey) )
		return; /* ignore key repeat, bad states */
	if ( !down && KEY_UP(vkey) )
		return; /* ignore bad states */

	key_table[vkey].count++;
}

void wnd_proc(UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_KILLFOCUS:
		keys_cleared = 0;
		keyhook_clear_states();
		break;

		/* :D */
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		unsigned long	p = (unsigned long)lparam;
		int				down = (umsg == WM_KEYDOWN || umsg == WM_SYSKEYDOWN);
		int				vkey = (int)wparam;

		unsigned int	scancode = (p >> 16) & 0x00FF;
		unsigned int	extended = (p >> 24) & 0x0001;

		/* :D :D :D :D :D */
		switch (vkey)
		{
		case VK_SHIFT:
			if (scancode == MapVirtualKey(VK_LSHIFT, 0))
				vkey = VK_LSHIFT;
			else if (scancode == MapVirtualKey(VK_RSHIFT, 0))
				vkey = VK_RSHIFT;
			break;

		case VK_CONTROL:
			vkey = extended ? VK_RCONTROL : VK_LCONTROL;
			break;
		case VK_MENU:
			vkey = extended ? VK_RMENU : VK_LMENU;
			break;
		}

		/* :D */
		if (KEY_DOWN(VK_LMENU) && vkey == VK_LMENU && down)
			break;
		if (KEY_UP(VK_LMENU) && vkey == VK_LMENU && !down)
			break;

		process_key(down, vkey);
	}
	break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		process_key((umsg == WM_LBUTTONDOWN), VK_LBUTTON);
		break;

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		process_key((umsg == WM_RBUTTONDOWN), VK_RBUTTON);
		break;

	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		process_key((umsg == WM_MBUTTONDOWN), VK_MBUTTON);
		break;

	case WM_MOUSEMOVE:
		mouse_position.x = ((int)(short)LOWORD(lparam));
		mouse_position.y = ((int)(short)HIWORD(lparam));
		break;
	}
}

void keyhook_run ( void )
{
	keys_cleared = 0;
	for ( int i = 0; i < 256; i++ )
	{
		key_table[i].consume = 0;

		if ( i == VK_PRIOR || i == VK_NEXT || i == VK_TAB )
			key_table[i].pstate = ( key_table[i].count & 1 );
		else
			key_table[i].pstate = KEY_DOWN( i );

		if ( key_table[i].count > 0 )
		{
			key_table[i].flip ^= 1;
			key_table[i].count--;
		}
	}
}

int keyhook_key_down ( int v )
{
	if ( key_table[v].consume )
		return 0;
	else
		return ( key_table[v].count & 1 ) ^ key_table[v].flip;
}

int keyhook_key_up ( int v )
{
	return !KEY_DOWN( v );
}

int keyhook_key_pressed ( int v )
{
	if ( key_table[v].consume )
		return 0;
	else if ( v == VK_PRIOR || v == VK_NEXT || v == VK_TAB )
		return KEY_DOWN( v ) && !( key_table[v].pstate ^ key_table[v].flip );
	else
		return KEY_DOWN( v ) && !key_table[v].pstate;
}

int keyhook_key_released ( int v )
{
	return KEY_UP( v ) && key_table[v].pstate;
}

void keyhook_key_consume ( int v )
{
	key_table[v].consume = 1;
}

void keyhook_clear_states ( void )
{
	if ( !keys_cleared )
	{
		keys_cleared = 1;
		for ( int i = 0; i < 256; i++ )
		{
			key_table[i].pstate = 0;
			key_table[i].count = 0;
			key_table[i].flip = 0;
		}
	}
}

bool keyhook_mouse_hovered(int x, int y, int w, int h)
{
	if (mouse_position.x > x && mouse_position.x < x + w && mouse_position.y > y && mouse_position.y < y + h)
		return true;
	return false;
}

POINT &keyhook_get_mouse_position()
{
	return mouse_position;
}
