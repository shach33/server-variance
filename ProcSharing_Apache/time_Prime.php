<?php
function microtime_float()
{
	return microtime(true);
}

function getPrimes(){
	$num=1;
	$limit = 20;
	$primes = 0;
	for($num = 1;$num < $limit ;$num++){
		$i = 2;
		while($i <= $num){
			if($num%$i == 0)
				break;
			$i++;

		}
		if($i == $num)
			$primes++;
	}
}

function cmain($nLoop){
	$i = 0;
	while($i < $nLoop){
		getPrimes();
		$i++;
	}
}


//echo date('h:i:s') . "<br>";
//echo $_GET['time'] . "<br>";
$t = ($_GET['time']);//*1000000;
$tm = ($t*100)/(0.00078);


$m3 = microtime_float(); // microtime();
cmain($tm);
$m4 = microtime_float(); // microtime();
echo "<h1>".($m4-$m3). "</h1>";//  . "<br>";

return;
?>
