<?php

if($_GET['off'] != 1){
    echo "1";
}elseif(date('G') > 7 && date('G') < 19){
    echo "1";   // display enable
}else{
    echo "0";   // display enable
}

echo "\r";
echo "12345"; // request interval
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['temperature'] ?? 0, 1)).'/'.(round((json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['humidity'] ?? 0)*100, 0));
echo "\r";
echo date('D');
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/coin.json'), true)['last'] ?? 0, 2));
echo "\r";
echo "";
echo "\r";
echo date('M.d');
echo "\r";
echo json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['skycon'] ?? 0; 
