/*
 * NewGameDialog.cs
 * Authors: Darius McAdam
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Ksu.Cis300.Checkers
{
    /// <summary>
    /// Class to hold the form that lets us adjust AI settings
    /// </summary>
    public partial class NewGameDialog : Form
    {
        /// <summary>
        /// Tells us which player color is the computer or if there is no computer
        /// </summary>
        public string ComputerPlayer
        {
            get
            {   
                if (uxComputerBlack.Checked)
                {
                    return "Black";
                }
                else if (uxComputerWhite.Checked)
                {
                    return "White";
                }
                return "";
            }
        }

        /// <summary>
        /// Level pulled from our numeric control
        /// </summary>
        public int Level
        {
            get { return Convert.ToInt32(uxLevel.Value); }
        }

        /// <summary>
        /// Constructor to create the newgamedialog
        /// </summary>
        public NewGameDialog()
        {
            InitializeComponent();
        }
    }
}
