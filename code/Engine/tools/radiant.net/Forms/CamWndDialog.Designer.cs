namespace radiant.net.forms
{
    partial class CamWndDialog
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
            this.renderModeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.wireframeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.unlitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.litRenderToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.RenderPanel = new System.Windows.Forms.Panel();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.renderModeToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(940, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            this.menuStrip1.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.menuStrip1_ItemClicked);
            // 
            // renderModeToolStripMenuItem
            // 
            this.renderModeToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.wireframeToolStripMenuItem,
            this.unlitToolStripMenuItem,
            this.litRenderToolStripMenuItem});
            this.renderModeToolStripMenuItem.Name = "renderModeToolStripMenuItem";
            this.renderModeToolStripMenuItem.Size = new System.Drawing.Size(90, 20);
            this.renderModeToolStripMenuItem.Text = "&Render Mode";
            // 
            // wireframeToolStripMenuItem
            // 
            this.wireframeToolStripMenuItem.Name = "wireframeToolStripMenuItem";
            this.wireframeToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
            this.wireframeToolStripMenuItem.Text = "&Wireframe";
            // 
            // unlitToolStripMenuItem
            // 
            this.unlitToolStripMenuItem.Name = "unlitToolStripMenuItem";
            this.unlitToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
            this.unlitToolStripMenuItem.Text = "&Unlit";
            this.unlitToolStripMenuItem.Click += new System.EventHandler(this.unlitToolStripMenuItem_Click);
            // 
            // litRenderToolStripMenuItem
            // 
            this.litRenderToolStripMenuItem.Name = "litRenderToolStripMenuItem";
            this.litRenderToolStripMenuItem.Size = new System.Drawing.Size(129, 22);
            this.litRenderToolStripMenuItem.Text = "&Lit Render";
            this.litRenderToolStripMenuItem.Click += new System.EventHandler(this.litRenderToolStripMenuItem_Click);
            // 
            // RenderPanel
            // 
            this.RenderPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.RenderPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.RenderPanel.Location = new System.Drawing.Point(0, 24);
            this.RenderPanel.Name = "RenderPanel";
            this.RenderPanel.Size = new System.Drawing.Size(940, 600);
            this.RenderPanel.TabIndex = 1;
            // 
            // CamWndDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(940, 624);
            this.Controls.Add(this.RenderPanel);
            this.Controls.Add(this.menuStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.MainMenuStrip = this.menuStrip1;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "CamWndDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "Camera Window";
            this.Load += new System.EventHandler(this.CamWndDialog_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem renderModeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem wireframeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem unlitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem litRenderToolStripMenuItem;
        private System.Windows.Forms.Panel RenderPanel;
    }
}