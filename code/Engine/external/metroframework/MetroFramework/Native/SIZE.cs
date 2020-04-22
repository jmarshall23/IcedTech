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
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace MetroFramework.Native
{
    //  JT: I think these are obsolete - it should be possible to use the equivalent struct from System.Drawing

    [DebuggerDisplay("({Width},{Height})")]
    [StructLayout(LayoutKind.Sequential)]
    [Obsolete("Use System.Drawing.Size")]
    public struct SIZE
    {
        public int Width;
        public int Height;

        public static SIZE Empty;

        public SIZE(int width, int height)
        {
            Width = width;
            Height = height;
        }

        public SIZE(Size other)
        {
            Width = other.Width;
            Height = other.Height;
        }

        public Size ToSize()
        {
            return new Size(Width, Height);
        }

        public static implicit operator SIZE(Size other)
        {
            return new SIZE(other);
        }

        public static implicit operator Size(SIZE other)
        {
            return other.ToSize();
        }
    }
}