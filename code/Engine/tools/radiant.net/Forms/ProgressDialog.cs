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
    public partial class ProgressDialog : Form
    {
        public bool hasCanceledWork;



        public ProgressDialog()
        {
            InitializeComponent();
            this.FormClosing += EntitySelection_FormClosing;
            hasCanceledWork = false;
        }

        private void EntitySelection_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.Hide();
            e.Cancel = true; // this cancels the close event.
        }

        private void ProgressDialog_Load(object sender, EventArgs e)
        {
            progressBar1.Style = ProgressBarStyle.Continuous;
        }

        public void SetLevelProgressData(string mapName, string workString, float completedPercent)
        {
            CenterToScreen();

            this.Text = string.Format("Please Wait...", mapName);

            LoadingMapNameTextBox.Text = string.Format("Loading {0} Please Wait...", mapName);
            LoadingWorkTextBox.Text = string.Format("{0}...", workString);
            progressBar1.Value = (int)(completedPercent * 100.0f);
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

        private void CancelButton_Click(object sender, EventArgs e)
        {
            hasCanceledWork = true;
        }

        private void LoadingMapNameTextBox_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
