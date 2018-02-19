using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace wcIDE
{
    public partial class CompilerOptions : Form
    {
        public CompilerOptions()
        {
            InitializeComponent();
            switch(wcIDEData.compileTarget)
            {
                case CompileTargetPlatform.Bytecode:
                    cbTargetPlatform.SelectedItem = "Bytecode";
                    break;
                case CompileTargetPlatform.SimpleBytecode:
                    cbTargetPlatform.SelectedItem = "Simple Bytecode";
                    break;
            }
            switch (wcIDEData.vmTarget)
            {
                case VMTargetPlatform.Bytecode:
                    cbVMPlatform.SelectedItem = "Default";
                    break;
                case VMTargetPlatform.SimpleBytecode:
                    cbVMPlatform.SelectedItem = "Simple";
                    break;
            }
            switch (wcIDEData.debugMode)
            {
                case DebugMode.Off:
                    cbDebug.SelectedItem = "Off";
                    break;
                case DebugMode.On:
                    cbDebug.SelectedItem = "Debug (include Symbols)";
                    break;
            }

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        //apply button
        private void button4_Click(object sender, EventArgs e)
        {
            switch (cbTargetPlatform.SelectedItem)
            {
                case "Bytecode":
                    wcIDEData.compileTarget = CompileTargetPlatform.Bytecode;
                    break;
                case "Simple Bytecode":
                    wcIDEData.compileTarget = CompileTargetPlatform.SimpleBytecode;
                    break;
            }
            switch (cbVMPlatform.SelectedItem)
            {
                case "Default":
                    wcIDEData.vmTarget = VMTargetPlatform.Bytecode;
                    break;
                case "Simple":
                    wcIDEData.vmTarget = VMTargetPlatform.SimpleBytecode;
                    break;
            }
            switch (cbDebug.SelectedItem )
            {
                case "Off":
                    wcIDEData.debugMode = DebugMode.Off;
                    break;
                case "Debug (include Symbols)":
                    wcIDEData.debugMode = DebugMode.On;
                    break;
            }
        }

        private void CompilerOptions_Load(object sender, EventArgs e)
        {

        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            //do apply
            button4_Click(sender, e);

        }
    }
}
