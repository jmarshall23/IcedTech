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

 * */
#endregion

#region Copyright (c) 2009, Yves Goergen
// Copyright (c) 2009, Yves Goergen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this list of conditions
//   and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this list of
//   conditions and the following disclaimer in the documentation and/or other materials provided
//   with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
#endregion

using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace MetroFramework.Controls
{
    [Designer("MetroFramework.Design.MetroProgressSpinnerDesigner, " + AssemblyRef.MetroFrameworkDesignSN)]
    [ToolboxBitmap(typeof(ProgressBar))]
    public partial class MetroProgressSpinner : MetroControlBase
    {

        #region Fields

        private readonly Timer timer;
        private int progress;
        private float angle = 270;

		/// <summary>
		/// Gets or sets a value indicating whether the progress spinner is spinning.
		/// </summary>
		[DefaultValue(true)]
		[Description("Specifies whether the progress spinner is spinning.")]
        [Category(MetroDefaults.CatBehavior)]
        public bool Spinning
        {
            get { return timer.Enabled; }
            set { timer.Enabled = value; }
        }

		/// <summary>
		/// Gets or sets the current progress value. Set -1 to indicate that the current progress is unknown.
		/// </summary>
		[DefaultValue(0)]
		[Description("The current progress value. Set -1 to indicate that the current progress is unknown.")]
        [Category(MetroDefaults.CatAppearance)]
        public int Value
        {
            get { return progress; }
            set
            {
                if (value != -1 && (value < minimum || value > maximum))
                    throw new ArgumentOutOfRangeException("Progress value must be -1 or between Minimum and Maximum.");
                progress = value;
                Refresh();
            }
        }

        private int minimum = 0;
		/// <summary>
		/// Gets or sets the minimum progress value.
		/// </summary>
		[DefaultValue(0)]
		[Description("The minimum progress value.")]
        [Category(MetroDefaults.CatAppearance)]
        public int Minimum
        {
            get { return minimum; }
            set
            {
                if (value < 0)
                    throw new ArgumentOutOfRangeException("Minimum value must be >= 0.");
                if (value >= maximum)
                    throw new ArgumentOutOfRangeException("Minimum value must be < Maximum.");
                minimum = value;
                if (progress != -1 && progress < minimum)
                    progress = minimum;
                Refresh();
            }
        }

        private int maximum = 100;
		/// <summary>
		/// Gets or sets the maximum progress value.
		/// </summary>
		[DefaultValue(0)]
		[Description("The maximum progress value.")]
        [Category(MetroDefaults.CatAppearance)]
        public int Maximum
        {
            get { return maximum; }
            set
            {
                if (value <= minimum)
                    throw new ArgumentOutOfRangeException("Maximum value must be > Minimum.");
                maximum = value;
                if (progress > maximum)
                    progress = maximum;
                Refresh();
            }
        }

        private bool ensureVisible = true;
		/// <summary>
		/// Gets or sets a value indicating whether the progress spinner should be visible at all progress values.
		/// </summary>
		[DefaultValue(true)]
		[Description("Specifies whether the progress spinner should be visible at all progress values.")]
        [Category(MetroDefaults.CatAppearance)]
        public bool EnsureVisible
        {
            get { return ensureVisible; }
            set { ensureVisible = value; Refresh(); }
        }

        private float speed = 1f;
		/// <summary>
		/// Gets or sets the speed factor. 1 is the original speed, less is slower, greater is faster.
		/// </summary>
		[DefaultValue(1f)]
		[Description("The speed factor. 1 is the original speed, less is slower, greater is faster.")]
        [Category(MetroDefaults.CatBehavior)]
        public float Speed
        {
            get { return speed; }
            set
            {
                if (value <= 0 || value > 10)
                    throw new ArgumentOutOfRangeException("Speed value must be > 0 and <= 10.");
                speed = value;
            }
        }

        private bool backwards;
		/// <summary>
		/// Gets or sets a value indicating whether the progress spinner should spin anti-clockwise.
		/// </summary>
		[DefaultValue(false)]
		[Description("Specifies whether the progress spinner should spin anti-clockwise.")]
        [Category(MetroDefaults.CatBehavior)]
        public bool Backwards
        {
            get { return backwards; }
            set { backwards = value; Refresh(); }
        }

        #endregion

        public MetroProgressSpinner()
        {
            SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer, true);
            UseTransparency();

            Width = 16;
            Height = 16;
            timer = new Timer { Interval = 20, Enabled = true };
        }

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
            if (!DesignMode) timer.Tick += (s, a) => { angle += speed * (backwards ? -6f : 6f); Invalidate(); };
        }

        #region Public Methods

		/// <summary>
		/// Resets the progress spinner's status.
		/// </summary>
        public void Reset()
        {
            progress = minimum;
            angle = 270;
            Refresh();
        }

        #endregion

        #region Paint Methods

        // override to always use style color
        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

        protected override void OnPaintForeground(PaintEventArgs e)
        {
            using (Pen forePen = new Pen(EffectiveForeColor, (float)Width / 5))
            {
                int padding = (int)Math.Ceiling((float)Width / 10);

                e.Graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;

                // Draw spinner pie
                if (progress != -1)
                {
				    // We have a progress value, draw a solid arc line
				    // angle is the back end of the line.
				    // angle +/- progress is the front end of the line
				
                    float sweepAngle;
                    float progFrac = (float)(progress - minimum) / (float)(maximum - minimum);

                    if (ensureVisible)
                        sweepAngle = 30 + 300f * progFrac;
                    else
                        sweepAngle = 360f * progFrac;
                    if (backwards)
                        sweepAngle = -sweepAngle;


                    e.Graphics.DrawArc(forePen, padding, padding, Width - 2 * padding - 1, Height - 2 * padding - 1, angle, sweepAngle);
                }
                else
                {
				    // No progress value, draw a gradient arc line
				    // angle is the opaque front end of the line
				    // angle +/- 180° is the transparent tail end of the line

                    const int maxOffset = 180;
                    for (int offset = 0; offset <= maxOffset; offset += 15)
                    {
                        int alpha = 290 - (offset * 290 / maxOffset);

                        if (alpha > 255)
                            alpha = 255;
                        else if (alpha < 0)
                            alpha = 0;
                        Color col = Color.FromArgb(alpha, forePen.Color);
                        using (Pen gradPen = new Pen(col, forePen.Width))
                        {
                            float startAngle = angle + (offset - (ensureVisible ? 30 : 0)) * (backwards ? 1 : -1);
                            float sweepAngle = 15 * (backwards ? 1 : -1);	// draw in reverse direction
                            e.Graphics.DrawArc(gradPen, padding, padding, Width - 2 * padding - 1, Height - 2 * padding - 1, startAngle, sweepAngle);
                        }
                    }
                }
            }
        }

        #endregion

    }
}
