<?php

if(date('G') > 7 && date('G') < 20){
    echo "1";   // display enable
}else{
    echo "0";   // display enable
}

echo "\r";
echo "12345"; // request interval
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/weather.json'), true)['temp'] ?? 0, 1)).'/'.(round(json_decode(@file_get_contents('/dev/shm/juhe_weather.json'), true)['result']['data']['realtime']['weather']['humidity'] ?? 0, 0));
echo "\r";
echo date('D');
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/coin.json'), true)['last'] ?? 0, 2));
echo "\r";
echo "";
echo "\r";
echo date('M.d');
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/juhe_weather.json'), true)['result']['data']['realtime']['weather']['img'] ?? 0, 0));
