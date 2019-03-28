<?php
$json = json_decode(@file_get_contents('/dev/shm/home_weather.json'),true);

function getTemperature($json){
        if(!$json){
            return '0';
        }
        foreach ($json as $item){
            if($item['name'] === 'portable_temperature'){
                return ($item['temperature'][count($item['temperature'])-1]??0);
            }
        }
        return '0';
}
function getHumidity($json){
        if(!$json){
            return '0';
        }
        foreach ($json as $item){
            if($item['name'] === 'portable_humidity'){
                return ($item['humidity'][count($item['humidity'])-1]??0);
            }
        }
        return '0';
}

echo "1";
echo "\r";
echo "12345"; // request interval
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['temperature'] ?? 0, 1)).'/'.(round((json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['humidity'] ?? 0)*100, 0));
echo "\r";
echo date('D');
echo "\r";
echo round(getTemperature($json),1);
echo "\r";
echo "";
echo "\r";
echo date('M.d');
echo "\r";
echo json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['skycon'] ?? 0; 
