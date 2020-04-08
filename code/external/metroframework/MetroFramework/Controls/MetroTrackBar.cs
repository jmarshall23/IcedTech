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

namespace MetroFramework.Controls
{
    [ToolboxBitmap(typeof(TrackBar))]
    [DefaultEvent("Scroll")]
    public partial class MetroTrackBar : MetroControlBase
    {

        protected override string MetroControlCategory { get { return "TrackBar"; } }

        #region Events

        public event EventHandler ValueChanged;

        private void OnValueChanged()
        {
            var ev = ValueChanged;
            if (ev != null) ev(this, EventArgs.Empty);
        }

        public event ScrollEventHandler Scroll;

        private void OnScroll(ScrollEventType scrollType, int newValue)
        {
            var ev = Scroll;
            if (ev != null) ev(this, new ScrollEventArgs(scrollType, newValue));
        }

        #endregion

        private const int DEFAULT_VALUE = 50;
        private const int DEFAULT_MINIMUM = 0;
        private const int DEFAULT_MAXIMUM = 100;

        #region Properties

        private int trackerValue = DEFAULT_VALUE;
        [DefaultValue(DEFAULT_VALUE)]
        public int Value
        {
            get { return trackerValue; }
            set
            {
                if (!(value >= barMinimum & value <= barMaximum)) throw new ArgumentOutOfRangeException("Value is outside appropriate range (min, max)");
                if (trackerValue != value)
                {
                    trackerValue = value;
                    OnValueChanged();
                    Invalidate();
                }
            }
        }

        private int barMinimum = DEFAULT_MINIMUM;
        [DefaultValue(DEFAULT_MINIMUM)]
        public int Minimum
        {
            get { return barMinimum; }
            set
            {
                if (value >= barMaximum) throw new ArgumentOutOfRangeException("Minimal value is greather than maximal one");
                barMinimum = value;
                if (Value < barMinimum) Value = barMinimum;
                Invalidate();
            }
        }


        private int barMaximum = DEFAULT_MAXIMUM;
        [DefaultValue(DEFAULT_MAXIMUM)]
        public int Maximum
        {
            get { return barMaximum; }
            set
            {
                if (value <= barMinimum) throw new ArgumentOutOfRangeException("Maximal value is lower than minimal one");
                
                barMaximum = value;
                if (Value > barMaximum) Value = barMaximum;
                Invalidate();
            }
        }

        private int smallChange = 1;
        [DefaultValue(1)]
        public int SmallChange 
        {
            get { return smallChange; }
            set { smallChange = value; }
        }

        private int largeChange = 5;
        [DefaultValue(5)]
        public int LargeChange
        {
            get { return largeChange; }
            set { largeChange = value; }
        }

        private int mouseWheelBarPartitions = 10;
        [DefaultValue(10)]
        public int MouseWheelBarPartitions
        {
            get { return mouseWheelBarPartitions; }
            set
            {
                if (value <= 0) throw new ArgumentOutOfRangeException("MouseWheelBarPartitions has to be greather than zero");
                mouseWheelBarPartitions = value;
            }
        }

        #endregion

        public MetroTrackBar()
        {
            SetStyle(ControlStyles.AllPaintingInWmPaint | 
                     ControlStyles.OptimizedDoubleBuffer |
                     ControlStyles.ResizeRedraw | 
                     ControlStyles.UserMouse , true);
            UseTransparency();
            UseStyleColor();
            UseSelectable();
        }

        public MetroTrackBar(int min, int max, int value) : this()
        {
            barMinimum = min;
            barMaximum = max;
            trackerValue = value;
        }

        #region Paint Methods

        protected override void OnPaintForeground(PaintEventArgs e)
        {
            DrawTrackBar(e.Graphics, EffectiveForeColor, GetThemeColor("Bar.BackColor"));
        }

        private void DrawTrackBar(Graphics g, Color thumbColor, Color barColor)
        {
            int trackX = (((trackerValue - barMinimum) * (Width - 6)) / (barMaximum - barMinimum));

            using (SolidBrush b = new SolidBrush(thumbColor))
            {
                Rectangle barRect = new Rectangle(0, Height / 2 - 2, trackX, 4);
                g.FillRectangle(b, barRect);

                Rectangle thumbRect = new Rectangle(trackX, Height / 2 - 8, 6, 16);
                g.FillRectangle(b, thumbRect);
            }

            using (SolidBrush b = new SolidBrush(barColor))
            {
                Rectangle barRect = new Rectangle(trackX + 7, Height / 2 - 2, Width - trackX + 7, 4);
                g.FillRectangle(b, barRect);
            }
        }

        #endregion

        #region Event handlers

        protected override bool ProcessDialogKey(Keys keyData)
        {
            if (keyData == Keys.Tab | ModifierKeys == Keys.Shift)
                return base.ProcessDialogKey(keyData);

            OnKeyDown(new KeyEventArgs(keyData));
            return true;
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            base.OnKeyUp(e);

            switch (e.KeyCode)
            {
                case Keys.Down:
                case Keys.Left:
                    SetProperValue(Value - (int)smallChange);
                    OnScroll(ScrollEventType.SmallDecrement, Value);
                    break;
                case Keys.Up:
                case Keys.Right:
                    SetProperValue(Value + (int)smallChange);
                    OnScroll(ScrollEventType.SmallIncrement, Value);
                    break;
                case Keys.Home:
                    Value = barMinimum;
                    break;
                case Keys.End:
                    Value = barMaximum;
                    break;
                case Keys.PageDown:
                    SetProperValue(Value - (int)largeChange);
                    OnScroll(ScrollEventType.LargeDecrement, Value);
                    break;
                case Keys.PageUp:
                    SetProperValue(Value + (int)largeChange);
                    OnScroll(ScrollEventType.LargeIncrement, Value);
                    break;
            }
            
            if (Value == barMinimum)
                OnScroll(ScrollEventType.First, Value);

            if (Value == barMaximum)
                OnScroll(ScrollEventType.Last, Value);

            Point pt = PointToClient(Cursor.Position);
            OnMouseMove(new MouseEventArgs(MouseButtons.None, 0, pt.X, pt.Y, 0));
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            if (e.Button != MouseButtons.Left) return;

            Capture = true;
            OnScroll(ScrollEventType.ThumbTrack, trackerValue);
            OnValueChanged();
            OnMouseMove(e);
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (!(Capture & e.Button == MouseButtons.Left)) return;

            ScrollEventType set = ScrollEventType.ThumbPosition;
            Point pt = e.Location;
            int p = pt.X;
                
            float coef = (float)(barMaximum - barMinimum) / (float)(ClientSize.Width - 3);
            trackerValue = (int)(p * coef + barMinimum);

            if (trackerValue <= barMinimum)
            {
                trackerValue = barMinimum;
                set = ScrollEventType.First;
            }
            else if (trackerValue >= barMaximum)
            {
                trackerValue = barMaximum;
                set = ScrollEventType.Last;
            }

            OnScroll(set, trackerValue);
            OnValueChanged();

            Invalidate();
        }

        protected override void OnMouseWheel(MouseEventArgs e)
        {
            base.OnMouseWheel(e);
            int v = e.Delta / 120 * (barMaximum - barMinimum) / mouseWheelBarPartitions;
            SetProperValue(Value + v);
        }

        #endregion

        #region Helper Methods

        private void SetProperValue(int val)
        {
            if (val < barMinimum) Value = barMinimum;
            else if (val > barMaximum) Value = barMaximum;
            else Value = val;
        }

        #endregion
    }
}
