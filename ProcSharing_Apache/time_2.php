<?php
function microtime_float()
{
    //list($usec, $sec) = explode(" ", microtime());
    //return ((float)$usec + (float)$sec);
	return microtime(true);
}


//echo date('h:i:s') . "<br>";
//echo $_GET['time'] . "<br>";
$t = ($_GET['time']);//*1000000;
$tm = $t*1000000;

//$path2exe = "/home/compassys/timeTest/test ";
if($t<=0.003)
	return;
//else
//	$t = $t-0.003;

$arg = ($t*1000)/(0.00046); 
//echo $arg."<br>";
//if($arg>6000)
//	$arg = ($t*1000)/(0.0009); 

$m1 = microtime_float(); // microtime();
//echo $m1 . "<br>";

$cmd =  "/home/compassys/timeTest/test " . $arg;
shell_exec($cmd);//,$outi, $exitc);
$m3 = microtime_float(); // microtime();
//echo $m3 . "<br>";
echo ($m3-$m1);//  . "<br>";


//echo $path2exe . "<br>";
//echo $cmd . "<br>";

//$m3 = microtime_float(); // microtime();
//do //while($m3-$m1 < $t)
//{

//}while(($m3 -$m1) < $t);
//echo ($m3-$m1) . "<br>";
//echo $m3 . "<br>";

//echo $m2 . "<br>";
//echo $exitc. "<br>";
//echo $outi."<br>";
//$m2 = microtime_float(); //microtime();
//echo $m2 . "<br>";

return;
?>
