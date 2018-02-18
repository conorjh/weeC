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

        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void button4_Click(object sender, EventArgs e)
        {

        }
    }
}
