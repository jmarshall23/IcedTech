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
            splitContainer1.Panel1.MouseDown += XYWndDialog_MouseDown;
            splitContainer1.Panel1.MouseUp += XYWndDialog_MouseUp;
            splitContainer1.Panel1.MouseMove += RenderPanel_MouseMove;
            splitContainer1.Panel1.Paint += RenderPanel_Paint;

            tabControl1.SelectedIndexChanged += TabControl1_SelectedIndexChanged;

            texWndPanel.MouseUp += TexWndPanel_MouseUp;
            texWndPanel.MouseDown += TexWndPanel_MouseDown;
            texWndPanel.Paint += TexWndPanel_Paint;
            this.StartPosition = FormStartPosition.Manual;

            UpdateTreeView();
        }

        private void TabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = (sender as TabControl).SelectedIndex;

            if(index == 1)
            {
                
            }
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
                //Editor.entityCreateDialog.SetEntityCreationLocation(e.Location);
                //Editor.entityCreateDialog.StartPosition = FormStartPosition.Manual;
                //Editor.entityCreateDialog.Left = screenPosition.X;
                //Editor.entityCreateDialog.Top = screenPosition.Y;
                //Editor.entityCreateDialog.ShowDialog(Editor.nativeWindow);
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
            return splitContainer1.Panel1.Handle;
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

        private void XYWndDialog_Load(object sender, EventArgs e)
        {

        }
        public void UpdateEntityList(string[] entities)
        {
            int index = 0;
            EntityListBox.Items.Clear();

            foreach (string name in entities)
            {
                EntityListBox.Items.Add(name);
                index++;
            }

            EntityListBox.Sorted = true;
        }

        private void UpdateTreeView()
        {
            entityTreeView.Nodes.Clear();
            string entityString = NativeAPI.RadiantAPI_GetEntityClassManaged();
            string[] entityList = entityString.Split(';');
            foreach (string entityName in entityList)
            {
                entityTreeView.Nodes.Add(entityName);
            }
        }
        private void CreateEntityButton_Click(object sender, EventArgs e)
        {
            int x = splitContainer1.Panel1.Width / 2;
            int y = splitContainer1.Panel1.Height / 2;

            NativeAPI.RadiantAPI_CreateEntity(x, y, entityTreeView.SelectedNode.Text);
        }

        private void gotoEntityButton_Click(object sender, EventArgs e)
        {
            string selected_entity = (string)EntityListBox.SelectedItem;
            if (selected_entity == "worldspawn")
                return;

            NativeAPI.RadiantAPI_GotoEntity(selected_entity);
            NativeAPI.RadiantAPI_SelectObject(selected_entity);
        }
    }
}
