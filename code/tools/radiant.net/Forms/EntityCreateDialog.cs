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
    public partial class EntityCreateDialog : Form
    {
        Point entityCreationLocation;
        public EntityCreateDialog()
        {
            InitializeComponent();
            UpdateTreeView();
            this.FormClosing += EntityCreateDialog_FormClosing;
        }

        public void SetEntityCreationLocation(Point location)
        {
            entityCreationLocation = location;
        }

        private void EntityCreateDialog_FormClosing(object sender, FormClosingEventArgs e)
        {

            this.Hide();
            e.Cancel = true; // this cancels the close event.
        }

        private void UpdateTreeView()
        {
            entityTreeView.Nodes.Clear();
            string entityString = NativeAPI.RadiantAPI_GetEntityClassManaged();
            string[] entityList = entityString.Split(';');
            foreach(string entityName in entityList)
            {
                entityTreeView.Nodes.Add(entityName);
            }
        }

        private void EntityCreateDialog_Load(object sender, EventArgs e)
        {
           
        }

        private void CreateEntityButton_Click(object sender, EventArgs e)
        {
            NativeAPI.RadiantAPI_CreateEntity(entityCreationLocation.X, entityCreationLocation.Y, entityTreeView.SelectedNode.Text);
            this.Hide();
        }
    }
}
