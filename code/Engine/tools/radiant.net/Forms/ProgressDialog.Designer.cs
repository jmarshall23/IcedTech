namespace radiant.net.forms
{
    partial class ProgressDialog
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
            this.LoadingMapNameTextBox = new System.Windows.Forms.TextBox();
            this.LoadingWorkTextBox = new System.Windows.Forms.TextBox();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.CancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // LoadingMapNameTextBox
            // 
            this.LoadingMapNameTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.LoadingMapNameTextBox.Location = new System.Drawing.Point(13, 24);
            this.LoadingMapNameTextBox.Name = "LoadingMapNameTextBox";
            this.LoadingMapNameTextBox.ReadOnly = true;
            this.LoadingMapNameTextBox.Size = new System.Drawing.Size(461, 13);
            this.LoadingMapNameTextBox.TabIndex = 0;
            this.LoadingMapNameTextBox.Text = "Loading <MAP> Please Wait...";
            this.LoadingMapNameTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.LoadingMapNameTextBox.TextChanged += new System.EventHandler(this.LoadingMapNameTextBox_TextChanged);
            // 
            // LoadingWorkTextBox
            // 
            this.LoadingWorkTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.LoadingWorkTextBox.Location = new System.Drawing.Point(6, 43);
            this.LoadingWorkTextBox.Name = "LoadingWorkTextBox";
            this.LoadingWorkTextBox.ReadOnly = true;
            this.LoadingWorkTextBox.Size = new System.Drawing.Size(461, 13);
            this.LoadingWorkTextBox.TabIndex = 1;
            this.LoadingWorkTextBox.Text = "Loading Models...";
            this.LoadingWorkTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(6, 74);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(472, 36);
            this.progressBar1.TabIndex = 2;
            // 
            // CancelButton
            // 
            this.CancelButton.Location = new System.Drawing.Point(162, 129);
            this.CancelButton.Name = "CancelButton";
            this.CancelButton.Size = new System.Drawing.Size(162, 40);
            this.CancelButton.TabIndex = 3;
            this.CancelButton.Text = "Cancel";
            this.CancelButton.UseVisualStyleBackColor = true;
            this.CancelButton.Click += new System.EventHandler(this.CancelButton_Click);
            // 
            // ProgressDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(486, 195);
            this.Controls.Add(this.CancelButton);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.LoadingWorkTextBox);
            this.Controls.Add(this.LoadingMapNameTextBox);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ProgressDialog";
            this.ShowIcon = false;
            this.Text = "ProgressDialog";
            this.Load += new System.EventHandler(this.ProgressDialog_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox LoadingMapNameTextBox;
        private System.Windows.Forms.TextBox LoadingWorkTextBox;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Button CancelButton;
    }
}