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
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using MetroFramework.Drawing;
using MetroFramework.Interfaces;

namespace MetroFramework.Components
{
    /// <summary>
    ///     Cascading theme and styles management.
    /// </summary>
    /// <remarks>
    ///     <para>
    ///         This class has multiple use cases:
    ///     </para>
    ///      <list type="numbered">
    ///     <item>
    ///         <description>
    ///             On the global level, it is instantited as a static singleton instance (<see cref="Default"/>).
    ///             Any changes applied here will be propagated to all active and future controls, unless 
    ///             more specific values are applied.
    ///             The global instance is instantiated using the default constructor and pulls default values
    ///             from <see cref="MetroDefaults"/>.
    ///         </description>
    ///         <description>
    ///             It can be added as a component to container controls (e.g. <see cref="MetroFramework.Forms.MetroForm"/>
    ///             or <see cref="MetroFramework.Controls.MetroUserControl"/>) to override settings 
    ///             on the container level. Any setting not specified is still managed by the global instance.
    ///             The designer is initializing components using the <see cref="MetroStyleManager(IContainer)"/>constructor
    ///             and the developer must assign the form to the style manager's _owner property.
    ///         </description>
    ///         <description>
    ///             On the control level, it acts as a property store for per-control overrides. 
    ///             Controls use the default constructor.
    ///         </description>
    ///     </item>
    ///     </list>
    /// </remarks>
    [Designer("MetroFramework.Design.MetroStyleManagerDesigner, " + AssemblyRef.MetroFrameworkDesignSN)]
    public sealed class MetroStyleManager : Component, ISupportInitialize, IMetroComponent, IMetroStyledComponent
    {

        #region Properties

        internal const string AMBIENT_VALUE = "";

        // The name of the Style property as it is used throughout MetroFramework
        internal const string STYLE_PROPERTY_NAME = "Style";

