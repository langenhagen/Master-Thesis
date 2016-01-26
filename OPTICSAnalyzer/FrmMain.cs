using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Configuration;
using System.IO;
using System.Globalization;

namespace OPTICSAnalyzer
{
    public partial class frmOPTICSAnalyzer : Form
    {
        private string _configPath;

        private List<string> _imagePaths;
        private List<int> _OPTICSMapping;
        private List<float> _rDistances;

        private uint _histHeight = 1024;
        private float _histMax;
        private Bitmap _histImg;
        private float _histHeightMultiplier = 1;
        private float _histWidthMultiplier = 1;

        private float _thresh = 0;
        private bool _isCurrentlySelecting = false;
        private float _selectionA = 0;
        private float _selectionB = 0;

        private ImageList _imageList = new ImageList();

        // ctor

        public frmOPTICSAnalyzer(string configPath)
        {
            InitializeComponent();
            _configPath = configPath;
            lblConfigPath.Text = configPath;

            var settings = ConfigurationManager.AppSettings;

            try
            {
                string[] array;
                array = File.ReadAllLines(settings["images_file"]);
                _imagePaths = new List<string>(array);
                array = File.ReadAllLines(settings["images_optics_ordered_file"]);
                _OPTICSMapping = array.Select(StringToInt).ToList();
                array = File.ReadAllLines(settings["reachabilities_file"]);
                _rDistances = array.Select(StringToFloat).ToList();
            }
            catch (Exception e) 
            {
                string msg = "The values in the config file are invalid:\n " + e.Message;
                MessageBox.Show(msg, "OPTICSAnalyzer", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
                Environment.Exit(0);
            }
            _histMax = _rDistances.Max();
            MakeHistogram(_rDistances, _histHeight);

            _histHeightMultiplier = 4;
            picHistogram.Height = (int)(_histImg.Height / _histHeightMultiplier);

            nudSelectionA.Maximum = _rDistances.Count - 1;
            nudSelectionB.Maximum = _rDistances.Count - 1;
            nudThreshold.DecimalPlaces = 2;
            nudThreshold.Maximum = (decimal)(_histMax+0.5);
        }

        // methods

        private void MakeHistogram(List<float> data, uint histHeight)
        {
            Bitmap histImg = new Bitmap(data.Count, (int)(histHeight));
            float histMax = data.Max();

            using (Graphics g = Graphics.FromImage(histImg))
            {
                for (int i = 0; i < data.Count; ++i)
                {
                    if (data[i] < 0)
                    {
                        g.DrawLine(
                            Pens.LightGreen,
                            new Point(i, histImg.Height),
                            new Point(i, 0)
                            );
                    }
                    else
                    {
                        float percent = data[i] / histMax;   // What percentage of the max is this value?
                        g.DrawLine(
                            Pens.Black,
                            new Point(i, histImg.Height),
                            new Point(i, histImg.Height - (int)(percent * histHeight))  // Use that percentage of the height
                            );
                    }
                }
            }
            _histImg = histImg;
            picHistogram.Image = histImg;
            picHistogram.Height = (int)(histImg.Height * _histHeightMultiplier);
            picHistogram.Width = (int)(histImg.Width * _histWidthMultiplier);
        }

        // helpers

        private static int StringToInt(string s)
        {
            int i;
            bool success = int.TryParse(s, out i);
            return i;
        }

        private static float StringToFloat(string s)
        {
            float f;
            bool success = float.TryParse(s, NumberStyles.AllowDecimalPoint, CultureInfo.InvariantCulture, out f);
            if (!success || f > float.MaxValue)
                f = -1;
            return f;
        }

        private void selectImages() 
        {
            lvImages.Items.Clear();
            if (lvImages.LargeImageList != null)
                lvImages.LargeImageList.Dispose();

            int lowerIdx = (int)Math.Min(_selectionA, _selectionB);
            int upperIdx = (int)Math.Max(_selectionA, _selectionB);

            _imageList = new ImageList();
            _imageList.ColorDepth = ColorDepth.Depth24Bit;
            _imageList.ImageSize = new Size(128, 128);

            ListView.ListViewItemCollection items = new ListView.ListViewItemCollection(lvImages);
            lvImages.LargeImageList = _imageList;

            for (int i = lowerIdx; i <= upperIdx; ++i)
            {
                string path = _imagePaths[_OPTICSMapping[i]];
                _imageList.Images.Add(Image.FromFile(path));
            }

            for (int i = lowerIdx; i <= upperIdx; ++i)
            {
                ListViewItem item = new ListViewItem();
                item.Text = _imagePaths[_OPTICSMapping[i]];
                item.Tag = _rDistances[i];
                item.ImageIndex = i - lowerIdx;
                item.StateImageIndex = _rDistances[i] > _thresh || _rDistances[i] < 0 ? 1 : 0;
                items.Add(item);
            }
        }

        // callbacks

        private void cmbWidth_TextChanged(object sender, EventArgs e)
        {
            float f;
            bool b = float.TryParse(cmbWidth.Text, out f);
            _histWidthMultiplier = 100 / f;
            picHistogram.Width = (int)(_histImg.Width / _histWidthMultiplier);

            picHistogram.Invalidate();
        }

        private void cmbHeight_TextChanged(object sender, EventArgs e)
        {
            float f;
            bool b = float.TryParse(cmbHeight.Text, out f);
            _histHeightMultiplier = 100 / f;
            picHistogram.Height = (int)(_histImg.Height / _histHeightMultiplier);
            
            picHistogram.Invalidate();
        }

        private void picHistogram_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                _thresh = (float)(picHistogram.Height - e.Y) * _histHeightMultiplier / _histHeight * _histMax;

                nudThreshold.Value = (decimal)(_thresh);
                picHistogram.Invalidate();
            }
            else if (e.Button == MouseButtons.Left)
            {
                _selectionA = e.X * _histWidthMultiplier;
                _selectionB = e.X * _histWidthMultiplier;
                _isCurrentlySelecting = true;

                nudSelectionA.Value = (int)(_selectionA);
                nudSelectionB.Value = (int)(_selectionA);
            }

        }

