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
            this.tabControl = new System.Windows.Forms.TabControl();
            this.xyWndTab = new System.Windows.Forms.TabPage();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.newEntityTab = new System.Windows.Forms.TabPage();
            this.CreateEntityButton = new System.Windows.Forms.Button();
            this.entityTreeView = new System.Windows.Forms.TreeView();
            this.editEntityTab = new System.Windows.Forms.TabPage();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.viewToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.topToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.bottomToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.texturesTab = new System.Windows.Forms.TabPage();
            this.texWndPanel = new System.Windows.Forms.Panel();
            this.EntityListBox = new System.Windows.Forms.ListBox();
            this.gotoEntityButton = new System.Windows.Forms.Button();
            this.gameTab = new System.Windows.Forms.TabPage();
            this.GamePanel = new System.Windows.Forms.Panel();
            this.tabControl.SuspendLayout();
            this.xyWndTab.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.newEntityTab.SuspendLayout();
            this.editEntityTab.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.texturesTab.SuspendLayout();
            this.gameTab.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl
            // 
            this.tabControl.Controls.Add(this.xyWndTab);
            this.tabControl.Controls.Add(this.texturesTab);
            this.tabControl.Controls.Add(this.gameTab);
            this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl.Location = new System.Drawing.Point(0, 0);
            this.tabControl.Name = "tabControl";
            this.tabControl.Padding = new System.Drawing.Point(6, 6);
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(1400, 755);
            this.tabControl.SizeMode = System.Windows.Forms.TabSizeMode.FillToRight;
            this.tabControl.TabIndex = 2;
            // 
            // xyWndTab
            // 
            this.xyWndTab.Controls.Add(this.splitContainer1);
            this.xyWndTab.Controls.Add(this.menuStrip1);
            this.xyWndTab.Location = new System.Drawing.Point(4, 28);
            this.xyWndTab.Name = "xyWndTab";
            this.xyWndTab.Padding = new System.Windows.Forms.Padding(3);
            this.xyWndTab.Size = new System.Drawing.Size(1392, 723);
            this.xyWndTab.TabIndex = 0;
            this.xyWndTab.Text = "World Edit:    ";
            this.xyWndTab.UseVisualStyleBackColor = true;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(3, 27);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.tabControl1);
            this.splitContainer1.Size = new System.Drawing.Size(1386, 693);
            this.splitContainer1.SplitterDistance = 1126;
            this.splitContainer1.TabIndex = 0;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.newEntityTab);
            this.tabControl1.Controls.Add(this.editEntityTab);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(256, 693);
            this.tabControl1.TabIndex = 0;
            // 
            // newEntityTab
            // 
            this.newEntityTab.Controls.Add(this.CreateEntityButton);
            this.newEntityTab.Controls.Add(this.entityTreeView);
            this.newEntityTab.Location = new System.Drawing.Point(4, 22);
            this.newEntityTab.Name = "newEntityTab";
            this.newEntityTab.Padding = new System.Windows.Forms.Padding(3);
            this.newEntityTab.Size = new System.Drawing.Size(248, 667);
            this.newEntityTab.TabIndex = 0;
            this.newEntityTab.Text = "New Entity";
            this.newEntityTab.UseVisualStyleBackColor = true;
            // 
            // CreateEntityButton
            // 
            this.CreateEntityButton.Dock = System.Windows.Forms.DockStyle.Top;
            this.CreateEntityButton.Location = new System.Drawing.Point(3, 478);
            this.CreateEntityButton.Name = "CreateEntityButton";
            this.CreateEntityButton.Size = new System.Drawing.Size(242, 58);
            this.CreateEntityButton.TabIndex = 2;
            this.CreateEntityButton.Text = "Create Entity";
            this.CreateEntityButton.UseVisualStyleBackColor = true;
            this.CreateEntityButton.Click += new System.EventHandler(this.CreateEntityButton_Click);
            // 
            // entityTreeView
            // 
            this.entityTreeView.Dock = System.Windows.Forms.DockStyle.Top;
            this.entityTreeView.Location = new System.Drawing.Point(3, 3);
            this.entityTreeView.Name = "entityTreeView";
            this.entityTreeView.Size = new System.Drawing.Size(242, 475);
            this.entityTreeView.TabIndex = 1;
            // 
            // editEntityTab
            // 
            this.editEntityTab.Controls.Add(this.gotoEntityButton);
            this.editEntityTab.Controls.Add(this.EntityListBox);
            this.editEntityTab.Location = new System.Drawing.Point(4, 22);
            this.editEntityTab.Name = "editEntityTab";
            this.editEntityTab.Padding = new System.Windows.Forms.Padding(3);
            this.editEntityTab.Size = new System.Drawing.Size(248, 667);
            this.editEntityTab.TabIndex = 1;
            this.editEntityTab.Text = "Edit Entity";
            this.editEntityTab.UseVisualStyleBackColor = true;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.viewToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(3, 3);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1386, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // viewToolStripMenuItem
            // 
            this.viewToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.topToolStripMenuItem,
            this.bottomToolStripMenuItem});
            this.viewToolStripMenuItem.Name = "viewToolStripMenuItem";
            this.viewToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.viewToolStripMenuItem.Text = "&View";
            // 
            // topToolStripMenuItem
            // 
            this.topToolStripMenuItem.Name = "topToolStripMenuItem";
            this.topToolStripMenuItem.Size = new System.Drawing.Size(114, 22);
            this.topToolStripMenuItem.Text = "&Top";
            // 
            // bottomToolStripMenuItem
            // 
            this.bottomToolStripMenuItem.Name = "bottomToolStripMenuItem";
            this.bottomToolStripMenuItem.Size = new System.Drawing.Size(114, 22);
            this.bottomToolStripMenuItem.Text = "&Bottom";
            // 
            // texturesTab
            // 
            this.texturesTab.Controls.Add(this.texWndPanel);
            this.texturesTab.Location = new System.Drawing.Point(4, 28);
            this.texturesTab.Name = "texturesTab";
            this.texturesTab.Padding = new System.Windows.Forms.Padding(3);
            this.texturesTab.Size = new System.Drawing.Size(1392, 723);
            this.texturesTab.TabIndex = 1;
            this.texturesTab.Text = "Textures";
            this.texturesTab.UseVisualStyleBackColor = true;
            // 
            // texWndPanel
            // 
            this.texWndPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.texWndPanel.Location = new System.Drawing.Point(3, 3);
            this.texWndPanel.Name = "texWndPanel";
            this.texWndPanel.Size = new System.Drawing.Size(1386, 717);
            this.texWndPanel.TabIndex = 0;
            // 
            // EntityListBox
            // 
            this.EntityListBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.EntityListBox.FormattingEnabled = true;
            this.EntityListBox.Location = new System.Drawing.Point(3, 3);
            this.EntityListBox.Name = "EntityListBox";
            this.EntityListBox.Size = new System.Drawing.Size(242, 199);
            this.EntityListBox.TabIndex = 1;
            // 
            // gotoEntityButton
            // 
            this.gotoEntityButton.Dock = System.Windows.Forms.DockStyle.Top;
            this.gotoEntityButton.Location = new System.Drawing.Point(3, 202);
            this.gotoEntityButton.Name = "gotoEntityButton";
            this.gotoEntityButton.Size = new System.Drawing.Size(242, 58);
            this.gotoEntityButton.TabIndex = 2;
            this.gotoEntityButton.Text = "Goto Entity";
            this.gotoEntityButton.UseVisualStyleBackColor = true;
            this.gotoEntityButton.Click += new System.EventHandler(this.gotoEntityButton_Click);
            // 
            // gameTab
            // 
            this.gameTab.Controls.Add(this.GamePanel);
            this.gameTab.Location = new System.Drawing.Point(4, 28);
            this.gameTab.Name = "gameTab";
            this.gameTab.Size = new System.Drawing.Size(1392, 723);
            this.gameTab.TabIndex = 2;
            this.gameTab.Text = "Game";
            this.gameTab.UseVisualStyleBackColor = true;
            // 
            // GamePanel
            // 
            this.GamePanel.AutoSize = true;
            this.GamePanel.BackColor = System.Drawing.Color.Black;
            this.GamePanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.GamePanel.Location = new System.Drawing.Point(0, 0);
            this.GamePanel.Name = "GamePanel";
            this.GamePanel.Size = new System.Drawing.Size(1392, 723);
            this.GamePanel.TabIndex = 0;
            // 
            // XYWndDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1400, 755);
            this.Controls.Add(this.tabControl);
            this.MainMenuStrip = this.menuStrip1;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "XYWndDialog";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "XY Window";
            this.Load += new System.EventHandler(this.XYWndDialog_Load);
            this.tabControl.ResumeLayout(false);
            this.xyWndTab.ResumeLayout(false);
            this.xyWndTab.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.newEntityTab.ResumeLayout(false);
            this.editEntityTab.ResumeLayout(false);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.texturesTab.ResumeLayout(false);
            this.gameTab.ResumeLayout(false);
            this.gameTab.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage xyWndTab;
        private System.Windows.Forms.TabPage texturesTab;
        private System.Windows.Forms.Panel texWndPanel;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem viewToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem topToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem bottomToolStripMenuItem;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage newEntityTab;
        private System.Windows.Forms.TabPage editEntityTab;
        private System.Windows.Forms.Button CreateEntityButton;
        private System.Windows.Forms.TreeView entityTreeView;
        private System.Windows.Forms.Button gotoEntityButton;
        private System.Windows.Forms.ListBox EntityListBox;
        private System.Windows.Forms.TabPage gameTab;
        private System.Windows.Forms.Panel GamePanel;
    }
}