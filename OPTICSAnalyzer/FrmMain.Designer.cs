namespace OPTICSAnalyzer
{
    partial class frmOPTICSAnalyzer
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.GroupBox groupBox1;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmOPTICSAnalyzer));
            System.Windows.Forms.GroupBox groupBox2;
            System.Windows.Forms.Panel panel1;
            System.Windows.Forms.Label label10;
            System.Windows.Forms.Label label9;
            System.Windows.Forms.Label label8;
            System.Windows.Forms.Label label7;
            System.Windows.Forms.Label label6;
            System.Windows.Forms.GroupBox groupBox4;
            System.Windows.Forms.Label label3;
            System.Windows.Forms.Label label2;
            System.Windows.Forms.Label label1;
            System.Windows.Forms.StatusStrip statusStrip1;
            this.lvImages = new System.Windows.Forms.ListView();
            this.imgListStatus = new System.Windows.Forms.ImageList(this.components);
            this.btnRefresh = new System.Windows.Forms.Button();
            this.picHistogram = new System.Windows.Forms.PictureBox();
            this.nudSelectionB = new System.Windows.Forms.NumericUpDown();
            this.nudSelectionA = new System.Windows.Forms.NumericUpDown();
            this.cmbWidth = new System.Windows.Forms.ComboBox();
            this.cmbHeight = new System.Windows.Forms.ComboBox();
            this.nudThreshold = new System.Windows.Forms.NumericUpDown();
            this.lblUnreachable = new System.Windows.Forms.Label();
            this.lblNotReachable = new System.Windows.Forms.Label();
            this.picThumbnail = new System.Windows.Forms.PictureBox();
            this.txtImageRDist = new System.Windows.Forms.TextBox();
            this.txtImageIndex = new System.Windows.Forms.TextBox();
            this.txtImagePath = new System.Windows.Forms.TextBox();
            this.lblConfigPath = new System.Windows.Forms.ToolStripStatusLabel();
            groupBox1 = new System.Windows.Forms.GroupBox();
            groupBox2 = new System.Windows.Forms.GroupBox();
            panel1 = new System.Windows.Forms.Panel();
            label10 = new System.Windows.Forms.Label();
            label9 = new System.Windows.Forms.Label();
            label8 = new System.Windows.Forms.Label();
            label7 = new System.Windows.Forms.Label();
            label6 = new System.Windows.Forms.Label();
            groupBox4 = new System.Windows.Forms.GroupBox();
            label3 = new System.Windows.Forms.Label();
            label2 = new System.Windows.Forms.Label();
            label1 = new System.Windows.Forms.Label();
            statusStrip1 = new System.Windows.Forms.StatusStrip();
            groupBox1.SuspendLayout();
            groupBox2.SuspendLayout();
            panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picHistogram)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudSelectionB)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudSelectionA)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudThreshold)).BeginInit();
            groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picThumbnail)).BeginInit();
            statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            groupBox1.Controls.Add(this.lvImages);
            groupBox1.Location = new System.Drawing.Point(12, 9);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new System.Drawing.Size(873, 462);
            groupBox1.TabIndex = 0;
            groupBox1.TabStop = false;
            groupBox1.Text = "Selected Images";
            // 
            // lvImages
            // 
            this.lvImages.BackColor = System.Drawing.SystemColors.ControlDark;
            this.lvImages.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lvImages.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lvImages.Location = new System.Drawing.Point(3, 18);
            this.lvImages.MultiSelect = false;
            this.lvImages.Name = "lvImages";
            this.lvImages.ShowGroups = false;
            this.lvImages.Size = new System.Drawing.Size(867, 441);
            this.lvImages.StateImageList = this.imgListStatus;
            this.lvImages.TabIndex = 0;
            this.lvImages.TileSize = new System.Drawing.Size(128, 128);
            this.lvImages.UseCompatibleStateImageBehavior = false;
            this.lvImages.SelectedIndexChanged += new System.EventHandler(this.lvImages_SelectedIndexChanged);
            this.lvImages.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvImages_KeyDown);
            // 
            // imgListStatus
            // 
            this.imgListStatus.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imgListStatus.ImageStream")));
            this.imgListStatus.TransparentColor = System.Drawing.Color.Transparent;
            this.imgListStatus.Images.SetKeyName(0, "green.png");
            this.imgListStatus.Images.SetKeyName(1, "red.png");
            // 
            // groupBox2
            // 
            groupBox2.Controls.Add(this.btnRefresh);
            groupBox2.Controls.Add(panel1);
            groupBox2.Controls.Add(label10);
            groupBox2.Controls.Add(this.nudSelectionB);
            groupBox2.Controls.Add(this.nudSelectionA);
            groupBox2.Controls.Add(label9);
            groupBox2.Controls.Add(this.cmbWidth);
            groupBox2.Controls.Add(this.cmbHeight);
            groupBox2.Controls.Add(this.nudThreshold);
            groupBox2.Controls.Add(label8);
            groupBox2.Controls.Add(label7);
            groupBox2.Controls.Add(label6);
            groupBox2.Location = new System.Drawing.Point(12, 480);
            groupBox2.Name = "groupBox2";
            groupBox2.Size = new System.Drawing.Size(1464, 410);
            groupBox2.TabIndex = 1;
            groupBox2.TabStop = false;
            groupBox2.Text = "Reachability Plot";
            // 
            // btnRefresh
            // 
            this.btnRefresh.Location = new System.Drawing.Point(1319, 21);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(139, 30);
            this.btnRefresh.TabIndex = 9;
            this.btnRefresh.Text = "Refresh";
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            // 
            // panel1
            // 
            panel1.AutoScroll = true;
            panel1.BackColor = System.Drawing.Color.White;
            panel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            panel1.Controls.Add(this.picHistogram);
            panel1.Location = new System.Drawing.Point(6, 56);
            panel1.Name = "panel1";
            panel1.Size = new System.Drawing.Size(1452, 348);
            panel1.TabIndex = 12;
            // 
            // picHistogram
            // 
            this.picHistogram.Location = new System.Drawing.Point(4, 4);
            this.picHistogram.Name = "picHistogram";
            this.picHistogram.Size = new System.Drawing.Size(925, 213);
            this.picHistogram.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.picHistogram.TabIndex = 0;
            this.picHistogram.TabStop = false;
            this.picHistogram.Paint += new System.Windows.Forms.PaintEventHandler(this.picHistogram_Paint);
            this.picHistogram.MouseDown += new System.Windows.Forms.MouseEventHandler(this.picHistogram_MouseDown);
            this.picHistogram.MouseMove += new System.Windows.Forms.MouseEventHandler(this.picHistogram_MouseMove);
            this.picHistogram.MouseUp += new System.Windows.Forms.MouseEventHandler(this.picHistogram_MouseUp);
            // 
            // label10
            // 
            label10.AutoSize = true;
            label10.Location = new System.Drawing.Point(643, 30);
            label10.Name = "label10";
            label10.Size = new System.Drawing.Size(116, 17);
            label10.TabIndex = 11;
            label10.Text = "Selection Index B";
            // 
            // nudSelectionB
            // 
            this.nudSelectionB.Location = new System.Drawing.Point(765, 27);
            this.nudSelectionB.Name = "nudSelectionB";
            this.nudSelectionB.Size = new System.Drawing.Size(82, 22);
            this.nudSelectionB.TabIndex = 7;
            this.nudSelectionB.ValueChanged += new System.EventHandler(this.nudSelectionB_ValueChanged);
            // 
            // nudSelectionA
            // 
            this.nudSelectionA.Location = new System.Drawing.Point(555, 28);
            this.nudSelectionA.Name = "nudSelectionA";
            this.nudSelectionA.Size = new System.Drawing.Size(82, 22);
            this.nudSelectionA.TabIndex = 6;
            this.nudSelectionA.ValueChanged += new System.EventHandler(this.nudSelectionA_ValueChanged);
            // 
            // label9
            // 
            label9.AutoSize = true;
            label9.Location = new System.Drawing.Point(431, 29);
            label9.Name = "label9";
            label9.Size = new System.Drawing.Size(116, 17);
            label9.TabIndex = 8;
            label9.Text = "Selection Index A";
            // 
            // cmbWidth
            // 
            this.cmbWidth.FormattingEnabled = true;
            this.cmbWidth.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "20",
            "25",
            "30",
            "50",
            "75",
            "100",
            "200",
            "400",
            "500"});
            this.cmbWidth.Location = new System.Drawing.Point(95, 26);
            this.cmbWidth.Name = "cmbWidth";
            this.cmbWidth.Size = new System.Drawing.Size(71, 24);
            this.cmbWidth.TabIndex = 4;
            this.cmbWidth.Text = "100";
            this.cmbWidth.TextChanged += new System.EventHandler(this.cmbWidth_TextChanged);
            // 
            // cmbHeight
            // 
            this.cmbHeight.FormattingEnabled = true;
            this.cmbHeight.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10",
            "20",
            "25",
            "30",
            "50",
            "75",
            "100",
            "200",
            "400",
            "500"});
            this.cmbHeight.Location = new System.Drawing.Point(266, 27);
            this.cmbHeight.Name = "cmbHeight";
            this.cmbHeight.Size = new System.Drawing.Size(71, 24);
            this.cmbHeight.TabIndex = 5;
            this.cmbHeight.Text = "25";
            this.cmbHeight.TextChanged += new System.EventHandler(this.cmbHeight_TextChanged);
            // 
            // nudThreshold
            // 
            this.nudThreshold.Location = new System.Drawing.Point(1073, 27);
            this.nudThreshold.Maximum = new decimal(new int[] {
            -727379968,
            232,
            0,
            0});
            this.nudThreshold.Name = "nudThreshold";
            this.nudThreshold.Size = new System.Drawing.Size(113, 22);
            this.nudThreshold.TabIndex = 8;
            this.nudThreshold.ValueChanged += new System.EventHandler(this.nudThreshold_ValueChanged);
            // 
            // label8
            // 
            label8.AutoSize = true;
            label8.Location = new System.Drawing.Point(868, 29);
            label8.Name = "label8";
            label8.Size = new System.Drawing.Size(199, 17);
            label8.TabIndex = 4;
            label8.Text = "Reachability Outlier Threshold";
            // 
            // label7
            // 
            label7.AutoSize = true;
            label7.Location = new System.Drawing.Point(172, 30);
            label7.Name = "label7";
            label7.Size = new System.Drawing.Size(88, 17);
            label7.TabIndex = 3;
            label7.Text = "Scale Height";
            // 
            // label6
            // 
            label6.AutoSize = true;
            label6.Location = new System.Drawing.Point(6, 29);
            label6.Name = "label6";
            label6.Size = new System.Drawing.Size(83, 17);
            label6.TabIndex = 2;
            label6.Text = "Scale Width";
            // 
            // groupBox4
            // 
            groupBox4.Controls.Add(this.lblUnreachable);
            groupBox4.Controls.Add(this.lblNotReachable);
            groupBox4.Controls.Add(this.picThumbnail);
            groupBox4.Controls.Add(this.txtImageRDist);
            groupBox4.Controls.Add(this.txtImageIndex);
            groupBox4.Controls.Add(this.txtImagePath);
            groupBox4.Controls.Add(label3);
            groupBox4.Controls.Add(label2);
            groupBox4.Controls.Add(label1);
            groupBox4.Location = new System.Drawing.Point(891, 9);
            groupBox4.Name = "groupBox4";
            groupBox4.Size = new System.Drawing.Size(585, 462);
            groupBox4.TabIndex = 3;
            groupBox4.TabStop = false;
            groupBox4.Text = "Selected Image";
            // 
            // lblUnreachable
            // 
            this.lblUnreachable.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))));
            this.lblUnreachable.Location = new System.Drawing.Point(18, 365);
            this.lblUnreachable.Name = "lblUnreachable";
            this.lblUnreachable.Size = new System.Drawing.Size(172, 49);
            this.lblUnreachable.TabIndex = 8;
            this.lblUnreachable.Text = "UNREACHABLE";
            this.lblUnreachable.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.lblUnreachable.Visible = false;
            // 
            // lblNotReachable
            // 
            this.lblNotReachable.BackColor = System.Drawing.Color.Yellow;
            this.lblNotReachable.Location = new System.Drawing.Point(18, 303);
            this.lblNotReachable.Name = "lblNotReachable";
            this.lblNotReachable.Size = new System.Drawing.Size(172, 48);
            this.lblNotReachable.TabIndex = 7;
            this.lblNotReachable.Text = "OUT OF THRESHOLD";
            this.lblNotReachable.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.lblNotReachable.Visible = false;
            // 
            // picThumbnail
            // 
            this.picThumbnail.Location = new System.Drawing.Point(196, 46);
            this.picThumbnail.Name = "picThumbnail";
            this.picThumbnail.Size = new System.Drawing.Size(384, 403);
            this.picThumbnail.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.picThumbnail.TabIndex = 6;
            this.picThumbnail.TabStop = false;
            this.picThumbnail.DoubleClick += new System.EventHandler(this.picThumbnail_DoubleClick);
            // 
            // txtImageRDist
            // 
            this.txtImageRDist.Location = new System.Drawing.Point(10, 116);
            this.txtImageRDist.Name = "txtImageRDist";
            this.txtImageRDist.ReadOnly = true;
            this.txtImageRDist.Size = new System.Drawing.Size(126, 22);
            this.txtImageRDist.TabIndex = 3;
            // 
            // txtImageIndex
            // 
            this.txtImageIndex.Location = new System.Drawing.Point(59, 53);
            this.txtImageIndex.Name = "txtImageIndex";
            this.txtImageIndex.ReadOnly = true;
            this.txtImageIndex.Size = new System.Drawing.Size(77, 22);
            this.txtImageIndex.TabIndex = 2;
            // 
            // txtImagePath
            // 
            this.txtImagePath.Location = new System.Drawing.Point(59, 21);
            this.txtImagePath.Name = "txtImagePath";
            this.txtImagePath.ReadOnly = true;
            this.txtImagePath.Size = new System.Drawing.Size(468, 22);
            this.txtImagePath.TabIndex = 1;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new System.Drawing.Point(7, 96);
            label3.Name = "label3";
            label3.Size = new System.Drawing.Size(144, 17);
            label3.TabIndex = 2;
            label3.Text = "Reachability Distance";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new System.Drawing.Point(12, 56);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(41, 17);
            label2.TabIndex = 1;
            label2.Text = "Index";
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new System.Drawing.Point(16, 21);
            label1.Name = "label1";
            label1.Size = new System.Drawing.Size(37, 17);
            label1.TabIndex = 0;
            label1.Text = "Path";
            // 
            // statusStrip1
            // 
            statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lblConfigPath});
            statusStrip1.Location = new System.Drawing.Point(0, 893);
            statusStrip1.Name = "statusStrip1";
            statusStrip1.Size = new System.Drawing.Size(1488, 25);
            statusStrip1.TabIndex = 4;
            statusStrip1.Text = "jo";
            // 
            // lblConfigPath
            // 
            this.lblConfigPath.Name = "lblConfigPath";
            this.lblConfigPath.Size = new System.Drawing.Size(151, 20);
            this.lblConfigPath.Text = "toolStripStatusLabel1";
            // 
            // frmOPTICSAnalyzer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1488, 918);
            this.Controls.Add(statusStrip1);
            this.Controls.Add(groupBox4);
            this.Controls.Add(groupBox2);
            this.Controls.Add(groupBox1);
            this.Name = "frmOPTICSAnalyzer";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "OPTICSAnalyzer";
            groupBox1.ResumeLayout(false);
            groupBox2.ResumeLayout(false);
            groupBox2.PerformLayout();
            panel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.picHistogram)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudSelectionB)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudSelectionA)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudThreshold)).EndInit();
            groupBox4.ResumeLayout(false);
            groupBox4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picThumbnail)).EndInit();
            statusStrip1.ResumeLayout(false);
            statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStripStatusLabel lblConfigPath;
        private System.Windows.Forms.ListView lvImages;
        private System.Windows.Forms.Label lblUnreachable;
        private System.Windows.Forms.Label lblNotReachable;
        private System.Windows.Forms.PictureBox picThumbnail;
        private System.Windows.Forms.TextBox txtImageRDist;
        private System.Windows.Forms.TextBox txtImageIndex;
        private System.Windows.Forms.TextBox txtImagePath;
        private System.Windows.Forms.NumericUpDown nudThreshold;
        private System.Windows.Forms.ComboBox cmbWidth;
        private System.Windows.Forms.ComboBox cmbHeight;
        private System.Windows.Forms.NumericUpDown nudSelectionB;
        private System.Windows.Forms.NumericUpDown nudSelectionA;
        private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.PictureBox picHistogram;
        private System.Windows.Forms.ImageList imgListStatus;
    }
}