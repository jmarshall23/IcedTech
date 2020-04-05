using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
namespace radiant.net
{
    public unsafe static class NativeAPI
    {
        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_GameWindowKeyboard(byte ch, bool down);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_GameWindowMouseKey(bool rightButton, bool leftButton, bool down);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool RadiantAPI_GameMouseFocus();

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_GameWindowMouseMovement(int mouseX, int mouseY);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_ShowDoom(bool isDoomVisible);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_SelectObject(string select_name);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_GotoEntity(string select_name);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_SetRenderMode(bool render);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadaintAPI_KeyEvent(char key, bool down);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_RedrawWindows();

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_XYMouseRight(bool down, int x, int y);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_XYMouseLeft(bool down, int x, int y);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr RadiantAPI_GetEntityClassList();

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_CreateEntity(int x, int y, string name);

        public static string RadiantAPI_GetEntityClassManaged()
        {
            return Marshal.PtrToStringAnsi(RadiantAPI_GetEntityClassList());
        }

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_MouseMove(int x, int y);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_CameraMouseLeft(bool down, int x, int y);

        [DllImport("DoomDLL.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern void RadiantAPI_TexWndMouseLeft(bool down, int x, int y);

        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        public static extern uint SendMessage(IntPtr hWnd,uint Msg, uint wParam, uint lParam);

        [DllImport("user32.dll", EntryPoint = "GetDC")]
        public static extern IntPtr GetDC(void *ptr);

        [DllImport("uxtheme.dll", CharSet = CharSet.Unicode)]
        public extern static int SetWindowTheme(IntPtr hWnd, string pszSubAppName, string pszSubIdList);
    }
}
