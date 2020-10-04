<?php
function microtime_float()
{
    list($usec, $sec) = explode(" ", microtime());
    return ((float)$usec + (float)$sec);
}

echo date('h:i:s') . "<br>";
echo $_GET['time'] . "<br>";
//zend_extension 'my_extension'; 
// echo phpinfo();
//sleep for 5 seconds
//echo $m1 . "<br>";
//echo $m2 . "<br>";
$t = $_GET['time'];
//if (extension_loaded("my_extension")){
//        echo $t . "<br>";
//}
//$var = 1;
$path2exe = "/home/compassys/timeTest/test ";
$cmd = $path2exe . $t;
//echo $path2exe . "<br>";
//echo $cmd . "<br>";
$m1 = microtime_float(); // microtime();
exec($cmd,$outi,$exitc);
echo $exitc. "<br>";
//shell_exec($cmd); 
//echo my_function($t). "<br>";
$m2 = microtime_float(); //microtime();
//while(($m2 - $m1) < $t){
//      echo ($m2-$m1)  . "<br>";
//        $m2 = microtime_float();
//      $var = $var + rand(10,1000);
//}
echo ($m2-$m1)  . "<br>";
//echo $ret;
//sleep($_GET['time']);

//start again
//echo date('h:i:s');
return;
?>
~

