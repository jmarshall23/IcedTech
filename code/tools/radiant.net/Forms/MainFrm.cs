using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MetroFramework.Components;
using MetroFramework.Forms;

namespace radiant.net.forms
{
    public partial class MainFrm : MetroForm
    {
        public MainFrm()
        {
            InitializeComponent();
            this.KeyPreview = true;
            this.StyleManager = metroStyleManager1;

            this.KeyUp += MainFrm_KeyUp;
            this.KeyDown += MainFrm_KeyDown;

            this.Load += MainFrm_Load1;
            this.FormClosed += MainFrm_FormClosed;
        }

        private void MainFrm_Load1(object sender, EventArgs e)
        {
            this.Text = "iceStudio64 - " + NativeAPI.RadiantAPI_GetGameNameManaged();
        }

        private void MainFrm_KeyDown(object sender, KeyEventArgs e)
        {
            if (Editor.xyWndDialog.IsGamePreviewVisible())
            {
                NativeAPI.RadiantAPI_GameWindowKeyboard(RadiantHelpers.GetKeyAscii2(e), true);
                return;
            }

            NativeAPI.RadaintAPI_KeyEvent(RadiantHelpers.GetKeyAscii(e), true);
        }

        private void MainFrm_KeyUp(object sender, KeyEventArgs e)
        {
            if (Editor.xyWndDialog.IsGamePreviewVisible())
            {
                NativeAPI.RadiantAPI_GameWindowKeyboard(RadiantHelpers.GetKeyAscii2(e), false);
                return;
            }
            NativeAPI.RadaintAPI_KeyEvent(RadiantHelpers.GetKeyAscii(e), false);
        }

        private void MainFrm_FormClosed(object sender, FormClosedEventArgs e)
        {
           NativeAPI.RadiantAPI_ProcessMenuCommands((int)radiant.net.Shared.MenuCommands.RadiantNetMenuCommands_t.RADIANT_NET_COMMAND_EXIT);
        }

        public System.Windows.Forms.SplitContainer GetMainSplitContainer()
        {
            return MainSplitContainer;
        }

        public System.Windows.Forms.SplitContainer GetLeftSplitContainer()
        {
            return leftSplitContainer;
        }

        private void MainFrm_Load(object sender, EventArgs e)
        {

        }
    }
}
