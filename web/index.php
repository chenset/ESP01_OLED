<?php

if(date('G') > 7 && date('G') < 23){
    echo "1";   // display enable
}else{
    echo "0";   // display enable
}

echo "\r";
//echo "12345"; // request interval
echo "3345"; // request interval
echo "\r";
echo 'Btc';
echo "\r";
echo date('l');
echo "\r";
echo (round(json_decode(@file_get_contents('/dev/shm/coin.json'), true)['last'] ?? 0, 2));
echo "\r";
echo "";
echo "\r";
echo date('M.d');