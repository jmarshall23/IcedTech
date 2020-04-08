#region Copyright (c) 2013 Jens Thiel, http://thielj.github.io/MetroFramework
/*
 
MetroFramework - Windows Modern UI for .NET WinForms applications

Copyright (c) 2013 Jens Thiel, http://thielj.github.io/MetroFramework

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation the rights to use, copy, 
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the 
following conditions:

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
Portions of this software are (c) 2011 Sven Walter, http://github.com/viperneo

 */
#endregion

using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Security;

namespace MetroFramework.Native
{
    // JT: do not make this class public if we have [SuppressUnmanagedCodeSecurity] applied
    [SuppressUnmanagedCodeSecurity]
    internal static class WinApi
    {
        #region Structs

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct ARGB
        {
            public byte Blue;
            public byte Green;
            public byte Red;
            public byte Alpha;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct BLENDFUNCTION
        {
            public byte BlendOp;
            public byte BlendFlags;
            public byte SourceConstantAlpha;
            public byte AlphaFormat;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct TCHITTESTINFO
        {
            public Point pt;
            public uint flags;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct NCCALCSIZE_PARAMS
        {
            /// <summary>
            ///     IN: New window rectangle; OUT: New client rectangle (all in parent coordinates).
            /// </summary>
            /// <remarks>
            ///     Contains the new coordinates of a window that has been moved or resized, that is, 
            ///     it is the proposed new window coordinates.
            ///     On return, Windows expects the new client rectangle (in parent coordinates).
            /// </remarks>
            public RECT rect0;

            ///  <summary>
            ///     IN: Old window rectangle; OUT: destination rectangle (all in parent coordinates)
            /// </summary>
            /// <remarks>
            ///     Contains the coordinates of the window before it was moved or resized.
            ///     When returning anything other than 0, Windows expects the 
            ///     client's new / destination rectangle (in parent coordinates).
            /// </remarks>
            public RECT rect1;

            /// <summary>
            ///     IN: Old client rectangle; OUT: Source rectangle (all in parent coordinates).
            /// </summary>
            /// <remarks>
            ///     Contains the coordinates of the window's client area before the window was moved or resized.
            ///     When returning anything other than 0, Windows expects the
            ///     source rectangle (in parent coordinates).
            /// </remarks>
            public RECT rect2;

            /// <summary>
            ///     Pointer to a <see cref="WINDOWPOS"/> structure that contains the size and position values specified 
            ///     in the operation that moved or resized the window.
            /// </summary>
            public IntPtr lpPos;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct MINMAXINFO
        {
            public Point ptReserved;
            /// <summary>
            ///     The size a window should be maximized to. This depends on the screen it will end up, 
            ///     so the window manager will request this info when we move the window around.
            /// </summary>
            public Size MaxSize;
            /// <summary>
            ///     The position of the window when maximized. Must be relative to the current screen, 
            ///     so it's often (0,0) or close to that if the task bar is in the way.
            /// </summary>
            public Point MaxPosition;
            /// <summary>
            ///     The minimum size a window should be allowed to be resized to by dragging it's border or resize handle.
            /// </summary>
            public Size MinTrackSize;
            /// <summary>
            ///     The maximum size a window should be allowed to be resized to by dragging it's border or resize handle.
            ///     This is usually the maximum dimensions of the virtual screen, i.e. the bounding box containing all screens.
            /// </summary>
            public Size MaxTrackSize;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct APPBARDATA
        {
            public uint cbSize;
            public IntPtr hWnd;
            public uint uCallbackMessage;
            public ABE uEdge;
            public RECT rc;
            public int lParam;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct WINDOWPOS
        {
            public int hwnd;
            public int hWndInsertAfter;
            public int x;
            public int y;
            public int cx;
            public int cy;
            public int flags;
        }

        #endregion

        #region Enums

        public enum ABM : uint
        {
            New = 0x00000000,
            Remove = 0x00000001,
            QueryPos = 0x00000002,
            SetPos = 0x00000003,
            GetState = 0x00000004,
            GetTaskbarPos = 0x00000005,
            Activate = 0x00000006,
            GetAutoHideBar = 0x00000007,
            SetAutoHideBar = 0x00000008,
            WindowPosChanged = 0x00000009,
            SetState = 0x0000000A,
        }

        public enum ABE : uint
        {
            Left = 0,
            Top = 1,
            Right = 2,
            Bottom = 3
        }

        public enum ScrollBar
        {
            Horizontal = 0,
            Vertical = 1,
            Control = 2,
            Both = 3
        }

        public enum HitTest
        {
            HTNOWHERE = 0,
            HTCLIENT = 1,
            HTCAPTION = 2,
            HTGROWBOX = 4,
            HTSIZE = HTGROWBOX,
            HTMINBUTTON = 8,
            HTMAXBUTTON = 9,
            HTLEFT = 10,
            HTRIGHT = 11,
            HTTOP = 12,
            HTTOPLEFT = 13,
            HTTOPRIGHT = 14,
            HTBOTTOM = 15,
            HTBOTTOMLEFT = 16,
            HTBOTTOMRIGHT = 17,
            HTREDUCE = HTMINBUTTON,
            HTZOOM = HTMAXBUTTON,
            HTSIZEFIRST = HTLEFT,
            HTSIZELAST = HTBOTTOMRIGHT,
            HTTRANSPARENT = -1
        }

        public enum TabControlHitTest
        {
            TCHT_NOWHERE = 1,
        }

        internal static class Messages
        {
            public const  int WM_NULL = 0x0;
            public const  int WM_CREATE = 0x1;
            public const  int WM_DESTROY = 0x2;
            public const  int WM_MOVE = 0x3;
            public const  int WM_SIZE = 0x5;
            public const  int WM_ACTIVATE = 0x6;
            public const  int WM_SETFOCUS = 0x7;
            public const  int WM_KILLFOCUS = 0x8;
            public const  int WM_ENABLE = 0xa;
            public const  int WM_SETREDRAW = 0xb;
            public const  int WM_SETTEXT = 0xc;
            public const  int WM_GETTEXT = 0xd;
            public const  int WM_GETTEXTLENGTH = 0xe;
            public const  int WM_PAINT = 0xf;
            public const  int WM_CLOSE = 0x10;
            public const  int WM_QUERYENDSESSION = 0x11;
            public const  int WM_QUERYOPEN = 0x13;
            public const  int WM_ENDSESSION = 0x16;
            public const  int WM_QUIT = 0x12;
            public const  int WM_ERASEBKGND = 0x14;
            public const  int WM_SYSCOLORCHANGE = 0x15;
            public const  int WM_SHOWWINDOW = 0x18;
            public const  int WM_WININICHANGE = 0x1a;
            public const  int WM_SETTINGCHANGE = WM_WININICHANGE;
            public const  int WM_DEVMODECHANGE = 0x1b;
            public const  int WM_ACTIVATEAPP = 0x1c;
            public const  int WM_FONTCHANGE = 0x1d;
            public const  int WM_TIMECHANGE = 0x1e;
            public const  int WM_CANCELMODE = 0x1f;
            public const  int WM_SETCURSOR = 0x20;
            public const  int WM_MOUSEACTIVATE = 0x21;
            public const  int WM_CHILDACTIVATE = 0x22;
            public const  int WM_QUEUESYNC = 0x23;
            public const  int WM_GETMINMAXINFO = 0x24;
            public const  int WM_PAINTICON = 0x26;
            public const  int WM_ICONERASEBKGND = 0x27;
            public const  int WM_NEXTDLGCTL = 0x28;
            public const  int WM_SPOOLERSTATUS = 0x2a;
            public const  int WM_DRAWITEM = 0x2b;
            public const  int WM_MEASUREITEM = 0x2c;
            public const  int WM_DELETEITEM = 0x2d;
            public const  int WM_VKEYTOITEM = 0x2e;
            public const  int WM_CHARTOITEM = 0x2f;
            public const  int WM_SETFONT = 0x30;
            public const  int WM_GETFONT = 0x31;
            public const  int WM_SETHOTKEY = 0x32;
            public const  int WM_GETHOTKEY = 0x33;
            public const  int WM_QUERYDRAGICON = 0x37;
            public const  int WM_COMPAREITEM = 0x39;
            public const  int WM_GETOBJECT = 0x3d;
            public const  int WM_COMPACTING = 0x41;
            public const  int WM_COMMNOTIFY = 0x44;
            public const  int WM_WINDOWPOSCHANGING = 0x46;
            public const  int WM_WINDOWPOSCHANGED = 0x47;
            public const  int WM_POWER = 0x48;
            public const  int WM_COPYDATA = 0x4a;
            public const  int WM_CANCELJOURNAL = 0x4b;
            public const  int WM_NOTIFY = 0x4e;
            public const  int WM_INPUTLANGCHANGEREQUEST = 0x50;
            public const  int WM_INPUTLANGCHANGE = 0x51;
            public const  int WM_TCARD = 0x52;
            public const  int WM_HELP = 0x53;
            public const  int WM_USERCHANGED = 0x54;
            public const  int WM_NOTIFYFORMAT = 0x55;
            public const  int WM_CONTEXTMENU = 0x7b;
            public const  int WM_STYLECHANGING = 0x7c;
            public const  int WM_STYLECHANGED = 0x7d;
            public const  int WM_DISPLAYCHANGE = 0x7e;
            public const  int WM_GETICON = 0x7f;
            public const  int WM_SETICON = 0x80;
            public const  int WM_NCCREATE = 0x81;
            public const  int WM_NCDESTROY = 0x82;
            public const  int WM_NCCALCSIZE = 0x83;
            public const  int WM_NCHITTEST = 0x84;
            public const  int WM_NCPAINT = 0x85;
            public const  int WM_NCACTIVATE = 0x86;
            public const  int WM_GETDLGCODE = 0x87;
            public const  int WM_SYNCPAINT = 0x88;
            public const  int WM_NCMOUSEMOVE = 0xa0;
            public const  int WM_NCLBUTTONDOWN = 0xa1;
            public const  int WM_NCLBUTTONUP = 0xa2;
            public const  int WM_NCLBUTTONDBLCLK = 0xa3;
            public const  int WM_NCRBUTTONDOWN = 0xa4;
            public const  int WM_NCRBUTTONUP = 0xa5;
            public const  int WM_NCRBUTTONDBLCLK = 0xa6;
            public const  int WM_NCMBUTTONDOWN = 0xa7;
            public const  int WM_NCMBUTTONUP = 0xa8;
            public const  int WM_NCMBUTTONDBLCLK = 0xa9;
            public const  int WM_NCXBUTTONDOWN = 0xab;
            public const  int WM_NCXBUTTONUP = 0xac;
            public const  int WM_NCXBUTTONDBLCLK = 0xad;
            public const  int WM_INPUT = 0xff;
            public const  int WM_KEYFIRST = 0x100;
            public const  int WM_KEYDOWN = 0x100;
            public const  int WM_KEYUP = 0x101;
            public const  int WM_CHAR = 0x102;
            public const  int WM_DEADCHAR = 0x103;
            public const  int WM_SYSKEYDOWN = 0x104;
            public const  int WM_SYSKEYUP = 0x105;
            public const  int WM_SYSCHAR = 0x106;
            public const  int WM_SYSDEADCHAR = 0x107;
            public const  int WM_UNICHAR = 0x109;
            public const  int WM_KEYLAST = 0x108;
            public const  int WM_IME_STARTCOMPOSITION = 0x10d;
            public const  int WM_IME_ENDCOMPOSITION = 0x10e;
            public const  int WM_IME_COMPOSITION = 0x10f;
            public const  int WM_IME_KEYLAST = 0x10f;
            public const  int WM_INITDIALOG = 0x110;
            public const  int WM_COMMAND = 0x111;
            public const  int WM_SYSCOMMAND = 0x112;
            public const  int WM_TIMER = 0x113;
            public const  int WM_HSCROLL = 0x114;
            public const  int WM_VSCROLL = 0x115;
            public const  int WM_INITMENU = 0x116;
            public const  int WM_INITMENUPOPUP = 0x117;
            public const  int WM_MENUSELECT = 0x11f;
            public const  int WM_MENUCHAR = 0x120;
            public const  int WM_ENTERIDLE = 0x121;
            public const  int WM_MENURBUTTONUP = 0x122;
            public const  int WM_MENUDRAG = 0x123;
            public const  int WM_MENUGETOBJECT = 0x124;
            public const  int WM_UNINITMENUPOPUP = 0x125;
            public const  int WM_MENUCOMMAND = 0x126;
            public const  int WM_CHANGEUISTATE = 0x127;
            public const  int WM_UPDATEUISTATE = 0x128;
            public const  int WM_QUERYUISTATE = 0x129;
            public const  int WM_CTLCOLOR = 0x19;
            public const  int WM_CTLCOLORMSGBOX = 0x132;
            public const  int WM_CTLCOLOREDIT = 0x133;
            public const  int WM_CTLCOLORLISTBOX = 0x134;
            public const  int WM_CTLCOLORBTN = 0x135;
            public const  int WM_CTLCOLORDLG = 0x136;
            public const  int WM_CTLCOLORSCROLLBAR = 0x137;
            public const  int WM_CTLCOLORSTATIC = 0x138;
            public const  int WM_MOUSEFIRST = 0x200;
            public const  int WM_MOUSEMOVE = 0x200;
            public const  int WM_LBUTTONDOWN = 0x201;
            public const  int WM_LBUTTONUP = 0x202;
            public const  int WM_LBUTTONDBLCLK = 0x203;
            public const  int WM_RBUTTONDOWN = 0x204;
            public const  int WM_RBUTTONUP = 0x205;
            public const  int WM_RBUTTONDBLCLK = 0x206;
            public const  int WM_MBUTTONDOWN = 0x207;
            public const  int WM_MBUTTONUP = 0x208;
            public const  int WM_MBUTTONDBLCLK = 0x209;
            public const  int WM_MOUSEWHEEL = 0x20a;
            public const  int WM_XBUTTONDOWN = 0x20b;
            public const  int WM_XBUTTONUP = 0x20c;
            public const  int WM_XBUTTONDBLCLK = 0x20d;
            public const  int WM_MOUSELAST = 0x20d;
            public const  int WM_PARENTNOTIFY = 0x210;
            public const  int WM_ENTERMENULOOP = 0x211;
            public const  int WM_EXITMENULOOP = 0x212;
            public const  int WM_NEXTMENU = 0x213;
            public const  int WM_SIZING = 0x214;
            public const  int WM_CAPTURECHANGED = 0x215;
            public const  int WM_MOVING = 0x216;
            public const  int WM_POWERBROADCAST = 0x218;
            public const  int WM_DEVICECHANGE = 0x219;
            public const  int WM_MDICREATE = 0x220;
            public const  int WM_MDIDESTROY = 0x221;
            public const  int WM_MDIACTIVATE = 0x222;
            public const  int WM_MDIRESTORE = 0x223;
            public const  int WM_MDINEXT = 0x224;
            public const  int WM_MDIMAXIMIZE = 0x225;
            public const  int WM_MDITILE = 0x226;
            public const  int WM_MDICASCADE = 0x227;
            public const  int WM_MDIICONARRANGE = 0x228;
            public const  int WM_MDIGETACTIVE = 0x229;
            public const  int WM_MDISETMENU = 0x230;
            public const  int WM_ENTERSIZEMOVE = 0x231;
            public const  int WM_EXITSIZEMOVE = 0x232;
            public const  int WM_DROPFILES = 0x233;
            public const  int WM_MDIREFRESHMENU = 0x234;
            public const  int WM_IME_SETCONTEXT = 0x281;
            public const  int WM_IME_NOTIFY = 0x282;
            public const  int WM_IME_CONTROL = 0x283;
            public const  int WM_IME_COMPOSITIONFULL = 0x284;
            public const  int WM_IME_SELECT = 0x285;
            public const  int WM_IME_CHAR = 0x286;
            public const  int WM_IME_REQUEST = 0x288;
            public const  int WM_IME_KEYDOWN = 0x290;
            public const  int WM_IME_KEYUP = 0x291;
            public const  int WM_MOUSEHOVER = 0x2a1;
            public const  int WM_MOUSELEAVE = 0x2a3;
            public const  int WM_NCMOUSELEAVE = 0x2a2;
            public const  int WM_WTSSESSION_CHANGE = 0x2b1;
            public const  int WM_TABLET_FIRST = 0x2c0;
            public const  int WM_TABLET_LAST = 0x2df;
            public const  int WM_CUT = 0x300;
            public const  int WM_COPY = 0x301;
            public const  int WM_PASTE = 0x302;
            public const  int WM_CLEAR = 0x303;
            public const  int WM_UNDO = 0x304;
            public const  int WM_RENDERFORMAT = 0x305;
            public const  int WM_RENDERALLFORMATS = 0x306;
            public const  int WM_DESTROYCLIPBOARD = 0x307;
            public const  int WM_DRAWCLIPBOARD = 0x308;
            public const  int WM_PAINTCLIPBOARD = 0x309;
            public const  int WM_VSCROLLCLIPBOARD = 0x30a;
            public const  int WM_SIZECLIPBOARD = 0x30b;
            public const  int WM_ASKCBFORMATNAME = 0x30c;
            public const  int WM_CHANGECBCHAIN = 0x30d;
            public const  int WM_HSCROLLCLIPBOARD = 0x30e;
            public const  int WM_QUERYNEWPALETTE = 0x30f;
            public const  int WM_PALETTEISCHANGING = 0x310;
            public const  int WM_PALETTECHANGED = 0x311;
            public const  int WM_HOTKEY = 0x312;
            public const  int WM_PRINT = 0x317;
            public const  int WM_PRINTCLIENT = 0x318;
            public const  int WM_APPCOMMAND = 0x319;
            public const  int WM_THEMECHANGED = 0x31a;
            public const  int WM_HANDHELDFIRST = 0x358;
            public const  int WM_HANDHELDLAST = 0x35f;
            public const  int WM_AFXFIRST = 0x360;
            public const  int WM_AFXLAST = 0x37f;
            public const  int WM_PENWINFIRST = 0x380;
            public const  int WM_PENWINLAST = 0x38f;
            public const  int WM_USER = 0x400;
            public const  int WM_REFLECT = 0x2000;
            public const  int WM_APP = 0x8000;

            public const int WM_DWMCOMPOSITIONCHANGED = 0x031E;

            public const int OCM__BASE = WM_USER + 0x1C00; // e.g. OCM_COMMAND = OCM__BASE + WM_COMMAND

            public const  int SC_MOVE = 0xF010;
            public const  int SC_MINIMIZE = 0XF020;
            public const  int SC_MAXIMIZE = 0xF030;
            public const  int SC_RESTORE = 0xF120;
        }

        public enum Bool
        {
            False = 0,
            True
        };

        #endregion

        #region Fields

        public const int Autohide = 0x0000001;
        public const int AlwaysOnTop = 0x0000002;

        public const Int32 MfByposition = 0x400;
        public const Int32 MfRemove = 0x1000;

        public const int TCM_HITTEST = 0x1313;

        public const Int32 ULW_COLORKEY = 0x00000001;
        public const Int32 ULW_ALPHA = 0x00000002;
        public const Int32 ULW_OPAQUE = 0x00000004;

        public const byte AC_SRC_OVER = 0x00;
        public const byte AC_SRC_ALPHA = 0x01;

        // GetWindow() constants
        public const int GW_HWNDFIRST = 0;
        public const int GW_HWNDLAST = 1;
        public const int GW_HWNDNEXT = 2;
        public const int GW_HWNDPREV = 3;
        public const int GW_OWNER = 4;
        public const int GW_CHILD = 5;
        public const int HC_ACTION = 0;
        public const int WH_CALLWNDPROC = 4;
        public const int GWL_WNDPROC = -4;

        #endregion

        #region API Calls

        [DllImport("user32.dll", ExactSpelling = true, SetLastError = true)]
        public static extern Bool UpdateLayeredWindow(IntPtr hwnd, IntPtr hdcDst, ref Point pptDst, ref Size psize, IntPtr hdcSrc, ref Point pprSrc, Int32 crKey, ref BLENDFUNCTION pblend, Int32 dwFlags);

        [DllImport("user32.dll", ExactSpelling = true, SetLastError = true)]
        public static extern IntPtr GetDC(IntPtr hWnd);

        [DllImport("gdi32.dll", ExactSpelling = true, SetLastError = true)]
        public static extern IntPtr CreateCompatibleDC(IntPtr hDC);

        [DllImport("gdi32.dll", ExactSpelling = true, SetLastError = true)]
        public static extern Bool DeleteDC(IntPtr hdc);

        [DllImport("gdi32.dll", ExactSpelling = true)]
        public static extern IntPtr SelectObject(IntPtr hDC, IntPtr hObject);

        [DllImport("gdi32.dll", ExactSpelling = true, SetLastError = true)]
        public static extern Bool DeleteObject(IntPtr hObject);

        [DllImport("user32.dll")]
        public static extern IntPtr GetSystemMenu(IntPtr hWnd, bool bRevert);

        [DllImport("user32.dll")]
        public static extern int GetMenuItemCount(IntPtr hMenu);

        [DllImport("user32.dll")]
        public static extern bool DrawMenuBar(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern bool RemoveMenu(IntPtr hMenu, uint uPosition, uint uFlags);

        [DllImport("user32.dll")]
        public static extern bool ReleaseCapture();

        [DllImport("user32.dll")]
        public static extern IntPtr SetCapture(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);

        [DllImport("user32.dll")]
        public static extern int SendMessage(IntPtr wnd, int msg, bool param, int lparam);

        [DllImport("shell32.dll", SetLastError = true)]
        public static extern IntPtr SHAppBarMessage(ABM dwMessage, [In] ref APPBARDATA pData);

        [DllImport("user32.dll", SetLastError = true)]
        public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        [DllImport("user32.dll")]
        public static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("user32.dll")]
        public static extern IntPtr GetDCEx(IntPtr hwnd, IntPtr hrgnclip, uint fdwOptions);

        [DllImport("user32.dll")]
        public static extern bool ShowScrollBar(IntPtr hWnd, ScrollBar bar, int cmd);

        [DllImport("User32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr GetWindowDC(IntPtr handle);

        [DllImport("User32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr ReleaseDC(IntPtr handle, IntPtr hDC);

        [DllImport("User32.dll", CharSet = CharSet.Auto)]
        public static extern int GetClassName(IntPtr hwnd, char[] className, int maxCount);

        [DllImport("User32.dll", CharSet = CharSet.Auto)]
        public static extern IntPtr GetWindow(IntPtr hwnd, int uCmd);

        [DllImport("User32.dll", CharSet = CharSet.Auto)]
        public static extern bool IsWindowVisible(IntPtr hwnd);

        [DllImport("user32", CharSet = CharSet.Auto)]
        public static extern int GetClientRect(IntPtr hwnd, ref RECT rect);

        [DllImport("user32", CharSet = CharSet.Auto)]
        public static extern bool MoveWindow(IntPtr hwnd, int X, int Y, int nWidth, int nHeight, bool bRepaint);

        [DllImport("user32", CharSet = CharSet.Auto)]
        public static extern bool UpdateWindow(IntPtr hwnd);

        [DllImport("user32", CharSet = CharSet.Auto)]
        public static extern bool InvalidateRect(IntPtr hwnd, ref RECT rect, bool bErase);

        [DllImport("user32", CharSet = CharSet.Auto)]
        public static extern bool ValidateRect(IntPtr hwnd, ref RECT rect);

        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        internal static extern bool GetWindowRect(IntPtr hWnd, [In, Out] ref RECT rect);

        //---- GetWindowLongPtr

        /// <summary>
        ///     Retrieves information about the specified window. The function also retrieves the value at a specified offset into the extra window memory.
        /// </summary>
        /// <remarks>
        ///     Certain window data is cached, so changes you made earlier might not have taken effect unless <seealso cref="SetWindowPos"/> was called.
        ///     Note: This method is compatible with 32-bit and 64-bit system and will dispatch to the specific API function at run-time.
        /// </remarks>
        /// <seealso href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms633585(v=vs.85).aspx"/>
        [SecurityCritical]
        public static IntPtr GetWindowLongPtr(IntPtr hWnd, int nIndex)
        {
            return IntPtr.Size == 4 ? GetWindowLong32(hWnd, nIndex) : GetWindowLongPtr64(hWnd, nIndex);
        }

        [DllImport("user32.dll", EntryPoint = "GetWindowLong", CharSet = CharSet.Auto)]
        private static extern IntPtr GetWindowLong32(IntPtr hWnd, int nIndex);

        [DllImport("user32.dll", EntryPoint = "GetWindowLongPtr", CharSet = CharSet.Auto)]
        private static extern IntPtr GetWindowLongPtr64(IntPtr hWnd, int nIndex);

        //---- SetWindowLongPtr

        /// <summary>
        ///     Changes an attribute of the specified window or sets a value at the specified offset in the extra window memory (32/64-bit).
        /// </summary>
        /// <remarks>
        ///     Certain window data is cached, so changes you make will not take effect until you call the SetWindowPos function.
        ///     Note: This method is compatible with 32-bit and 64-bit system and will dispatch to the specific API function at run-time.
        /// </remarks>
        /// <seealso href="http://msdn.microsoft.com/en-gb/library/windows/desktop/ms644898(v=vs.85).aspx"/>
        [SecurityCritical]
        public static IntPtr SetWindowLongPtr(IntPtr hWnd, int nIndex, IntPtr dwNewLong)
        {
            return IntPtr.Size == 4 ? SetWindowLong32(hWnd, nIndex, dwNewLong) : SetWindowLongPtr64(hWnd, nIndex, dwNewLong);
        }

        [DllImport("user32.dll", EntryPoint = "SetWindowLong", CharSet = CharSet.Auto)]
        private static extern IntPtr SetWindowLong32(IntPtr hWnd, int nIndex, IntPtr dwNewLong);

        [DllImport("user32.dll", EntryPoint = "SetWindowLongPtr", CharSet = CharSet.Auto)]
        private static extern IntPtr SetWindowLongPtr64(IntPtr hWnd, int nIndex, IntPtr dwNewLong);

        //---- GetClassLongPtr

        /// <summary>
        ///     Retrieves the specified value from the WNDCLASSEX structure associated with the specified window.
        /// </summary>
        /// <seealso href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms633581(v=vs.85).aspx"/>
        [SecurityCritical]
        public static IntPtr GetClassLongPtr(IntPtr hWnd, int nIndex)
        {
            return IntPtr.Size == 4 ? GetClassLong32(hWnd, nIndex) : GetClassLongPtr64(hWnd, nIndex);
        }

        [DllImport("user32.dll", EntryPoint = "GetClassLong", CharSet = CharSet.Auto)]
        private static extern IntPtr GetClassLong32(IntPtr hwnd, int nIndex);

        [DllImport("user32.dll", EntryPoint = "GetClassLongPtr", CharSet = CharSet.Auto)]
        private static extern IntPtr GetClassLongPtr64(IntPtr hwnd, int nIndex);

        //---- SetClassLongPtr

        /// <summary>
        ///     Replaces the specified value at the specified offset in the extra class memory or the WNDCLASSEX structure for 
        ///     the class to which the specified window belongs.
        /// </summary>
        /// <remarks>
        ///     Note: This method is compatible with 32-bit and 64-bit system and will dispatch to the specific API function at run-time.
        /// </remarks>
        /// <seealso href="http://msdn.microsoft.com/en-us/library/windows/desktop/ms633589(v=vs.85).aspx"/>
        [SecurityCritical]
        public static IntPtr SetClassLongPtr(IntPtr hWnd, int nIndex, IntPtr dwNewLong)
        {
            return IntPtr.Size == 4 ? SetClassLong32(hWnd, nIndex, dwNewLong) : SetClassLongPtr64(hWnd, nIndex, dwNewLong);
        }

        [DllImport("user32.dll", EntryPoint = "SetClassLong", CharSet = CharSet.Auto)]
        private static extern IntPtr SetClassLong32(IntPtr hwnd, int nIndex, IntPtr dwNewLong);

        [DllImport("user32.dll", EntryPoint = "SetClassLongPtr", CharSet = CharSet.Auto)]
        private static extern IntPtr SetClassLongPtr64(IntPtr hwnd, int nIndex, IntPtr dwNewLong);

        // ----- SetWindowPos

        /// <summary>
        /// Changes the size, position, and Z order of a child, pop-up or top-level window.
        /// </summary>
        /// <remarks>
        ///     If you have changed certain window data using <seealso cref="SetWindowLongPtr"/>, 
        ///     you must call this function with the following flags for the changes to take effect: 
        ///     <seealso cref="SetWindowPosFlags.NOMOVE"/> | <seealso cref="SetWindowPosFlags.NOSIZE"/> | <seealso cref="SetWindowPosFlags.NOZORDER"/> | <seealso cref="SetWindowPosFlags.FRAMECHANGED"/>.
        /// </remarks>
        /// <param name="hWnd">A handle to the window.</param>
        /// <param name="hWndInsertAfter">A handle to the window to precede the positioned window in the Z order. See MSDN for special values.</param>
        /// <param name="x">The new position of the left side of the window, in client coordinates.</param>
        /// <param name="y">The new position of the top of the window, in client coordinates.</param>
        /// <param name="w">The new width of the window, in pixels.</param>
        /// <param name="h">The new height of the window, in pixels.</param>
        /// <param name="uFlags">The window sizing and positioning flags. (SetWindowPosFlags value)</param>
        /// <returns>Nonzero if function succeeds, zero if function fails.</returns>
        /// <seealso href="http://msdn.microsoft.com/en-gb/library/windows/desktop/ms633545(v=vs.85).aspx"/>
        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int x, int y, int w, int h, SetWindowPosFlags uFlags);

        /// <summary>
        /// SetWindowPos Flags
        /// </summary>
        [Flags]
        public enum SetWindowPosFlags : uint
        {
            /// <summary>Retains the current size (ignores the w and h parameters).</summary>
            NOSIZE = 0x0001,
            /// <summary>Retains the current position (ignores X and Y parameters).</summary>
            NOMOVE = 0x0002,
            /// <summary>Retains the current Z order (ignores the hWndInsertAfter parameter).</summary>
            NOZORDER = 0x0004,
            /// <summary>Does not redraw changes. If this flag is set, no repainting of any kind occurs. This applies to the client area, the nonclient area (including the title bar and scroll bars),</summary>
            NOREDRAW = 0x0008,
            /// <summary>Does not activate the window.</summary> 
            NOACTIVATE = 0x0010,
            DRAWFRAME = 0x0020,
            /// <summary>Applies new frame styles set using the SetWindowLong function. Sends a WM_NCCALCSIZE message to the window, even if the window's size is not being changed. If this flag is not specified, WM_NCCALCSIZE is sent only when the window's size is being changed.</summary>
            FRAMECHANGED = 0x0020,
            /// <summary>Displays the window.</summary>
            SHOWWINDOW = 0x0040,
            /// <summary>Hides the window.</summary>
            HIDEWINDOW = 0x0080,
            NOCOPYBITS = 0x0100,
            /// <summary>Does not change the owner window's position in the Z order.</summary>
            NOOWNERZORDER = 0x0200,
            /// <summary>Same as the SWP_NOOWNERZORDER flag.</summary>
            NOREPOSITION = 0x0200,
            /// <summary>Prevents the window from receiving the WM_WINDOWPOSCHANGING message.</summary>
            NOSENDCHANGING = 0x0400,
            DEFERERASE = 0x2000,
            ASYNCWINDOWPOS = 0x4000
        }

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool RedrawWindow(IntPtr hWnd, ref RECT lprcUpdate, IntPtr hrgnUpdate, RedrawWindowFlags flags);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool RedrawWindow(IntPtr hWnd, IntPtr lprcUpdate, IntPtr hrgnUpdate, RedrawWindowFlags flags);

        [Flags]
        public enum RedrawWindowFlags : uint
        {
            /// <summary>
            /// Invalidates the rectangle or region that you specify in lprcUpdate or hrgnUpdate.
            /// You can set only one of these parameters to a non-NULL value. If both are NULL, RDW_INVALIDATE invalidates the entire window.
            /// </summary>
            Invalidate = 0x1,

            /// <summary>Causes the OS to post a WM_PAINT message to the window regardless of whether a portion of the window is invalid.</summary>
            InternalPaint = 0x2,

            /// <summary>
            /// Causes the window to receive a WM_ERASEBKGND message when the window is repainted.
            /// Specify this value in combination with the RDW_INVALIDATE value; otherwise, RDW_ERASE has no effect.
            /// </summary>
            Erase = 0x4,

            /// <summary>
            /// Validates the rectangle or region that you specify in lprcUpdate or hrgnUpdate.
            /// You can set only one of these parameters to a non-NULL value. If both are NULL, RDW_VALIDATE validates the entire window.
            /// This value does not affect internal WM_PAINT messages.
            /// </summary>
            Validate = 0x8,

            NoInternalPaint = 0x10,

            /// <summary>Suppresses any pending WM_ERASEBKGND messages.</summary>
            NoErase = 0x20,

            /// <summary>Excludes child windows, if any, from the repainting operation.</summary>
            NoChildren = 0x40,

            /// <summary>Includes child windows, if any, in the repainting operation.</summary>
            AllChildren = 0x80,

            /// <summary>Causes the affected windows, which you specify by setting the RDW_ALLCHILDREN and RDW_NOCHILDREN values, to receive WM_ERASEBKGND and WM_PAINT messages before the RedrawWindow returns, if necessary.</summary>
            UpdateNow = 0x100,

            /// <summary>
            /// Causes the affected windows, which you specify by setting the RDW_ALLCHILDREN and RDW_NOCHILDREN values, to receive WM_ERASEBKGND messages before RedrawWindow returns, if necessary.
            /// The affected windows receive WM_PAINT messages at the ordinary time.
            /// </summary>
            EraseNow = 0x200,

            /// <summary>
            /// Causes any part of the nonclient area of the window that intersects the update region to receive a WM_NCPAINT message. The RDW_INVALIDATE flag must also be specified; otherwise, RDW_FRAME has no effect.
            /// </summary>
            Frame = 0x400,

            NoFrame = 0x800
        }

        #endregion
    }
}
