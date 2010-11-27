﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;


namespace Maptool
{
    public partial class frmMain : Form
    {
        private Tool currTool = null;
        private Entity currEntity = null;
        private EntityType currEntityType = null;

        private int downX, downY;
        private List<Entity> entities;

        private WallTool tool_wall = new WallTool();
        private ZoneTool tool_zone = new ZoneTool();


        public frmMain()
        {
            InitializeComponent();
            entities = new List<Entity>();

        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            list.Select();
            resetUI();
            this.WindowState = FormWindowState.Maximized;
            this.KeyPreview = true;
        }


        /**
         * Resets the user interface to the initial defaults
         **/
        private void resetUI()
        {
            panMap.Size = new Size(1, 1);
            panMap.Size = new Size(2000, 2000);
            toggleOn(toolAreas);
            setCurrEntity(null);
            setCurrEntityType(null);
            setCurrTool(null);
            panMap.Refresh();
            grid.Refresh();
        }

        /**
         * Toggles one of the tool buttons to be on, turning off the others
         **/
        private void toggleOn(ToolStripButton on)
        {
            toolAreas.Checked = false;
            toolWalls.Checked = false;
            toolObjects.Checked = false;
            toolParticleGenerators.Checked = false;
            toolZones.Checked = false;
            on.Checked = true;
        }


        /**
         * Engages the 'Areas' tool
         **/
        private void toolAreas_Click(object sender, EventArgs e)
        {
            toggleOn(toolAreas);
            setCurrTool(null);
        }

        /**
         * Engages the 'Walls' tool
         **/
        private void toolWalls_Click(object sender, EventArgs e)
        {
            toggleOn(toolWalls);
            setCurrTool(this.tool_wall);
        }

        /**
         * Engages the 'Objects' tool
         **/
        private void toolObjects_Click(object sender, EventArgs e)
        {
            toggleOn(toolObjects);
            setCurrTool(null);
        }

        /**
         * Engages the 'Particle Generators' tool
         **/
        private void toolParticleGenerators_Click(object sender, EventArgs e)
        {
            toggleOn(toolParticleGenerators);
            setCurrTool(null);
        }

        /**
         * Engages the 'Zones' tool
         **/
        private void toolZones_Click(object sender, EventArgs e)
        {
            toggleOn(toolZones);
            setCurrTool(this.tool_zone);
        }


        /**
         * Blanks the map
         **/
        private void mnuFileNew_Click(object sender, EventArgs e)
        {
            resetUI();
        }

        private void mnuFileOpen_Click(object sender, EventArgs e)
        {
            if (diaOpen.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                Map m = new Map();
                m.Load(diaOpen.FileName);
                this.entities = m.Entities;
                resetUI();
            }
        }

        private void mnuFileSaveas_Click(object sender, EventArgs e)
        {
            if (diaSave.ShowDialog() == System.Windows.Forms.DialogResult.OK) {
                Map m = new Map();
                m.Entities = this.entities;
                m.Save(diaSave.FileName);
            }
        }

        private void mnuEditMapsettings_Click(object sender, EventArgs e)
        {
            frmMapSettings f = new frmMapSettings();
            f.ShowDialog();
        }


        /**
         * Select a tool
         **/
        private void setCurrTool(Tool tool)
        {
            currTool = tool;

            setCurrEntity(null);
            setCurrEntityType(null);
            list.Items.Clear();

            if (tool == null) return;

            foreach (EntityType type in tool.getTypes()) {
                EntityTypeListItem itm = new EntityTypeListItem();
                itm.EntityType = type;
                list.Items.Add(itm);
            }

            if (list.Items.Count > 0) {
                list.Items[0].Selected = true;
                setCurrEntityType(((EntityTypeListItem)list.Items[0]).EntityType);
            }
        }

        /**
         * Select an entity
         **/
        private void setCurrEntity(Entity ent)
        {
            currEntity = ent;
            grid.SelectedObject = ent;

            if (ent != null) {
                foreach (EntityTypeListItem item in list.Items) {
                    if (item.EntityType == ent.Type) {
                        item.Selected = true;
                        list.Refresh();
                        break;
                    }
                }
            }
        }

        /**
         * Select an entity type
         **/
        private void setCurrEntityType(EntityType type)
        {
            currEntityType = type;
        }



