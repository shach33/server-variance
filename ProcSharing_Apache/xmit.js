<!--

function sleep(delay) {
	var start = new Date().getTime();
	while (new Date().getTime() < start + delay);
}
//$(document).ready(function(){
//$(document).onload(function(){
//    $("button").click(function(){
//var servTime = new Array;
function sendReqs(){
	var ind = 0;
	var tt = 0;
	var indIAT = 0;
	var sumS = 0;
	while(ind < 10){
		var pFile = "parNums" + ind + ".txt";
		//$.get('parNums.txt', function(data){
		//console.log("reading files:" + ind);
		(function(ind)
		 {
		 $.get(pFile, function(data){
			 //console.log("parNums read");                        
			 eFile = "IAT" + ind + ".txt";
			 TservTime = data.split('\n');
			 //console.log("Reading iat: " +ind);
			 $("#div1").html(TservTime.length);
			 (function(servTime)
			  {       
			  $.get(eFile, function(iat){
				  iaTime = iat.split('\n');
				  i = 0;
				  var datetime = new Date();
				  //console.log("iaTime Length: " + iaTime.length);
				  //console.log("datetime: " + datetime.getTime());
				  //do{  
				 //	j =(new Date().getTime() - datetime.getTime());
				//	console.log("j:: " + j); 
				 //  }while((new Date().getTime() - datetime.getTime()) < iaTime[i]); 
				  
				  while(i < (servTime.length-1)){
				  $("#div4").html(servTime.length);
				  tm = servTime[i];
				  initTime = datetime.getTime() + parseFloat(iaTime[i]);//  parseFloat(tempIAT[i]);
				  //console.log(servTime);
				  //var r;
				  (function(tmS,it)//tempIAT)//i, tempServe, tempIAT)
				  {
				  //tm = tempServe[i];     
				  //sumS += parseFloat(tm); 
				  //console.log("timeiF:: "  + initTime);
				  $.ajax({
					url: 'time.php',
					type : 'GET',
				  	//data: {time:5},
					data: {time:tmS},
					success: function(result){
					dt = new Date();        
					t1 = dt.getTime() -it;// initTime;//(datetime.getTime() + iaTime[i]);
					tt = tt + t1;// (dt.getTime() - (datetime.getTime() + iaTime[i]));
				  	//console.log("total resp time: " + tt);
				  	//console.log("resp time for " + i + " : " +t1);
				  	//console.log("dt.getTime(): " +dt.getTime() + "- " + it);
				  	$("#div3").html(tt);
				  	//$("#div5").html(sumS);
				  	//console.log("Response time: " + tt);
					//console.log("sum of service time: " + sumS);
				  	}
				  });
					
				  })(tm,initTime);//iaTime);//(i,servTime,iaTime);
				  i++;

				  //do{
				  //	h = (new Date().getTime() - datetime.getTime());
				//	console.log(h + "---" + iaTime[i]);
				 // } while(1);//h < parseFloat(iaTime[i]));// iaTime[i]); 
				  while((new Date().getTime() - datetime.getTime()) < parseFloat(iaTime[i]));// iaTime[i]); 
				  }

			  });
			  })(TservTime);
		 });
		 })(ind);
		ind++;
	}
	//        });
	//});
}
-->
