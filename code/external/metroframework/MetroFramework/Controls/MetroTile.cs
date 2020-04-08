#region Copyright (c) 2013 Jens Thiel, http://thielj.github.io/MetroFramework
/*
 
MetroFramework - Windows Modern UI for .NET WinForms applications

Copyright (c) 2013 Jens Thiel, http://thielj.github.io/MetroFramework

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in the 
Software without restriction, including without limitation the rights to use, copy, 
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the 
following conditions:

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
Portions of this software are (c) 2011 Sven Walter, http://github.com/viperneo

 */
#endregion

using System;
using System.Diagnostics;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;

using MetroFramework.Drawing;

namespace MetroFramework.Controls
{
    [Designer("MetroFramework.Design.MetroTileDesigner, " + AssemblyRef.MetroFrameworkDesignSN)]
    [ToolboxBitmap(typeof(Button))]
    public partial class MetroTile : MetroButtonBase, IContainerControl
    {

        protected override string MetroControlCategory { get { return "Tile"; } }

        private const int PRESSED_SHRINK = 2;

        #region Interface

        private Control activeControl = null;
        [Browsable(false)]
        [DefaultValue(null)]
        public Control ActiveControl
        {
            get { return activeControl; }
            set { activeControl = value; }
        }

        public bool ActivateControl(Control ctrl)
        {
            if (Controls.Contains(ctrl))
            {
                ctrl.Select();
                activeControl = ctrl;
                return true;
            }

            return false;
        }

        #endregion

        #region Properties

        private bool paintTileCount = true;
        [DefaultValue(true)]
        [Category(MetroDefaults.CatAppearance)]
        public bool PaintTileCount
        {
            get { return paintTileCount; }
            set { paintTileCount = value; Invalidate();}
        }

        private int tileCount = 0;
        [DefaultValue(0)]
        public int TileCount
        {
            get { return tileCount; }
            set { tileCount = value; Invalidate();}
        }

        [DefaultValue(ContentAlignment.BottomLeft)]
        public new ContentAlignment TextAlign
        {
            get { return base.TextAlign; }
            set { base.TextAlign = value; Invalidate();}
        }

        private Image tileImage = null;
        [DefaultValue(null)]
        [Category(MetroDefaults.CatAppearance)]
        public Image TileImage
        {
            get { return tileImage; }
            set { tileImage = value; Invalidate();}
        }

        private bool useTileImage = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseTileImage
        {
            get { return useTileImage; }
            set { useTileImage = value; Invalidate();}
        }

        private ContentAlignment tileImageAlign = ContentAlignment.TopLeft;
        [DefaultValue(ContentAlignment.TopLeft)]
        [Category(MetroDefaults.CatAppearance)]
        public ContentAlignment TileImageAlign
        {
            get { return tileImageAlign; }
            set { tileImageAlign = value; Invalidate();}
        }

        #endregion

        public MetroTile()
        {
            SetStyle(/*ControlStyles.AllPaintingInWmPaint |*/ ControlStyles.OptimizedDoubleBuffer | ControlStyles.ResizeRedraw, true);
            UseTransparency();
            UseSelectable();
            ButtonState.Changed += (s, e) => UpdateClientRectangle();
            UseFontStyle();

            TextAlign = ContentAlignment.BottomLeft;
        }

        void UpdateClientRectangle()
        {
            Padding = new Padding(ButtonState.IsPressed ? PRESSED_SHRINK : 0);
            PerformLayout();
            Invalidate();
        }

        public override Rectangle DisplayRectangle
        {
            get { var r = base.DisplayRectangle; if (ButtonState.IsPressed) r.Inflate(-PRESSED_SHRINK, -PRESSED_SHRINK); return r; }
        }

        #region Paint Methods

        // Override to make the current style color the effective background color
        //public override Color EffectiveBackColor { get { return ShouldSerializeBackColor() ? base.EffectiveBackColor : GetStyleColor(); } }

        protected override void OnPaintForeground(PaintEventArgs e)
        {
            Color foreColor = EffectiveForeColor;
            Color tileColor = GetStyleColor();
            Rectangle buttonRect = DisplayRectangle;

            using (SolidBrush b = new SolidBrush(tileColor))
            {
                e.Graphics.FillRectangle(b, buttonRect);
            }

            if (UseTileImage && TileImage != null)
            {
                e.Graphics.DrawImage(TileImage, buttonRect, TileImageAlign);
            }

            e.Graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            e.Graphics.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;

            if (PaintTileCount && TileCount > 0)
            {
                e.Graphics.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAliasGridFit;
                TextRenderer.DrawText(e.Graphics, TileCount.ToString(), GetThemeFont("Tile.Count"), buttonRect, foreColor,tileColor, TextFormatFlags.Right | TextFormatFlags.Top);
                e.Graphics.TextRenderingHint = System.Drawing.Text.TextRenderingHint.SystemDefault;
            }

            TextRenderer.DrawText(e.Graphics, Text, EffectiveFont, buttonRect, foreColor, tileColor, TextAlign.AsTextFormatFlags() | TextFormatFlags.LeftAndRightPadding | TextFormatFlags.EndEllipsis );
        }

        #endregion

    }
}
