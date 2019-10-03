using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using radiant.net.forms;

namespace radiant.net
{
    class RadiantHelpers
    {
        public static char GetKeyAscii(KeyEventArgs e)
        {
            int keyValue = e.KeyValue;
            // The MFC code is expecting caps
            //if (!e.Shift && keyValue >= (int)Keys.A && keyValue <= (int)Keys.Z)
            //    return (char)(keyValue + 32);
            return (char)keyValue;
        }
    }
}
