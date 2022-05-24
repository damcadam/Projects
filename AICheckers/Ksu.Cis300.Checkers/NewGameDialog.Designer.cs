namespace Ksu.Cis300.Checkers
{
    partial class NewGameDialog
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
            this.uxComputerBlack = new System.Windows.Forms.RadioButton();
            this.uxComputerWhite = new System.Windows.Forms.RadioButton();
            this.uxNoComputer = new System.Windows.Forms.RadioButton();
            this.uxLevelLabel = new System.Windows.Forms.Label();
            this.uxLevel = new System.Windows.Forms.NumericUpDown();
            this.uxOK = new System.Windows.Forms.Button();
            this.uxCancel = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.uxLevel)).BeginInit();
            this.SuspendLayout();
            // 
            // uxComputerBlack
            // 
            this.uxComputerBlack.AutoSize = true;
            this.uxComputerBlack.Location = new System.Drawing.Point(12, 12);
            this.uxComputerBlack.Name = "uxComputerBlack";
            this.uxComputerBlack.Size = new System.Drawing.Size(127, 17);
            this.uxComputerBlack.TabIndex = 0;
            this.uxComputerBlack.TabStop = true;
            this.uxComputerBlack.Text = "Computer plays Black";
            this.uxComputerBlack.UseVisualStyleBackColor = true;
            // 
            // uxComputerWhite
            // 
            this.uxComputerWhite.AutoSize = true;
            this.uxComputerWhite.Location = new System.Drawing.Point(12, 44);
            this.uxComputerWhite.Name = "uxComputerWhite";
            this.uxComputerWhite.Size = new System.Drawing.Size(128, 17);
            this.uxComputerWhite.TabIndex = 1;
            this.uxComputerWhite.TabStop = true;
            this.uxComputerWhite.Text = "Computer plays White";
            this.uxComputerWhite.UseVisualStyleBackColor = true;
            // 
            // uxNoComputer
            // 
            this.uxNoComputer.AutoSize = true;
            this.uxNoComputer.Location = new System.Drawing.Point(12, 77);
            this.uxNoComputer.Name = "uxNoComputer";
            this.uxNoComputer.Size = new System.Drawing.Size(117, 17);
            this.uxNoComputer.TabIndex = 2;
            this.uxNoComputer.TabStop = true;
            this.uxNoComputer.Text = "No computer player";
            this.uxNoComputer.UseVisualStyleBackColor = true;
            // 
            // uxLevelLabel
            // 
            this.uxLevelLabel.AutoSize = true;
            this.uxLevelLabel.Location = new System.Drawing.Point(9, 111);
            this.uxLevelLabel.Name = "uxLevelLabel";
            this.uxLevelLabel.Size = new System.Drawing.Size(36, 13);
            this.uxLevelLabel.TabIndex = 3;
            this.uxLevelLabel.Text = "Level:";
            // 
            // uxLevel
            // 
            this.uxLevel.Location = new System.Drawing.Point(51, 111);
            this.uxLevel.Maximum = new decimal(new int[] {
            12,
            0,
            0,
            0});
            this.uxLevel.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.uxLevel.Name = "uxLevel";
            this.uxLevel.Size = new System.Drawing.Size(88, 20);
            this.uxLevel.TabIndex = 4;
            this.uxLevel.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.uxLevel.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // uxOK
            // 
            this.uxOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.uxOK.Location = new System.Drawing.Point(12, 146);
            this.uxOK.Name = "uxOK";
            this.uxOK.Size = new System.Drawing.Size(51, 23);
            this.uxOK.TabIndex = 5;
            this.uxOK.Text = "OK";
            this.uxOK.UseVisualStyleBackColor = true;
            // 
            // uxCancel
            // 
            this.uxCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.uxCancel.Location = new System.Drawing.Point(88, 146);
            this.uxCancel.Name = "uxCancel";
            this.uxCancel.Size = new System.Drawing.Size(51, 23);
            this.uxCancel.TabIndex = 6;
            this.uxCancel.Text = "Cancel";
            this.uxCancel.UseVisualStyleBackColor = true;
            // 
            // NewGameDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(151, 180);
            this.Controls.Add(this.uxCancel);
            this.Controls.Add(this.uxOK);
            this.Controls.Add(this.uxLevel);
            this.Controls.Add(this.uxLevelLabel);
            this.Controls.Add(this.uxNoComputer);
            this.Controls.Add(this.uxComputerWhite);
            this.Controls.Add(this.uxComputerBlack);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "NewGameDialog";
            this.Text = "New Game";
            ((System.ComponentModel.ISupportInitialize)(this.uxLevel)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.RadioButton uxComputerBlack;
        private System.Windows.Forms.RadioButton uxComputerWhite;
        private System.Windows.Forms.RadioButton uxNoComputer;
        private System.Windows.Forms.Label uxLevelLabel;
        private System.Windows.Forms.NumericUpDown uxLevel;
        private System.Windows.Forms.Button uxOK;
        private System.Windows.Forms.Button uxCancel;
    }
}