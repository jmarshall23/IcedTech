using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using radiant.net;

namespace radiant.net.forms
{
    public partial class EntitySelection : Form
    {
        public EntitySelection()
        {
            InitializeComponent();
            this.FormClosing += EntitySelection_FormClosing;
        }

        private void EntitySelection_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.Hide();
            e.Cancel = true; // this cancels the close event.
        }

        public void UpdateEntityList(string[] entities)
        {
            EntityListBox.Items.Clear();
            
            foreach(string name in entities)
            {
                EntityListBox.Items.Add(name);
            }

            EntityListBox.Sorted = true;
        }

        private void EntityListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            string selected_entity = (string)EntityListBox.SelectedItem;
            NativeAPI.RadiantAPI_SelectObject(selected_entity);
        }

        private void EntitySelection_Load(object sender, EventArgs e)
        {

        }

        private void GotoEntityButton_Click(object sender, EventArgs e)
        {
            string selected_entity = (string)EntityListBox.SelectedItem;
            NativeAPI.RadiantAPI_GotoEntity(selected_entity);
        }
    }
}
