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
//echo date('D');
$exchange = json_decode(@file_get_contents('/dev/shm/exchangeratesapi.json'), true)['rates']['USD'] ?? 0; 
echo $exchange> 0 ? round(1/$exchange,3):0;
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/coin.json'), true)['last'] ?? 0, 2));
echo "\r";
$exchange = json_decode(@file_get_contents('/dev/shm/exchangeratesapi.json'), true)['rates']['HKD'] ?? 0; 
echo $exchange> 0 ? round(1/$exchange*10,2):0;
//echo "";
echo "\r";
//echo date('M.d');
$exchange = json_decode(@file_get_contents('/dev/shm/exchangeratesapi.json'), true)['rates']['JPY'] ?? 0; 
echo $exchange> 0 ? round(1/$exchange*100,3):0;
echo "\r";
echo json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['skycon'] ?? 0; 
