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
        static EntitySelection entiySelectionDialog;
        public static NativeWindow nativeWindow;
        static ProgressDialog progressDialog;
        //static InspectorDialog inspectorDialog;
        static CamWndDialog camWndDialog;
        static XYWndDialog xyWndDialog;

        public static EntityCreateDialog entityCreateDialog;

        static IntPtr _xyhdc = IntPtr.Zero;
        static IntPtr _camhdc = IntPtr.Zero;
        static IntPtr _texhdc = IntPtr.Zero;

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public static void InitRadiant(IntPtr ParentWindow)
        {
            Application.EnableVisualStyles();

            entiySelectionDialog = new EntitySelection();
            progressDialog = new ProgressDialog();
            camWndDialog = new CamWndDialog();
            xyWndDialog = new XYWndDialog();
            entityCreateDialog = new EntityCreateDialog();

            //  inspectorDialog = new InspectorDialog();

            nativeWindow = new NativeWindow();
            nativeWindow.AssignHandle(ParentWindow);

            camWndDialog.Show(nativeWindow);
            xyWndDialog.Show(nativeWindow);

            //   inspectorDialog.Show(nativeWindow);
        }

        [DllExport(CallingConvention = CallingConvention.Cdecl)]
        public unsafe static void SetMapName(string mapName)
        {
            xyWndDialog.SetMapName(mapName);
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
        public static void ShowEntitySelection(string entities)
        {
            string[] entityList = entities.Split(';');
            entiySelectionDialog.UpdateEntityList(entityList);
            entiySelectionDialog.Show(nativeWindow);
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
