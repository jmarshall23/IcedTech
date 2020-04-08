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

 */
#endregion

using System;
using System.Drawing;
using System.ComponentModel;
using System.Security;
using System.Windows.Forms;

using MetroFramework.Components;
using MetroFramework.Drawing;
using MetroFramework.Interfaces;

namespace MetroFramework.Controls
{
    // NOTE: This won't support transparency

    [Designer("MetroFramework.Design.MetroLabelDesigner, " + AssemblyRef.MetroFrameworkDesignSN)]
    [ToolboxBitmap(typeof(Label))]
    public partial class MetroSelectableLabel : MetroTextBoxBase
    {

        protected override string MetroControlCategory { get { return "Label"; } }

        public MetroSelectableLabel()
        {
            SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.ResizeRedraw, true);
            UseStyleColor();
            UseFontStyle();
            BorderStyle = BorderStyle.None;
            ReadOnly = true;
            //Multiline = true;
        }

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            base.OnMetroStyleChanged(e);
            BaseFont = EffectiveFont;
            Color backColor = EffectiveBackColor;
            if( backColor.A == 255) BaseBackColor = backColor;
            BaseForeColor = EffectiveForeColor;
            if( IsHandleCreated)
                Size = GetPreferredSize(Size.Empty);
        }

        protected override void OnTextChanged(EventArgs e)
        {
            base.OnTextChanged(e);
            if(IsHandleCreated)
                Size = GetPreferredSize(Size.Empty);
        }

        public override Size GetPreferredSize(Size proposedSize)
        {
            using (var g = CreateGraphics())
            {
                proposedSize = new Size(int.MaxValue, int.MaxValue);
                return TextRenderer.MeasureText(g, Text, EffectiveFont, proposedSize, TextAlign.AsTextFormatFlags() | TextFormatFlags.EndEllipsis);
            }
        }

        [SecuritySafeCritical]
        protected override void OnClick(EventArgs e)
        {
            base.OnClick(e);
            Native.WinCaret.HideCaret(Handle);
        }

        [SecuritySafeCritical]
        protected override void OnDoubleClick(EventArgs e)
        {
            SelectAll();
            Native.WinCaret.HideCaret(Handle);
        }

    }
}
