using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace BuildTool
{
    class Program
    {
        public const string GameName = "Darklight/";
        public const string DirectoryOffset = "../../../";
        public const string DirectoryGameSource = DirectoryOffset + GameName + "game/";
        public const string DirectoryAnimBlueprint = DirectoryOffset + "/" + GameName + "game/anim/blueprints/";

        static void Main(string[] args)
        {
            string[] animBlueprints = Directory.GetFiles(DirectoryAnimBlueprint, "*.animbp*");
            foreach(string animPath in animBlueprints)
            {
                Console.WriteLine(animPath);
                AnimBPFile animBP = new AnimBPFile(animPath);

                string sourceName = Path.GetFileNameWithoutExtension(animPath);
                sourceName += ".cpp";

                animBP.WriteCPP(DirectoryGameSource + sourceName);
            }
        }
    }
}
