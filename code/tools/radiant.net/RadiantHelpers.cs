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

        public static byte GetKeyAscii2(KeyEventArgs e)
        {
            int keyValue = e.KeyValue;
            // The MFC code is expecting caps
            if (!e.Shift && keyValue >= (int)Keys.A && keyValue <= (int)Keys.Z)
                return (byte)(keyValue + 32);

            if (keyValue == (int)Keys.Up)
            {
                return (byte)KeyEnum.keyNum_t.K_UPARROW;
            }

            if (keyValue == (int)Keys.Down)
            {
                return (byte)KeyEnum.keyNum_t.K_DOWNARROW;
            }

            if (keyValue == (int)Keys.Left)
            {
                return (byte)KeyEnum.keyNum_t.K_LEFTARROW;
            }

            if (keyValue == (int)Keys.Right)
            {
                return (byte)KeyEnum.keyNum_t.K_RIGHTARROW;
            }

            if (keyValue == (int)Keys.Escape)
            {
                return (byte)KeyEnum.keyNum_t.K_ESCAPE;
            }

            if (keyValue == (int)Keys.Space)
            {
                return (byte)KeyEnum.keyNum_t.K_SPACE;
            }

            if (keyValue == (int)Keys.Back)
            {
                return (byte)KeyEnum.keyNum_t.K_BACKSPACE;
            }

            if (keyValue == (int)Keys.Tab)
            {
                return (byte)KeyEnum.keyNum_t.K_TAB;
            }

            if (keyValue == (int)Keys.Oem2)
            {
                return (byte)47;
            }

            if (keyValue == 189) // underscore
            {
                return (byte)'_';
            }

            return (byte)keyValue;
        }
    }
}
