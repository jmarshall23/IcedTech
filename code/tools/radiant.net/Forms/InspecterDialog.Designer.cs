namespace radiant.net.forms
{
    partial class InspecterDialog
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
            this.tabControl1 = new MetroFramework.Controls.MetroTabControl();
            this.consolePage = new System.Windows.Forms.TabPage();
            this.materialPage = new System.Windows.Forms.TabPage();
            this.entityTab = new System.Windows.Forms.TabPage();
            this.EntitySplitContainer = new System.Windows.Forms.SplitContainer();
            this.entityLayoutTable = new System.Windows.Forms.TableLayoutPanel();
            this.classNameText = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.entityClassnameText = new System.Windows.Forms.Label();
            this.entityPropertyGrid = new System.Windows.Forms.PropertyGrid();
            this.consoleTxt = new System.Windows.Forms.TextBox();
            this.materialListDir = new System.Windows.Forms.ListView();
            this.tabControl1.SuspendLayout();
            this.consolePage.SuspendLayout();
            this.materialPage.SuspendLayout();
            this.entityTab.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.EntitySplitContainer)).BeginInit();
            this.EntitySplitContainer.Panel1.SuspendLayout();
            this.EntitySplitContainer.Panel2.SuspendLayout();
            this.EntitySplitContainer.SuspendLayout();
            this.entityLayoutTable.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Alignment = System.Windows.Forms.TabAlignment.Bottom;
            this.tabControl1.Controls.Add(this.consolePage);
            this.tabControl1.Controls.Add(this.materialPage);
            this.tabControl1.Controls.Add(this.entityTab);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.Padding = new System.Drawing.Point(24, 12);
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(800, 450);
            this.tabControl1.SizeMode = System.Windows.Forms.TabSizeMode.FillToRight;
            this.tabControl1.TabIndex = 0;
            // 
            // consolePage
            // 
            this.consolePage.Controls.Add(this.consoleTxt);
            this.consolePage.Location = new System.Drawing.Point(4, 4);
            this.consolePage.Name = "consolePage";
            this.consolePage.Padding = new System.Windows.Forms.Padding(3);
            this.consolePage.Size = new System.Drawing.Size(792, 406);
            this.consolePage.TabIndex = 0;
            this.consolePage.Text = "Console";
            this.consolePage.UseVisualStyleBackColor = true;
            // 
            // materialPage
            // 
            this.materialPage.Controls.Add(this.materialListDir);
            this.materialPage.Location = new System.Drawing.Point(4, 4);
            this.materialPage.Name = "materialPage";
            this.materialPage.Padding = new System.Windows.Forms.Padding(3);
            this.materialPage.Size = new System.Drawing.Size(792, 406);
            this.materialPage.TabIndex = 1;
            this.materialPage.Text = "Materials";
            this.materialPage.UseVisualStyleBackColor = true;
            // 
            // entityTab
            // 
            this.entityTab.Controls.Add(this.EntitySplitContainer);
            this.entityTab.Location = new System.Drawing.Point(4, 4);
            this.entityTab.Name = "entityTab";
            this.entityTab.Size = new System.Drawing.Size(792, 406);
            this.entityTab.TabIndex = 2;
            this.entityTab.Text = "Entity";
            this.entityTab.UseVisualStyleBackColor = true;
            // 
            // EntitySplitContainer
            // 
            this.EntitySplitContainer.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.EntitySplitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.EntitySplitContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.EntitySplitContainer.Location = new System.Drawing.Point(0, 0);
            this.EntitySplitContainer.Name = "EntitySplitContainer";
            this.EntitySplitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // EntitySplitContainer.Panel1
            // 
            this.EntitySplitContainer.Panel1.Controls.Add(this.entityLayoutTable);
            // 
            // EntitySplitContainer.Panel2
            // 
            this.EntitySplitContainer.Panel2.Controls.Add(this.entityPropertyGrid);
            this.EntitySplitContainer.Size = new System.Drawing.Size(792, 406);
            this.EntitySplitContainer.SplitterDistance = 64;
            this.EntitySplitContainer.TabIndex = 0;
            // 
            // entityLayoutTable
            // 
            this.entityLayoutTable.ColumnCount = 2;
            this.entityLayoutTable.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 100F));
            this.entityLayoutTable.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.entityLayoutTable.Controls.Add(this.classNameText, 1, 1);
            this.entityLayoutTable.Controls.Add(this.label1, 0, 0);
            this.entityLayoutTable.Controls.Add(this.label2, 0, 1);
            this.entityLayoutTable.Controls.Add(this.entityClassnameText, 1, 0);
            this.entityLayoutTable.Dock = System.Windows.Forms.DockStyle.Fill;
            this.entityLayoutTable.Location = new System.Drawing.Point(0, 0);
            this.entityLayoutTable.Name = "entityLayoutTable";
            this.entityLayoutTable.RowCount = 2;
            this.entityLayoutTable.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.entityLayoutTable.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.entityLayoutTable.Size = new System.Drawing.Size(790, 62);
            this.entityLayoutTable.TabIndex = 0;
            // 
            // classNameText
            // 
            this.classNameText.AutoSize = true;
            this.classNameText.Dock = System.Windows.Forms.DockStyle.Fill;
            this.classNameText.Location = new System.Drawing.Point(103, 31);
            this.classNameText.Name = "classNameText";
            this.classNameText.Size = new System.Drawing.Size(684, 31);
            this.classNameText.TabIndex = 3;
            this.classNameText.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label1.Location = new System.Drawing.Point(3, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(94, 31);
            this.label1.TabIndex = 0;
            this.label1.Text = "Name";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label2.Location = new System.Drawing.Point(3, 31);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(94, 31);
            this.label2.TabIndex = 1;
            this.label2.Text = "Class";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // entityClassnameText
            // 
            this.entityClassnameText.AutoSize = true;
            this.entityClassnameText.Dock = System.Windows.Forms.DockStyle.Fill;
            this.entityClassnameText.Location = new System.Drawing.Point(103, 0);
            this.entityClassnameText.Name = "entityClassnameText";
            this.entityClassnameText.Size = new System.Drawing.Size(684, 31);
            this.entityClassnameText.TabIndex = 2;
            this.entityClassnameText.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // entityPropertyGrid
            // 
            this.entityPropertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.entityPropertyGrid.Location = new System.Drawing.Point(0, 0);
            this.entityPropertyGrid.Name = "entityPropertyGrid";
            this.entityPropertyGrid.Size = new System.Drawing.Size(790, 336);
            this.entityPropertyGrid.TabIndex = 0;
            this.entityPropertyGrid.Click += new System.EventHandler(this.entityPropertyGrid_Click);
            // 
            // consoleTxt
            // 
            this.consoleTxt.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.consoleTxt.Dock = System.Windows.Forms.DockStyle.Fill;
            this.consoleTxt.ForeColor = System.Drawing.SystemColors.Info;
            this.consoleTxt.Location = new System.Drawing.Point(3, 3);
            this.consoleTxt.Multiline = true;
            this.consoleTxt.Name = "consoleTxt";
            this.consoleTxt.ReadOnly = true;
            this.consoleTxt.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.consoleTxt.Size = new System.Drawing.Size(786, 400);
            this.consoleTxt.TabIndex = 0;
            // 
            // materialListDir
            // 
            this.materialListDir.BackColor = System.Drawing.SystemColors.ScrollBar;
            this.materialListDir.Dock = System.Windows.Forms.DockStyle.Fill;
            this.materialListDir.HideSelection = false;
            this.materialListDir.Location = new System.Drawing.Point(3, 3);
            this.materialListDir.Name = "materialListDir";
            this.materialListDir.Size = new System.Drawing.Size(786, 400);
            this.materialListDir.TabIndex = 0;
            this.materialListDir.UseCompatibleStateImageBehavior = false;
            // 
            // InspecterDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "InspecterDialog";
            this.Text = "InspecterDialog";
            this.tabControl1.ResumeLayout(false);
            this.consolePage.ResumeLayout(false);
            this.consolePage.PerformLayout();
            this.materialPage.ResumeLayout(false);
            this.entityTab.ResumeLayout(false);
            this.EntitySplitContainer.Panel1.ResumeLayout(false);
            this.EntitySplitContainer.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.EntitySplitContainer)).EndInit();
            this.EntitySplitContainer.ResumeLayout(false);
            this.entityLayoutTable.ResumeLayout(false);
            this.entityLayoutTable.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private MetroFramework.Controls.MetroTabControl tabControl1;
        private System.Windows.Forms.TabPage consolePage;
        private System.Windows.Forms.TabPage materialPage;
        private System.Windows.Forms.TabPage entityTab;
        private System.Windows.Forms.SplitContainer EntitySplitContainer;
        private System.Windows.Forms.TableLayoutPanel entityLayoutTable;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label classNameText;
        private System.Windows.Forms.Label entityClassnameText;
        private System.Windows.Forms.PropertyGrid entityPropertyGrid;
        private System.Windows.Forms.TextBox consoleTxt;
        private System.Windows.Forms.ListView materialListDir;
    }
}