using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using radiant.net.Shared;
using MetroFramework.Components;
using MetroFramework.Forms;

namespace radiant.net.forms
{
    public partial class InspecterDialog : MetroForm
    {
        public InspecterDialog()
        {
            InitializeComponent();
            tabControl1.KeyDown += TabControl1_KeyDown;
            this.Load += InspecterDialog_Load;
        }

        private void InspecterDialog_Load(object sender, EventArgs e)
        {
            string[] materialDirectories = NativeAPI.RadiantAPI_GetMaterialDirectoriesManaged().Split(';');
            ImageList Imagelist = new ImageList();
            Imagelist.Images.Add("folder", Image.FromFile("editor/folder-icon.png"));
            Imagelist.ImageSize = new Size(64, 64);

            materialListDir.View = View.LargeIcon;
            materialListDir.LargeImageList = Imagelist;
            materialListDir.SmallImageList = Imagelist;
            foreach (string dir in materialDirectories)
            {
                var item = new ListViewItem();
                item.ImageKey = "folder";
                item.Text = dir;

                materialListDir.Items.Add(item);
            }

            materialListDir.DoubleClick += MaterialListDir_DoubleClick;
        }

        private void MaterialListDir_DoubleClick(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_LoadTextureDirectory(materialListDir.SelectedItems[0].SubItems[0].Text);
        }

        public void RadiantPrint(string txt)
        {
            entityPropertyGrid.PropertyValueChanged += EntityPropertyGrid_PropertyValueChanged;

            consoleTxt.Text += txt;
            consoleTxt.SelectionStart = consoleTxt.TextLength;
            consoleTxt.ScrollToCaret();
            
        }

        private void EntityPropertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            string key = e.ChangedItem.Label;
            string value = (string)e.ChangedItem.Value;

            NativeAPI.RadiantAPI_UpdateSelectedEntityAttribute(key, value);
        }

        public void UpdateEntitySel(string classname, string name)
        {
            entityClassnameText.Text = classname;
            classNameText.Text = name;

            List<CustomProperty> properties = new List<CustomProperty>();

            string str = NativeAPI.RadiantAPI_GetSelectedEntityAttributesManaged();

            string[] tokens = str.Split(';');
            for(int i = 0; i < tokens.Length-1; i+=2)
            {
                string key = tokens[i + 0];
                string value = tokens[i + 1];

                properties.Add(new CustomProperty { Name = key, Desc = "", DefaultValue = value });
            }

            var obj = new CustomObjectType { props = properties };

            entityPropertyGrid.SelectedObject = obj;
        }

        private void TabControl1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Left || e.KeyCode == Keys.Right)
            {
                e.SuppressKeyPress = true;
            }
        }

        private void ConsoleTxt_KeyDown(object sender, KeyEventArgs e)
        {

        }

        private void entityPropertyGrid_Click(object sender, EventArgs e)
        {

        }
    }
}
