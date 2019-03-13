var shell = require('shelljs');
var fs = require('fs');
var AipSpeechClient = require("baidu-aip-sdk").speech;
// 新建一个对象，建议只保存一个对象调用服务接口
const conf = require('./conf.js');
var client = new AipSpeechClient(conf.id, conf.ak, conf.sk);

exports.play = function (text,vol) {
    var v = 10;
    if(vol){
        v = vol;
    }
    client.text2audio(text, { spd: 5, per: 0 ,vol: v}).then(function (result) {
        if (result.data) {
            var time = (new Date()).getTime();
            fs.writeFileSync(`${time}.mp3`, result.data);
            shell.exec(`sox ${time}.mp3 ./wavs/${time}.wav`, function (code, stdout, stderr) {
                console.log('Exit code:', code);
                console.log('Program output:', stdout);
                console.log('Program stderr:', stderr);
                shell.rm('-rf',`${time}.mp3`);
                shell.exec(`aplay ./wavs/${time}.wav -Dplughw:1,0`, function (code, stdout, stderr) {
                    console.log('Exit code:', code);
                    console.log('Program output:', stdout);
                    console.log('Program stderr:', stderr);
                });
            });
        } else {
            // 服务发生错误
            console.log(result);
        }
    }, function (e) {
        // 发生网络错误
        console.log(e);
    });
}
