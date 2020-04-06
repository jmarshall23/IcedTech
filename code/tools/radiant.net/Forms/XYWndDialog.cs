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
using System.Windows.Input;
using radiant.net.Shared;

namespace radiant.net.forms
{
    public partial class XYWndDialog : Form
    {
        Timer _leftMouseButtonTimer;
        Timer _rightMouseButtonTimer;
        Timer _gameMouseMoveTimer;

        MouseEventArgs _leftMouseButtonEventArgs;
        Point _leftCursorPosition;

        MouseEventArgs _rightMouseButtonEventArgs;

        Point _lastScreenPosition = new Point(0, 0);

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

            GamePanel.MouseDown += GamePanel_MouseDown;
            GamePanel.MouseUp += GamePanel_MouseUp;

            tabControl.SelectedIndexChanged += TabControl_SelectedIndexChanged;
            tabControl.KeyDown += TabControl_KeyDown;

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

            _gameMouseMoveTimer = new Timer();
            _gameMouseMoveTimer.Interval = 3;
            _gameMouseMoveTimer.Enabled = true;
            _gameMouseMoveTimer.Tick += GameMouseMoveTimer_Tick;

            UpdateTreeView();
        }

        private void GamePanel_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                NativeAPI.RadiantAPI_GameWindowMouseKey(false, true, false);
            }
            else if (e.Button == MouseButtons.Right)
            {
                NativeAPI.RadiantAPI_GameWindowMouseKey(true, false, false);
            }
        }

        private void GamePanel_MouseDown(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Left)
            {
                NativeAPI.RadiantAPI_GameWindowMouseKey(false, true, true);
            }
            else if(e.Button == MouseButtons.Right)
            {
                NativeAPI.RadiantAPI_GameWindowMouseKey(true, false, true);
            }
        }

        private void GameMouseMoveTimer_Tick(object sender, EventArgs e)
        {
            if (!NativeAPI.RadiantAPI_GameMouseFocus())
            {
                Cursor.Clip = Screen.PrimaryScreen.Bounds;
                return;
            }

            if (tabControl.SelectedIndex != 2)
            {
                Cursor.Clip = Screen.PrimaryScreen.Bounds;
                return;
            }

            Point screenGamePanelPosition = this.PointToScreen(GamePanel.Location);

            int mouseX = Cursor.Position.X - (screenGamePanelPosition.X + 300);
            int mouseY = Cursor.Position.Y - (screenGamePanelPosition.Y + 300);

            NativeAPI.RadiantAPI_GameWindowMouseMovement(mouseX, mouseY);

            Point newCursorPosition = screenGamePanelPosition;
            newCursorPosition.X += 300;
            newCursorPosition.Y += 300;
            Cursor.Position = newCursorPosition;
        }

        private void TabControl_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Left || e.KeyCode == Keys.Right)
            {
                e.Handled = true;
            }
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

        private void TabControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            int index = (sender as TabControl).SelectedIndex;

            if(index == 2)
            {
                NativeAPI.RadiantAPI_ShowDoom(true);
            }
            else
            {
                NativeAPI.RadiantAPI_ShowDoom(false);
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
            if (tabControl.SelectedIndex == 2)
            {
                NativeAPI.RadiantAPI_GameWindowKeyboard(RadiantHelpers.GetKeyAscii2(e), false);
                return;
            }
            NativeAPI.RadaintAPI_KeyEvent(RadiantHelpers.GetKeyAscii(e), false);
        }

        private void XYWndDialog_KeyDown(object sender, KeyEventArgs e)
        {
            if (tabControl.SelectedIndex == 2)
            {
                NativeAPI.RadiantAPI_GameWindowKeyboard(RadiantHelpers.GetKeyAscii2(e), true);
                return;
            }

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

        public IntPtr GetGameWndHWND()
        {
            return GamePanel.Handle;
        }

        public int GetGameWndWidth()
        {
            return GamePanel.Width;
        }

        public int GetGameWndHeight()
        {
            return GamePanel.Height;
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
            NativeAPI.SetWindowTheme(Handle, "explorer", null);
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

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_OPEN);
        }

        private void undotCtrlZToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void toolSaveStripButton_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_SAVE);
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_OPEN);
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_SAVEAS);
        }

        private void topToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_NEW);
        }

        private void saveSelectedToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_SAVESELECTION);
        }

        private void saveRegionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_SAVEREGION);
        }

        private void newProjectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_NEWPROJECT);
        }

        private void loadprojectToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_LOADPROJECT);
        }

        private void projectSettingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_PROJECTSETTINGS);
        }

        private void pointfileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_POINTFILE);
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_ProcessMenuCommands((int)MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_EXIT);
        }
    }
}
