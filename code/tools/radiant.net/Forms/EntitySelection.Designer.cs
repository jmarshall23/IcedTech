namespace radiant.net.forms
{
    partial class EntitySelection
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
            this.EntityListBox = new System.Windows.Forms.ListBox();
            this.GotoEntityButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // EntityListBox
            // 
            this.EntityListBox.FormattingEnabled = true;
            this.EntityListBox.Location = new System.Drawing.Point(13, 13);
            this.EntityListBox.Name = "EntityListBox";
            this.EntityListBox.Size = new System.Drawing.Size(365, 472);
            this.EntityListBox.TabIndex = 0;
            this.EntityListBox.SelectedIndexChanged += new System.EventHandler(this.EntityListBox_SelectedIndexChanged);
            // 
            // GotoEntityButton
            // 
            this.GotoEntityButton.Location = new System.Drawing.Point(134, 491);
            this.GotoEntityButton.Name = "GotoEntityButton";
            this.GotoEntityButton.Size = new System.Drawing.Size(113, 41);
            this.GotoEntityButton.TabIndex = 1;
            this.GotoEntityButton.Text = "Go to Entity";
            this.GotoEntityButton.UseVisualStyleBackColor = true;
            this.GotoEntityButton.Click += new System.EventHandler(this.GotoEntityButton_Click);
            // 
            // EntitySelection
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(398, 547);
            this.Controls.Add(this.GotoEntityButton);
            this.Controls.Add(this.EntityListBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "EntitySelection";
            this.Text = "EntitySelection";
            this.Load += new System.EventHandler(this.EntitySelection_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListBox EntityListBox;
        private System.Windows.Forms.Button GotoEntityButton;
    }
}