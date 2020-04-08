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
using MetroFramework.Native;

namespace MetroFramework.Controls
{
    [ToolboxBitmap(typeof(ComboBox))]
    public partial class MetroComboBox : MetroComboBoxBase
    {

        #region Fields

        [DefaultValue(DrawMode.OwnerDrawFixed)]
        [Browsable(false)]
        public new DrawMode DrawMode
        {
            get { return DrawMode.OwnerDrawFixed; }
            set { base.DrawMode = DrawMode.OwnerDrawFixed; }
        }

        [DefaultValue(ComboBoxStyle.DropDownList)]
        [Browsable(false)]
        public new ComboBoxStyle DropDownStyle
        {
            get { return ComboBoxStyle.DropDownList; }
            set { base.DropDownStyle = ComboBoxStyle.DropDownList; }
        }

        private string promptText = "";
        [Browsable(true)]
        [DefaultValue("")]
        [Category(MetroDefaults.CatAppearance)]
        public string PromptText
        {
            get { return promptText; }
            set { promptText = value ?? String.Empty; Invalidate(); }
        }

        private bool drawPrompt;

        #endregion

        public MetroComboBox()
        {
            SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.ResizeRedraw, true);
            UseTransparency();
            UseSelectable();

            base.DrawMode = DrawMode.OwnerDrawFixed;
            base.DropDownStyle = ComboBoxStyle.DropDownList;

            //drawPrompt = (SelectedIndex == -1);
        }

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
            ItemHeight = GetPreferredSize(Size.Empty).Height;
        }

        #region Paint Methods

        protected override void OnPaintForeground(PaintEventArgs e)
        {
            Color foreColor = EffectiveForeColor;

            // border
            using (Pen p = new Pen(GetThemeColor("BorderColor")))
            {
                Rectangle boxRect = new Rectangle(0, 0, Width - 1, Height - 1);
                e.Graphics.DrawRectangle(p, boxRect);
            }

            // drop-down arrow
            using (SolidBrush b = new SolidBrush(foreColor))
            {
                e.Graphics.FillPolygon(b, new[]
                    {
                        new Point(Width - 20, (Height / 2) - 2), 
                        new Point(Width - 9, (Height / 2) - 2), 
                        new Point(Width - 15,  (Height / 2) + 4)
                    });
            }

            Rectangle textRect = new Rectangle(2, 2, Width - 20, Height - 4);
            TextRenderer.DrawText(e.Graphics, Text, EffectiveFont, textRect, foreColor, /*EffectiveBackColor,*/ TextFormatFlags.Left | TextFormatFlags.VerticalCenter);

            if (drawPrompt)
            {
                DrawTextPrompt(e.Graphics);
            }
        }

        protected override void OnDrawItem(DrawItemEventArgs e)
        {
            if (e.Index < 0)
            {
                base.OnDrawItem(e);
                return;
            }

            bool normal = e.State == (DrawItemState.NoAccelerator | DrawItemState.NoFocusRect) || e.State == DrawItemState.None;
            // TODO: We need a real background color for transparent drawing
            Color backColor = normal ? EffectiveBackColor : GetStyleColor();
            // replace transparent background colors for drawing items
            if (normal && backColor.A < 255) backColor = GetThemeColor("BackColor");
            // TODO: We need complementing foreground colors for the styles !!
            Color foreColor = normal ? EffectiveForeColor : GetThemeColor("Tile.ForeColor.Normal");

            using (SolidBrush b = new SolidBrush(backColor))
            {
                e.Graphics.FillRectangle(b, e.Bounds);
            }
            TextRenderer.DrawText(e.Graphics, GetItemText(Items[e.Index]), EffectiveFont, e.Bounds, foreColor, backColor, TextFormatFlags.Left | TextFormatFlags.VerticalCenter);
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);

            // AFAIK, OCM_MESSAGES is MFC-specific. The WM_PAINT is already handled by us.
            const int OCM_COMMAND = WinApi.Messages.OCM__BASE + WinApi.Messages.WM_COMMAND;
            //if (((m.Msg == WinApi.Messages.WM_PAINT) || (m.Msg == OCM_COMMAND)) && (drawPrompt))
            //{
            //    DrawTextPrompt();
            //}
        }

        private void DrawTextPrompt()
        {
            using (Graphics graphics = CreateGraphics())
            {
                DrawTextPrompt(graphics);
            }
        }

        private void DrawTextPrompt(Graphics g)
        {
            Rectangle textRect = new Rectangle(2, 2, Width - 20, Height - 4);
            TextRenderer.DrawText(g, promptText, EffectiveFont, textRect, GetThemeColor("ForeColor", "Disabled"),
                /*EffectiveBackColor,*/ TextFormatFlags.Left | TextFormatFlags.VerticalCenter | TextFormatFlags.EndEllipsis);
        }

        #endregion

        #region Overridden Methods

        public override Size GetPreferredSize(Size proposedSize)
        {
            using (var g = CreateGraphics())
            {
                string measureText = Text.Length > 0 ? Text : "MeasureText";
                proposedSize = new Size(int.MaxValue, int.MaxValue);
                Size preferredSize = TextRenderer.MeasureText(g, measureText, EffectiveFont, proposedSize, TextFormatFlags.LeftAndRightPadding );
                preferredSize.Height += 4;
                return preferredSize;
            }
        }

        protected override void OnSelectedIndexChanged(EventArgs e)
        {
            base.OnSelectedIndexChanged(e);
            drawPrompt = (SelectedIndex == -1);
            Invalidate();
        }

        #endregion
    }
}
