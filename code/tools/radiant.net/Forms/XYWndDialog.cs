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
        Timer _leftMouseButtonTimer;
        Timer _rightMouseButtonTimer;

        MouseEventArgs _leftMouseButtonEventArgs;
        Point _leftCursorPosition;

        MouseEventArgs _rightMouseButtonEventArgs;

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

            _leftMouseButtonTimer = new Timer();
            _leftMouseButtonTimer.Interval = 11;
            _leftMouseButtonTimer.Enabled = false;
            _leftMouseButtonTimer.Tick += LeftMouseButtonTimer_Tick;
            _rightMouseButtonTimer = new Timer();
            _rightMouseButtonTimer.Interval = 11;
            _rightMouseButtonTimer.Enabled = false;
            _rightMouseButtonTimer.Tick += RightMouseButtonTimer_Tick;

            UpdateTreeView();
        }

        private void LeftMouseButtonTimer_Tick(object sender, EventArgs e)
        {
            Point relativePoint = new Point();

            relativePoint.X = _leftMouseButtonEventArgs.X + (Cursor.Position.X - _leftCursorPosition.X);
            relativePoint.Y = _leftMouseButtonEventArgs.Y + (Cursor.Position.Y - _leftCursorPosition.Y);

            NativeAPI.RadiantAPI_XYMouseLeft(true, relativePoint.X, relativePoint.Y);
            NativeAPI.RadiantAPI_RedrawWindows();
        }

        private void RightMouseButtonTimer_Tick(object sender, EventArgs e)
        {
            Point relativePoint = this.PointToScreen(Cursor.Position);

            NativeAPI.RadiantAPI_XYMouseRight(true, relativePoint.X, relativePoint.Y);
            NativeAPI.RadiantAPI_RedrawWindows();
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
                _rightMouseButtonTimer.Enabled = false;                
                NativeAPI.RadiantAPI_XYMouseRight(false, e.X, e.Y);                
            }
            else if(e.Button == MouseButtons.Left)
            {
                _leftMouseButtonTimer.Enabled = false;
                NativeAPI.RadiantAPI_XYMouseLeft(false, e.X, e.Y);
            }
        }

        private void XYWndDialog_MouseDown(object sender, MouseEventArgs e)
        {
            Point screenPosition = this.PointToScreen(e.Location);
            if (e.Button == MouseButtons.Right)
            {
                _rightMouseButtonTimer.Enabled = true;                
                _rightMouseButtonEventArgs = e;
            }
            else if(e.Button == MouseButtons.Left)
            {
                _leftMouseButtonTimer.Enabled = true;
                _leftCursorPosition = Cursor.Position;
                _leftMouseButtonEventArgs = e;
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
