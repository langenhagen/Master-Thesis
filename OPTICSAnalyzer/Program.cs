using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Configuration;
using System.Reflection;

namespace OPTICSAnalyzer
{
    static class Program
    {
        /// <summary>
        /// Der Haupteinstiegspunkt für die Anwendung.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            string configFile = string.Empty;
            if (args.Length > 0)
                configFile = args[0];

            configFile = SetConfigFile(configFile);

            if (!System.IO.File.Exists(configFile))
            {
                string msg = "The file \"" + configFile + "\" does not exist.";
                DialogResult result = MessageBox.Show(msg, "OPTICSAnalyzer", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
                Environment.Exit(0);
            }

            var settings = ConfigurationManager.AppSettings;
            if (settings["images_file"] == null && 
                settings["images_optics_ordered_file"] == null && 
                settings["reachabilities_file"] == null)
            {
                string msg = "The path \"" + configFile + "\" does not hold a valid config file: \n\n" +
                    "images_file: " + settings["images_file"] + "\n" +
                    "images_optics_ordered_file: " + settings["images_optics_ordered_file"] + "\n" +
                    "reachabilities_file: " + settings["reachabilities_file"];
                DialogResult result = MessageBox.Show(msg, "OPTICSAnalyzer", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
                Environment.Exit(0);
            }

            Application.Run(new frmOPTICSAnalyzer(configFile));
        }

        /// <summary>
        /// sets the application's config file.
        /// </summary>
        /// <param name="path"></param>
        static string SetConfigFile(string path)
        {
            if (path.CompareTo(string.Empty) == 0)
            {
                OpenFileDialog dlg = new OpenFileDialog();
                dlg.Filter = "config files (*.cfg)|*.cfg|All files (*.*)|*.*";

                dlg.InitialDirectory = System.IO.Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

                DialogResult dlgResult = dlg.ShowDialog();

                if ( dlgResult == DialogResult.OK)
                    path = dlg.FileName;
                else if (dlgResult == DialogResult.Cancel)
                {
                    Application.Exit();
                    Environment.Exit(0);
                }
            }

            Configuration config
                = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
            config.AppSettings.File = path;
            config.Save(ConfigurationSaveMode.Modified);
            ConfigurationManager.RefreshSection("appSettings");

            return path;
        }
    }
}
