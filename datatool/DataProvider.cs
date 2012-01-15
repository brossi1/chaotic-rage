﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using Ionic.Utils.Zip;
using System.IO;

namespace datatool
{
    public class DataProvider
    {
        ConfuseReader read;
        private string datapath;
        private ZipFile zf;

        private List<base_item> areatypes;
        private List<base_item> modifiers;
        private List<base_item> particletypes;
        private List<base_item> particlegenerators;
        private List<base_item> unitclasses;
        private List<base_item> weapontypes;


        /**
        * Loads data files
        **/
        public bool load(string filename, bool isZip)
        {
            this.datapath = filename;
            this.read = new ConfuseReader();

            this.areatypes = new List<base_item>();
            this.modifiers = new List<base_item>();
            this.particletypes = new List<base_item>();
            this.particlegenerators = new List<base_item>();
            this.unitclasses = new List<base_item>();
            this.weapontypes = new List<base_item>();

            if (isZip) {
                this.zf = new ZipFile(filename);
            } else {
                this.zf = null;
            }

            load_particletypes();
            load_weapontypes();

            return true;
        }


        /**
         * The currently loaded datapath.
         **/
        public string Filename
        {
            get { return this.datapath; }
        }

        /**
         * Is it a ZIP currently loaded?
         **/
        public bool IsZip
        {
            get { return (this.zf != null); }
        }

        public string GetFriendlyFilename()
        {
            if (this.zf != null) {
                return "[Mod; " + Path.GetFileName(this.datapath) + "]";
            } else {
                return "[Directory; " + this.datapath + "]";
            }
        }


        /**
         * Read a file from the disk or zip
         **/
        private string readFile(string filename)
        {
            if (this.zf != null) {
                MemoryStream ms = new MemoryStream();
                zf.Extract(filename, ms);
                ms.Seek(0, 0);
                StreamReader reader = new StreamReader(ms);
                return reader.ReadToEnd();

            } else {
                return File.ReadAllText(this.datapath + "\\" + filename);
            }
        }

        /**
         * Write a file to the disk or zip
         **/
        private void writeFile(string filename, string content)
        {
            if (this.zf != null) {
                try {
                    zf.RemoveEntry(filename);
                } catch (Exception ex) {}
                zf.AddStringAsFile(content, filename, "");

            } else {
                File.WriteAllText(this.datapath + "\\" + filename, content);
            }
        }
        


        /**
        * Saves data files
        **/
        public bool save()
        {
            string o;

            // Particles 
            o = "";
            foreach (base_item i in this.particletypes)
            {
                o += "particle {\n\t" + i.getConfItem() + "\n}\n";
            }
            this.writeFile("particletypes.conf", o);
            
            // Weapons
            o = "";
            foreach (base_item i in this.weapontypes) {
                o += "weapon {\n\t" + i.getConfItem() + "\n}\n";
            }
            this.writeFile("weapontypes.conf", o);

            if (this.zf != null) {
                this.zf.Save();
                this.zf = new ZipFile(this.datapath);
            }

            return true;
        }


        /**
         * Loads the particletypes
         **/
        private void load_particletypes()
        {
            ConfuseSection sect = null;
            string file = this.readFile("particletypes.conf");

            // Parse
            try {
                sect = read.Parse(file);
            } catch (Exception ex) {
                MessageBox.Show("Error loading particletypes:\n" + ex.Message);
                Application.Exit();
            }

            // Load
            foreach (ConfuseSection s in sect.subsections) {
                particletype_item i = new particletype_item("");
                particletypes.Add(i);

                if (!s.values.ContainsKey("name")) throw new Exception("Particletype defined without a name");

                i.Name = s.get_string("name", "");
                i.Model = s.get_string("model", "");
                i.MaxSpeed = s.get_range("max_speed", new range(0)).toString();
                i.BeginSpeed = s.get_range("begin_speed", new range(0)).toString();
                i.Accel = s.get_range("accel", new range(0)).toString();
                i.Age = s.get_range("age", new range(0)).toString();
                i.UnitDamage = s.get_range("unit_damage", new range(0)).toString();
                i.WallDamage = s.get_range("wall_damage", new range(0)).toString();
                i.UnitHits = s.get_range("unit_hits", new range(0)).toString();
                i.WallHits = s.get_range("wall_hits", new range(0)).toString();
            }
        }


        /**
        * Loads the particletypes
        **/
        private void load_weapontypes()
        {
            ConfuseSection sect = null;
            string file = this.readFile("weapontypes.conf");

            // Parse
            try {
                sect = read.Parse(file);
            } catch (Exception ex) {
                MessageBox.Show("Error loading weapontypes:\n" + ex.Message);
                return;
            }

            // Load
            foreach (ConfuseSection s in sect.subsections) {
                weapontype_item i = new weapontype_item("");
                weapontypes.Add(i);

                if (!s.values.ContainsKey("name")) throw new Exception("Weapontype defined without a name");

                i.Name = s.get_string("name", "");
                i.Title = s.get_string("title", "");

                i.AngleRange = s.get_int("angle_range", 0);
                i.FireDelay = s.get_int("fire_delay", 250);
                i.ReloadDelay = s.get_int("reload_delay", 1000);
                i.Continuous = s.get_bool("continuous", false);
                i.MagazineLimit = s.get_int("magazine_limit", 100);
                i.BeltLimit = s.get_int("belt_limit", 1000);
                i.Range = s.get_float("range", 50);
                i.UnitDamage = s.get_float("unit_damage", 10);
                i.WallDamage = s.get_float("wall_damage", 10);

                i.Particle = this.FindParticletype(s.get_string("particle", ""));
                i.ParticleGen = this.FindParticlegenerator(s.get_string("particlegen", ""));

                i.Sounds.Clear();
                foreach (ConfuseSection ss in s.subsections) {
                    if (ss.name == "sound") {
                        weapontype_sound sss = new weapontype_sound();
                        sss.Sound = ss.get_string("sound", "");
                        sss.Type = ss.get_int("type", 0);
                        i.Sounds.Add(sss);
                    }
                }
            }
        }


        public List<base_item> AreaTypes
        {
            get { return areatypes; }
        }

        public List<base_item> Modifiers
        {
            get { return modifiers; }
        }

        public List<base_item> ParticleTypes
        {
            get { return particletypes; }
        }

        public List<base_item> ParticleGenerators
        {
            get { return particlegenerators; }
        }

        public List<base_item> UnitClasses
        {
            get { return unitclasses; }
        }

        public List<base_item> WeaponTypes
        {
            get { return weapontypes; }
        }



        public particletype_item FindParticletype(string name)
        {
            if (name == null || name == "") return null;
            foreach (particletype_item item in particletypes) {
                if (item.Name == name) return item;
            }
            return null;
        }

        public particlegenerator_item FindParticlegenerator(string name)
        {
            if (name == null || name == "") return null;
            foreach (particlegenerator_item item in particlegenerators) {
                if (item.Name == name) return item;
            }
            return null;
        }

    }
}
