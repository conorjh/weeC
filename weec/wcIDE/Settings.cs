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
    public partial class Settings : Form
    {
        public Settings()
        {
            InitializeComponent();
        }

        private void Settings_Load(object sender, EventArgs e)
        {
            tempCompilerPath = tbCompilerPath.Text = wcIDEData.pathCompiler;
            tempVMPath = tbVMPath.Text = wcIDEData.pathVM;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var FD = new System.Windows.Forms.OpenFileDialog();
            if (FD.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;

            wcIDEData.pathCompiler = FD.FileName;
            tbCompilerPath.Text = wcIDEData.pathCompiler;
        }

        private void btnBrowseVM_Click(object sender, EventArgs e)
        {
            var FD = new System.Windows.Forms.OpenFileDialog();
            if (FD.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;

            wcIDEData.pathVM = FD.FileName;
        }

        public string tempVMPath, tempCompilerPath;

        private void button2_Click(object sender, EventArgs e)
        {
            button4_Click(sender,e);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            wcIDEData.pathCompiler = tempCompilerPath;
            wcIDEData.pathVM = tempVMPath;
        }
    }
}
