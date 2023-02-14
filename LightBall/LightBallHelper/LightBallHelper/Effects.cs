using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.IO;
using System.Windows.Forms;

namespace LightBallHelper {
    class KeyPoint {
        public static Color defaultColor = Color.Bisque;
        public const double defaultDuration = 1; // in seconds
        public const double defaultDuty = 100; // in percentage
        public TimeSpan startTime;
        public double duration; // in seconds
        public double duty;     // in percentage

        public double colorTransition;
        public double brightTransition;
        public Color color;

        /* Effect Attr (Auto Generation) */
        public string Time_Info {
            get {
                return (string.Format("{0:D2}:{1:D2}:{2:D3}", (int)startTime.TotalMinutes, startTime.Seconds
                    , startTime.Milliseconds) + " \t " + string.Format("{0:0.000}", duration));
            }
        }
        public KeyPoint(TimeSpan start_time, Color col, double dur, double duty, double colTran, double briTran) {
            this.startTime = start_time;
            duration = dur;
            this.duty = duty;
            color = col;
            colorTransition = colTran;
            brightTransition = briTran;
        }
        public KeyPoint(TimeSpan start_time, Color col, double dur = defaultDuration, double duty = defaultDuty) {
            this.startTime = start_time;
            duration = dur;
            this.duty = duty;
            color = col;
            colorTransition = 0;
            brightTransition = 0;
        }
        public string getBasicInfo(int phase) {
            TimeSpan t_start = startTime.Subtract(Effects.globalStartingTime);
            string[] startHex = KeyPoint.getHexFromTimeSpan(t_start);
            string[] durHex = KeyPoint.get2HexFromInt((int)(1000 * duration * duty / 100));
            color = HSLColor.offsetColor(color, phase, 0);
            string[] colorHex = KeyPoint.getHexFromColor(color);
            return String.Format(" 0x{0}, 0x{1}, 0x{2}, 0x{3}, 0x{4}, 0x{5}, 0x{6}, 0x{7}, 0x{8}", startHex[0], startHex[1], startHex[2], startHex[3], durHex[0], durHex[1], colorHex[0], colorHex[1], colorHex[2]);
        }
        public static int getFormatedFloat(double d) {
            return (int)Math.Round(d * 100.0) + 127;
        }
        public static string[] getHexFromTimeSpan(TimeSpan time) {
            long start = (long)time.TotalMilliseconds;
            string hex = Convert.ToString(start, 16).PadLeft(8, '0');
            string[] hex_arr = new string[4];
            for ( int i = 0; i < 4; i++ )
                hex_arr[i] = hex.Substring(2 * i, 2);
            return hex_arr;
        }
        public static string get1HexFromInt(int time) {
            string hex = Convert.ToString(time, 16).PadLeft(2, '0');
            return hex;
        }
        public static string[] get2HexFromInt(int time) {
            string hex = Convert.ToString(time, 16).PadLeft(4, '0');
            string[] hex_arr = new string[2];
            for ( int i = 0; i < 2; i++ )
                hex_arr[i] = hex.Substring(2 * i, 2);
            return hex_arr;
        }
        public static string[] getHexFromColor(Color color) {
            string[] hex = {
                Convert.ToString((int)(0.7*color.GetHue()), 16).PadLeft(2, '0'),
                Convert.ToString((int)(color.GetSaturation()*255.0), 16).PadLeft(2, '0'),
                Convert.ToString((int)(color.GetBrightness()*255.0), 16).PadLeft(2, '0')};
            return hex;
        }
    }
    abstract class Effects {
        public enum AutoGenMode { enable, disable, either };
        public enum EffectsNames { EffectHSL, EffectColorTransition, EffectSparkAsync, EffectSparkSync, EffectMeteorAsync, EffectMeteorSync, End };
        public EffectsNames effectName;
        public AutoGenMode autoGenMode;
        public bool autoGen = false;
        public int para_len;
        public string[] para_label;
        public string[] para_value;
        public string[] para_unit;
        public string effectCmdName = "";
        public const int defaultDuration = 10; // in seconds
        public const int defaultTransition = 0; // in seconds
        public int effectOffset = 0;
        public static TimeSpan globalStartingTime = TimeSpan.Zero;
        public static int numOfBall = 1;
        public string Effect_Name {
            get {
                return (this.effectName.ToString() + "\t" + string.Format("{0:D2}:{1:D2}:{2:D3}", (int)startTime.TotalMinutes, startTime.Seconds, startTime.Milliseconds));
            }
        }
        public string Time_Info {
            get { return string.Format("{0:D2}:{1:D2}:{2:D3}", (int)startTime.TotalMinutes, startTime.Seconds, startTime.Milliseconds); }
        }
        public TimeSpan startTime;
        public TimeSpan endTime;
        public List<KeyPoint> keypoints;

