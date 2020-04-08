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
using MetroFramework.Components;
using MetroFramework.Drawing;
using MetroFramework.Interfaces;
using MetroFramework.Native;

// ReSharper disable VirtualMemberNeverOverriden.Global

namespace MetroFramework.Controls
{


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroControlBase : Control, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroControlBase()
        {
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "Control"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroButtonBase : Button, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroButtonBase()
        {
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "Button"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroCheckBoxBase : CheckBox, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroCheckBoxBase()
        {
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "CheckBox"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroComboBoxBase : ComboBox, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroComboBoxBase()
        {
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "ComboBox"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroFormBase : Form, IMetroContainerControl, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroFormBase()
        {
			Debug.Assert(GetStyle(ControlStyles.ContainerControl));
			Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
			////Debug.Assert(GetStyle(ControlStyles.ContainerControl));
			//Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
	    /// <summary>
        ///     A style manager controlling this container and all child controls.
        /// </summary>
        /// <remarks>
        ///     To assign a Style Manager to a <see cref="Forms.MetroForm"/> or
        ///     <see cref="Controls.MetroUserControl"/>, add a <see cref="Components.MetroStyleManager"/>
        ///     component to the designer and assign the form or user control as Owner.
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public MetroStyleManager StyleManager { get; set; }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "Form"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroLabelBase : Label, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroLabelBase()
        {
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "Label"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroPanelBase : Panel, IMetroContainerControl, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroPanelBase()
        {
			Debug.Assert(GetStyle(ControlStyles.ContainerControl));
			Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
			////Debug.Assert(GetStyle(ControlStyles.ContainerControl));
			//Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
	    /// <summary>
        ///     A style manager controlling this container and all child controls.
        /// </summary>
        /// <remarks>
        ///     To assign a Style Manager to a <see cref="Forms.MetroForm"/> or
        ///     <see cref="Controls.MetroUserControl"/>, add a <see cref="Components.MetroStyleManager"/>
        ///     component to the designer and assign the form or user control as Owner.
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public MetroStyleManager StyleManager { get; set; }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "Panel"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroProgressBarBase : ProgressBar, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroProgressBarBase()
        {
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "ProgressBar"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroRadioButtonBase : RadioButton, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroRadioButtonBase()
        {
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "RadioButton"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroTabControlBase : TabControl, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroTabControlBase()
        {
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "TabControl"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroTabPageBase : TabPage, IMetroContainerControl, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroTabPageBase()
        {
			Debug.Assert(GetStyle(ControlStyles.ContainerControl));
			Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
			////Debug.Assert(GetStyle(ControlStyles.ContainerControl));
			//Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
	    /// <summary>
        ///     A style manager controlling this container and all child controls.
        /// </summary>
        /// <remarks>
        ///     To assign a Style Manager to a <see cref="Forms.MetroForm"/> or
        ///     <see cref="Controls.MetroUserControl"/>, add a <see cref="Components.MetroStyleManager"/>
        ///     component to the designer and assign the form or user control as Owner.
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public MetroStyleManager StyleManager { get; set; }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "TabPage"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroTextBoxBase : TextBox, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroTextBoxBase()
        {
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "TextBox"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


	[EditorBrowsable(EditorBrowsableState.Advanced)]
    public abstract class MetroUserControlBase : UserControl, IMetroContainerControl, 
		IMetroControl, IMetroStyledComponent
    {

		/**************************************************************************************
									GENERATED FILE - DO NOT EDIT
		 **************************************************************************************/

		#region Fields, Constructor & IDisposable

		private readonly MetroStyleManager _styleManager;

	    protected MetroUserControlBase()
        {
			Debug.Assert(GetStyle(ControlStyles.ContainerControl));
			Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
            _styleManager = new MetroStyleManager();
            _styleManager.MetroStyleChanged += NotifyMetroStyleChanged;
        }

#if DEBUG
		private bool _controlWasPainted;
#endif
                   
        protected override void Dispose(bool disposing)
        {
            if (disposing) 
			{
#if DEBUG
				Debug.WriteLineIf(!_controlWasPainted, GetType().Name + ": never received OnPaint() event.");
#endif
				_styleManager.Dispose();
			}
            base.Dispose(disposing);
        }

		#endregion

		[Browsable(false)]
		public event PaintEventHandler UserPaint;

		protected override void OnPaint(PaintEventArgs e)
		{
#if DEBUG			
			_controlWasPainted = true;
			////Debug.Assert(GetStyle(ControlStyles.ContainerControl));
			//Debug.Assert(!GetStyle(ControlStyles.AllPaintingInWmPaint));
#endif
			try
			{
				//Debug.WriteLine(Name + ": OnPaint: " + e.ClipRectangle);
				if( GetStyle(ControlStyles.AllPaintingInWmPaint)) OnPaintBackground(e);
				OnPaintForeground(e);
				if(GetStyle(ControlStyles.UserPaint))
				{
					var ev = UserPaint;
					if(ev!=null) ev(this, e);
				}
			}
			catch(Exception ex)
			{
				Trace.WriteLine(ex);
				Invalidate();
			}
		}

		protected virtual void OnPaintForeground(PaintEventArgs e)
		{
			base.OnPaint(e);
		}

        #region Style Manager Interface

        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; }
            // NOTE: we don't replace our style manager, but instead assign the style manager a new manager
            set { ((IMetroStyledComponent)_styleManager).InternalStyleManager = value; /* no need to Invalidate() here */ }
        }

        // Event handler for our style manager's updates
        // NOTE: The event may have been triggered from a different thread.
        private void NotifyMetroStyleChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new Action<EventArgs>(OnMetroStyleChanged), e);
            else
                OnMetroStyleChanged(e);
        }

		protected virtual void OnMetroStyleChanged(EventArgs e)
		{
			_effectiveFont = GetThemeFont();
			Invalidate();
		}

        protected override void OnHandleCreated(EventArgs e)
        {
            base.OnHandleCreated(e);
			OnMetroStyleChanged(e);
        }

        // Override Site property to set the style manager into design mode, too.
        public override ISite Site
        {
            get { return base.Site; } set { base.Site = _styleManager.Site = value; }
        }

        #endregion

		#region Properties

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Theme
#pragma warning restore 109
        {
            get { return _styleManager.Theme; } set { _styleManager.Theme = value; /* no need to Invalidate() here */}
        }

		[DefaultValue(MetroStyleManager.AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
#pragma warning disable 109
        public new string Style
#pragma warning restore 109
        {
            get { return _styleManager.Style; } set { _styleManager.Style = value; /* no need to Invalidate() here */ }
        }

	
	    /// <summary>
        ///     A style manager controlling this container and all child controls.
        /// </summary>
        /// <remarks>
        ///     To assign a Style Manager to a <see cref="Forms.MetroForm"/> or
        ///     <see cref="Controls.MetroUserControl"/>, add a <see cref="Components.MetroStyleManager"/>
        ///     component to the designer and assign the form or user control as Owner.
        /// </remarks>
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        public MetroStyleManager StyleManager { get; set; }

	
		#endregion

		protected virtual string MetroControlCategory { get { return "UserControl"; } }

		protected virtual string MetroControlState { get { return Enabled ? "Normal" : "Disabled"; } }

		protected virtual bool TryGetThemeProperty<T>(string property, out T value, string state = null, string category = null) 
		{
			return _styleManager.TryGetThemeProperty(property, out value, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		protected virtual Color GetStyleColor()
		{
			return _styleManager.GetStyleColor();
		}

		private Color _backColor = Color.Empty;
		protected bool ShouldSerializeBackColor() { return !_backColor.IsEmpty; }
		private new void ResetBackColor() { _backColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color BackColor 
		{ 
			get { return _backColor; } set { _backColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}

	    protected Color BaseBackColor 
		{
			get { return base.BackColor; } set { base.BackColor = value; } 
		}

		[Browsable(false)]
		public virtual Color EffectiveBackColor 
		{ 
			get { return ShouldSerializeBackColor() ? _backColor : GetThemeColor("BackColor"); } 
		}

		private Color _foreColor = Color.Empty;
		protected bool ShouldSerializeForeColor() { return !_foreColor.IsEmpty; }
		private new void ResetForeColor() { _foreColor = Color.Empty; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Color ForeColor 
		{ 
			get { return _foreColor; } set { _foreColor = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    protected Color BaseForeColor 
		{ 
			get { return base.ForeColor; } set { base.ForeColor = value; } 
		}
		[Browsable(false)]
		public virtual Color EffectiveForeColor 
		{ 
			get { return ShouldSerializeForeColor() ? _foreColor : GetThemeColor("ForeColor"); } 
		}

		protected virtual Color GetThemeColor(string property, string state = null, string category = null)
		{
			return _styleManager.GetThemeColor(property, state ?? MetroControlState, category ?? MetroControlCategory);
		}

		private Font _font;
		protected bool ShouldSerializeFont() { return _font != null; }
		private new void ResetFont() { _font = null; }

	    [Category(MetroDefaults.CatAppearance)]
	    public new Font Font 
		{ 
			get { return _font; } set { _font = value; OnMetroStyleChanged( new EventArgs() ); } 
		}
	    
		protected Font BaseFont 
		{ 
			get { return base.Font; } set { base.Font = value; } 
		}
		
		private Font _effectiveFont;
		protected virtual Font EffectiveFont 
		{ 
			get { return _effectiveFont; } 
		}

        protected virtual Font GetThemeFont( string category = null)
        {
			return ShouldSerializeFont() ? Font : GetThemeFont(MetroFontSize.Default, MetroFontWeight.Default, category);
		}

        protected Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = null)
        {
            if (size == MetroFontSize.Default && !TryGetThemeProperty("MetroFontSize", out size) )
                size = MetroDefaults.MetroFontSize;

            if (weight == MetroFontWeight.Default && !TryGetThemeProperty("MetroFontWeight", out weight))
                weight = MetroDefaults.MetroFontWeight;

            return _styleManager.GetThemeFont(size, weight, category ?? MetroControlCategory);
        }


        protected bool ShouldSerializeUseVisualStyleBackColor()
        {
            return false;
        }

#if DEBUG

        protected override void WndProc(ref Message m)
        {
            if (DesignMode)
            {
                base.WndProc(ref m);
                return;
            }

            switch (m.Msg)
            {
                //case WinApi.Messages.WM_CTLCOLORBTN:
                    //Debug.WriteLine("WM_CTLCOLORBTN", GetType().Name);
                    //break;
                //case WinApi.Messages.WM_CTLCOLORSTATIC:
                    //Debug.WriteLine("WM_CTLCOLORSTATIC", GetType().Name);
                    //break;
                case WinApi.Messages.WM_CTLCOLOREDIT:
                    Debug.WriteLine("WM_CTLCOLOREDIT", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORDLG:
                    Debug.WriteLine("WM_CTLCOLORDLG ", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORLISTBOX:
                    Debug.WriteLine("WM_CTLCOLORLISTBOX", GetType().Name);
                    break;
                case WinApi.Messages.WM_CTLCOLORSCROLLBAR:
                    Debug.WriteLine("WM_CTLCOLORSCROLLBAR ", GetType().Name);
                    break;
            }
            base.WndProc(ref m);
        }

        protected override void SetBoundsCore(int x, int y, int width, int height, BoundsSpecified specified)
        {
            //Debug.WriteLine( "{0}::SetBoundsCore(x={1} y={2} width={3} height={4} specified={5}", Name, x, y, width, height, specified);
            //Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height);
            //if (specified == BoundsSpecified.None)
            //{
            //    x = Left;
            //    y = Top;
            //    width = Width;
            //    height = Height;
            //}
			int xx = Left, yy = Top, ww = Width, hh = Height;
            base.SetBoundsCore(x, y, width, height, specified);
			if( specified == BoundsSpecified.None && ( ww != Width || hh != Height ) && ( width == 0 || height == 0 ))
			{
				Debug.WriteLine("{0}::Pre(x={1} y={2} width={3} height={4}", Name, xx, yy, ww, hh); 
				Debug.WriteLine("{0}::Set(x={1} y={2} width={3} height={4}", Name, x, y, width, height); 
				Debug.WriteLine("{0}::Post(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
				base.SetBoundsCore(x, y, ww, hh, specified);
				Debug.WriteLine("{0}::Fix(x={1} y={2} width={3} height={4}", Name, Left, Top, Width, Height); 
			}
        }

#endif

    }


}
 
