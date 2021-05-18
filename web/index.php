<?php

if($_GET['off'] != 1){
    echo "1";
}elseif(date('G') > 9 && date('G') < 21){
    echo "1";   // display enable
}else{
    echo "0";   // display disable
}

echo "\r";
echo "12345"; // request interval
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['temperature'] ?? 0, 1)).'/'.(round((json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['humidity'] ?? 0)*100, 0));
echo "\r";
$exchange = json_decode(@file_get_contents('/dev/shm/juhe_exchangerates.json'), true)['result'][0]['data1']['bankConversionPri'] ?? 0; 
echo round(10000/$exchange,3);
echo "\r";
$todayStock = json_decode(@file_get_contents('/dev/shm/juhe_stock.json'), true)['result']['nowpri'] ?? 0; 
echo round($todayStock,1);
echo "\r";
$incrPer = json_decode(@file_get_contents('/dev/shm/juhe_stock.json'), true)['result']['increPer'] ?? 0; 
echo round($incrPer,2)."%";
echo "\r";
$exchange = json_decode(@file_get_contents('/dev/shm/juhe_exchangerates.json'), true)['result'][0]['data4']['bankConversionPri'] ?? 0; 
echo round(100/$exchange,3);
echo "\r";
echo json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['skycon'] ?? 0; 

