<?php
function microtime_float()
{
    list($usec, $sec) = explode(" ", microtime());
    return ((float)$usec + (float)$sec);
}


echo date('h:i:s') . "<br>";
echo $_GET['time'] . "<br>";
//sleep for 5 seconds
$m1 = microtime_float(); // microtime();
//echo $m1 . "<br>";
$m2 = microtime_float(); //microtime();
//echo $m2 . "<br>";
$t = ($_GET['time']);//*1000000;
//echo $t . "<br>";
while(($m2 - $m1) < $t){
//	echo ($m2-$m1)  . "<br>";
	$m2 = microtime_float();
}
	echo ($m2-$m1);//  . "<br>";
//sleep($_GET['time']);

//start again
//echo date('h:i:s');
return;
?>
~                                                                               
~                                                                               
~                                                                               
~                                                                               
~                                                                               
~                                                                               
~                
