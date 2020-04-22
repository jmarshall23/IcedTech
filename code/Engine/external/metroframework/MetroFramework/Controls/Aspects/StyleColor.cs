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

// ReSharper disable MemberCanBePrivate.Global
// ReSharper disable PossibleNullReferenceException

namespace MetroFramework.Controls
{

	/**************************************************************************************

							GENERATED FILE - DO NOT EDIT

	 **************************************************************************************/


    #region MetroCheckBox

    public partial class MetroCheckBox
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		private bool _useStyleColors = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseStyleColors
        {
            get { return _useStyleColors; } set { _useStyleColors = value; Invalidate(); }
        }

		[Conditional("DEBUG")]
		private void UseStyleColor()
		{
			// nothing
		}

        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || !UseStyleColors || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

    }

    #endregion


    #region MetroLabel

    public partial class MetroLabel
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		private bool _useStyleColors = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseStyleColors
        {
            get { return _useStyleColors; } set { _useStyleColors = value; Invalidate(); }
        }

		[Conditional("DEBUG")]
		private void UseStyleColor()
		{
			// nothing
		}

        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || !UseStyleColors || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

    }

    #endregion


    #region MetroLink

    public partial class MetroLink
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		private bool _useStyleColors = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseStyleColors
        {
            get { return _useStyleColors; } set { _useStyleColors = value; Invalidate(); }
        }

		[Conditional("DEBUG")]
		private void UseStyleColor()
		{
			// nothing
		}

        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || !UseStyleColors || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

    }

    #endregion


    #region MetroRadioButton

    public partial class MetroRadioButton
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		private bool _useStyleColors = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseStyleColors
        {
            get { return _useStyleColors; } set { _useStyleColors = value; Invalidate(); }
        }

		[Conditional("DEBUG")]
		private void UseStyleColor()
		{
			// nothing
		}

        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || !UseStyleColors || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

    }

    #endregion


    #region MetroSelectableLabel

    public partial class MetroSelectableLabel
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		private bool _useStyleColors = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseStyleColors
        {
            get { return _useStyleColors; } set { _useStyleColors = value; Invalidate(); }
        }

		[Conditional("DEBUG")]
		private void UseStyleColor()
		{
			// nothing
		}

        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || !UseStyleColors || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

    }

    #endregion


    #region MetroTabControl

    public partial class MetroTabControl
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		private bool _useStyleColors = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseStyleColors
        {
            get { return _useStyleColors; } set { _useStyleColors = value; Invalidate(); }
        }

		[Conditional("DEBUG")]
		private void UseStyleColor()
		{
			// nothing
		}

        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || !UseStyleColors || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

    }

    #endregion


    #region MetroTextBox

    public partial class MetroTextBox
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		private bool _useStyleColors = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseStyleColors
        {
            get { return _useStyleColors; } set { _useStyleColors = value; Invalidate(); }
        }

		[Conditional("DEBUG")]
		private void UseStyleColor()
		{
			// nothing
		}

        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || !UseStyleColors || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

    }

    #endregion


    #region MetroToggle

    public partial class MetroToggle
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		private bool _useStyleColors = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseStyleColors
        {
            get { return _useStyleColors; } set { _useStyleColors = value; Invalidate(); }
        }

		[Conditional("DEBUG")]
		private void UseStyleColor()
		{
			// nothing
		}

        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || !UseStyleColors || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

    }

    #endregion


    #region MetroTrackBar

    public partial class MetroTrackBar
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		private bool _useStyleColors = false;
        [DefaultValue(false)]
        [Category(MetroDefaults.CatAppearance)]
        public bool UseStyleColors
        {
            get { return _useStyleColors; } set { _useStyleColors = value; Invalidate(); }
        }

		[Conditional("DEBUG")]
		private void UseStyleColor()
		{
			// nothing
		}

        public override Color EffectiveForeColor
        {
            get { return ShouldSerializeForeColor() || !UseStyleColors || MetroControlState != "Normal" ? base.EffectiveForeColor : GetStyleColor(); }
        }

    }

    #endregion


}
 
