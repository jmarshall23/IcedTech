/**
 * MetroFramework - Modern UI for WinForms
 * 
 * The MIT License (MIT)
 * Copyright (c) 2011 Sven Walter, http://github.com/viperneo
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in the 
 * Software without restriction, including without limitation the rights to use, copy, 
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the 
 * following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#if HAVE_HTML_PARSER

using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.ComponentModel;
using System.Windows.Forms;

using MetroFramework.Components;
using MetroFramework.Interfaces;

namespace MetroFramework.Controls
{
    #region Enums

    public enum MetroTilePartContentType
    {
        Text,
        Image,
        Html
    }

    #endregion

    [ToolboxItem(false)]
    public class MetroTilePart : Aspects.MetroControlTransparent
    {

        #region Fields

        private MetroTilePartContentType partContentType = MetroTilePartContentType.Text;
        [DefaultValue(MetroTilePartContentType.Text)]
        [Category(MetroDefaults.CatBehavior)]
        public MetroTilePartContentType ContentType
        {
            get { return partContentType; }
            set { partContentType = value; }
        }

        private string partHtmlContent = "";
        [DefaultValue("")]
        [Category(MetroDefaults.CatAppearance)]
        public string HtmlContent
        {
            get { return partHtmlContent; }
            set { partHtmlContent = value; }
        }

        #endregion

        #region Constructor

        public MetroTilePart()
        {
            SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            UseTransparency();
            Dock = DockStyle.Fill;
        }

        #endregion

        #region Paint Methods

        protected override void OnPaintForeground(PaintEventArgs e)
        {
            if (partContentType == MetroTilePartContentType.Html)
            {
                MetroFramework.Drawing.Html.HtmlRenderer.Render(e.Graphics, partHtmlContent, ClientRectangle, true);
            }
        }

        #endregion

    }

#region MetroTilePartCollection

    public class MetroTilePartCollection : CollectionBase
    {
        private object owner = null;
        public object Owner
        {
            get { return owner; }
            set { owner = value; }
        }

        public MetroTilePartCollection(object owner)
        {
            this.Owner = owner;
        }

        public int Add(MetroTilePart item)
        {
            return base.List.Add(item);
        }

        public bool Contains(MetroTilePart item)
        {
            return base.List.Contains(item);
        }

        public int IndexOf(MetroTilePart item)
        {
            return base.List.IndexOf(item);
        }

        public void Insert(int index, MetroTilePart item)
        {
            base.List.Insert(index, item);
        }

        public void Remove(MetroTilePart item)
        {
            base.List.Remove(item);
        }

        public MetroTilePart this[string index]
        {
            get
            {
                foreach (MetroTilePart item in base.List)
                {
                    if (item.Name == index)
                    {
                        return item;
                    }
                }
                return null;
            }
            set
            {
                int num = 0;
                foreach (MetroTilePart item in base.List)
                {
                    if (item.Name == index)
                    {
                        base.List[num] = value;
                    }
                    num++;
                }
            }
        }

        public MetroTilePart this[int index]
        {
            get
            {
                return (MetroTilePart)base.List[index];
            }
            set
            {
                base.List[index] = value;
            }
        }
    }

    #endregion
    
}

#endif
