const express = require('express');
const fs = require('fs');
var formidable = require('formidable');
const app = express();
const port = 10240;
var light_state = [0, 0, 0]
var light_effect = [0, 0, 0]
var EXE_MODE = 0 //0 auto 1 manual

var Time = 0;

let EffectMapData = fs.readFileSync('public/test.json');
let EffectMap = JSON.parse(EffectMapData);

//setInterval(state_update, 1000)

function createEnum(name_list) {
    enum_dict = {};
    id = 0
    name_list.forEach((e) => {
        enum_dict[e] = id;
        id += 1
    })
    //console.log(enum_dict)
    return enum_dict
}

const ENUM_FUNC_NAMES = [
    "FuncNone",
    "FuncConst",
    "FuncRamp",
    "FuncTri",
    "FuncPulse",
    "FuncStep"
]

const ENUM_MODES_NAMES = [
    "MODES_CLEAR",
    "MODES_PLAIN",
    "MODES_SQUARE",
    "MODES_SICKLE",
    "MODES_FAN",
    "MODES_BOXES",
    "MODES_SICKLE_ADV",
    "MODES_FAN_ADV",
    "MODES_MAP_ES",
    "MODES_MAP_ES_ZH",
    "MODES_CMAP_DNA",
    "MODES_CMAP_FIRE",
    "MODES_CMAP_BENSON",
    "MODES_CMAP_YEN",
    "MODES_CMAP_LOVE",
    "MODES_CMAP_GEAR"
]

ENUM_MODES = createEnum(ENUM_MODES_NAMES);
ENUM_FUNC = createEnum(ENUM_FUNC_NAMES);

const ENG_MARK = ["XH", "XS", "XV", "YH", "YS", "YV", "X", "Y", "Z", "U", "V", "W"]

function stringify(content) {
    var s = ""
    s += "M" + ENUM_MODES[content.mode] + "S" + content.start_time + "D" + content.duration
    for (let i = 0; i < ENG_MARK.length / 2; i++) {
        var num1 = content[ENG_MARK[i]].func * 256 * 256 + content[ENG_MARK[i]].range * 256 + content[ENG_MARK[i]].lower
        var num2 = content[ENG_MARK[i]].p1 * 256 + content[ENG_MARK[i]].p2
        s += ENG_MARK[i + 6] + num1 + "," + num2
    }
    var num1 = content.p1 * 256 + content.p2
    var num2 = content.p3 * 256 + content.p4
    s += "P" + num1 + "," + num2 + ";"
    return s
}

app.use(express.static(__dirname + '/public'));

app.get("/get_effect", (req, res) => {
    //console.log(EffectMap[0].mode);
    var ID = req.query.id;
    if (ID >= Object.keys(EffectMap).length) {
        res.send("ERROR!!")
    }
    else {
        res.send(stringify(EffectMap[ID]))
    }

    //console.log(ID);
    //res.send(EffectMap);
})

app.get("/start", (req, res) => {
    //if (req.query.time !== undefined) {
    var time = req.query.time
    Time = time;
    res.send(time)
})

app.get("/esp_time", (req, res) => {
    var id = req.query.id
    var now = new Date();
    light_state[id] = now.getTime()
    light_effect[id] = req.query.effect
    var mode = (EXE_MODE == 0) ? "A" : "M"
    res.send(mode + Time.toString())
})

/*function state_update() {
    var now = new Date();
    for (var i = 0; i < 3; i++) {
        if (now.getTime() - light_state[i] > 5000) {
            light_state[i] = 0
        }
    }
}*/

app.get("/exe_mode", (req, res) => {
    EXE_MODE = req.query.mode
    res.send(EXE_MODE)
})

app.get("/get_stat", (req, res) => {
    var id = req.query.id
    res.send(light_state[id].toString());
})

app.get("/get_light", (req, res) => {
    var id = req.query.id
    res.send(light_effect[id].toString());
})

/*app.post('/fileupload', function (req, res) {
    var uploadedFile = req.files.uploadingFile;
    var tmpPath = uploadedFile.path;
    var targetPath = './' + uploadedFile.name;

    fs.rename(tmpPath, targetPath, function (err) {
        if (err) throw err;
        fs.unlink(tmpPath, function () {

            console.log('File Uploaded to ' + targetPath + ' - ' + uploadedFile.size + ' bytes');
        });
    });
    res.send('file upload is done.');
    res.end();
});*/
app.post('/fileupload', function (req, res) {
    var form = new formidable.IncomingForm();
    form.uploadDir = "./public/uploads"
    form.parse(req, function (err, fields, files) {
        //console.log("Parsing")
        var oldpath = files.file.path;
        var newpath = './public/uploads/' + files.file.name;
        fs.rename(oldpath, newpath, function (err) {
            if (err) throw err;
            res.write('File uploaded and moved!');
            res.end();
        });
    });
})

console.log(`Listening on port:${port} `);
app.listen(port);
