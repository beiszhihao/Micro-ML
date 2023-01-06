const { autoUpdater } = require('electron');
var echarts = require('echarts');
var ipcRenderer = require('electron').ipcRenderer
var app = require('electron').app

var net = require('net');
var fs = require("fs")
const { isUint8Array } = require('util/types');
const { send } = require('process');
var socket = new net.Socket();
var is_cn = false;
var is_model = 0;
var exArray = [];
var myChart;
var option;
var data_arr = [];
var data_number = 0;
var mediaOpts = { video: { width: 640, height: 480 } };
var medstream = null;
var canvas;
var ctx;
var take_camera;

ipcRenderer.on('set_img_file_path', (event, data) => {
    var file = document.getElementById('image_path');
    if(data != '') {
        file.value = data;
    }
    wait_ld(false);

});

ipcRenderer.on('set_model_file_path', (event, data) => {
    var file = document.getElementById('model_file');
    if(data != '') {
        file.value = data;
    }
    wait_ld(false);

});

function close_window() {
    ipcRenderer.send("window-close");
}

function max_window() {
    ipcRenderer.send("window-max");
}

function min_window() {
    ipcRenderer.send("window-min");
}

function show_camera() {
    var oDiv = document.getElementById('camera');
    var oDivi = document.getElementById('image');
    var btCam = document.getElementById('bt_camera');
    var btImage = document.getElementById('bt_image');

    oDiv.style.opacity = 1;
    oDivi.style.opacity = 0;
    btCam.style.background = 'rgba(255, 255, 255, 0.5)';
    btImage.style.background = 'rgba(255, 255, 255, 0)';
    oDiv.style.zIndex = 4;
    oDivi.style.zIndex = 3;
    is_model = 1;

}

function show_image() {
    var oDiv = document.getElementById('camera');
    var oDivi = document.getElementById('image');
    var btCam = document.getElementById('bt_camera');
    var btImage = document.getElementById('bt_image');

    oDiv.style.opacity = 0;
    oDivi.style.opacity = 1;
    btCam.style.background = 'rgba(255, 255, 255, 0)';
    btImage.style.background = 'rgba(255, 255, 255, 0.5)';
    oDiv.style.zIndex = 3;
    oDivi.style.zIndex = 4;
    is_model = 0;
}

function messag_bt() {
    var oDiv = document.getElementById('cm');
    var mDiv = document.getElementById('ms');
    oDiv.style.opacity = 0;
    mDiv.style.opacity = 0;
    oDiv.style.pointerEvents = "none";

}

function log(value, tc) {
    document.getElementById("status").innerText = value;

    if(tc == true) {
        var oDiv = document.getElementById('cm');
        var mDiv = document.getElementById('ms');
        var mx = document.getElementById('m1');

        oDiv.style.opacity = 0.5;
        mDiv.style.opacity = 1;
        oDiv.style.pointerEvents = "auto";
        mx.innerText = value;

    }

}

