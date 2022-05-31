<?php

if($_GET['off'] != 1){
    echo "1";
}elseif(date('Gi') > 855 && date('G') < 22){
    echo "1";   // display enable
}else{
    echo "0";   // display disable
}

$sina = explode(',', trim(@file_get_contents('/dev/shm/sina_stock.js')??''));

$SH50ETF_earn = empty($sina[6]) ? 0: ( ($sina[6]-2.706)*37000 + ($sina[6]-2.739)*10900 + ($sina[6]-2.716)*25700 - (41.88+60.07+17.91) + 10.9 );

$SH50ETF_amount = 69843.08 - 41.88 + 100182.07 - 60.07 + 29862.11 - 17.91;


echo "\r";
echo "12345"; // request interval
echo "\r";
#echo (round(json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['temperature'] ?? 0, 1)).'/'.(round((json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['humidity'] ?? 0)*100, 0));
echo round($sina[6] ?? 0, 3);
echo "\r";
#$exchange = json_decode(@file_get_contents('/dev/shm/juhe_exchangerates.json'), true)['result'][0]['data1']['bankConversionPri'] ?? 0; 
#echo round(10000/$exchange,3);
$sina = explode(',', trim(@file_get_contents('/dev/shm/sina_stock.js')??''));
#echo  isset($sina[8]) ?( round($sina[8] ?? 0, 1)."/".round((($sina[6] ?? 0)/2.706-1)*100, 1)  ):0;
echo  isset($sina[8]) ?( round($sina[8] ?? 0, 1)."/".round(    $SH50ETF_earn/$SH50ETF_amount*100   , 1)  ):0;
echo "\r";
$sina = explode(',', trim(@file_get_contents('/dev/shm/sina_stock.js')??''));
echo round($sina[1] ?? 0, 1);
#$todayStock = json_decode(@file_get_contents('/dev/shm/juhe_stock.json'), true)['result']['nowpri'] ?? 0; 
#echo round($todayStock,1);
echo "\r";
echo round($sina[3] ?? 0, 2).'%';
#$incrPer = json_decode(@file_get_contents('/dev/shm/juhe_stock.json'), true)['result']['increPer'] ?? 0; 
#echo round($incrPer,2)."%";
echo "\r";
#$exchange = json_decode(@file_get_contents('/dev/shm/juhe_exchangerates.json'), true)['result'][0]['data4']['bankConversionPri'] ?? 0; 
#echo round(100/$exchange,3);
#$sina = explode(',', trim(@file_get_contents('/dev/shm/sina_stock.js')??''));
#echo round($sina[6] ?? 0, 3);
#echo (round(json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['temperature'] ?? 0, 1)).'/'.(round((json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['humidity'] ?? 0)*100, 0));
echo empty($sina[6]) ? "-": round($SH50ETF_earn,1);
echo "\r";
echo json_decode(@file_get_contents('/dev/shm/caiyun_realtime_weather.json'), true)['result']['skycon'] ?? 0; 
echo "\r";
echo time();

