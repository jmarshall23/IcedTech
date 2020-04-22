using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace BuildTool
{
    public class AnimBPCodeBlock
    {
        public string name;
        public string buffer;
    }

    public class AnimBPEvent
    {
        string _eventName;
        List<string> _macro = new List<string>();
        string _codeName;

        public string EventName
        {
            get
            {
                return _eventName;
            }
        }

        public List<string> Macro
        {
            get
            {
                return _macro;
            }
        }

        public string CodeName
        {
            get
            {
                return _codeName;
            }
        }

        public AnimBPEvent(ref Parser _parser)
        {
            _eventName = _parser.GetToken(false);
            _eventName = _eventName.Trim('"');
            _parser.GetToken(false); // "{"

            while (!_parser.EndOfFile)
            {
                string token = _parser.GetToken(false);

                if (token == "}")
                {
                    break;
                }
                else if (token == "macro")
                {
                    string macro = _parser.ReadRestOfLine();
                    macro = macro.Trim('"');
                    if (macro.Length == 1)
                        macro = "";

                    _macro.Add(macro);
                }
                else if (token == "code")
                {
                    _codeName = _parser.GetToken(false);
                }
                else
                {
                    throw new Exception("AnimBPEvent unknown or unexpcted token " + token);
                }

            }
        }
    }

    public class AnimBP
    {
        string name;
        List<AnimBPCodeBlock> _animBPCodeBlocks = new List<AnimBPCodeBlock>();
        public List<AnimBPEvent> _animBPEvents = new List<AnimBPEvent>();

        public AnimBPCodeBlock FindCodeBlock(string codeBlockName)
        {
            for(int i = 0; i < _animBPCodeBlocks.Count; i++)
            {
                if (_animBPCodeBlocks[i].name == codeBlockName)
                    return _animBPCodeBlocks[i];
            }

            throw new Exception("Failed to find codeblock " + codeBlockName);
        }

        public string Name
        {
            get
            {
                return name;
            }
        }

        public AnimBP(ref Parser _parser)
        {
            name = _parser.GetToken(false);
            _parser.GetToken(false); // "{"

            while (!_parser.EndOfFile)
            {
                string token = _parser.GetToken(false);

                if (token == "}")
                {
                    break;
                }
                else if(token == "code_block")
                {
                    AnimBPCodeBlock codeBlock = new AnimBPCodeBlock();
                    codeBlock.name = _parser.GetToken(false);
                    codeBlock.buffer = _parser.ReadTillToken("end_code_block");
                    _animBPCodeBlocks.Add(codeBlock);
                }
                else if(token == "event")
                {
                    AnimBPEvent ev = new AnimBPEvent(ref _parser);
                    _animBPEvents.Add(ev);
                }
                else
                {
                    throw new Exception("AnimBP unknown or unexpcted token " + token);
                }
            }
        }
    }

    public class AnimBPFile
    {
        Parser _parser;
        List<AnimBP> _animBP = new List<AnimBP>();
        string classname = "";

        public AnimBPFile(string fileName)
        {
            _parser = new Parser(File.ReadAllText(fileName));
            LoadAnimBP();
        }

        void LoadAnimBP()
        {
            while(!_parser.EndOfFile)
            {
                string token = _parser.GetToken(false);

                if(token == "animbp")
                {
                    AnimBP animBP = new AnimBP(ref _parser);
                    _animBP.Add(animBP);
                }
                else if(token == "classname")
                {
                    classname = _parser.GetToken(false);
                }
                else
                {
                    throw new Exception("AnimBPFile unknown or unexpcted token " + token);
                }
            }
        }

        public string EvaluateCodeBlock(string codeblock, List<string> macros)
        {
            string _openingMacro = "";
            string _endingMacro = "";
            string _outBlock = codeblock;

            for(int i = 0; i < macros.Count; i++)
            {
                _openingMacro += string.Format("#define {0}\n", macros[i]);
                _endingMacro += string.Format("#undef {0}\n", macros[i]);
            }

            _outBlock = _outBlock.Replace("<FUNCTION_START>", _openingMacro);
            _outBlock = _outBlock.Replace("<FUNCTION_END>", _endingMacro);

            return _outBlock;
        }

        public void WriteCPP(string fileName)
        {
            string __cppBuffer = "";
            string __fileNameOnly = Path.GetFileName(fileName);

            __cppBuffer += string.Format("// {0} - This file has been auto generated by the BuildTool\n", __fileNameOnly);
            __cppBuffer += "// Please do NOT modify this file!\n";
            __cppBuffer += "\n";            
            __cppBuffer += "#include \"game_precompiled.h\"\n";
            __cppBuffer += "#pragma hdrstop\n";
            __cppBuffer += "#include \"Game_local.h\"\n";
            __cppBuffer += "// This file has been auto generated by the BuildTool Please do NOT modify this file!\n";
            __cppBuffer += "\n";
            __cppBuffer += string.Format("CLASS_STATES_DECLARATION({0})\n", classname);
            for (int i = 0; i < _animBP.Count; i++)
            {
                AnimBP _anim = _animBP[i];

                foreach (AnimBPEvent ev in _anim._animBPEvents)
                {
                    __cppBuffer += string.Format("\tSTATE(\"{0}\", {1}::State_{2})\n", ev.EventName, classname, ev.EventName);
                }
            }
            __cppBuffer += "END_CLASS_STATES\n";

            __cppBuffer += "// This file has been auto generated by the BuildTool Please do NOT modify this file!\n";
            for (int i = 0; i < _animBP.Count; i++)
            {
                AnimBP _anim = _animBP[i];

                foreach (AnimBPEvent ev in _anim._animBPEvents)
                {
                    AnimBPCodeBlock codeBlock = _anim.FindCodeBlock(ev.CodeName);

                    __cppBuffer += string.Format("stateResult_t {0}::State_{1}(const stateParms_t&parms)\n", classname, ev.EventName);
                    __cppBuffer += EvaluateCodeBlock(codeBlock.buffer, ev.Macro);
                }
            }

            File.WriteAllText(fileName, __cppBuffer);
        }
    }
}