function isValidIP(ip) {
    var reg = /^(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/
    return reg.test(ip);
} 

function wait_ld(l) {
    var oDiv = document.getElementById('cm');
    var mDiv = document.getElementById('ld');

    if(l == true) {
        oDiv.style.opacity = 0.5;
        mDiv.style.opacity = 1;
        oDiv.style.pointerEvents = "auto";

    } else {
        oDiv.style.opacity = 0;
        mDiv.style.opacity = 0;
        oDiv.style.pointerEvents = "none";

    }

}

function push_runtime(value) {
    let ti = document.getElementById('runt');

    if(data_number >= 10) {
        let value_runtime = 0;
        for(let i = 1; i < data_number; ++i) {
            value_runtime += parseInt(data_arr[i]);
        }

        value_runtime = parseInt(value_runtime) / parseInt(data_number - 1);
        ti.innerText = 'Average time consumption: ' + value_runtime + ' ms';
        data_number = 0;
        //data_arr = [];
    }
    if(data_number == 0) {
        data_arr[data_number++] = 0;
    }

    data_arr[data_number++] = value;
    option = {
        series: [{
                 type: 'line',
                 smooth: true,
                 data:data_arr
        }]
    };
    myChart.setOption(option);
    
}

function dr_rc(x1, y1, x2, y2) {
    let box = document.getElementById('bx');
    let Hc = 0;
    let Vc = 0;
    let left = 0;
    let top = 0;
    let width = 0;
    let height = 0;
    if(is_model == 0) {
        Hc = parseInt(400 / 56);
        Vc = parseInt(180 / 56);
        left = parseInt(x1 * Hc) + 160;
        top = parseInt(y1 * Vc) + 10;
        width = parseInt(x2 * 4) + 20;
        height = parseInt(y2 * Vc);
        if(180 - (height + top) < 0) {
            height -= ((height + top) - 180);
        }
    
        if(400 - (width + (left - 160)) < 0) {
            width -=  ((width + (left - 160)) - 400);
           
        }
    }
    //230、20、240、180
    if(is_model == 1) {
        Hc = parseInt(240 / 56);
        Vc = parseInt(180 / 56);
        left = parseInt(x1 * Hc) + 230;
        top = parseInt(y1 * Vc) + 20;
        width = parseInt(x2 * 3);
        height = parseInt(y2 * Vc);
        if(180 - (height + top) < 0) {
            height -= ((height + top) - 180);
        }
        if(240 - (width + (left - 230)) < 0) {
            width -=  ((width + (left - 230)) - 240);
           
        }

    }
   
    
   
    box.style.opacity = '1';
    box.style.left = left + 'px';
    box.style.top = top + 'px';
    box.style.width = width + 'px';
    box.style.height = height + 'px';
}

socket.on('data', (data)=>{
    let bs = data.toString();
    let cpuinfo = document.getElementById('cpuinfo');

    if(bs.substring(8, 12)== 'cpui') {
        let index1 = bs.indexOf('[');
        let index2 = bs.indexOf(']');
        let index3 = bs.indexOf('\n')   
        let text = 'Chip: ';
        text += bs.substring(index1 + 1, index3);
        text += '\n';
        text += 'Cores: ' ;
        text += bs.substring(index3 + 1, index2);
        cpuinfo.innerText = text;
    }
    if(bs.substring(8, 11) == 'roi') {
        let roi = document.getElementById('roi');
        let mx = document.getElementById('mx');
        let dx = document.getElementById('dx');
        var numArr = bs.substring(8, 256).match(/\d+/g);
        let roi_txt = 'ROI:[' + numArr[0] + ', ' + numArr[1] + ', ' + numArr[2] + ', ' + numArr[3] + ']';
        roi.innerText  = roi_txt;
        mx.innerText  = "";
        dx.innerText  = "";
        dr_rc(numArr[0], numArr[1], numArr[2], numArr[3]);
        push_runtime(numArr[4]);

    }

});

function send_data(cmd, end, f_size, t_size, data, data_length) {
    let buffer = new ArrayBuffer(1024);
    let d_cmd = new Uint8Array(buffer, 0, 1);
    let d_end = new Uint8Array(buffer, 1, 1);
    let d_fsize = new Uint16Array(buffer, 2, 1);
    let d_tsize = new Uint32Array(buffer, 4, 1);
    let d_data = new Uint8Array(buffer, 8);
    let packet = new Uint8Array(buffer, 0);

    d_cmd[0] = cmd;
    d_end[0] = end;
    d_fsize[0] = f_size;
    d_tsize[0] = t_size;
    if(data_length != 0) {
        for(let i = 0; i < data_length; ++i) {
            d_data[i] = data[i];
        }
    }

    //10.239.133.42
    socket.write(packet);
}

function send_file(cmd, file_path, model) {
   
    fs.readFile(file_path, function(err, data) {
        if(err) {
            return false;
        }

        let number = data.length / 1016;
        if(number.toString().indexOf('.') != -1) {
            number = parseInt(number + 1);
        }
        if(number == 0) {
            send_data(cmd, 0, data.length, data.length, data, data.length);
        }
        else {
            let tolat_len = 0;
            let end = 0;
            let cp_buf = new Uint8Array(1016);
            let cp_index = 0;
            let send_packet = new Uint8Array(data);
            for(let i = 0; i < number; ++i) {
                let d_len = 0;
    
                if((data.length - tolat_len) > 1016) {
                    d_len = 1016;
                }else {
                    d_len = data.length - tolat_len;
                }

                if (i == number - 1) {
                    end = 1;
                }

                tolat_len += d_len;
                cp_buf = [];

                for(let i = 0; i < d_len; ++i) {
                    cp_buf[i] = send_packet[cp_index];
                    cp_index++;
                }
                send_data(cmd, end, d_len, data.length, cp_buf, d_len);
                
            }
            if(model == 1) {
                send_data(3, 0, 0, 0, 0, 0);
                send_data(4, 0, 0, 0, 0, 0);
            }
        }
    })

    return true;
}

function start_image(path) {
    send_file(6, path, 1);
}

function connect_get() {
    let tip = document.getElementById('ip1');
    let tport = document.getElementById('port1');
    let tbt = document.getElementById('connect');
    let tse = document.getElementById('se');
    let ip1 = tip.value;
    let port = tport.value;

    if(tbt.innerText == 'Close') {
        if(is_cn == true) {
            socket.end();
            socket.destroy();
        }
        is_cn = false;
        tip.disabled = false;
        tport.disabled = false;
        tse.disabled = false;
        tbt.innerText = 'Connect'
        log('Close connect', false);
        return;
    }

    if(isValidIP(ip1) == false) {
        log('This is not a legal IP address', true);
        return;
    }

    if(port <= 0) {
        log('Invalid port number', true)
        return;
    }

    if (is_cn == true) {
        log('The network is connected, please do not connect again', true);
        return;
    }

    socket.connect(parseInt(port), ip1, () => {
        is_cn = true;
    });

    wait_ld(true);
    
    setTimeout(function () {
        wait_ld(false);
        if (is_cn == false) {
            log("connection timeout", true);
        } else {
            send_data(5, 0, 0, 0, 0, 0);
            tip.disabled = true;
            tport.disabled = true;
            tse.disabled = true;
            tbt.innerText = 'Close'
            log("connection succeeded", false);
        }

    }, 3000)

    /*
    sock.connect(parseInt(port), ip, () => {
        //sock.write('hello word');
    });
    sock.on('error', function(err){
        log('error');
    });
    */
}

function open_file() {
    wait_ld(true);
    ipcRenderer.send("openFile");

}

function init_echarts() {
    myChart = echarts.init(document.getElementById('runtime'), 'dark');
    option = {
        animationDuration: 10000,
        backgroundColor: 'transparent',
        tooltip: {},
        xAxis: {
                type: 'category',
                boundaryGap: false,
                data: ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']
            },
        yAxis:   [ { 
                    type : 'value',
                    scale : true,
                    max : 200,
                    min : 0,
                    splitNumber : 5,
                    boundaryGap : [ 0.2, 0.2 ]
            } ],
        series: [{
                 name: 'Sales',
                 type: 'line',
                 smooth: true,
                 symbol: 'none',
                 areaStyle: {},
                 data:[0]
                }]
               };
        myChart.setOption(option, true);

        canvas = document.getElementById('canvas');
        ctx = canvas.getContext('2d');
        canvas.style.opacity = '0';
}

function send_camera() {
            ctx.drawImage(video, 0, 0, 640, 480);
            var dataURL = canvas.toDataURL('image/jpeg');
            const base64 = dataURL.replace(/^data:image\/\w+;base64,/, '');
            let dataBuffer = new Buffer(base64, 'base64');
            let number = dataBuffer.length / 1016;

            if(number.toString().indexOf('.') != -1) {
                number = parseInt(number + 1);
            }
            if(number == 0) {
                send_data(cmd, 0, dataBuffer.length, dataBuffer.length, dataBuffer, dataBuffer.length);
            }
            let tolat_len = 0;
            let end = 0;
            let cp_buf = new Uint8Array(1016);
            let cp_index = 0;
            let send_packet = new Uint8Array(dataBuffer);
            
            for(let i = 0; i < number; ++i) {
                let d_len = 0;
    
                if((dataBuffer.length - tolat_len) > 1016) {
                    d_len = 1016;
                }else {
                    d_len = dataBuffer.length - tolat_len;
                }

                if (i == number - 1) {
                    end = 1;
                }

                tolat_len += d_len;
                cp_buf = [];

                for(let i = 0; i < d_len; ++i) {
                    cp_buf[i] = send_packet[cp_index];
                    cp_index++;
                }
                send_data(6, end, d_len, dataBuffer.length, cp_buf, d_len);
                
            }
            
            {
                send_data(3, 0, 0, 0, 0, 0);
                send_data(4, 0, 0, 0, 0, 0);
            }
            
            /*
            fs.writeFile('./test.jpg', dataBuffer, (err)=>{
                if(err){
                  console.log('拍照失败...')
                }else{
                  console.log('照片成功保存到本地，路径为：' + phototakePath)
                }
            });
            */
}

function start() {
    let image_path = document.getElementById('image_path');
    let ico = document.getElementById('ico1');
    let str = image_path.value;
    var st_bt = document.getElementById('st');
    let mx = document.getElementById('mx');
    let dx = document.getElementById('dx');
    let roi = document.getElementById('roi');
    let box = document.getElementById('bx');

    if(st_bt.innerText == 'Stop') {
        video.pause();
        medstream.stop();
        clearInterval(take_camera);
        st_bt.innerText = 'Start';
        return;
    }
    
    if(is_cn == false) {
        log('Device not connected', true);
        return;
    }
    
    box.style.opacity = '0';

    if(is_model == 0) {
        if (str == ''){
            log('No picture file selected', true);
            return;
        }
        let box = document.getElementById('bx');
        box.style.left = '160px';
        box.style.top = '10px';
        box.style.width =  '0px';
        box.style.height = '0px';
        box.style.opacity = '0';
        str = str.replaceAll('\\','/');
        start_image(str);
        video.style.opacity = '0';
        ico.style.opacity = '1';
        ico.style.backgroundImage = 'url(' + str + ')';
    }

    if(is_model == 1) {
        var sel = document.getElementById('se_c');
        var index = sel.selectedIndex;
        if(index < 0) {
            log('Please select a camera first', true);
            return;
        }

        mediaOpts ={
            video:
            {
                deviceId: { exact: exArray[index+1] }
            }
        };
        let video = document.getElementById('video');
        function success(stream) {
            medstream = stream.getTracks()[0];
            st_bt.innerText = 'Stop';
            ico.style.opacity = '0';
            video.style.opacity = '1';
            roi.innerText = 'ROI:[]';
            mx.style.opacity = '0';
            dx.style.opacity = '0';
            video.srcObject = stream;
            video.play();
            take_camera = setInterval("send_camera()",100);
            /*
            var canvas = document.getElementById('canvas');
            var ctx = canvas.getContext('2d');
            ctx.drawImage(video, 0, 0, 640, 480);
            var dataURL = canvas.toDataURL('image/jpeg');
            //const base64 = dataURL.replace(/^data:image\/\w+;base64,/, '');
            let dataBuffer = new Buffer(base64, 'base64');
            fs.writeFile('./test.jpg', dataBuffer, (err)=>{
                if(err){
                  console.log('拍照失败...')
                }else{
                  console.log('照片成功保存到本地，路径为：' + phototakePath)
                }
            });
            */

        }
        function error(error) {
            console.log(`访问用户媒体设备失败${error.name}, ${error.message}`);
        }
        getUserMedia(mediaOpts, success, error);
    }

}

function getUserMedia(constraints, success, error) {
    if (navigator.mediaDevices.getUserMedia) {
        //new api
        navigator.mediaDevices.getUserMedia(constraints).then(success).catch(error);
    } else if (navigator.webkitGetUserMedia) {
        //webkit
        navigator.webkitGetUserMedia(constraints, success, error)
    } else if (navigator.mozGetUserMedia) {
        //firfox
        navigator.mozGetUserMedia(constraints, success, error);
    } else if (navigator.getUserMedia) {
        //old api
        navigator.getUserMedia(constraints, success, error);
    }
}

function retrieval_camera() {
    let se_c = document.getElementById('se_c');
    exArray = [];

    wait_ld(true);
    if (navigator.mediaDevices.getUserMedia || navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia) {
        if (!navigator.mediaDevices || !navigator.mediaDevices.enumerateDevices) {
            wait_ld(false);
            log('Unable to enumerate devices', true);
            return;
        }

        navigator.mediaDevices.enumerateDevices()
        .then(function (devices) {
            devices.forEach(function (device) {
               if (device.kind == "videoinput") {
                   se_c.options.add(new Option(device.label, 'text'));
                   exArray.push(device.deviceId);
               }
           });
        })
        .catch(function (err) {
            log(err.name + ": " + err.message, true);
        });

    }else {
        wait_ld(false);
        log('Can\'t access this PC device', true);
        return;
    }

    setTimeout(function () {
        wait_ld(false);
    }, 1000)

}

function select_model() {
    wait_ld(true);
    ipcRenderer.send("openModel");
}

function upload_model() {
    let model_path = document.getElementById('model_file');

    if(is_cn == false) {
        log('Please connect to the device first', true);
        return;
    }
    if(model_path.value.length == 0) {
        log('Please select a model file', true);
        return;
    }

    //wait_ld(true);
    if(send_file(1, model_path.value, 0) == false) {
        log('Model open failed or transfer disconnected', true);
        return;
    }

    //wait_ld(false);
    log('Model transmission completed', true);

}