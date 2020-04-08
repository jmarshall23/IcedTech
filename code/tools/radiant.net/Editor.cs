using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using RGiesecke.DllExport;
using radiant.net.forms;

namespace radiant.net
{
    public static class Editor
    {
        public static NativeWindow nativeWindow;
        static ProgressDialog progressDialog;
        static InspecterDialog inspecterDialog;
        static CamWndDialog camWndDialog;
        public static XYWndDialog xyWndDialog;
        static MainFrm mainFrm;

        public static string[] entityList;
        static IntPtr _xyhdc = IntPtr.Zero;
        static IntPtr _camhdc = IntPtr.Zero;
        static IntPtr _texhdc = IntPtr.Zero;
        static IntPtr _gamehdc = IntPtr.Zero;

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public static void RadiantPrint(string txt)
        {
            txt = txt.Replace("\n", "\r\n"); // Windows text boxes need CRLF as line terminators, not just LF.
            inspecterDialog.RadiantPrint(txt);
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public static void UpdateEntitySel(string classname, string name)
        {
            inspecterDialog.UpdateEntitySel(classname, name);
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public static void InitRadiant(IntPtr ParentWindow)
        {
            Application.EnableVisualStyles();

            progressDialog = new ProgressDialog();
            camWndDialog = new CamWndDialog();
            xyWndDialog = new XYWndDialog();
            mainFrm = new MainFrm();

            inspecterDialog = new InspecterDialog();

            nativeWindow = new NativeWindow();
            nativeWindow.AssignHandle(ParentWindow);

            {
                //camWndDialog.Parent = mainFrm;
                inspecterDialog.TopLevel = false;
                inspecterDialog.Padding = new Padding(0, 0, 0, 0);
                inspecterDialog.DisplayHeader = false;
                inspecterDialog.BorderStyle = MetroFramework.Drawing.MetroBorderStyle.None;
                inspecterDialog.FormBorderStyle = FormBorderStyle.None;
                inspecterDialog.ShadowType = MetroFramework.Forms.MetroForm.MetroFormShadowType.None;
                inspecterDialog.ControlBox = false;
                inspecterDialog.Dock = DockStyle.Fill;
                inspecterDialog.Show();
                mainFrm.GetLeftSplitContainer().Panel2.Controls.Add(inspecterDialog);
            }

            {
                //camWndDialog.Parent = mainFrm;
                camWndDialog.TopLevel = false;
                camWndDialog.Padding = new Padding(0, 0, 0, 0);
                camWndDialog.DisplayHeader = false;
                camWndDialog.BorderStyle = MetroFramework.Drawing.MetroBorderStyle.None;
                camWndDialog.FormBorderStyle = FormBorderStyle.None;
                camWndDialog.ShadowType = MetroFramework.Forms.MetroForm.MetroFormShadowType.None;
                camWndDialog.ControlBox = false;
                camWndDialog.Dock = DockStyle.Fill;
                camWndDialog.Show();
                mainFrm.GetLeftSplitContainer().Panel1.Controls.Add(camWndDialog);
            }

            {
              //  xyWndDialog.Parent = mainFrm;
                xyWndDialog.TopLevel = false;
                xyWndDialog.Padding = new Padding(0, 0, 0, 0);
                xyWndDialog.DisplayHeader = false;
                xyWndDialog.BorderStyle = MetroFramework.Drawing.MetroBorderStyle.None;
                xyWndDialog.FormBorderStyle = FormBorderStyle.None;
                xyWndDialog.ShadowType = MetroFramework.Forms.MetroForm.MetroFormShadowType.None;
                xyWndDialog.ControlBox = false;
                xyWndDialog.Dock = DockStyle.Fill;
                xyWndDialog.Show();
                mainFrm.GetMainSplitContainer().Panel2.Controls.Add(xyWndDialog);
            }

            mainFrm.Show(nativeWindow);

            //   inspectorDialog.Show(nativeWindow);
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static void SetMapName(string mapName)
        {
            xyWndDialog.SetMapName(mapName);
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static IntPtr GetGameWindowHandle()
        {
            if(_gamehdc == IntPtr.Zero)
            {
                _gamehdc = NativeAPI.GetDC(xyWndDialog.GetGameWndHWND().ToPointer());
            }
            return _gamehdc;
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static IntPtr GetGameWindowHWND()
        {
            return xyWndDialog.GetGameWndHWND();
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static int GetGameWindowWidth()
        {
            return xyWndDialog.GetGameWndWidth();
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static int GetGameWindowHeight()
        {
            return xyWndDialog.GetGameWndHeight();
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static IntPtr GetTexWndHandle()
        {
            // IntPtr hdc = NativeAPI.GetDC(xyWndDialog.GetTexWndHWND().ToPointer());
            if (_texhdc == IntPtr.Zero)
            {
                _texhdc = NativeAPI.GetDC(xyWndDialog.GetTexWndHWND().ToPointer());
            }
            return _texhdc;
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static IntPtr GetXYWindowHandle()
        {
            if (_xyhdc == IntPtr.Zero)
            {
                _xyhdc = NativeAPI.GetDC(xyWndDialog.GetHWND().ToPointer());
            }

            return _xyhdc;
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static IntPtr GetXYDialogHandle()
        {
            return xyWndDialog.Handle;
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static IntPtr GetCameraWindowHandle()
        {
            //IntPtr hdc = NativeAPI.GetDC(camWndDialog.GetHWND().ToPointer());
            if (_camhdc == IntPtr.Zero)
            {
                _camhdc = NativeAPI.GetDC(camWndDialog.GetHWND().ToPointer());
            }
            return _camhdc;
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static IntPtr GetCameraDialogHandle()
        {
            return camWndDialog.Handle;
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public static void RedrawCameraWindow()
        {
            camWndDialog.Invalidate();
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public static void UpdateLevelEntities(string entities)
        {
            entityList = entities.Split(';');
            xyWndDialog.UpdateEntityList(entityList);
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public static void ToggleProgressDialog(bool showDialog, string mapName, string workString, float completedPercent)
        {
            if (showDialog)
            {
                progressDialog.TopMost = true;
                progressDialog.SetLevelProgressData(mapName, workString, completedPercent);
                if (progressDialog.Visible != showDialog)
                {
                    progressDialog.Show(nativeWindow);
                }
                progressDialog.Refresh();
                //  progressDialog.BringToFront();
            }
            else
            {
                if (progressDialog.Visible != showDialog)
                {
                    progressDialog.Hide();
                }
                progressDialog.hasCanceledWork = false;
            }
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public static bool ProgressBarUserCancaled()
        {
            return progressDialog.hasCanceledWork;
        }
    }
}
