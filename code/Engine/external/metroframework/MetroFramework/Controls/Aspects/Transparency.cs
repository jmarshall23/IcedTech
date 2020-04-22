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
using System.Diagnostics;
using System.Drawing;
using System.ComponentModel;
using System.Windows.Forms;

// ReSharper disable SuggestUseVarKeywordEvident
// ReSharper disable PossibleNullReferenceException

namespace MetroFramework.Controls
{


	#region MetroCheckBox

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroCheckBox
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroComboBox

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroComboBox
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroLabel

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroLabel
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroLink

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroLink
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroPanel

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroPanel
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroProgressSpinner

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroProgressSpinner
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroRadioButton

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroRadioButton
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroTabPage

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroTabPage
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroTile

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroTile
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroToggle

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroToggle
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroTrackBar

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroTrackBar
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


	#region MetroUserControl

	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public partial class MetroUserControl
    {
#if DEBUG
		private static bool _loggedTransparencySupport;
#endif

		private void UseTransparency()
		{
            if( !GetStyle(ControlStyles.SupportsTransparentBackColor) )
			{
#if DEBUG
				Debug.WriteLineIf(!_loggedTransparencySupport, base.GetType().Name + ": enabling SupportsTransparentBackColor.");
				_loggedTransparencySupport = true;
#endif
				SetStyle(ControlStyles.UserPaint | ControlStyles.SupportsTransparentBackColor, true);
			}
			Debug.Assert(GetStyle(ControlStyles.ContainerControl) || GetStyle(ControlStyles.AllPaintingInWmPaint));
		}

        protected override void OnMetroStyleChanged(EventArgs e)
        {
            BaseBackColor = EffectiveBackColor;
            base.OnMetroStyleChanged(e);
        }

        
        protected override void OnPaintBackground(PaintEventArgs e)
        {
			try // without ControlStyles.AllPaintingInWmPaint, we need some error handling
			{
				Color backColor = EffectiveBackColor;

				if (backColor.A == 255)
				{
					e.Graphics.Clear(backColor);
					return;
				}
				base.OnPaintBackground(e);
			}
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
                Invalidate();
            }
        }
                  
    }

	#endregion


}
 
