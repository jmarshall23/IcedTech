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
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using MetroFramework.Components;
using MetroFramework.Drawing;
using MetroFramework.Interfaces;

namespace MetroFramework.Design
{

    /// <summary>
    ///     Design-time support for <see cref="Forms.MetroForm"/> and <see cref="Controls.MetroUserControl"/>.
    /// </summary>
    /// <remarks>
    ///     Implements a designer verb to recursively reset styles to default values.
    ///     The style change is applied using <see cref="TypeDescriptor"/> class to allow the designer property grid
    ///     to persist the changes.
    /// </remarks>
    internal class MetroStyleManagerDesigner : ComponentDesigner
    {
        DesignerVerbCollection _verbs;

        public override DesignerVerbCollection Verbs
        {
            get
            {
                return _verbs ?? (_verbs = new DesignerVerbCollection
                    {
                        new DesignerVerb("Reset Styles", OnResetStyles)
                    });
            }
        }

        private IDesignerHost _designerHost;
        public IDesignerHost DesignerHost
        {
            get
            {
                return _designerHost ?? (_designerHost = (IDesignerHost)(GetService(typeof(IDesignerHost))));
            }
        }

        private IComponentChangeService  _componentChangeService;
        public IComponentChangeService ComponentChangeService
        {
            get
            {
                return _componentChangeService ?? (_componentChangeService = (IComponentChangeService)(GetService(typeof(IComponentChangeService))));
            }
        }

        public override void Initialize(IComponent component)
        {
            base.Initialize(component);

            var dte = (EnvDTE.DTE)GetService(typeof(EnvDTE.DTE));
            if (dte != null && dte.Solution != null && dte.Solution.Saved )
            {
                string folder = Path.GetDirectoryName(Path.GetFullPath(dte.Solution.FileName));
                string path = Path.Combine(folder, MetroStyles.THEMES_XML);
                if (!File.Exists(path)) return;
                try
                {
                    var db = MetroStyles.Create(path);
                    if (db != null) MetroStyleManager.Styles = db;
                }
                catch (Exception)
                {
                    // ignore
                }
            }
        }

        private void OnResetStyles(object sender, EventArgs args)
        {
            //DesignerVerb dv = (DesignerVerb)sender;
            MetroStyleManager component = (MetroStyleManager)Component;
            if (component.Owner == null)
            {
                MessageBox.Show("StyleManager needs the Owner property assigned to before it can reset styles.", "Error", MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }

            //DesignerTransaction dt = DesignerHost.CreateTransaction();
            ResetStyles(component, (Control)component.Owner);
            //dt.Commit();
        }

        private void ResetStyles(MetroStyleManager styleManager, Control control)
        {
            // Skip container controls with a dedicated style manager component
            IMetroContainerControl container = control as IMetroContainerControl;
            if (container != null && ! ReferenceEquals(styleManager, container.StyleManager)) return;

             if (control is IMetroStyledComponent)
            {
                ResetProperty(control, MetroStyleManager.THEME_PROPERTY_NAME, MetroStyleManager.AMBIENT_VALUE);
                ResetProperty(control, MetroStyleManager.STYLE_PROPERTY_NAME, MetroStyleManager.AMBIENT_VALUE);
            }

            if (control.ContextMenuStrip != null) ResetStyles(styleManager, control.ContextMenuStrip);

            // descend into tab pages
            TabControl tabControl = control as TabControl;
            if (tabControl != null) foreach (TabPage tp in tabControl.TabPages) ResetStyles(styleManager, tp);

            // descend into child controls
            if (control.Controls != null) foreach (Control child in control.Controls) ResetStyles(styleManager, child);
        }

        void ResetProperty(Control control, string name, object newValue)
        {
            var td = TypeDescriptor.GetProperties(control)[name];
            if(td == null ) return;
            object oldValue = td.GetValue(control);
            if (newValue.Equals(oldValue)) return;
            ComponentChangeService.OnComponentChanging(control, td);
            td.SetValue(control, newValue);
            ComponentChangeService.OnComponentChanged(control, td, oldValue, newValue);
        }
    }
}