        private string _metroStyle = AMBIENT_VALUE;
        [DefaultValue(AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
        public string Style
        {
            get
            {
                return (DesignMode || _metroStyle != AMBIENT_VALUE) ? _metroStyle : _styleManager.Style ;
            }
            set
            {
                // The singleton instance must always have a non-default value
                if (_styleManager == null && value == AMBIENT_VALUE) value = Styles.DefaultStyle.Name;

                bool changed = Style != value;
                _metroStyle = value;
                if(changed) OnMetroStyleChanged(this, new EventArgs());
            }
        }

        // The name of the Theme property as it is used throughout MetroFramework
        internal const string THEME_PROPERTY_NAME = "Theme";

        private string _metroTheme = AMBIENT_VALUE;
        [DefaultValue(AMBIENT_VALUE)]
        [Category(MetroDefaults.CatAppearance)]
        public string Theme
        {
            get
            {
                return (DesignMode || _metroTheme != AMBIENT_VALUE) ? _metroTheme : _styleManager.Theme;
            }
            set
            {
                // The singleton instance must always have a non-default value
                if (_styleManager == null && value == AMBIENT_VALUE) value = Styles.DefaultTheme.Name;

                bool changed = Theme != value;
                _metroTheme = value;
                if(changed) OnMetroStyleChanged(this, new EventArgs());
            }         
        }

        private MetroStyleManager _styleManager;
        [Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
        MetroStyleManager IMetroStyledComponent.InternalStyleManager
        {
            get { return _styleManager; } 
            set
            {
                if(value==null) throw new ArgumentNullException();
                if(value==this) throw new ArgumentException();
                bool changed = value.Theme != Theme || value.Style != Style;
                if( _styleManager != null) _styleManager.MetroStyleChanged -= OnMetroStyleChanged;
                _styleManager = value;
                _styleManager.MetroStyleChanged += OnMetroStyleChanged;
                if(changed) OnMetroStyleChanged(this, new EventArgs());
            }
        }

        /// <summary>
        ///     The container control (e.g. form or user control) this style manager is managing.
        /// </summary>
        private IMetroContainerControl _owner;

        [ImmutableObject(true)]
        public IMetroContainerControl Owner
        {
            get { return _owner; }
            set
            {
                // We attach to ControlAdded to propagate styles to dynamically added controls

                if (_owner != null)
                {
                    if (_owner.StyleManager == this) _owner.StyleManager = null;
                    ContainerControl cc = _owner as ContainerControl;
                    if (cc != null) cc.ControlAdded -= ControlAdded;
                }

                _owner = value;

                if (value != null)
                {
                    value.StyleManager = this;
                    ContainerControl cc = _owner as ContainerControl;
                    if (cc != null) cc.ControlAdded += ControlAdded;

                    if (!isInitializing)
                    {
                        UpdateControl((Control)value);
                    }
                }
            }
        }

        #endregion

        #region Events

        public event EventHandler MetroStyleChanged;

        private void OnMetroStyleChanged(object sender, EventArgs e)
        {
            if (!isInitializing)
            {
                Update();
                var ev = MetroStyleChanged;
                if (ev != null) ev(this, e);
            }
        }

        #endregion

        #region Ctors & Dispose

        public MetroStyleManager()
        {
            _styleManager = Default;
            if (_styleManager != null)
            {
                _styleManager.MetroStyleChanged += OnMetroStyleChanged;
            }
            else // we are the singleton instance - provide actual defaults here
            {
                _metroTheme = Styles.DefaultTheme.Name;
                _metroStyle = Styles.DefaultStyle.Name;
            }
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && _styleManager != null)
            {
                _styleManager.MetroStyleChanged -= OnMetroStyleChanged;
                // NOTE: we don't dispose _styleManager as we don't have ownership !
            }
            base.Dispose(disposing);
        }

        /// <summary>
        ///     The Container owning the components. This is usualy the form's "components" field, not the form itself.
        /// </summary>
        private readonly IContainer _parentContainer;

        public MetroStyleManager(IContainer parentContainer)
            : this()
        {
            if (parentContainer != null)
            {
                _parentContainer = parentContainer;
                _parentContainer.Add(this);
            }
        }

        #endregion

        #region ISupportInitialize

        /// <summary>
        ///     Defer propagating style information until all controls and components have ben added 
        ///     and all properties have been set.
        /// </summary>
        private bool isInitializing;

        void ISupportInitialize.BeginInit()
        {
            isInitializing = true;
        }

        void ISupportInitialize.EndInit()
        {
            isInitializing = false;
            Update();
        }

        #endregion

        #region Style updates

        private void ControlAdded(object sender, ControlEventArgs e)
        {
            if (!isInitializing)
            {
                UpdateControl(e.Control);
            }
        } 
        
        public void Update()
        {
            if (_owner != null)
            {
                UpdateControl((Control)_owner);
            }

            if (_parentContainer == null || _parentContainer.Components == null)
            {
                return;
            }

            // propagate style information to components, i.e. MetroStyleExtender
            var stylesComponents = _parentContainer.Components.Cast<object>()
                                                   .Where(obj => !ReferenceEquals(obj, this))
                                                   .OfType<IMetroStyledComponent>();
            foreach (var c in stylesComponents) c.InternalStyleManager = this;
        }

        private void UpdateControl(Control control)
        {
            if (control == null)
            {
                return;
            }

            // If a container control is exposing a Style Manager, we link to it
            // but do not access the container's children.
            IMetroContainerControl container = control as IMetroContainerControl;
            if (container != null && container.StyleManager != null && !ReferenceEquals(this, container.StyleManager))
            {
                ((IMetroStyledComponent)container.StyleManager).InternalStyleManager = this;
                return;
            }

            // Link to metro controls
            IMetroStyledComponent styledComponent = control as IMetroStyledComponent;
            if (styledComponent != null)
            {
                styledComponent.InternalStyleManager = this;
            }

            if (control.ContextMenuStrip != null)
            {
                UpdateControl(control.ContextMenuStrip);
            }

            // descend into tab pages
            TabControl tabControl = control as TabControl;
            if (tabControl != null)
            {
                foreach (TabPage tp in tabControl.TabPages)
                {
                    UpdateControl(tp);
                }
            }

            // descend into child controls
            if (control.Controls != null)
            {
                foreach (Control child in control.Controls)
                {
                    UpdateControl(child);
                }
            }

        }

        #endregion

        //[Guid("04A72314-32E9-48E2-9B87-A63603454F3E")]
        //private interface _DTE
        //{
        //}

        private static MetroStyles _styles;

        public static MetroStyles Styles
        {
            get { return _styles;  }
            internal set
            {
                _styles = value;
                Default.OnMetroStyleChanged(Default, new EventArgs());
            }
        }

        // must be initialized after loading the styles
        public static readonly MetroStyleManager Default;

        static MetroStyleManager()
        {
            try
            {
                string path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, MetroStyles.THEMES_XML);
                Trace.WriteLine(path);
                if (File.Exists(path))
                {
                    _styles = MetroStyles.Create(path);
                    return;
                }
            }
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
            }

            try
            {
                _styles = MetroStyles.Create();
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
            }

            Default = new MetroStyleManager();
        }


