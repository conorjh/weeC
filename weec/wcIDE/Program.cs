using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace wcIDE {
	static class Program {
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() {
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			Application.Run(new MainForm());
		}
	}

    public enum CompileTargetPlatform { Bytecode, SimpleBytecode, HostedEXE, AnsiC, x86 };
    public enum DebugMode { Off,On };
    public enum VMTargetPlatform { Auto, Bytecode, SimpleBytecode };

    static public class wcIDEData
    {
        static wcIDEData()
        {
            compileTarget = CompileTargetPlatform.SimpleBytecode;
            vmTarget = VMTargetPlatform.SimpleBytecode;
            debugMode = DebugMode.Off;
            pathCompiler = "../../../Debug/wcc.exe";
            pathVM = "../../../Debug/wcvm.exe";
        }
        public static string currentScriptFilename;
        public static string pathCompiler, pathVM;
        public static CompileTargetPlatform compileTarget;
        public static VMTargetPlatform vmTarget;
        public static DebugMode debugMode;
    }

    static public class wcIDEHelper
    {
        public static string getCompilerCommandLine()
        {
            string output = wcIDEData.pathCompiler;

            //source filename
            output += " -s \"" + wcIDEData.currentScriptFilename+"\"";

            //target compiler
            switch(wcIDEData.compileTarget)
            {
                case CompileTargetPlatform.Bytecode:
                    output += " -t bytecode";       
                    break;

                case CompileTargetPlatform.SimpleBytecode:
                    output += " -t simple";
                    break;
            }

            //produce an AST
            output += " -ast";
            
            return output;
        }

        public static string getVMCommandLine()
        {

            return "";
        }
    }
}
