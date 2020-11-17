/*
* This is a personal academic project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
*/
using System;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32.SafeHandles;

namespace instdrv_sharp
{
    public partial class Form1 : Form
    {

        private SafeFileHandle _file;

        public Form1()
        {
            InitializeComponent();
        }

        private static uint CTL_CODE(uint deviceType, uint function, uint method, uint access)
        {
            return ((deviceType) << 16) | ((access) << 14) | ((function) << 2) | (method);
        }

        void Print(string message)
        {
            textBox2.AppendText(message + Environment.NewLine);
        }

        private void button9_Click(object sender, EventArgs e)
        {
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            manager.InstallDriver(textBox1.Text, textBox4.Text);
        }

        private void button8_Click(object sender, EventArgs e)
        {
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            manager.DeleteDriver(textBox4.Text);
        }

        private void button7_Click(object sender, EventArgs e)
        {
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            manager.RunDriver(textBox4.Text);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            manager.StopService(textBox4.Text);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            manager.OpenDevice(textBox3.Text, out _file);
        }

        private void button4_Click(object sender, EventArgs e)
        {
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            manager.CloseDevice(ref _file, textBox3.Text);
        }

        private void button2_Click(object sender, EventArgs e)
        {

            var data = new[]{ (byte)'a', (byte)'b', (byte)'c', (byte)'d', (byte)'e', (byte)'f', (byte)'g', (byte)'h', (byte)'i', (byte)'j', (byte)'k', (byte)'l', (byte)'m', (byte)'n', (byte)'o', (byte)'p' };
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            manager.Write(textBox3.Text, ref data, 16);

            Print(data[0].ToString("X") + data[1].ToString("X") + data[2].ToString("X") + data[3].ToString("X"));

        }

        private void button3_Click(object sender, EventArgs e)
        {
            var data = new byte[8];
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            manager.Read(textBox3.Text, ref data, 8);

            Print(data[0].ToString("X") + data[1].ToString("X") + data[2].ToString("X") + data[3].ToString("X"));
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = openFileDialog1.FileName;
            }
        }

        private void button11_Click(object sender, EventArgs e)
        {
            ScManager.PrintDelegate printDelegate = Print;
            var outBuf = new byte[8];
            var inBuf = "abcdefghijklmnop";
            var manager = new ScManager(printDelegate);
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x800, 0, 0), ref outBuf, in inBuf);
            Print(outBuf[0].ToString("X") + outBuf[1].ToString("X") + outBuf[2].ToString("X") + outBuf[3].ToString("X"));
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x801, 1, 0), ref outBuf, in inBuf);
            Print(outBuf[0].ToString("X") + outBuf[1].ToString("X") + outBuf[2].ToString("X") + outBuf[3].ToString("X"));
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x802, 2, 0), ref outBuf, in inBuf);
            Print(outBuf[0].ToString("X") + outBuf[1].ToString("X") + outBuf[2].ToString("X") + outBuf[3].ToString("X"));
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x803, 3, 0), ref outBuf, in inBuf);
            Print(outBuf[0].ToString("X") + outBuf[1].ToString("X") + outBuf[2].ToString("X") + outBuf[3].ToString("X"));
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x804, 0, 0), ref outBuf, in inBuf);
            Print(outBuf[0].ToString("X") + outBuf[1].ToString("X") + outBuf[2].ToString("X") + outBuf[3].ToString("X"));
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x805, 1, 0), ref outBuf, in inBuf);
            Print(outBuf[0].ToString("X") + outBuf[1].ToString("X") + outBuf[2].ToString("X") + outBuf[3].ToString("X"));
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x806, 2, 0), ref outBuf, in inBuf);
            Print(outBuf[0].ToString("X") + outBuf[1].ToString("X") + outBuf[2].ToString("X") + outBuf[3].ToString("X"));
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x807, 3, 0), ref outBuf, in inBuf);
            Print(outBuf[0].ToString("X") + outBuf[1].ToString("X") + outBuf[2].ToString("X") + outBuf[3].ToString("X"));

        }

        private void button13_Click(object sender, EventArgs e)
        {
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            var inBuf = textBox5.Text;
            var outBuf = new byte[8];
            Print("Creating device");
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x800, 0, 0), ref outBuf, in inBuf);
            Print("Device created");
        }

        private void button12_Click(object sender, EventArgs e)
        {
            ScManager.PrintDelegate printDelegate = Print;
            var manager = new ScManager(printDelegate);
            var inBuf = textBox5.Text;
            var outBuf = new byte[8];
            Print("Deleting device");
            manager.SendCodes(textBox3.Text, CTL_CODE(0x00008301, 0x801, 0, 0), ref outBuf, in inBuf);
            Print("Device deleted");
        }

        private void textBox5_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
