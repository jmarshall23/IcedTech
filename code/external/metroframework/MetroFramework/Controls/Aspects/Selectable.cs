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
using System.Windows.Forms;

using MetroFramework.Interfaces;

// ReSharper disable MemberCanBePrivate.Global
// ReSharper disable PossibleNullReferenceException

namespace MetroFramework.Controls
{


    #region MetroButton

    public partial class MetroButton
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        protected Helper.ButtonState ButtonState;
		private void UseSelectable()
		{
			SetStyle( ControlStyles.Selectable, true);
			ButtonState = new Helper.ButtonState(this);
		}
        protected override string MetroControlState { get { return ButtonState != null ? ButtonState.ToString() : base.MetroControlState; } }
    }

    #endregion


    #region MetroCheckBox

    public partial class MetroCheckBox
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        protected Helper.ButtonState ButtonState;
		private void UseSelectable()
		{
			SetStyle( ControlStyles.Selectable, true);
			ButtonState = new Helper.ButtonState(this);
		}
        protected override string MetroControlState { get { return ButtonState != null ? ButtonState.ToString() : base.MetroControlState; } }
    }

    #endregion


    #region MetroComboBox

    public partial class MetroComboBox
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        protected Helper.ButtonState ButtonState;
		private void UseSelectable()
		{
			SetStyle( ControlStyles.Selectable, true);
			ButtonState = new Helper.ButtonState(this);
		}
        protected override string MetroControlState { get { return ButtonState != null ? ButtonState.ToString() : base.MetroControlState; } }
    }

    #endregion


    #region MetroLink

    public partial class MetroLink
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        protected Helper.ButtonState ButtonState;
		private void UseSelectable()
		{
			SetStyle( ControlStyles.Selectable, true);
			ButtonState = new Helper.ButtonState(this);
		}
        protected override string MetroControlState { get { return ButtonState != null ? ButtonState.ToString() : base.MetroControlState; } }
    }

    #endregion


    #region MetroRadioButton

    public partial class MetroRadioButton
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        protected Helper.ButtonState ButtonState;
		private void UseSelectable()
		{
			SetStyle( ControlStyles.Selectable, true);
			ButtonState = new Helper.ButtonState(this);
		}
        protected override string MetroControlState { get { return ButtonState != null ? ButtonState.ToString() : base.MetroControlState; } }
    }

    #endregion


    #region MetroTile

    public partial class MetroTile
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        protected Helper.ButtonState ButtonState;
		private void UseSelectable()
		{
			SetStyle( ControlStyles.Selectable, true);
			ButtonState = new Helper.ButtonState(this);
		}
        protected override string MetroControlState { get { return ButtonState != null ? ButtonState.ToString() : base.MetroControlState; } }
    }

    #endregion


    #region MetroToggle

    public partial class MetroToggle
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        protected Helper.ButtonState ButtonState;
		private void UseSelectable()
		{
			SetStyle( ControlStyles.Selectable, true);
			ButtonState = new Helper.ButtonState(this);
		}
        protected override string MetroControlState { get { return ButtonState != null ? ButtonState.ToString() : base.MetroControlState; } }
    }

    #endregion


    #region MetroTrackBar

    public partial class MetroTrackBar
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

        protected Helper.ButtonState ButtonState;
		private void UseSelectable()
		{
			SetStyle( ControlStyles.Selectable, true);
			ButtonState = new Helper.ButtonState(this);
		}
        protected override string MetroControlState { get { return ButtonState != null ? ButtonState.ToString() : base.MetroControlState; } }
    }

    #endregion


	#region Internal helper class

	namespace Helper
	{
		/// <summary>Tracks selectable controls and assigns them one of 4 button states: Normal, Hover, Press and Disabled.</summary>
		[EditorBrowsable(EditorBrowsableState.Never)]
		public class ButtonState
		{
			private readonly Control _owner;

			public bool IsHovered { get; private set; }
			public bool IsPressed  { get; private set; }
			public bool IsFocused  { get; private set; }

			public event EventHandler Changed;

			public override string ToString()
			{
				if (!_owner.Enabled) return "Disabled"; if (!IsHovered) return "Normal"; return IsPressed ? "Press" : "Hover";
			}

			private void NotifyChanged()
			{
				var ev = Changed;
				if (ev != null) ev(_owner, new EventArgs());
				_owner.Invalidate();
			}

			internal ButtonState(Control owner)
			{
				_owner = owner;

				// _owner.ChangeUICues ??

				// Focus
				_owner.GotFocus += (s, e) => { IsFocused = true; NotifyChanged(); };
				_owner.LostFocus += (s, e) => { IsFocused = IsHovered = IsPressed = false; NotifyChanged(); };
				_owner.Enter += (s, e) => { IsFocused = true; NotifyChanged(); };
				_owner.Leave += (s, e) => { IsFocused = IsHovered = IsPressed = false; NotifyChanged(); };

				// Keys
				_owner.KeyDown += (s, e) => {if (e.KeyCode != Keys.Space) return; IsHovered = IsPressed = true; NotifyChanged(); };
				_owner.KeyUp += (s, e) => { IsHovered = IsPressed = false; NotifyChanged(); };

				// Mouse
				_owner.MouseEnter += (s, e) => { IsHovered = true; NotifyChanged(); };
				_owner.MouseDown += (s, e) => { if (e.Button != MouseButtons.Left) return; IsPressed = true; NotifyChanged(); };
				_owner.MouseUp += (s, e) => { IsPressed = false; NotifyChanged(); };
				_owner.MouseLeave += (s, e) => { IsHovered = false; NotifyChanged(); };

                // DrawFocusRectangle
                _owner.Paint += DrawFocusRectangle;
			    ((IMetroControl)_owner).UserPaint += DrawFocusRectangle;
            }

		    private void DrawFocusRectangle(object s, PaintEventArgs e)
		    {
                if (MetroDefaults.DrawFocusRectangle && IsFocused) 
					ControlPaint.DrawFocusRectangle(e.Graphics, _owner.ClientRectangle);
		    }
		}
	}

	#endregion


}
 
