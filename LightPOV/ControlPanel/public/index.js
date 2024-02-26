const NUM_OF_LUX = 3
//const ENUM = { "EFFECT_ES1": 0, "EFFECT_ES2": 1 };
//const ENUM = { "MODES_SQUARE": 0, "MODES_SICKLE": 1, "MODES_FAN": 2 }
var NUM_OF_EFFECT

$(document).ready(function () {
    table_append()
    create_effect_table()
    setInterval(timeupdate, 200);
    setInterval(stat_update, 500);
    setInterval(checkbox_update, 500);
});


function createEnum(name_list) {
    enum_dict = {};
    id = 0
    name_list.forEach((e) => {
        enum_dict[e] = id;
        id += 1
    })
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

function timeupdate() {
    var today = new Date();
    $("#nowtime").html(today);
}
function checkbox_update() {
    if ($('#auto').is(":checked")) $.get("/exe_mode?mode=1");
    else $.get("/exe_mode?mode=0");
}
function stat_update() {
    for (var i = 0; i < NUM_OF_LUX; i++) {
        const dd = i
        $.get("/get_stat?id=" + dd).success(function (data) {
            var id = "#" + (dd + 1).toString() + "-state"
            var now = new Date();
            if (now - data < 5000) {
                $(id).html("已連線")
                id = "#" + (dd + 1).toString() + "-last"
                $(id).html("")
            }
            else {
                $(id).html("斷線")
                id = "#" + (dd + 1).toString() + "-last"
                $(id).html((now - data) / 1000)
            }
        });
        $.get("/get_light?id=" + dd).success(function (data) {
            var id = "#" + (dd + 1).toString() + "-light"
            var lig = getEnumKey(ENUM_MODES, data)
            $(id).html(lig.substring(6))
        });
    }

}
var i;
function table_append() {
    var $myTable = $('#table1');
    if (NUM_OF_LUX == 0) $myTable.html("")
    for (i = 0; i < NUM_OF_LUX; i++) {
        var rowElements = function (row) {
            var $row = $('<tr class="tr1"></tr>');

            var n = (i + 1).toString();
            var $col_1 = $('<td class="td1"></td>').html(n);
            var $col_2 = $('<td class="td1" id="' + n + '-state"></td>')
            var $col_3 = $('<td class="td1" id="' + n + '-light"></td>')
            var $col_4 = $('<td class="td1" id="' + n + '-last"></td>')
            /*var $checkbox_control = '<input type="checkbox" class="checkbox" id="' + n + '-checkbox" value="blue">'
            var $col_5 = $('<td class="td1"></td>').html($checkbox_control);
            var $bright = '<output id="' + n + '-b-val"></output><input type="range" id="' + n + '-brightness" min="0" max="100">'
            var $col_6 = $('<td class="td1"></td>').html($bright);
            var $speed = '<output id="' + n + '-s-val"></output><input type="range" id="' + n + '-speed" min="0" max="100">'
            var $col_7 = $('<td class="td1"></td>').html($speed);
            var $color = '<input type="color" id="' + n + '-color">'
            var $col_8 = $('<td class="td1"></td>').html($color);
            var $checkbox_auto = '<input type="checkbox" class="checkbox" id="' + n + '-auto" value="blue">'
            var $col_9 = $('<td class="td1"></td>').html($checkbox_auto);*/


            // Add the columns to the row
            //$row.append($col_1, $col_2, $col_3, $col_4, $col_5, $col_6, $col_7, $col_8, $col_9);
            $row.append($col_1, $col_2, $col_3, $col_4);

            // Add to the newly-generated array
            return $row;
        };

        // Finally, put ALL of the rows into your table
        $myTable.append(rowElements);
    }

}
function getEnumKey(enum_dict, id) {
    return Object.keys(enum_dict)[id]
}
function create_effect_table() {
    $.getJSON("test.json", function (data) {
        //$.getJSON("FormozaWorldNow.json", function (data) {
        //console.log(data)
        var len = Object.keys(data).length
        NUM_OF_EFFECT = len
        //console.log(len)
        var $myTable = $('#table3');
        for (var i = 0; i < len; i++) {
            var rowElements = function (row) {
                var $row = $('<tr class="tr3"></tr>');

                //var n = (i + 1).toString();
                var n = i.toString();
                var ID = '<td class="td3" id="effect-' + n + '"></td>'
                var $col_1 = $(ID).html(n);
                var $col_2 = $('<td class="td3"></td>').html(data[i].mode.substring(6))
                var time = '<td class="td3" id="effect-time-' + n + '"></td>'
                var $col_3 = $(time).html(data[i].start_time)
                var $col_4 = $('<td class="td3"></td>').html(data[i].duration)
                var $col_5 = $('<td class="td3"></td>').html(getEnumKey(ENUM_FUNC, data[i].XH.func).substring(4));
                var $col_6 = $('<td class="td3"></td>').html(data[i].XH.range);
                var $col_7 = $('<td class="td3"></td>').html(data[i].XH.lower);
                var $col_8 = $('<td class="td3"></td>').html(data[i].XH.p1);
                var $col_9 = $('<td class="td3"></td>').html(data[i].XH.p2);
                var $col_10 = $('<td class="td3"></td>').html(getEnumKey(ENUM_FUNC, data[i].XS.func).substring(4));
                var $col_11 = $('<td class="td3"></td>').html(data[i].XS.range);
                var $col_12 = $('<td class="td3"></td>').html(data[i].XS.lower);
                var $col_13 = $('<td class="td3"></td>').html(data[i].XS.p1);
                var $col_14 = $('<td class="td3"></td>').html(data[i].XS.p2);
                var $col_15 = $('<td class="td3"></td>').html(getEnumKey(ENUM_FUNC, data[i].XV.func).substring(4));
                var $col_16 = $('<td class="td3"></td>').html(data[i].XV.range);
                var $col_17 = $('<td class="td3"></td>').html(data[i].XV.lower);
                var $col_18 = $('<td class="td3"></td>').html(data[i].XV.p1);
                var $col_19 = $('<td class="td3"></td>').html(data[i].XV.p2);
                var $col_20 = $('<td class="td3"></td>').html(getEnumKey(ENUM_FUNC, data[i].YH.func).substring(4));
                var $col_21 = $('<td class="td3"></td>').html(data[i].YH.range);
                var $col_22 = $('<td class="td3"></td>').html(data[i].YH.lower);
                var $col_23 = $('<td class="td3"></td>').html(data[i].YH.p1);
                var $col_24 = $('<td class="td3"></td>').html(data[i].YH.p2);
                var $col_25 = $('<td class="td3"></td>').html(getEnumKey(ENUM_FUNC, data[i].YS.func).substring(4));
                var $col_26 = $('<td class="td3"></td>').html(data[i].YS.range);
                var $col_27 = $('<td class="td3"></td>').html(data[i].YS.lower);
                var $col_28 = $('<td class="td3"></td>').html(data[i].YS.p1);
                var $col_29 = $('<td class="td3"></td>').html(data[i].YS.p2);
                var $col_30 = $('<td class="td3"></td>').html(getEnumKey(ENUM_FUNC, data[i].YV.func).substring(4));
                var $col_31 = $('<td class="td3"></td>').html(data[i].YV.range);
                var $col_32 = $('<td class="td3"></td>').html(data[i].YV.lower);
                var $col_33 = $('<td class="td3"></td>').html(data[i].YV.p1);
                var $col_34 = $('<td class="td3"></td>').html(data[i].YV.p2);
                var $col_35 = $('<td class="td3"></td>').html(data[i].p1);
                var $col_36 = $('<td class="td3"></td>').html(data[i].p2);
                var $col_37 = $('<td class="td3"></td>').html(data[i].p3);
                var $col_38 = $('<td class="td3"></td>').html(data[i].p4);

                // Add the columns to the row
                $row.append($col_1, $col_2, $col_3, $col_4, $col_5, $col_6, $col_7, $col_8, $col_9, $col_10);
                $row.append($col_11, $col_12, $col_13, $col_14, $col_15, $col_16, $col_17, $col_18, $col_19, $col_20);
                $row.append($col_21, $col_22, $col_23, $col_24, $col_25, $col_26, $col_27, $col_28, $col_29, $col_30);
                $row.append($col_31, $col_32, $col_33, $col_34, $col_35, $col_36, $col_37, $col_38);

                // Add to the newly-generated array
                return $row;
            };

            $myTable.append(rowElements);
        }
    });

}
var music_id = document.getElementById("music")
document.getElementById("music").ontimeupdate = function () {
    $.get("/start?time=" + music_id.currentTime.toFixed(3) * 1000);
    $('#music-time').html(music_id.currentTime.toFixed(2))
    for (var i = 0; i < NUM_OF_EFFECT - 1; i++) {
        var time1 = "#effect-time-" + i.toString()
        var time2 = "#effect-time-" + (i + 1).toString()
        var value1 = parseInt($(time1).text())
        var value2 = parseInt($(time2).text())
        var ID = "#effect-" + i
        if (value1 < (music_id.currentTime.toFixed(3) * 1000) && (value2 > (music_id.currentTime.toFixed(3) * 1000))) {
            $(ID).css("background-color", "red")
        }
        else (
            $(ID).css("background-color", "white")
        )
    }

}

document.getElementById("music-play").onclick = function () { //音樂播放按鈕
    document.getElementById("music").play();
}
document.getElementById("music-pause").onclick = function () { //音樂暫停按鈕
    document.getElementById("music").pause();
}
document.getElementById("music-restart").onclick = function () { //音樂重頭開始按鈕
    document.getElementById("music").currentTime = 0;
}
