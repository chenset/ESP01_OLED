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


echo "1";   // display enable
echo "\r";
echo "52345"; // request interval
echo "\r";
echo round(getTemperature($json),1).'/'.round(getHumidity($json));
echo "\r";
echo "";
echo "\r";
echo "";
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/juhe_weather.json'), true)['result']['data']['realtime']['weather']['img'] ?? 0, 0));
echo "\r";
//echo date('M.d');
echo (round(json_decode(@file_get_contents('/dev/shm/weather.json'), true)['temp'] ?? 0, 1)).'/'.(round(json_decode(@file_get_contents('/dev/shm/juhe_weather.json'), true)['result']['data']['realtime']['weather']['humidity'] ?? 0, 0));

