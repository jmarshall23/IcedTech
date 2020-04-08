using System;
using System.Drawing;
using System.Globalization;
using System.Threading;
using System.Linq;
using System.Windows.Forms;
using MetroFramework.Components;
using MetroFramework.Forms;

namespace MetroFramework.Demo
{

    public partial class MainForm : MetroForm
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void metroTileSwitch_Click(object sender, EventArgs e)
        {
            metroTileSwitch.TileCount++;
            Random rng = new Random();
            var styles = MetroStyleManager.Styles.Styles.Keys;
            while (true)
            {
                string newStyle = styles.ElementAt(rng.Next(styles.Count));
                if (newStyle == metroStyleManager.Style) continue;
                metroStyleManager.Style = newStyle;
                return;
            }
        }

        private void metroTile1_Click(object sender, EventArgs e)
        {
            metroTile1.TileCount++;
            var rng = new Random();
            var themes = MetroStyleManager.Styles.Themes.Keys;
            while (true)
            {
                string newTheme = themes.ElementAt(rng.Next(themes.Count));
                if( newTheme == metroStyleManager.Theme) continue;
                metroStyleManager.Theme = newTheme;
                return;
            }
        }

        // I'm deliberately NOT using a Forms timer to have callbacks from the "wrong" thread
        private System.Threading.Timer timer;

        private void EnsureTimer()
        {
             if(timer != null ) return;
            timer = new System.Threading.Timer(RandomizeGlobalStyles);
        }

        private void RandomizeGlobalStyles(object state)
        {
            Random rng = new Random();

            var themes = MetroStyleManager.Styles.Themes.Keys;
            MetroStyleManager.Default.Theme = themes.ElementAt(rng.Next(themes.Count));

            var styles = MetroStyleManager.Styles.Styles.Keys;
            MetroStyleManager.Default.Style = styles.ElementAt(rng.Next(styles.Count));
        }


        private void metroToggle4_CheckedChanged(object sender, EventArgs e)
        {
            EnsureTimer();
            long interval = metroToggle4.Checked ? 5000 : -1;
            timer.Change(0, interval);
        }

    }
}
