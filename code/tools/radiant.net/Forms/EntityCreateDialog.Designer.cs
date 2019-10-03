namespace radiant.net.forms
{
    partial class EntityCreateDialog
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.entityTreeView = new System.Windows.Forms.TreeView();
            this.CreateEntityButton = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.entityTreeView);
            this.groupBox1.Location = new System.Drawing.Point(13, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(347, 513);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Entity Types";
            // 
            // entityTreeView
            // 
            this.entityTreeView.Location = new System.Drawing.Point(7, 20);
            this.entityTreeView.Name = "entityTreeView";
            this.entityTreeView.Size = new System.Drawing.Size(334, 475);
            this.entityTreeView.TabIndex = 0;
            // 
            // CreateEntityButton
            // 
            this.CreateEntityButton.Location = new System.Drawing.Point(110, 542);
            this.CreateEntityButton.Name = "CreateEntityButton";
            this.CreateEntityButton.Size = new System.Drawing.Size(139, 47);
            this.CreateEntityButton.TabIndex = 2;
            this.CreateEntityButton.Text = "Create Entity";
            this.CreateEntityButton.UseVisualStyleBackColor = true;
            this.CreateEntityButton.Click += new System.EventHandler(this.CreateEntityButton_Click);
            // 
            // EntityCreateDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(372, 610);
            this.Controls.Add(this.CreateEntityButton);
            this.Controls.Add(this.groupBox1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "EntityCreateDialog";
            this.ShowIcon = false;
            this.Text = "Create a Entity...";
            this.Load += new System.EventHandler(this.EntityCreateDialog_Load);
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TreeView entityTreeView;
        private System.Windows.Forms.Button CreateEntityButton;
    }
}