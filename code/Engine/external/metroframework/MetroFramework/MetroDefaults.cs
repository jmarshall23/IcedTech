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
using MetroFramework.Drawing;
using MetroFramework.Forms;

namespace MetroFramework
{
    internal static class MetroDefaults
    {
        // NOTE: Do NOT set to MetroStyleManager.AMBIENT_VALUE !
        public const string Theme = "Light";

        // NOTE: Do NOT set to MetroStyleManager.AMBIENT_VALUE !
        public const string Style = "Blue";

        public const MetroBorderStyle BorderStyle = MetroBorderStyle.None;

        // This will massively reduce flicker by disabling redrawing during resize
        public static bool FormSuspendLayoutDuringResize = false;

        public const MetroForm.MetroFormShadowType FormShadowType = MetroForm.MetroFormShadowType.SystemAeroShadow;

        public static bool DrawFocusRectangle = false;

        // Font fallbacks
        public const MetroFontSize MetroFontSize = MetroFramework.Drawing.MetroFontSize.Medium;
        public const MetroFontWeight MetroFontWeight = MetroFramework.Drawing.MetroFontWeight.Regular;
        public static readonly string FontFamily = SystemFonts.DefaultFont.Name;
        public static readonly FontStyle FontStyle = SystemFonts.DefaultFont.Style;
        public const float FontSize = 14f;

        // Categories

        public const string CatAppearance = "Metro Appearance";
        public const string CatBehavior = "Metro Behavior";

    }
}
