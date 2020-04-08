/*
 
MetroFramework - Modern UI for WinForms

Copyright (c) 2013 Jens Thiel, http://github.com/thielj/winforms-modernui

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
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using MetroFramework.Components;
using MetroFramework.Interfaces;

namespace MetroFramework.Design
{

    // GRRRRRR.... This only works for child controls, not top level documents :((

    /// <summary>
    ///     Design-time support for <see cref="Forms.MetroForm"/> and <see cref="Controls.MetroUserControl"/>.
    /// </summary>
    /// <remarks>
    ///     Implements a designer verb to recursively reset styles to default values.
    ///     The style change is applied using <see cref="TypeDescriptor"/> class to allow the designer property grid
    ///     to persist the changes.
    /// </remarks>
    internal class MetroContainerControlDesigner : DocumentDesigner
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

        private void OnResetStyles(object sender, EventArgs args)
        {
            //DesignerVerb dv = (DesignerVerb)sender;
            ResetStyles(Control);
        }

        private void ResetStyles(Control control)
        {
            // Skip container controls with a dedicated style manager component
            IMetroContainerControl container = control as IMetroContainerControl;
            if (container != null && container.StyleManager != null) return;

            IMetroStyledComponent styledComponent = control as IMetroStyledComponent;
            if (styledComponent != null)
            {
                var propc = TypeDescriptor.GetProperties(control);
                var propTheme = propc.Find(MetroStyleManager.THEME_PROPERTY_NAME, false);
                if( propTheme != null && propTheme.CanResetValue(control) ) propTheme.ResetValue(control);
                var propStyle = propc.Find(MetroStyleManager.STYLE_PROPERTY_NAME, true);
                if (propStyle != null && propStyle.CanResetValue(control)) propStyle.ResetValue(control);
            }

            if (control.ContextMenuStrip != null) ResetStyles(control.ContextMenuStrip);

            // descend into tab pages
            TabControl tabControl = control as TabControl;
            if (tabControl != null) foreach (TabPage tp in tabControl.TabPages) ResetStyles(tp);

            // descend into child controls
            if (control.Controls != null) foreach (Control child in control.Controls) ResetStyles(child);
        }

    }
}