        protected abstract void paraInit();
        public Effects(TimeSpan nowtime, EffectsNames name, double dur = defaultDuration) {
            effectName = name;
            startTime = nowtime;
            endTime = startTime.Add(new TimeSpan(0, 0, 0, (int)dur));
            keypoints = new List<KeyPoint>();
            keypoints.Add(new KeyPoint(startTime, KeyPoint.defaultColor));
        }
        public Effects(TimeSpan nowtime, TimeSpan end, EffectsNames name) {
            effectName = name;
            startTime = nowtime;
            endTime = end;
            keypoints = new List<KeyPoint>();
        }
        public void Add(TimeSpan nowtime, double dur = defaultDuration) {
            keypoints.Add(new KeyPoint(nowtime, KeyPoint.defaultColor));
        }
        public void Add(TimeSpan start_time, double dur, double duty, Color col, double colTran, double briTran) {
            keypoints.Add(new KeyPoint(start_time, col, dur, duty, colTran, briTran));
        }
        public void Delete(int index) {
            keypoints.RemoveAt(index);
        }
        public abstract string GetHistory(int id);
        public abstract void LoadKeypoints(string[] strs);
        public abstract string GetTasks(int ballID);
        public static void SaveHistory(string filename, List<Effects> effects) {
            string cmd = "0," + LightBallHelper.getFormatedTimeString(globalStartingTime) + "," + numOfBall + "\n";
            int cmd_id = 1;
            effects.ForEach((effect) =>
            {
                cmd += effect.GetHistory(cmd_id);
                cmd_id++;
            });
            cmd += cmd_id + ",End\n";
            StreamWriter sw = new StreamWriter(filename);
            sw.WriteLine(cmd);
            sw.Close();
        }
        public static void RestoreHistory(string filename, out List<Effects> effects) {
            effects = new List<Effects>();
            StreamReader sr = new StreamReader(filename);
            Effects now_eff = null;
            string line = "";
            int cmd_id = -1;
            while ( (line = sr.ReadLine()) != null ) {
                string[] strs = line.Split(',');
                if ( strs.Length <= 1 )
                    continue;
                int id;
                if ( int.TryParse(strs[0], out id) ) {
                    if ( id == 0 ) {
                        globalStartingTime = LightBallHelper.getTimeSpanFromString(strs[1]);
                        numOfBall = LightBallHelper.getIntFromString(strs[2]);
                    }
                    else if ( cmd_id != id ) {
                        if ( now_eff != null )
                            effects.Add(now_eff);
                        EffectsNames effect = (EffectsNames)Enum.Parse(typeof(EffectsNames), strs[1]);
                        switch ( effect ) {
                            case EffectsNames.EffectHSL:
                                now_eff = new EffectHSL(strs); break;
                            case EffectsNames.EffectColorTransition:
                                now_eff = new EffectColorTransition(strs); break;
                            case EffectsNames.EffectSparkAsync:
                                now_eff = new EffectSparkAsync(strs); break;
                            case EffectsNames.EffectSparkSync:
                                now_eff = new EffectSparkSync(strs); break;
                            case EffectsNames.EffectMeteorAsync:
                                now_eff = new EffectMeteorAsync(strs); break;
                            case EffectsNames.EffectMeteorSync:
                                now_eff = new EffectMeteorSync(strs); break;
                            default:
                                break;
                        }
                    }
                    else
                        now_eff.LoadKeypoints(strs);
                    cmd_id = id;
                }
            }
            sr.Close();
        }
        public static void ExportTasks(string filename, List<Effects> effects) {
            for ( int i = 0; i < numOfBall; i++ ) {
                filename = filename + i.ToString() + ".h";
                string cmd = "";
                effects.ForEach((eff) =>
                {
                    cmd += eff.GetTasks(i);
                });
                StreamWriter sw = new StreamWriter(filename);
                sw.WriteLine(cmd);
                sw.Close();
            }
        }
        protected double[] getParas() {
            double[] paras = new double[para_len];
            for ( int i = 0; i < para_len; i++ ) {
                paras[i] = LightBallHelper.getDoubleFromString(para_value[i]);
            }
            return paras;
        }
        protected string getParaCmd() {
            string cmd = "";
            for ( int i = 0; i < this.para_len; i++ )
                cmd += "," + para_value[i];
            return cmd;
        }
        protected void loadPara(string[] strs, int index) {
            for ( int i = 0; i < this.para_len; i++ )
                para_value[i] = strs[index + i];
        }
    }
    class EffectHSL : Effects {
        protected override void paraInit() {
            base.autoGenMode = AutoGenMode.either;
            base.para_label = new string[] { "Count", "Duration", "Duty" };
            base.para_unit = new string[] { "times", "sec", "%" };
            base.para_value = new string[] { "1", "0.45", "100" };
            base.effectCmdName = "LM_SET_HSL";
            para_len = para_value.Length;
        }
        public EffectHSL(TimeSpan start_time)
            : base(start_time, EffectsNames.EffectHSL) {
            paraInit();
        }
        public EffectHSL(string[] strs)
            : base(LightBallHelper.getTimeSpanFromString(strs[2]), LightBallHelper.getTimeSpanFromString(strs[3]), EffectsNames.EffectHSL) {
            paraInit();
            autoGen = (strs[4] == "autoGen" ? true : false);
            loadPara(strs, 5);
        }
        public override string GetHistory(int id) {
            string autoGenStr = (autoGen ? "autoGen" : "manGen");
            string cmd = id + "," + this.effectName + "," + LightBallHelper.getFormatedTimeString(startTime) + "," + LightBallHelper.getFormatedTimeString(endTime) + "," + autoGenStr + getParaCmd() + "\n";
            keypoints.ForEach((key) =>
            {
                cmd += id + "," + this.effectName + "," + LightBallHelper.getFormatedTimeString(key.startTime) + "," +
                key.duration + "," + key.duty + "," + key.color.R + "," + key.color.G + "," + key.color.B + "\n";
            });
            return cmd;
        }
        public override void LoadKeypoints(string[] strs) {
            this.Add(LightBallHelper.getTimeSpanFromString(strs[2]),    // Start_time
            LightBallHelper.getDoubleFromString(strs[3]),               // Duration
            LightBallHelper.getDoubleFromString(strs[4]),               // Duty
            Color.FromArgb(LightBallHelper.getIntFromString(strs[5]),   // R                         
                           LightBallHelper.getIntFromString(strs[6]),   // G
                           LightBallHelper.getIntFromString(strs[7])),  // B
            0, 0);
        }
        public override string GetTasks(int ballID) {
            string cmd = "";
            keypoints.ForEach((key) =>
            {
                cmd += "{"+effectCmdName+", " + key.getBasicInfo(0) + ", 0x00, 0x00, 0x00, 0x00},\n";
            });
            return cmd;
        }
    }
    class EffectColorTransition : Effects {
        protected override void paraInit() {
            base.autoGenMode = AutoGenMode.either;
            para_label = new string[] { "Count", "Duration", "Duty", "ColorTran", "BrightTran", "Phase" };
            para_unit = new string[] { "times", "sec", "%", "sec", "sec", "deg" };
            para_value = new string[] { "1", "0.45", "100", "0", "0", "0" };
            base.effectCmdName = "LM_SET_HSL_PROGRESSIVE";
            para_len = para_value.Length;
        }
        public EffectColorTransition(TimeSpan start_time)
            : base(start_time, EffectsNames.EffectColorTransition) {
            paraInit();
        }
        public EffectColorTransition(string[] strs)
            : base(LightBallHelper.getTimeSpanFromString(strs[2]), LightBallHelper.getTimeSpanFromString(strs[3]), EffectsNames.EffectColorTransition) {
            paraInit();
            autoGen = (strs[4] == "autoGen" ? true : false);
            loadPara(strs, 5);
        }
        public override string GetHistory(int id) {
            string autoGenStr = (autoGen ? "autoGen" : "manGen");
            string cmd = id + "," + this.effectName + "," + LightBallHelper.getFormatedTimeString(startTime) + "," + LightBallHelper.getFormatedTimeString(endTime) + "," + autoGenStr + getParaCmd() + "\n";
            keypoints.ForEach((key) =>
            {
                cmd += id + "," + this.effectName + "," + LightBallHelper.getFormatedTimeString(key.startTime) + "," + key.duration + "," + key.duty + "," + key.colorTransition + "," + key.brightTransition + "," + key.color.R + "," + key.color.G + "," + key.color.B + "\n";
            });
            return cmd;
        }
        public override void LoadKeypoints(string[] strs) {
            this.Add(LightBallHelper.getTimeSpanFromString(strs[2]),    // Start_time
            LightBallHelper.getDoubleFromString(strs[3]),               // Duration
            LightBallHelper.getDoubleFromString(strs[4]),               // Duty
            Color.FromArgb(LightBallHelper.getIntFromString(strs[7]),   // R
                            LightBallHelper.getIntFromString(strs[8]),  // G
                            LightBallHelper.getIntFromString(strs[9])), // B
            LightBallHelper.getDoubleFromString(strs[5]),               // Color Tran
            LightBallHelper.getDoubleFromString(strs[6]));              // Bright Tran
        }
        public override string GetTasks(int ballID) {
            string cmd = "";
            double phase = LightBallHelper.getDoubleFromString(para_value[5]);
            keypoints.ForEach((key) =>
            {
                cmd += "{"+effectCmdName+"," + key.getBasicInfo(ballID * (int)phase) + ", 0x" + KeyPoint.get1HexFromInt((int)key.colorTransition) + ", 0x" + KeyPoint.get1HexFromInt(KeyPoint.getFormatedFloat(key.brightTransition)) + ", 0x00, 0x00},\n";
            });
            return cmd;
        }
    }
    abstract class EffectSpark : Effects {
        protected override void paraInit() {
            base.autoGenMode = AutoGenMode.enable;
            base.para_label = new string[] { "Count", "Duration", "Duty", "ColorTran", "BrightTran", "Phase" };
            base.para_unit = new string[] { "times", "sec", "%", "sec", "sec", "deg" };
            base.para_value = new string[] { "1", "0.45", "100", "0", "0", "0" };
            para_len = para_value.Length;
        }
        public EffectSpark(TimeSpan start_time, EffectsNames name) : base(start_time, name) {
            paraInit();
        }
        public EffectSpark(string[] strs, EffectsNames name) : base(LightBallHelper.getTimeSpanFromString(strs[2]), LightBallHelper.getTimeSpanFromString(strs[3]), name) {
            paraInit();
            loadPara(strs, 7);
            this.Add(this.startTime, 0, 0,
            Color.FromArgb(LightBallHelper.getIntFromString(strs[4]),   // R
                        LightBallHelper.getIntFromString(strs[5]),   // G
                        LightBallHelper.getIntFromString(strs[6])),  // B
            0, 0);              // Bright Tran
        }
        public override string GetHistory(int id) {
            KeyPoint key = keypoints[0];
            string cmd = id + "," + this.effectName + "," + LightBallHelper.getFormatedTimeString(startTime) + "," + LightBallHelper.getFormatedTimeString(endTime) + "," + key.color.R + "," + key.color.G + "," + key.color.B + getParaCmd() + "\n";
            return cmd;
        }
        public override void LoadKeypoints(string[] strs) {
            /* No need */
        }
        public override string GetTasks(int ballID) {
            double[] paras = getParas();
            string[] startHex = KeyPoint.getHexFromTimeSpan(this.startTime.Subtract(globalStartingTime));
            int dur = (int)(endTime - startTime).TotalMilliseconds;
            string[] durHex = KeyPoint.get2HexFromInt(dur);
            double phase = LightBallHelper.getDoubleFromString(para_value[5]);
            Color color = HSLColor.offsetColor(this.keypoints[0].color, ballID * (int)phase, 0);
            string[] colorHex = KeyPoint.getHexFromColor(color);
            string colTranHex = KeyPoint.get1HexFromInt((int)paras[3]);
            string briTranHex = KeyPoint.get1HexFromInt(KeyPoint.getFormatedFloat(paras[4]));
            string count = KeyPoint.get1HexFromInt((int)paras[0]);
            string duty = KeyPoint.get1HexFromInt((int)paras[2]);
            string cmd = "{"+effectCmdName+", " + String.Format("0x{0}, 0x{1}, 0x{2}, 0x{3}, 0x{4}, 0x{5}, 0x{6}, 0x{7}, 0x{8}, 0x{9}, 0x{10}, 0x{11}, 0x{12}", startHex[0], startHex[1], startHex[2], startHex[3], durHex[0], durHex[1], colorHex[0], colorHex[1], colorHex[2], colTranHex, briTranHex, count, duty) + "},\n";
            return cmd;
        }
    }
    class EffectSparkAsync : EffectSpark {
        public EffectSparkAsync(TimeSpan start_time)
            : base(start_time, EffectsNames.EffectSparkAsync) {
            base.effectCmdName = "LM_SET_HSL_SPARK_ASYNC";
        }
        public EffectSparkAsync(string[] strs)
            : base(strs, EffectsNames.EffectSparkAsync) {
            base.effectCmdName = "LM_SET_HSL_SPARK_ASYNC";
        }
    }
    class EffectSparkSync : EffectSpark {
        public EffectSparkSync(TimeSpan start_time)
            : base(start_time, EffectsNames.EffectSparkSync) {
            base.effectCmdName = "LM_SET_HSL_SPARK_SYNC";
        }
        public EffectSparkSync(string[] strs)
            : base(strs, EffectsNames.EffectSparkSync) {
            base.effectCmdName = "LM_SET_HSL_SPARK_SYNC";
        }
    }
    abstract class EffectMeteor : Effects {
        protected override void paraInit() {
            base.autoGenMode = AutoGenMode.enable;
            base.para_label = new string[] { "Count", "Duration", "Duty", "ColorTran", "MeteorTran", "Phase" };
            base.para_unit = new string[] { "times", "sec", "%", "sec", "sec", "deg" };
            base.para_value = new string[] { "1", "0.45", "100", "0", "0", "0" };
            para_len = para_value.Length;
        }
        public EffectMeteor(TimeSpan start_time, EffectsNames name) : base(start_time, name) {
            paraInit();
        }
        public EffectMeteor(string[] strs, EffectsNames name) : base(LightBallHelper.getTimeSpanFromString(strs[2]), LightBallHelper.getTimeSpanFromString(strs[3]), name) {
            paraInit();
            loadPara(strs, 7);
            this.Add(this.startTime, 0, 0,
            Color.FromArgb(LightBallHelper.getIntFromString(strs[4]),   // R
                        LightBallHelper.getIntFromString(strs[5]),   // G
                        LightBallHelper.getIntFromString(strs[6])),  // B
            0, 0);              // Bright Tran
        }
        public override string GetHistory(int id) {
            KeyPoint key = keypoints[0];
            string cmd = id + "," + this.effectName + "," + LightBallHelper.getFormatedTimeString(startTime) + "," + LightBallHelper.getFormatedTimeString(endTime) + "," + key.color.R + "," + key.color.G + "," + key.color.B + getParaCmd() + "\n";
            return cmd;
        }
        public override void LoadKeypoints(string[] strs) {
            /* No need */
        }
        public override string GetTasks(int ballID) {
            double[] paras = getParas();
            string[] startHex = KeyPoint.getHexFromTimeSpan(this.startTime.Subtract(globalStartingTime));
            int dur = (int)(endTime - startTime).TotalMilliseconds;
            string[] durHex = KeyPoint.get2HexFromInt(dur);
            double phase = LightBallHelper.getDoubleFromString(para_value[5]);
            Color color = HSLColor.offsetColor(this.keypoints[0].color, ballID * (int)phase, 0);
            string[] colorHex = KeyPoint.getHexFromColor(color);
            string colTranHex = KeyPoint.get1HexFromInt((int)paras[3]);
            string meteorTranHex = KeyPoint.get1HexFromInt((int)paras[4]);
            string count = KeyPoint.get1HexFromInt((int)paras[0]);
            string duty = KeyPoint.get1HexFromInt((int)paras[2]);
            string cmd = "{"+effectCmdName+", " + String.Format("0x{0}, 0x{1}, 0x{2}, 0x{3}, 0x{4}, 0x{5}, 0x{6}, 0x{7}, 0x{8}, 0x{9}, 0x{10}, 0x{11}, 0x{12}", startHex[0], startHex[1], startHex[2], startHex[3], durHex[0], durHex[1], colorHex[0], colorHex[1], colorHex[2], colTranHex, meteorTranHex, count, duty) + "},\n";
            return cmd;
        }
    }
    class EffectMeteorAsync : EffectMeteor {
        public EffectMeteorAsync(TimeSpan start_time)
            : base(start_time, EffectsNames.EffectMeteorAsync) {
            base.effectCmdName = "LM_SET_HSL_METEOR_ASYNC";
        }
        public EffectMeteorAsync(string[] strs)
            : base(strs, EffectsNames.EffectMeteorAsync) {
            base.effectCmdName = "LM_SET_HSL_METEOR_ASYNC";
        }
    }
    class EffectMeteorSync : EffectMeteor {
        public EffectMeteorSync(TimeSpan start_time)
            : base(start_time, EffectsNames.EffectMeteorSync) {
            base.effectCmdName = "LM_SET_HSL_METEOR_SYNC";
        }
        public EffectMeteorSync(string[] strs)
            : base(strs, EffectsNames.EffectMeteorSync) {
            base.effectCmdName = "LM_SET_HSL_METEOR_SYNC";
        }
    }
}
