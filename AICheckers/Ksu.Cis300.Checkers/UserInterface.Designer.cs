namespace Ksu.Cis300.Checkers
{
    partial class UserInterface
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
            this.uxMenuBar = new System.Windows.Forms.MenuStrip();
            this.uxGameMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.uxNew = new System.Windows.Forms.ToolStripMenuItem();
            this.uxUndo = new System.Windows.Forms.ToolStripMenuItem();
            this.uxStatus = new System.Windows.Forms.ToolStripTextBox();
            this.uxBoard = new System.Windows.Forms.FlowLayoutPanel();
            this.uxMenuBar.SuspendLayout();
            this.SuspendLayout();
            // 
            // uxMenuBar
            // 
            this.uxMenuBar.GripMargin = new System.Windows.Forms.Padding(2, 2, 0, 2);
            this.uxMenuBar.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.uxMenuBar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.uxGameMenu,
            this.uxStatus});
            this.uxMenuBar.Location = new System.Drawing.Point(0, 0);
            this.uxMenuBar.Name = "uxMenuBar";
            this.uxMenuBar.Size = new System.Drawing.Size(800, 37);
            this.uxMenuBar.TabIndex = 3;
            this.uxMenuBar.Text = "menuStrip1";
            // 
            // uxGameMenu
            // 
            this.uxGameMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.uxNew,
            this.uxUndo});
            this.uxGameMenu.Name = "uxGameMenu";
            this.uxGameMenu.Size = new System.Drawing.Size(74, 31);
            this.uxGameMenu.Text = "Game";
            // 
            // uxNew
            // 
            this.uxNew.Name = "uxNew";
            this.uxNew.Size = new System.Drawing.Size(270, 34);
            this.uxNew.Text = "New Game";
            this.uxNew.Click += new System.EventHandler(this.uxNew_Click);
            // 
            // uxUndo
            // 
            this.uxUndo.Name = "uxUndo";
            this.uxUndo.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
            this.uxUndo.Size = new System.Drawing.Size(270, 34);
            this.uxUndo.Text = "Undo";
            this.uxUndo.Click += new System.EventHandler(this.uxUndo_Click);
            // 
            // uxStatus
            // 
            this.uxStatus.Font = new System.Drawing.Font("Segoe UI", 9F);
            this.uxStatus.Name = "uxStatus";
            this.uxStatus.ReadOnly = true;
            this.uxStatus.Size = new System.Drawing.Size(150, 31);
            this.uxStatus.Text = "Black\'s turn.";
            this.uxStatus.TextBoxTextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // uxBoard
            // 
            this.uxBoard.Location = new System.Drawing.Point(12, 46);
            this.uxBoard.Name = "uxBoard";
            this.uxBoard.Size = new System.Drawing.Size(493, 396);
            this.uxBoard.TabIndex = 4;
            // 
            // UserInterface
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.uxMenuBar);
            this.Controls.Add(this.uxBoard);
            this.MaximizeBox = false;
            this.Name = "UserInterface";
            this.Text = "Checkers";
            this.uxMenuBar.ResumeLayout(false);
            this.uxMenuBar.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip uxMenuBar;
        private System.Windows.Forms.ToolStripMenuItem uxGameMenu;
        private System.Windows.Forms.ToolStripMenuItem uxNew;
        private System.Windows.Forms.ToolStripMenuItem uxUndo;
        private System.Windows.Forms.ToolStripTextBox uxStatus;
        private System.Windows.Forms.FlowLayoutPanel uxBoard;
    }
}

