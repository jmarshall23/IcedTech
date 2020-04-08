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

/**************************************************************************************

                        GENERATED FILE - DO NOT EDIT

 **************************************************************************************/

 
 
 

using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Windows.Forms;
using MetroFramework.Drawing;

using MetroFramework.Interfaces;

// ReSharper disable MemberCanBePrivate.Global
// ReSharper disable PossibleNullReferenceException


#region MetroPanel

namespace MetroFramework.Controls
{

    public partial class MetroPanel
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        /// <summary>
        ///     A border drawn inside the client area.
        /// </summary>
        /// <remarks>
        ///     This currently only supports <see cref="MetroBorderStyle.None"/> (the default) and 
        ///     <see cref="MetroBorderStyle.FixedSingle"/> (a thin grey line).
        /// </remarks>
        private MetroBorderStyle _borderStyle = MetroBorderStyle.Default;
        [DefaultValue(MetroBorderStyle.Default)]
        [Browsable(true)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new MetroBorderStyle BorderStyle
#pragma warning restore 109
        {
            get { return _borderStyle; } set { _borderStyle = value; Invalidate(); }
        }

		private void UseBorderStyle()
		{
			// ideally, we should be painting the border with the background...
			//Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
            Paint += DrawBorder;
			UserPaint += DrawBorder;
		}

        private void DrawBorder(Object sender, PaintEventArgs e)
        {
			MetroBorderStyle bs = BorderStyle;
            if (bs == MetroBorderStyle.Default && ! TryGetThemeProperty("BorderStyle", out bs) ) return;
            if (bs != MetroBorderStyle.FixedSingle) return;
            using (Pen pen = new Pen(GetThemeColor("BorderColor")))
                e.Graphics.DrawRectangle(pen, 0, 0, Width-1, Height-1);
		}

    }

}

#endregion


#region MetroUserControl

namespace MetroFramework.Controls
{

    public partial class MetroUserControl
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        /// <summary>
        ///     A border drawn inside the client area.
        /// </summary>
        /// <remarks>
        ///     This currently only supports <see cref="MetroBorderStyle.None"/> (the default) and 
        ///     <see cref="MetroBorderStyle.FixedSingle"/> (a thin grey line).
        /// </remarks>
        private MetroBorderStyle _borderStyle = MetroBorderStyle.Default;
        [DefaultValue(MetroBorderStyle.Default)]
        [Browsable(true)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new MetroBorderStyle BorderStyle
#pragma warning restore 109
        {
            get { return _borderStyle; } set { _borderStyle = value; Invalidate(); }
        }

		private void UseBorderStyle()
		{
			// ideally, we should be painting the border with the background...
			//Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
            Paint += DrawBorder;
			UserPaint += DrawBorder;
		}

        private void DrawBorder(Object sender, PaintEventArgs e)
        {
			MetroBorderStyle bs = BorderStyle;
            if (bs == MetroBorderStyle.Default && ! TryGetThemeProperty("BorderStyle", out bs) ) return;
            if (bs != MetroBorderStyle.FixedSingle) return;
            using (Pen pen = new Pen(GetThemeColor("BorderColor")))
                e.Graphics.DrawRectangle(pen, 0, 0, Width-1, Height-1);
		}

    }

}

#endregion


 
