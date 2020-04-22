using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BuildTool
{
    public class Parser
    {
        private string[] _tokens;
        private int _currentToken = 0;

        public Parser(string buffer)
        {
             _tokens = SplitAndKeepSeparators(buffer, new char[] { ' ', '\t', '\n', '\r' }, StringSplitOptions.None);
        }

        private static string[] SplitAndKeepSeparators(string value, char[] separators, StringSplitOptions splitOptions)
        {
            List<string> splitValues = new List<string>();
            int itemStart = 0;
            for (int pos = 0; pos < value.Length; pos++)
            {
                for (int sepIndex = 0; sepIndex < separators.Length; sepIndex++)
                {
                    if (separators[sepIndex] == value[pos])
                    {
                        // add the section of string before the separator 
                        // (unless its empty and we are discarding empty sections)
                        if (itemStart != pos || splitOptions == StringSplitOptions.None)
                        {
                            splitValues.Add(value.Substring(itemStart, pos - itemStart));
                        }
                        itemStart = pos + 1;

                        // add the separator
                        splitValues.Add(separators[sepIndex].ToString());
                        break;
                    }
                }
            }

            // add anything after the final separator 
            // (unless its empty and we are discarding empty sections)
            if (itemStart != value.Length || splitOptions == StringSplitOptions.None)
            {
                splitValues.Add(value.Substring(itemStart, value.Length - itemStart));
            }

            return splitValues.ToArray();
        }

        public string GetToken(bool keepDelimitors)
        {
            if (keepDelimitors)
            {
                return _tokens[_currentToken++];
            }

            string token = "";
            while(true)
            {
                token = _tokens[_currentToken++];
                if (token.Length > 0 && token[0] != ' ' && token[0] != '\t' && token[0] != '\n' && token[0] != '\r')
                    break;
            }
            return token;
        }

        public bool EndOfFile
        {
            get
            {
                return _currentToken >= _tokens.Length;
            }
        }

        public string ReadRestOfLine()
        {
            string buffer = "";

            while (true)
            {
                string token = GetToken(true);

                if (token[0] == '\n' || token[0] == '\r')
                    break;

                buffer += token;
            }

            return buffer;
        }

        public string GetBracedSection()
        {
            int indent = 0;
            string buffer = GetToken(false); // "{"

            while(true)
            {
                string token = GetToken(true);

                if(token == "}")
                {
                    if(indent == 0)
                    {
                        break;
                    }
                    else
                    {
                        indent--;
                    }
                }
                else if(token == "{")
                {
                    indent++;
                }

                buffer += token;
            }

            return buffer;
        }

        public string ReadTillToken(string stopToken)
        {
            string buffer = "";
            while (true)
            {
                string token = GetToken(true);

                if (token == stopToken)
                {
                    break;
                }

                buffer += token;
            }

            return buffer;
        }
    }
}
