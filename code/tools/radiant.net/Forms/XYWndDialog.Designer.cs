namespace radiant.net.forms
{
    partial class XYWndDialog
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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.topToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.sideToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.RenderPanel = new System.Windows.Forms.Panel();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.xyWndTab = new System.Windows.Forms.TabPage();
            this.texturesTab = new System.Windows.Forms.TabPage();
            this.texWndPanel = new System.Windows.Forms.Panel();
            this.menuStrip1.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.xyWndTab.SuspendLayout();
            this.texturesTab.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.viewToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(800, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // viewToolStripMenuItem
            // 
            this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.topToolStripMenuItem,
            this.sideToolStripMenuItem});
            this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            this.viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.viewToolStripMenuItem.Text = "&View";
            // 
            // topToolStripMenuItem
            // 
            this.topToolStripMenuItem.Name = "topToolStripMenuItem";
            this.topToolStripMenuItem.Size = new System.Drawing.Size(96, 22);
            this.topToolStripMenuItem.Text = "&Top";
            // 
            // sideToolStripMenuItem
            // 
            this.sideToolStripMenuItem.Name = "sideToolStripMenuItem";
            this.sideToolStripMenuItem.Size = new System.Drawing.Size(96, 22);
            this.sideToolStripMenuItem.Text = "&Side";
            // 
            // RenderPanel
            // 
            this.RenderPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.RenderPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.RenderPanel.Location = new System.Drawing.Point(3, 3);
            this.RenderPanel.Name = "RenderPanel";
            this.RenderPanel.Size = new System.Drawing.Size(786, 388);
            this.RenderPanel.TabIndex = 1;
            // 
            // tabControl1
            // 
            this.tabControl1.Alignment = System.Windows.Forms.TabAlignment.Bottom;
            this.tabControl1.Controls.Add(this.xyWndTab);
            this.tabControl1.Controls.Add(this.texturesTab);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 24);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.Padding = new System.Drawing.Point(6, 6);
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(800, 426);
            this.tabControl1.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
            this.tabControl1.TabIndex = 2;
            // 
            // xyWndTab
            // 
            this.xyWndTab.Controls.Add(this.RenderPanel);
            this.xyWndTab.Location = new System.Drawing.Point(4, 4);
            this.xyWndTab.Name = "xyWndTab";
            this.xyWndTab.Padding = new System.Windows.Forms.Padding(3);
            this.xyWndTab.Size = new System.Drawing.Size(792, 394);
            this.xyWndTab.TabIndex = 0;
            this.xyWndTab.Text = "XY Window";
            this.xyWndTab.UseVisualStyleBackColor = true;
            // 
            // texturesTab
            // 
            this.texturesTab.Controls.Add(this.texWndPanel);
            this.texturesTab.Location = new System.Drawing.Point(4, 4);
            this.texturesTab.Name = "texturesTab";
            this.texturesTab.Padding = new System.Windows.Forms.Padding(3);
            this.texturesTab.Size = new System.Drawing.Size(792, 394);
            this.texturesTab.TabIndex = 1;
            this.texturesTab.Text = "Textures";
            this.texturesTab.UseVisualStyleBackColor = true;
            // 
            // texWndPanel
            // 
            this.texWndPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.texWndPanel.Location = new System.Drawing.Point(3, 3);
            this.texWndPanel.Name = "texWndPanel";
            this.texWndPanel.Size = new System.Drawing.Size(786, 388);
            this.texWndPanel.TabIndex = 0;
            // 
            // XYWndDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "XYWndDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "XY Window";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.tabControl1.ResumeLayout(false);
            this.xyWndTab.ResumeLayout(false);
            this.texturesTab.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem topToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem sideToolStripMenuItem;
        private System.Windows.Forms.Panel RenderPanel;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage xyWndTab;
        private System.Windows.Forms.TabPage texturesTab;
        private System.Windows.Forms.Panel texWndPanel;
    }
}