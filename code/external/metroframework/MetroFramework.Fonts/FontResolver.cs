/*
 
MetroFramework - Modern UI for WinForms

Copyright (c) 2013 Jens Thiel, http://thielj.github.io/MetroFramework
Portions of this software are Copyright (c) 2011 Sven Walter, http://github.com/viperneo

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

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Text;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

// NOTE: The namespace and classname must match the string specified in 
//      
//       AssemblyRef.MetroFrameworkFontResolver

namespace MetroFramework.Fonts
{
    /// <summary>
    ///     Replace Sgoe UI font family with a free font (Open Sans).
    /// </summary>
    /// <remarks>
    ///     NOTE: The namespace and typename must match the string defined in
    ///     MetroFramework/Properties/AssemblyInfo.cs (see <see cref="AssemblyRef.MetroFrameworkFontResolver"/>).
    /// </remarks>
    public class FontResolver : MetroFonts.IMetroFontResolver
    {
        public Font ResolveFont(string familyName, float emSize, FontStyle fontStyle, GraphicsUnit unit)
        {
            Font fontTester = new Font(familyName, emSize, fontStyle, unit);
            if (fontTester.Name == familyName || ! TryResolve(ref familyName, ref fontStyle) )
            {
                return fontTester;
            }
            fontTester.Dispose();

            FontFamily fontFamily = GetFontFamily(familyName);
            return new Font(fontFamily, emSize, fontStyle, unit);
        }

        private const string OPEN_SANS_REGULAR = "Open Sans";
        private const string OPEN_SANS_LIGHT = "Open Sans Light";
        private const string OPEN_SANS_BOLD = "Open Sans Bold";

        private readonly PrivateFontCollection fontCollection = new PrivateFontCollection();

        private static bool TryResolve(ref string familyName, ref FontStyle fontStyle)
        {
            if (familyName == "Segoe UI Light")
            {
                familyName = OPEN_SANS_LIGHT;
                if( fontStyle != FontStyle.Bold) fontStyle = FontStyle.Regular;
                return true;
            }

            if (familyName == "Segoe UI")
            {
                if (fontStyle == FontStyle.Bold)
                {
                    familyName = OPEN_SANS_BOLD;
                    return true;
                }

                familyName = OPEN_SANS_REGULAR;
                return true;
            }

            return false;
        }

        private FontFamily GetFontFamily(string familyName)
        {
            lock (fontCollection)
            {
                foreach (FontFamily fontFamily in fontCollection.Families)
                    if (fontFamily.Name == familyName) return fontFamily;

                string resourceName = GetType().Namespace + ".Resources." + familyName.Replace(' ', '_') + ".ttf";

                Stream fontStream = null;
                IntPtr data = IntPtr.Zero;
                try
                {
                    fontStream = GetType().Assembly.GetManifestResourceStream(resourceName);
                    int bytes = (int)fontStream.Length;
                    data = Marshal.AllocCoTaskMem(bytes);
                    byte[] fontdata = new byte[bytes];
                    fontStream.Read(fontdata, 0, bytes);
                    Marshal.Copy(fontdata, 0, data, bytes);
                    fontCollection.AddMemoryFont(data, bytes);
                    return fontCollection.Families[fontCollection.Families.Length - 1];
                }
                finally
                {
                    if (fontStream != null) fontStream.Dispose();
                    if (data != IntPtr.Zero) Marshal.FreeCoTaskMem(data);
                }
            }
        }



    }
}
