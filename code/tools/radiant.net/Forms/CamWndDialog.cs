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
using MetroFramework.Components;
using MetroFramework.Forms;

namespace radiant.net.forms
{
    public partial class CamWndDialog : MetroForm
    {
        public CamWndDialog()
        {
            InitializeComponent();

            this.MouseDown += CamWndDialog_MouseDown;
            this.RenderPanel.Paint += RenderPanel_Paint;
            this.RenderPanel.MouseDown += RenderPanel_MouseDown;
            this.RenderPanel.MouseUp += RenderPanel_MouseUp;
            this.StartPosition = FormStartPosition.Manual;
        }

        private void RenderPanel_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                NativeAPI.RadiantAPI_CameraMouseLeft(false, e.X, e.Y);
            }
        }

        private void RenderPanel_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                NativeAPI.RadiantAPI_CameraMouseLeft(true, e.X, e.Y);
            }
        }

        private void RenderPanel_Paint(object sender, PaintEventArgs e)
        {
            NativeAPI.RadiantAPI_RedrawWindows();
        }

        private void CamWndDialog_MouseDown(object sender, MouseEventArgs e)
        {

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

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void litRenderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_SetRenderMode(true);
        }

        private void unlitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_SetRenderMode(false);
        }

        private void CamWndDialog_Load(object sender, EventArgs e)
        {

        }
    }
}
