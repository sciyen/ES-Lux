using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Windows.Forms;
using NAudio.Wave;
using WaveFormRendererLib;

namespace LightBallHelper {
    public partial class LightBallHelper : Form {
        private IWavePlayer wavePlayer;
        private AudioFileReader audioFileReader;
        private string fileName = "";

        private readonly WaveFormRenderer waveFormRenderer;
        private readonly WaveFormRendererSettings standardSettings;

        string prefixNowTime = "Current: ";
        string prefixTotalTime ="Total: ";

        const int SEC_PIXEL_RATIOL = 60;
        const int START_POS = 50;
        const int WAVE_LOC_Y = 540;
        private int waveImageLen = 0;

        private List<Effects> effectListObj;
        private Color keyColor;
        public static string outputTaskName = "Task";

        /* Effect Attr */
        const int maxEffectAttr = 6;
        Label[] effectAttrLabel = new Label[maxEffectAttr];
        TextBox[] effectAttrValue = new TextBox[maxEffectAttr];
        Label[] effectAttrUnit = new Label[maxEffectAttr];
        public LightBallHelper() {
            InitializeComponent();
            labelNowTime.Text = prefixNowTime;
            labelTotalTime.Text = prefixTotalTime;
            /* Waveform setup */
            waveFormRenderer = new WaveFormRenderer();
            standardSettings = new StandardWaveFormRendererSettings() { Name = "Standard" };

            BtnTopColour.BackColor = standardSettings.TopPeakPen.Color;
            BtnBottomColour.BackColor = standardSettings.BottomPeakPen.Color;

            updownTopHeight.Value = 80;
            updownBottomHeight.Value = 80;
            labelRendering.Visible = false;
            timer1.Enabled = false;
            Bitmap bitmap = new Bitmap(Resource1.indicator);
            pictureBoxIndicator.BackColor = Color.Transparent;
            pictureBoxIndicator.Location = new Point(START_POS - 5, WAVE_LOC_Y - 5);
            pictureBoxIndicator.Image = bitmap;

            UpdownOffset.Value = 50;

            comboBoxPeakCalculationStrategy.Items.Add("Max Rms Value");
            comboBoxPeakCalculationStrategy.Items.Add("Max Absolute Value");
            comboBoxPeakCalculationStrategy.Items.Add("Sampled Peaks");
            comboBoxPeakCalculationStrategy.Items.Add("Scaled Average");
            comboBoxPeakCalculationStrategy.SelectedIndex = 0;
            comboBoxPeakCalculationStrategy.SelectedIndexChanged += (sender, args) => {
                if ( !String.IsNullOrEmpty(fileName) )
                    RenderWaveform();
            };

            /* Effects Setup */
            effectListObj = new List<Effects>();

            effectsOptions.Items.Add(Effects.EffectsNames.EffectHSL.ToString());
            effectsOptions.Items.Add(Effects.EffectsNames.EffectColorTransition.ToString());
            effectsOptions.Items.Add(Effects.EffectsNames.EffectSparkAsync.ToString());
            effectsOptions.Items.Add(Effects.EffectsNames.EffectSparkSync.ToString());
            effectsOptions.Items.Add(Effects.EffectsNames.EffectMeteorAsync.ToString());
            effectsOptions.Items.Add(Effects.EffectsNames.EffectMeteorSync.ToString());
            keyColor = Color.AliceBlue;
            setColor(keyColor);
            textBoxGlobalStarting.Text = getFormatedTimeString(Effects.globalStartingTime);
            textBoxNumberOfBalls.Text = Effects.numOfBall.ToString();

            // Setup Effect Attr Objects
            {
                effectAttrLabel[0] = this.labelEffectAttr0;
                effectAttrValue[0] = this.textBoxEffectAttr0;
                effectAttrUnit[0] = this.unitEffectAttr0;
                effectAttrLabel[1] = this.labelEffectAttr1;
                effectAttrValue[1] = this.textBoxEffectAttr1;
                effectAttrUnit[1] = this.unitEffectAttr1;
                effectAttrLabel[2] = this.labelEffectAttr2;
                effectAttrValue[2] = this.textBoxEffectAttr2;
                effectAttrUnit[2] = this.unitEffectAttr2;
                effectAttrLabel[3] = this.labelEffectAttr3;
                effectAttrValue[3] = this.textBoxEffectAttr3;
                effectAttrUnit[3] = this.unitEffectAttr3;
                effectAttrLabel[4] = this.labelEffectAttr4;
                effectAttrValue[4] = this.textBoxEffectAttr4;
                effectAttrUnit[4] = this.unitEffectAttr4;
                effectAttrLabel[5] = this.labelEffectAttr5;
                effectAttrValue[5] = this.textBoxEffectAttr5;
                effectAttrUnit[5] = this.unitEffectAttr5;
            }

            /* Configuration */
            textBoxGlobalStarting.Text = LightBallHelper.getFormatedTimeString(TimeSpan.Zero);
            textBoxEffectAttr5.Text = "0";
        }
        private void LightBallHelper_Paint(object sender, PaintEventArgs e) {
        }
        public static string getFormatedTimeString(TimeSpan time) {
            return string.Format("{0:D2}:{1:D2}:{2:D3}", (int)time.TotalMinutes, time.Seconds, time.Milliseconds);
        }
        public static TimeSpan getTimeSpanFromString(string str) {
            string[] words = str.Split(':');
            int min, sec, mill;
            if ( words.Length == 3
                && int.TryParse(words[0], out min)
                && int.TryParse(words[1], out sec)
                && int.TryParse(words[2], out mill) )
                return new TimeSpan(0, 0, min, sec, mill);
            MessageBox.Show("Time Format Error!\n" + str, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            return TimeSpan.Zero;
        }
        public static double getDoubleFromString(string str) {
            double result;
            if ( double.TryParse(str, out result) )
                return result;
            MessageBox.Show("Double Format Error!\n" + str, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            return 0;
        }
        public static int getIntFromString(string str) {
            int result;
            if ( int.TryParse(str, out result) )
                return result;
            MessageBox.Show("Int Format Error!\n" + str, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            return 0;
        }
        #region PlayAudio
        private static string FormatTimeSpan(TimeSpan ts) {
            return string.Format("{0:D2}:{1:D2}:{2:D3}", (int)ts.TotalMinutes, ts.Seconds, ts.Milliseconds);
        }
        private void GenerateAudioFile() {
            if ( wavePlayer == null || audioFileReader == null ) {
                wavePlayer = CreateWavePlayer();
                audioFileReader = new AudioFileReader(fileName);
                wavePlayer.Init(audioFileReader);
                wavePlayer.PlaybackStopped += OnPlaybackStopped;
                labelTotalTime.Text = prefixTotalTime + FormatTimeSpan(audioFileReader.TotalTime);
                waveImageLen = (int)audioFileReader.TotalTime.TotalSeconds * SEC_PIXEL_RATIOL;
                pictureBox1.Size = new Size(waveImageLen, 265);
            }
        }
        private string SelectInputFile() {
            CloseWavePlayer();
            timer1.Enabled = true;
            var ofd = new OpenFileDialog();
            ofd.Filter = "Audio Files|*.mp3;*.wav;*.aiff;*.wma";
            if ( ofd.ShowDialog() == DialogResult.OK ) {
                fileName = ofd.FileName;
                GenerateAudioFile();
                RenderWaveform();
                return ofd.FileName;
            }
            return null;
        }
        private void BtnLoadFile_Click(object sender, EventArgs e) {
            fileName = SelectInputFile();
        }
        void OnTimerTick(object sender, EventArgs e) {
            if ( wavePlayer != null && audioFileReader != null ) {
                TimeSpan currentTime = (wavePlayer.PlaybackState == PlaybackState.Stopped) ? TimeSpan.Zero : audioFileReader.CurrentTime;
                labelNowTime.Text = prefixNowTime + FormatTimeSpan(currentTime);
                audioTrackBar.Value = Math.Min(audioTrackBar.Maximum, (int)(audioTrackBar.Maximum * currentTime.TotalSeconds / audioFileReader.TotalTime.TotalSeconds));
                pictureBox1.Location = new Point(START_POS - (int)currentTime.TotalMilliseconds * SEC_PIXEL_RATIOL / 1000, WAVE_LOC_Y);
            }
            else {
                labelNowTime.Text = prefixNowTime + "00:00:000";
                audioTrackBar.Value = 0;
            }
        }

        private void BtnPlay_Click(object sender, EventArgs e) {
            // wavePlayer already init
            if ( wavePlayer != null ) {
                //timer1.Enabled = !timer1.Enabled;
                if ( wavePlayer.PlaybackState == PlaybackState.Playing )
                    wavePlayer.Pause();
                else
                    wavePlayer.Play();
            }
            else {
                SelectInputFile();
            }
        }
        private IWavePlayer CreateWavePlayer() {
            switch ( 1 ) {
                //switch ( comboBoxOutputDriver.SelectedIndex ) {
                case 2:
                    return new WaveOutEvent();
                case 1:
                    return new WaveOut(WaveCallbackInfo.FunctionCallback());
                default:
                    return new WaveOut();
            }
        }

        private void CloseWavePlayer() {
            if ( wavePlayer != null )
                wavePlayer.Stop();
            if ( audioFileReader != null ) {
                // this one really closes the file and ACM conversion
                audioFileReader.Dispose();
                audioFileReader = null;
            }
            if ( wavePlayer != null ) {
                wavePlayer.Dispose();
                wavePlayer = null;
            }
        }
        void OnPlaybackStopped(object sender, StoppedEventArgs e) {
            // we want to be always on the GUI thread and be able to change GUI components
            Debug.Assert(!InvokeRequired, "PlaybackStopped on wrong thread");
            // we want it to be safe to clean up input stream and playback device in the handler for PlaybackStopped
            CloseWavePlayer();
            if ( e.Exception != null ) {
                MessageBox.Show(String.Format("Playback Stopped due to an error {0}", e.Exception.Message));
            }
        }

        private void BtnStop_Click(object sender, EventArgs e) {
            CloseWavePlayer();
        }

        private void AudioTrackBar_Scroll(object sender, EventArgs e) {
            if ( wavePlayer != null ) {
                audioFileReader.CurrentTime = TimeSpan.FromSeconds(audioFileReader.TotalTime.TotalSeconds
                    * audioTrackBar.Value / audioTrackBar.Maximum);
            }
        }
        #endregion PlayAudio
        #region Draw Wave Form
        private IPeakProvider GetPeakProvider() {
            switch ( comboBoxPeakCalculationStrategy.SelectedIndex ) {
                case 0:
                    return new RmsPeakProvider(100);
                case 1:
                    return new MaxPeakProvider();
                case 2:
                    return new SamplingPeakProvider(100);
                case 3:
                    return new AveragePeakProvider(4);
                default:
                    throw new InvalidOperationException("Unknown calculation strategy");
            }
        }
        private void GetRendererSettings() {
            standardSettings.TopHeight = (int)updownTopHeight.Value;
            standardSettings.BottomHeight = (int)updownBottomHeight.Value;
            standardSettings.Width = waveImageLen;// this.Size.Width;
            //standardSettings.DecibelScale = checkBoxDecibels.Checked;
        }
        private void RenderWaveform() {
            if ( String.IsNullOrEmpty(fileName) ) return;
            GetRendererSettings();
            //if ( imageFile != null ) {
            //    settings.BackgroundImage = new Bitmap(imageFile);
            //}
            pictureBox1.Image = null;
            labelRendering.Visible = true;
            Enabled = false;
            var peakProvider = GetPeakProvider();
            Task.Factory.StartNew(() => RenderThreadFunc(peakProvider, standardSettings));
        }
        private void RenderThreadFunc(IPeakProvider peakProvider, WaveFormRendererSettings settings) {
            Image image = null;
            try {
                image = waveFormRenderer.Render(fileName, peakProvider, settings);
            }
            catch ( Exception e ) {
                MessageBox.Show("error");
                MessageBox.Show(e.Message);
            }
            BeginInvoke((Action)(() => FinishedRender(image)));
        }

        private void FinishedRender(Image image) {
            labelRendering.Visible = false;
            pictureBox1.Image = image;
            Enabled = true;
        }
        private void OnColorButtonClick(object sender, EventArgs e) {
            var button = (Button) sender;
            var cd = new ColorDialog();
            cd.Color = button.BackColor;
            if ( cd.ShowDialog(this) == DialogResult.OK ) {
                button.BackColor = cd.Color;

                standardSettings.TopPeakPen = new Pen(BtnTopColour.BackColor);
                standardSettings.BottomPeakPen = new Pen(BtnBottomColour.BackColor);
                if ( !String.IsNullOrEmpty(fileName) )
                    RenderWaveform();
            }
        }
        #endregion Draw Wave Form
        
        private void BtnBackwardMove_Click(object sender, EventArgs e) {
            audioFileReader.CurrentTime = audioFileReader.CurrentTime.Subtract(new TimeSpan(0, 0, 0, 0, (int)UpdownOffset.Value));
        }
        private void BtnForwardMove_Click(object sender, EventArgs e) {
            audioFileReader.CurrentTime = audioFileReader.CurrentTime.Add(new TimeSpan(0, 0, 0, 0, (int)UpdownOffset.Value));
        }
        
        #region Effect Add and Del
        private Effects getEffectsById(int i) {
            switch ( i ) {
                case 0:
                    return new EffectHSL(audioFileReader.CurrentTime);
                case 1:
                    return new EffectColorTransition(audioFileReader.CurrentTime);
                case 2:
                    return new EffectSparkAsync(audioFileReader.CurrentTime);
                case 3:
                    return new EffectSparkSync(audioFileReader.CurrentTime);
                case 4:
                    return new EffectMeteorAsync(audioFileReader.CurrentTime);
                case 5:
                    return new EffectMeteorSync(audioFileReader.CurrentTime);
                default:
                    return new EffectColorTransition(audioFileReader.CurrentTime);
            }
        }
        private void BtnAddEffect_Click(object sender, EventArgs e) {
            if ( audioFileReader == null )
                MessageBox.Show("Please load music file before setting effects!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            else if ( effectsOptions.SelectedIndex < 0 )
                MessageBox.Show("Please select an effect from effect list!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            else {
                Effects new_effect = getEffectsById(effectsOptions.SelectedIndex);
                effectsList.Items.Insert(effectsList.SelectedIndex+1, new_effect.Effect_Name);
                effectListObj.Insert(effectsList.SelectedIndex+1, new_effect);
                /*
                if ( effectsList.SelectedIndex >= 0 ) {
                    effectsList.Items.Insert(effectsList.SelectedIndex, new_effect.Effect_Name);
                    effectListObj.Insert(effectsList.SelectedIndex, new_effect);
                }
                else {
                    effectsList.Items.Add(new_effect.Effect_Name);
                    effectListObj.Add(new_effect);
                }*/
            }
        }
        private void BtnDelEffect_Click(object sender, EventArgs e) {
            if ( effectsList.SelectedIndex >= 0 ) {
                int index = effectsList.SelectedIndex;
                effectListObj.RemoveAt(index);
                effectsList.Items.RemoveAt(index);
            }
        }
        #endregion

        #region Effect (Effect selection changed)
        private void CheckBoxAutoGen_CheckedChanged(object sender, EventArgs e) {
            Effects selected = effectListObj[effectsList.SelectedIndex];
            if ( selected.autoGenMode != Effects.AutoGenMode.either && checkBoxAutoGen.Checked ) {
                BtnAddKeypoint.Enabled = false;
                BtnDelKeypoint.Enabled = false;
                selected.autoGen = true;
                keypointStartingColor.Text = "Effect Starting Color";
            }
            else {
                BtnAddKeypoint.Enabled = true;
                BtnDelKeypoint.Enabled = true;
                selected.autoGen = false;
                keypointStartingColor.Text = "Keypoint Starting Color";
            }
        }
        /* Prevent recursive setting while reloading effect attributes. */
        private bool mainTextBoxLoading = false;
        private void EffectAutoGen_TextChanged(object sender, EventArgs e) {
            Effects selected = effectListObj[effectsList.SelectedIndex];
            if ( !mainTextBoxLoading && effectsList.SelectedIndex >= 0 ) {
                /* Effect Auto Gen*/
                if ( selected.autoGenMode != Effects.AutoGenMode.disable ) {
                    for ( int i = 0; i < maxEffectAttr; i++ ) {
                        if ( selected.para_len > i ) {
                            selected.para_label[i] = effectAttrLabel[i].Text;
                            selected.para_value[i] = effectAttrValue[i].Text;
                            selected.para_unit[i] = effectAttrUnit[i].Text;
                        }
                    }
                }
            }
        }
        private void EffectSetting_TextChanged(object sender, EventArgs e) {
            if ( !mainTextBoxLoading && effectsList.SelectedIndex >= 0 ) {
                Effects selected = effectListObj[effectsList.SelectedIndex];
                selected.startTime = getTimeSpanFromString(textBoxEffectStart.Text);
                selected.endTime = getTimeSpanFromString(textBoxEffectEnd.Text);
                // Update name in listbox
                effectsList.Items[effectsList.SelectedIndex] = selected.Effect_Name;
                textBoxEffectDuration.Text = (selected.endTime - selected.startTime).TotalSeconds.ToString();
            }
        }
        private void reloadEffect() {
            Effects selected = effectListObj[effectsList.SelectedIndex];
            SetKeypointOnlyColor(selected.autoGenMode == Effects.AutoGenMode.enable);
            /* Effect Setting*/
            textBoxEffectName.Text = selected.effectName.ToString();                        // Name
            textBoxEffectStart.Text = getFormatedTimeString(selected.startTime);            // Start time 
            textBoxEffectEnd.Text = getFormatedTimeString(selected.endTime);                //End time
            textBoxEffectDuration.Text = (selected.endTime - selected.startTime).TotalSeconds.ToString(); // Duration

            /* Effect Attr*/
            if ( selected.autoGenMode == Effects.AutoGenMode.enable )
                checkBoxAutoGen.Checked = true;

            if ( selected.autoGenMode != Effects.AutoGenMode.either )
                checkBoxAutoGen.Enabled = false;
            else
                checkBoxAutoGen.Enabled = true;

            if ( selected.autoGenMode != Effects.AutoGenMode.disable ) {
                for ( int i = 0; i < maxEffectAttr; i++ ) {
                    if ( selected.para_len > i ) {
                        effectAttrValue[i].Enabled = true;
                        effectAttrLabel[i].Text = selected.para_label[i];
                        effectAttrValue[i].Text = selected.para_value[i];
                        effectAttrUnit[i].Text = selected.para_unit[i];
                    }
                    else
                        effectAttrValue[i].Enabled = false;
                }
            }
        }
        private void EffectsList_SelectedIndexChanged(object sender, EventArgs e) {
            if ( effectsList.SelectedIndex >= 0 ) {
                mainTextBoxLoading = true;
                reloadEffect();  // Reload Effect Setting and Auto Gen
                mainTextBoxLoading = false;

                keypointsList.Items.Clear();
                effectListObj[effectsList.SelectedIndex].keypoints.ForEach((keypoint) =>
                {
                    keypointsList.Items.Add(keypoint.Time_Info);
                });
            }
        }
        private void BtnSetStartNowtime_Click(object sender, EventArgs e) {
            textBoxEffectStart.Text = audioFileReader != null ? getFormatedTimeString(audioFileReader.CurrentTime) : "0";
            EffectSetting_TextChanged(sender, e);
        }
        private void BtnSetEndNowtime_Click(object sender, EventArgs e) {
            textBoxEffectEnd.Text = audioFileReader != null ? getFormatedTimeString(audioFileReader.CurrentTime) : "0";
            EffectSetting_TextChanged(sender, e);
        }
        #endregion

        #region Color Selector
        private void setColor(Color color) {
            keyColor = color;
            BtnColorShowStart.BackColor = color;
            labelColorH.Text = "H= " + Math.Round(color.GetHue(), 0).ToString();
            labelColorS.Text = "S= " + Math.Round(color.GetSaturation(), 2).ToString();
            labelColorL.Text = "L= " + Math.Round(color.GetBrightness(), 2).ToString();
            setShowEndColor(color);
        }
        private void setShowEndColor(Color color) {
            HSLColor hsl = new HSLColor(color);
            HSLColor hsl2;
            labelColorH.Text = "H= " + hsl.Hue.ToString();
            Color col = hsl2 = hsl.offsetColor(
                getDoubleFromString(textBoxKeypointDuration.Text)* getDoubleFromString(textBoxKeypointDuty.Text)/100.0,
                getDoubleFromString(textBoxColorTransition.Text),
                getDoubleFromString(textBoxBrightTransition.Text));
            BtnColorShowEnd.BackColor = col;
            labelEndColorH.Text = "H= " + Math.Round(col.GetHue(), 0).ToString();
            labelEndColorS.Text = "S= " + Math.Round(col.GetSaturation(), 2).ToString();
            labelEndColorL.Text = "L= " + Math.Round(col.GetBrightness(), 2).ToString();
        }
        #endregion
        #region Keypoint
        private bool keypointTextBoxLoading = false;
        private void updateKeypointInputs() {
            if ( effectsList.SelectedIndex >= 0 && keypointsList.SelectedIndex >= 0 ) {
                KeyPoint target = effectListObj[effectsList.SelectedIndex].keypoints[keypointsList.SelectedIndex];
                textBoxColorStart.Text = getFormatedTimeString(target.startTime);
                textBoxKeypointDuration.Text = target.duration.ToString();
                textBoxKeypointDuty.Text = target.duty.ToString();
                setColor(target.color);
                textBoxColorTransition.Text = target.colorTransition.ToString();
                textBoxBrightTransition.Text = target.brightTransition.ToString();
                audioFileReader.CurrentTime = target.startTime;
            }
        }
        private void setKeypointInputs() {
            if (  keypointsList.SelectedIndex >= 0 ) {
                KeyPoint target = effectListObj[effectsList.SelectedIndex].keypoints[keypointsList.SelectedIndex];
                target.startTime = getTimeSpanFromString(textBoxColorStart.Text);
                target.duration = getDoubleFromString(textBoxKeypointDuration.Text);
                target.duty = getDoubleFromString(textBoxKeypointDuty.Text);
                target.color = keyColor;
                target.colorTransition = getDoubleFromString(textBoxColorTransition.Text);
                target.brightTransition = getDoubleFromString(textBoxBrightTransition.Text);
                keypointsList.Items[keypointsList.SelectedIndex] = target.Time_Info;
            }
        }
        private void KeypointsList_SelectedIndexChanged(object sender, EventArgs e) {
            if ( keypointsList.SelectedIndex >= 0 ) {
                keypointTextBoxLoading = true;
                updateKeypointInputs();
                keypointTextBoxLoading = false;
            }
        }
        private void BtnAddKeypoint_Click(object sender, EventArgs e) {
            if ( audioFileReader == null )
                MessageBox.Show("Please load music file before setting effects!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            else if ( effectsList.SelectedIndex < 0 )
                MessageBox.Show("Please select a effect first!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            else if ( keypointsList.SelectedIndex < 0 )
                MessageBox.Show("Please select a keypoint first!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            // Allow to create keypoint
            else if (effectListObj[effectsList.SelectedIndex].autoGenMode ==   Effects.AutoGenMode.either ) {
                //Common attribute
                KeyPoint new_keypoint;
                Effects selected = effectListObj[effectsList.SelectedIndex];
                double dur=0, duty=0, colorTran=0, brightTran=0;
                Color color = keyColor;
                // Code assistent generation (NOT auto generation for all )
                // Only Auto Gen mode is either can do this
                if ( checkBoxAutoGen.Checked ) {
                    // Create keypoint according to last keypoint
                    for ( int i = 0; i < selected.para_len; i++ ) {
                        if ( selected.para_label[i] == "Duration" )
                            dur = getDoubleFromString(effectAttrValue[i].Text);
                        if ( selected.para_label[i] == "Duty" )
                            duty = getDoubleFromString(effectAttrValue[i].Text);
                        if ( selected.para_label[i] == "ColorTran" )
                            colorTran = getDoubleFromString(effectAttrValue[i].Text);
                        if ( selected.para_label[i] == "BrightTran" )
                            brightTran = getDoubleFromString(effectAttrValue[i].Text);
                    }
                    Color starting_color = selected.keypoints[0].color;
                    color = new HSLColor(starting_color).offsetColor(
                        (audioFileReader.CurrentTime.Subtract(selected.keypoints[0].startTime).TotalSeconds),
                        colorTran, brightTran);
                }
                else {
                    if (keypointsList.SelectedIndex >= 0 ) {
                        // The previous keypoint setting (If it exist)
                        KeyPoint last_point = effectListObj[effectsList.SelectedIndex].keypoints[keypointsList.SelectedIndex];
                        dur = last_point.duration;
                        duty = last_point.duty;
                        colorTran = last_point.colorTransition;
                        brightTran = last_point.brightTransition;
                    }
                }
                new_keypoint = new KeyPoint(audioFileReader.CurrentTime, color, dur, duty, colorTran, brightTran);
                effectListObj[effectsList.SelectedIndex].keypoints.Add(new_keypoint);
                keypointsList.Items.Add(new_keypoint.Time_Info);
                keypointsList.SelectedIndex += 1;
                audioFileReader.CurrentTime = audioFileReader.CurrentTime.Add(TimeSpan.FromSeconds(dur));
            }
        }
        private void BtnDelKeypoint_Click(object sender, EventArgs e) {
            if ( effectsList.SelectedIndex >= 0 && keypointsList.SelectedIndex >= 0 ) {
                if ( effectListObj[effectsList.SelectedIndex].keypoints.Count <= 1 ) {
                    MessageBox.Show("There must be at least a keypoint in an effect!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                int index = keypointsList.SelectedIndex;
                effectListObj[effectsList.SelectedIndex].keypoints.RemoveAt(index);
                keypointsList.Items.RemoveAt(index);
            }
        }
        private void KeypointSetting_TextChanged(object sender, EventArgs e) {
            setKeypointInputs();
        }
        private void BtnSetColorNowtime_Click(object sender, EventArgs e) {
            textBoxColorStart.Text = audioFileReader != null ? getFormatedTimeString(audioFileReader.CurrentTime) : "0";
            setKeypointInputs();
        }
        private void showEndColor_Changed(object sender, EventArgs e) {
            setShowEndColor(keyColor);
            setKeypointInputs();
        }
        private void BtnKeypointSave_Click(object sender, EventArgs e) {
            setKeypointInputs();
        }
        private void SetKeypointOnlyColor(bool mode) { // True for only color textbox enabled
            textBoxColorStart.Enabled = !mode;
            textBoxKeypointDuration.Enabled = !mode;
            textBoxKeypointDuty.Enabled = !mode;
            textBoxColorTransition.Enabled = !mode;
            textBoxBrightTransition.Enabled = !mode;
        }
        #endregion

        #region Set Configuration
        private void BtnSetGlobalStarting_Click(object sender, EventArgs e) {
            textBoxGlobalStarting.Text = audioFileReader != null ? getFormatedTimeString(audioFileReader.CurrentTime) : "0";
            Effects.globalStartingTime = audioFileReader.CurrentTime;
        }
        private void BtnHistorySave_Click(object sender, EventArgs e) {
            var ofd = new SaveFileDialog();
            ofd.Filter = "LightBallHistory|*.lbh";
            if ( ofd.ShowDialog() == DialogResult.OK ) {
                Effects.SaveHistory(ofd.FileName, effectListObj);
            }
        }
        private void BtnHistoryLoad_Click(object sender, EventArgs e) {
            var ofd = new OpenFileDialog();
            ofd.Filter = "LightBallHistory|*.lbh";
            if ( ofd.ShowDialog() == DialogResult.OK ) {
                Effects.RestoreHistory(ofd.FileName, out effectListObj);
                effectsList.Items.Clear();
                effectListObj.ForEach((eff) =>
                {
                    effectsList.Items.Add(eff.Effect_Name);
                });
                textBoxGlobalStarting.Text = getFormatedTimeString(Effects.globalStartingTime);
                textBoxNumberOfBalls.Text = Effects.numOfBall.ToString();
            }
        }
        private void BtnExport_Click(object sender, EventArgs e) {
            var fbd = new FolderBrowserDialog();
            if ( fbd.ShowDialog() == DialogResult.OK && !string.IsNullOrWhiteSpace(fbd.SelectedPath)) {
                Effects.ExportTasks(Path.Combine(fbd.SelectedPath, outputTaskName), effectListObj);
            }
        }

        #endregion
        private void BtnColorShowStart_Click(object sender, EventArgs e) {
            ColorDialog colorDlg = new ColorDialog();
            colorDlg.Color = keyColor;
            if ( colorDlg.ShowDialog() == DialogResult.OK ) {
                setColor(colorDlg.Color);
            }
            setKeypointInputs();
        }

        private void BtnAutoGen_Click(object sender, EventArgs e) {

        }

        private void TextBoxGlobalStarting_Leave(object sender, EventArgs e) {
            Effects.globalStartingTime = getTimeSpanFromString(textBoxGlobalStarting.Text);
        }

        private void TextBoxNumberOfBalls_Leave(object sender, EventArgs e) {
            Effects.numOfBall = getIntFromString(textBoxNumberOfBalls.Text);
        }

        private void TextBoxNumberOfBalls_TextChanged(object sender, EventArgs e) {
            Effects.numOfBall = getIntFromString(textBoxNumberOfBalls.Text);
        }
    }
}