        private void picHistogram_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isCurrentlySelecting)
            {
                _selectionB = Math.Max( 0, Math.Min( e.X * _histWidthMultiplier, _rDistances.Count-1));
                

                nudSelectionB.Value = (int)(_selectionB);
                picHistogram.Invalidate();
            }
        }

        private void picHistogram_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left && _isCurrentlySelecting)
            {
                /*float currentSelection = e.X * _histWidthMultiplier;

                float tmp = Math.Max(0, Math.Min(currentSelection, _selectionA));
                _selectionB = Math.Min(Math.Max(currentSelection, _selectionA), _rDistances.Count-1);
                _selectionA = tmp;*/
                _selectionB = Math.Max(0, Math.Min(e.X * _histWidthMultiplier, _rDistances.Count - 1));

                _isCurrentlySelecting = false;

                nudSelectionA.Value = (int)(_selectionA);
                nudSelectionB.Value = (int)(_selectionB);
                picHistogram.Invalidate();
                selectImages();
            }
        }

        private void nudSelectionA_ValueChanged(object sender, EventArgs e)
        {
            _selectionA = (float)(nudSelectionA.Value);
            picHistogram.Invalidate();
        }

        private void nudSelectionB_ValueChanged(object sender, EventArgs e)
        {
            _selectionB = (float)(nudSelectionB.Value);
            picHistogram.Invalidate();
        }

        private void nudThreshold_ValueChanged(object sender, EventArgs e)
        {
            _thresh = (float)(nudThreshold.Value);
            picHistogram.Invalidate();
        }

        private void picHistogram_Paint(object sender, PaintEventArgs e)
        {
            Color c = Color.Blue;
            int y = (int) ((1 -_thresh / _histMax) * picHistogram.Height);
            e.Graphics.DrawLine(new Pen(c), new Point(0, y), new Point(picHistogram.Width, y));

            
            int x = (int)(Math.Min(_selectionA, _selectionB) / _histWidthMultiplier);
            y = 0;
            int width = (int)(Math.Abs(_selectionA - _selectionB) / _histWidthMultiplier);
            int height = picHistogram.Height;

            c = Color.FromArgb(128, Color.Pink);
            SolidBrush pinkBrush = new SolidBrush(c);

            e.Graphics.FillRectangle(pinkBrush, x, y, width, height);


            if (lvImages.SelectedItems.Count > 0)
            {
                ListViewItem item = lvImages.SelectedItems[0];
                int index = _OPTICSMapping.IndexOf( _imagePaths.IndexOf(item.Text));
                c = Color.FromArgb(64, Color.DarkRed);

                x = (int)( (float)index / (_rDistances.Count - 1) * picHistogram.Width);

                e.Graphics.DrawLine(new Pen(c), new Point(x, 0), new Point(x, picHistogram.Height));
            }


        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            selectImages();
        }

        private void lvImages_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lvImages.SelectedItems.Count <= 0)
                return;

            ListViewItem item = lvImages.SelectedItems[0];
            txtImagePath.Text = item.Text;
            txtImageIndex.Text = _OPTICSMapping.IndexOf( _imagePaths.IndexOf(item.Text)).ToString();
            txtImageRDist.Text = item.Tag.ToString() == "-1" ? "UNDEFINED" : item.Tag.ToString();

            float rdist;
            bool b = float.TryParse(item.Tag.ToString(), out rdist);
            if (b == false)
                MessageBox.Show("Error parsing item.Tag to rdist", "OPTICSAnalyzer", MessageBoxButtons.OK, MessageBoxIcon.Error);

            if (rdist > _thresh)
                lblNotReachable.Visible = true;
            else
                lblNotReachable.Visible = false;

            if (rdist < 0)
                lblUnreachable.Visible = true;
            else
                lblUnreachable.Visible = false;

            picThumbnail.Image = item.ImageList.Images[item.ImageIndex];
            picHistogram.Invalidate();
        }

        private void picThumbnail_DoubleClick(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start(txtImagePath.Text);
        }

        private void lvImages_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter && lvImages.SelectedItems.Count > 0)
            {
                System.Diagnostics.Process.Start(lvImages.SelectedItems[0].Text);
            }
        }
    }
}
