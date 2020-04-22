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

using System.Threading.Tasks;
using System.Diagnostics.Contracts;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Xml.Serialization;
using MetroFramework.XML;

namespace MetroFramework
{
    public class MetroStyles
    {
        private const string RESOURCE_NAMESPACE = "MetroFramework";

        internal const string THEMES_XML = "themes.xml";

        private readonly IDictionary<string, Style> _styles;
        public IDictionary<string, Style> Styles { get { return _styles; } }
        public Style DefaultStyle { get; private set; }

        private readonly IDictionary<string, Theme> _themes;
        public IDictionary<string, Theme> Themes { get { return _themes; } }
        public Theme this[string key] { get { return _themes[key]; } }
        public Theme DefaultTheme { get; private set; }

        public static MetroStyles Create()
        {
            const string resID = RESOURCE_NAMESPACE + "." + THEMES_XML;
            using (Stream s = Assembly.GetExecutingAssembly().GetManifestResourceStream(resID))
                return new MetroStyles(s);
        }

        public static MetroStyles Create(string configFileName = THEMES_XML)
        {
            if(String.IsNullOrEmpty(configFileName)) throw new ArgumentException();
            String path = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, configFileName);
            using (Stream s = File.OpenRead(path)) 
                return new MetroStyles( s );
        }

        public MetroStyles(Stream s)
        {
            var xml = (metroframework) _serializer.Deserialize(s);

            _styles = xml.styles.color.ToDictionary(
                color => color.name,
                color => new Style(color.name, ColorTranslator.FromHtml(color.value)), 
                StringComparer.InvariantCultureIgnoreCase);

            DefaultStyle = _styles[xml.styles.@default];

            _themes = xml.themes.theme.ToDictionary(
                theme => theme.name,
                theme => new Theme(theme.name, theme.property.ToDictionary(prop => prop.name, prop => GetValue(prop.value, prop.type))),
                StringComparer.InvariantCultureIgnoreCase);

            DefaultTheme = _themes[xml.themes.@default];
        }

        private object GetValue(string value, string type)
        {
            Type t = GetType(type);
            TypeConverter converter;
            if (! _converters.TryGetValue(t, out converter))
            {
                converter = TypeDescriptor.GetConverter(t);
                _converters.Add(t, converter);
            }
            return converter.ConvertFromInvariantString(value);
        }

        private static Type GetType(string type)
        {
            if (type.StartsWith("System.Drawing.") && type.IndexOf(',') < 0)
                type += ", " + typeof(Color).Assembly.FullName;
            return Type.GetType(type, true);
        }

        // NOTE: This is a workaround for new XmlSerializer(typeof(metroframework)) to prevent a first-chance FileNotFoundException
        private static readonly XmlSerializer _serializer = XmlSerializer.FromTypes(new[] {typeof (metroframework)})[0];

        private readonly Dictionary<Type, TypeConverter> _converters = new Dictionary<Type, TypeConverter>();

        public class Theme
        {
            public string Name { get; private set; }
            private readonly IDictionary<string, object> _properties;
            public object this[string key] { get { return _properties[key]; } }
            public bool TryGetValue<T>(string key, out T @object)
            {
                Object o;
                if (_properties.TryGetValue(key, out o))
                {
                    if (o is T)
                    {
                        @object = (T) o;
                        return true;
                    }
                    Trace.WriteLine("Unexpected property type: " + o.GetType().Name + " (expected: " + typeof(T) + ")");
                }

                @object = default(T);
                return false;
            }

            internal Theme(string name, IDictionary<string, object> properties)
            {
                Name = name;
                _properties = properties;
            }
        }

        public class Style
        {
            public string Name { get; private set; }
            public Color Color { get; private set; }

            internal Style(string name, Color color)
            {
                Name = name;
                Color = color;
            }
        }

    }
}