        /**
         * List selection change
         **/
        private void list_SelectedIndexChanged(object sender, EventArgs e)
        {
            setCurrEntityType(null);
            if (list.SelectedItems.Count > 0) {
                setCurrEntityType(((EntityTypeListItem)list.SelectedItems[0]).EntityType);
            }

            if (currEntity != null) {
                currEntity.Type = currEntityType;
                panMap.Refresh();
                grid.Refresh();
            }
        }

        /**
        * Mouse down logic
        **/
        private void panMap_MouseDown(object sender, MouseEventArgs e)
        {
            downX = downY = -1;

            if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                try_select(e.X, e.Y);
                if (currEntity != null) {
                    downX = e.X;
                    downY = e.Y;
                }
            }

            panMap.Refresh();
            grid.Refresh();
        }

        private void panMap_MouseMove(object sender, MouseEventArgs e)
        {
            if (downX == -1) return;
            if (currEntity == null) return;

            currEntity.X = e.X;
            currEntity.Y = e.Y;

            panMap.Refresh();
            grid.Refresh();
        }

        /**
        * Mouse up logic
        **/
        private void panMap_MouseUp(object sender, MouseEventArgs e)
        {
            downX = downY = -1;

            if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                try_add(e.X, e.Y);
            }

            panMap.Refresh();
            grid.Refresh();
        }


        /**
        * Keyboard up logic
        **/
        private void frmMain_KeyUp(object sender, KeyEventArgs e)
        {
            e.Handled = true;
            switch (e.KeyCode) {
                case Keys.Left:
                    if (currEntity != null) currEntity.X -= (e.Control ? 15 : 1);
                    break;

                case Keys.Right:
                    if (currEntity != null) currEntity.X += (e.Control ? 15 : 1);
                    break;

                case Keys.Up:
                    if (currEntity != null) currEntity.Y -= (e.Control ? 15 : 1);
                    break;

                case Keys.Down:
                    if (currEntity != null) currEntity.Y += (e.Control ? 15 : 1);
                    break;

                case Keys.Escape:
                    setCurrEntity(null);
                    break;

                case Keys.Delete:
                    if (currEntity != null) {
                        entities.Remove(currEntity);
                        setCurrEntity(null);
                    }
                    break;

                default:
                    e.Handled = false;
                    break;
            }

            panMap.Refresh();
            grid.Refresh();
        }


        /**
         * Draws everything
         **/
        private void panMap_Paint(object sender, PaintEventArgs e)
        {
            Graphics g = e.Graphics;

            Brush br_darkgray = new SolidBrush(Color.FromArgb(20, 20, 20));


            // Zones
            foreach (Entity ent in entities) {
                if (!(ent is ZoneEntity)) continue;

                ZoneEntity entc = (ZoneEntity) ent;

                g.DrawRectangle((ent == currEntity ? Pens.Red : Pens.White), entc.X - entc.Width / 2, entc.Y - entc.Height / 2, entc.Width, entc.Height);
            }

            // Walls
            foreach (Entity ent in entities) {
                if (! (ent is WallEntity)) continue;

                g.FillEllipse(br_darkgray, ent.X - 30, ent.Y - 30, 60, 60);
                g.DrawEllipse((ent == currEntity ? Pens.Red : Pens.White), ent.X - 5, ent.Y - 5, 10, 10);
            }
        }

        /**
        * If there is an entity at the current location, select it for editing
        **/
        public void try_select(int mousex, int mousey)
        {
            if (currTool == null) return;

            setCurrEntity(null);

            foreach (Entity e in entities) {
                if (!currTool.isThisOurs(e)) continue;

                double dist = Math.Sqrt(((e.X - mousex) * (e.X - mousex)) + ((e.Y - mousey) * (e.Y - mousey)));
                if (dist < 10) {
                    setCurrEntity(e);
                    return;
                }
            }
        }

        /**
        * If there is nothing selected, do an add
        **/
        private void try_add(int mousex, int mousey)
        {
            if (currTool == null) return;
            if (currEntity != null) return;
            if (currEntityType == null) return;

            Entity ent = currTool.createEntity();
            ent.X = mousex;
            ent.Y = mousey;
            ent.Type = currEntityType;
            entities.Add(ent);
            setCurrEntity(ent);
        }
    }

    class EntityTypeListItem : ListViewItem
    {
        private EntityType type;

        public EntityType EntityType
        {
            get { return type; }
            set {
                this.type = value;
                this.Text = value.Name;
            }
        }
    }
}
