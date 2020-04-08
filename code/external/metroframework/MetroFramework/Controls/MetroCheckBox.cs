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
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;

using MetroFramework.Drawing;

namespace MetroFramework.Controls
{
    [ToolboxBitmap(typeof(CheckBox))]
    [Designer("MetroFramework.Design.MetroCheckBoxDesigner, " + AssemblyRef.MetroFrameworkDesignSN)]
    public partial class MetroCheckBox : MetroCheckBoxBase
    {

        public MetroCheckBox()
        {
            SetStyle( ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.ResizeRedraw, true);
            UseTransparency();
            UseSelectable();
            UseStyleColor();
            UseFontStyle();
        }

        protected override void OnPaintForeground(PaintEventArgs e)
        {
            using (Pen p = new Pen(GetThemeColor("BorderColor")))
            {
                Rectangle boxRect = new Rectangle(0, Height / 2 - 6, 12, 12);
                e.Graphics.DrawRectangle(p, boxRect);
            }

            if (Checked)
            {
                Color fillColor = CheckState == CheckState.Indeterminate ? GetThemeColor("BorderColor") : GetStyleColor();
                using (SolidBrush b = new SolidBrush(fillColor))
                {
                    Rectangle boxRect = new Rectangle(2, Height / 2 - 4, 9, 9);
                    e.Graphics.FillRectangle(b, boxRect);
                }
            }

            Rectangle textRect = new Rectangle(16, 0, Width - 16, Height);
            TextRenderer.DrawText(e.Graphics, Text, EffectiveFont, textRect, EffectiveForeColor, TextAlign.AsTextFormatFlags() | TextFormatFlags.EndEllipsis);
        }

        protected override void OnCheckedChanged(EventArgs e)
        {
            base.OnCheckedChanged(e);
            Invalidate();
        }

        public override Size GetPreferredSize(Size proposedSize)
        {
            using (var g = CreateGraphics())
            {
                proposedSize = new Size(int.MaxValue, int.MaxValue);
                Size preferredSize = TextRenderer.MeasureText(g, Text, EffectiveFont, proposedSize, TextAlign.AsTextFormatFlags() | TextFormatFlags.EndEllipsis);
                preferredSize.Width += 16;
                return preferredSize;
            }

        }

    }
}
