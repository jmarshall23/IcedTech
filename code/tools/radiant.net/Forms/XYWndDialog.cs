using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using radiant.net;

namespace radiant.net.forms
{
    public partial class XYWndDialog : Form
    {
        public XYWndDialog()
        {
            InitializeComponent();
            this.KeyPreview = true;

            this.KeyDown += XYWndDialog_KeyDown;
            this.KeyUp += XYWndDialog_KeyUp;
            RenderPanel.MouseDown += XYWndDialog_MouseDown;
            RenderPanel.MouseUp += XYWndDialog_MouseUp;
            RenderPanel.MouseMove += RenderPanel_MouseMove;
            RenderPanel.Paint += RenderPanel_Paint;

            texWndPanel.MouseUp += TexWndPanel_MouseUp;
            texWndPanel.MouseDown += TexWndPanel_MouseDown;
            texWndPanel.Paint += TexWndPanel_Paint;
            this.StartPosition = FormStartPosition.Manual;
        }

        private void TexWndPanel_MouseUp(object sender, MouseEventArgs e)
        {
            NativeAPI.RadiantAPI_TexWndMouseLeft(false, e.X, e.Y);
        }

        private void TexWndPanel_MouseDown(object sender, MouseEventArgs e)
        {
            NativeAPI.RadiantAPI_TexWndMouseLeft(true, e.X, e.Y);
        }

        private void TexWndPanel_Paint(object sender, PaintEventArgs e)
        {
            NativeAPI.RadiantAPI_RedrawWindows();
        }

        private void RenderPanel_MouseMove(object sender, MouseEventArgs e)
        {
            NativeAPI.RadiantAPI_MouseMove(e.X, e.Y);
        }

        private void XYWndDialog_MouseUp(object sender, MouseEventArgs e)
        {
            Point screenPosition = this.PointToScreen(e.Location);
            if (e.Button == MouseButtons.Right)
            {
                // NativeAPI.RadiantAPI_XYMouseRight(false, e.X, e.Y);
            }
            else if(e.Button == MouseButtons.Left)
            {
                NativeAPI.RadiantAPI_XYMouseLeft(false, e.X, e.Y);
            }
        }

        private void XYWndDialog_MouseDown(object sender, MouseEventArgs e)
        {
            Point screenPosition = this.PointToScreen(e.Location);
            if (e.Button == MouseButtons.Right)
            {
                //NativeAPI.RadiantAPI_XYMouseRight(true, e.X, e.Y);
                Editor.entityCreateDialog.SetEntityCreationLocation(e.Location);
                Editor.entityCreateDialog.StartPosition = FormStartPosition.Manual;
                Editor.entityCreateDialog.Left = screenPosition.X;
                Editor.entityCreateDialog.Top = screenPosition.Y;
                Editor.entityCreateDialog.ShowDialog(Editor.nativeWindow);
            }
            else if(e.Button == MouseButtons.Left)
            {
                NativeAPI.RadiantAPI_XYMouseLeft(true, e.X, e.Y);
            }
        }

        private void RenderPanel_Paint(object sender, PaintEventArgs e)
        {
            NativeAPI.RadiantAPI_RedrawWindows();
        }

        private void XYWndDialog_KeyUp(object sender, KeyEventArgs e)
        {
            NativeAPI.RadaintAPI_KeyEvent(RadiantHelpers.GetKeyAscii(e), false);
        }

        private void XYWndDialog_KeyDown(object sender, KeyEventArgs e)
        {
            NativeAPI.RadaintAPI_KeyEvent(RadiantHelpers.GetKeyAscii(e), true);
        }

        private const int CP_NOCLOSE_BUTTON = 0x200;
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams myCp = base.CreateParams;
                myCp.ClassStyle = myCp.ClassStyle | CP_NOCLOSE_BUTTON;
                return myCp;
            }


        }

        public IntPtr GetHWND()
        {
            return RenderPanel.Handle;
        }

        public IntPtr GetTexWndHWND()
        {
            return texWndPanel.Handle;
        }

        private void fileToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        public void SetMapName(string mapName)
        {
            xyWndTab.Text = "World Edit: " + mapName;
        }
    }
}