        public Color GetStyleColor()
        {
            Debug.Assert(Style!=null);
            MetroStyles.Style style;
            if (!Styles.Styles.TryGetValue(Style, out style)) style = Styles.DefaultStyle;
            return style.Color;
        }

        /// <summary>
        ///     Get a property from the themes database.
        /// </summary>
        /// <remarks>
        ///     If <paramref name="property"/> is fully qualified, i.e. has at least 2 dots in it 
        ///     (e.g "Form.BackColor.Normal"), a single attempt to retrieve this property is made 
        ///     and the other parameters remain unused.
        ///     Otherwise, this method tries to retrieve a property, first by comnining 
        ///     "{category}.{property}.{state}", then "*.{property}.{state}".
        /// </remarks>
        /// <returns><c>NULL</c> if the property could not be found.</returns>
        public bool TryGetThemeProperty<T>(string property, out T value, string state = "*", string category = "*")
        {
            if (state == null) throw new ArgumentNullException();
            if (category == null) throw new ArgumentNullException();
            Debug.Assert(Theme != null);

            // TODO: cache lookups

            MetroStyles.Theme theme;
            if (!Styles.Themes.TryGetValue(Theme, out theme)) theme = Styles.DefaultTheme;

            int dots = property.Count(c => c == '.');
            Debug.Assert(dots > 1 || (category != null && state != null));

            // Fully-qualified property name
            if (dots > 1) return theme.TryGetValue(property, out value);

            // Try Category.Property.State, avoid *.Property.*
            if (state != "*" && category != "*" && theme.TryGetValue(category + "." + property + "." + state, out value))
                return true;

            // Try Category.Property.*, if we have a category  (i.e. avoid *.Property.*)
            if (category != "*" && theme.TryGetValue(category + "." + property + ".*", out value))
                return true;

            // eliminate subcontrol specifiers and try the lookup again
            if (dots == 1)
                return TryGetThemeProperty(property.Substring(property.LastIndexOf('.') + 1), out value, state, category);

            // Try *.Property.State, if we have a state (i.e. avoid *.Property.*)
            if (state != "*" && theme.TryGetValue("*." + property + "." + state, out value))
                return true;

            // Try *.Property.* and give up if it wasn't found
            return theme.TryGetValue("*." + property + ".*", out value);
        }


        /// <summary>
        ///     Get a property from the themes database.
        /// </summary>
        /// <remarks>
        ///     If <paramref name="property"/> is fully qualified, i.e. has at least 2 dots in it 
        ///     (e.g "Form.BackColor.Normal"), a single attempt to retrieve this property is made 
        ///     and the other parameters remain unused.
        ///     Otherwise, this method tries to retrieve a property, first by comnining 
        ///     "{category}.{property}.{state}", then "*.{property}.{state}".
        /// </remarks>
        /// <param name="property"></param>
        /// <param name="state"></param>
        /// <param name="category"></param>
        /// <returns><c>NULL</c> if the property could not be found.</returns>
        [Obsolete]
        public object GetThemeProperty(string property, string state = "*", string category = "*")
        {
            object result;
            return TryGetThemeProperty(property, out result, state, category) ? result : null;
        }

        public Color GetThemeColor(string property, string state = "*", string category = "*")
        {
            object o;
            if( TryGetThemeProperty(property, out o, state, category) && o is Color) 
                return (Color)o;
            Trace.WriteLine(String.Format("({0}, {1}, {2}): Color lookup failed.", property, category, state));
            return Color.DeepPink;
        }

        public Font GetThemeFont(MetroFontSize size, MetroFontWeight weight, string category = "*")
        {
            float fontSize;
            if (!TryGetThemeProperty("FontSize", out fontSize, size.ToString(), category))
                fontSize = MetroDefaults.FontSize;

            string fontFamily;
            if (!TryGetThemeProperty("FontFamily", out fontFamily, weight.ToString(), category))
                fontFamily = MetroDefaults.FontFamily;

            FontStyle fontStyle;
            if (!TryGetThemeProperty("FontStyle", out fontStyle, weight.ToString(), category))
                fontStyle = MetroDefaults.FontStyle;

            return MetroFonts.ResolveFont(fontFamily, fontSize, fontStyle);
        }
    }
}
