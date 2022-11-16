var variantsInfo;
var allVariants;


function swipeRemoveRequest() {
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", swipeRemoveListener);
	oReq.open("GET", frontend+"swipeRemoveAction", false);
	oReq.send();
}

function swipeRemoveListener () {
	var dates = [];
	var response=this.responseText;
	var arrayResponse=response.split("£")
	document.getElementById("digraphF").innerHTML = arrayResponse[0];
	document.getElementById("digraphP").innerHTML = arrayResponse[1];
	//document.getElementById("variants_alternative").innerHTML = arrayResponse[3];
	change_filter_type()
	
	
	//var response = arrayResponse[2];
	if (redo_function==true|| filtered_timeframe == true || filtered_perf == true || filtered_attribute == true) { // 
		/*
		document.getElementById("digraphF").innerHTML = response.split("|||")[0];
		document.getElementById("digraphP").innerHTML = response.split("|||")[1];
		variantsInfo = response.split("|||")[2];
		*/
		// console.log(variantsInfo)
		
	}
	else{
		
		//variantsInfo = response;
	}
	/*
	// console.log(variantsInfo)
	json = JSON.parse(variantsInfo);
	// console.log("Total response log: "+json);

	allVariants = Object.keys(json);
	// console.log("Total variants: " +String(allVariants.length));
	//allVariants.lenght = Number of variants
	//Object.values(allVariants) = array with Variants ID

	var partial_trace_number=0
	var partial_event_number=0
	var partial_max_event_length=0
	var partial_min_event_length=Number.MAX_SAFE_INTEGER

	for(var i=0; i<allVariants.length; i++){
		//console.log(allVariants[i])
		partial_trace_number=partial_trace_number+json[allVariants[i]].length

		for(var j=0; j<json[allVariants[i]].length; j++){
			var chiave=Object.keys(json[allVariants[i]][j])
			// console.log(chiave)
			// console.log(json[allVariants[i]][j][chiave].length)
			partial_event_number=partial_event_number+json[allVariants[i]][j][chiave].length
			
			if(partial_max_event_length<json[allVariants[i]][j][chiave].length){
				partial_max_event_length=json[allVariants[i]][j][chiave].length
			}

			if(partial_min_event_length>json[allVariants[i]][j][chiave].length){
				partial_min_event_length=json[allVariants[i]][j][chiave].length
			}
		}
	}

	var partial_avarage_trace_length=partial_event_number/partial_trace_number;
	
	$("#log_detail").text("");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of traces: "+"<span style='color:blue;'>"+partial_trace_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of events: "+"<span style='color:blue;'>"+partial_event_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Avarage trace length: "+"<span style='color:blue;'>"+partial_avarage_trace_length.toFixed(2) +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Max trace length: "+"<span style='color:blue;'>"+partial_max_event_length+"</span>"+"</div> <br>");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Min trace length: "+"<span style='color:blue;'>"+partial_min_event_length+"</span>"+"</div> <br>");
	// $("#log_detail").append("Number of activities involved: "+"&#13;&#10;" +"&#13;&#10;");
	
	// console.log("number of trace: "+partial_trace_number)
	// console.log("number of event: "+partial_event_number)
	// console.log("max event: "+partial_max_event_length)
	// console.log("min event: "+partial_min_event_length)

	
	var percentage__trace=partial_trace_number*100/total_trace_number
	// console.log(percentage__trace)
	$("#percentage_trace").text("Cases: "+percentage__trace.toFixed(2)+"%")
	var percentage__event=partial_event_number*100/total_event_number
	// console.log(percentage__event)
	$("#percentage_event").text("Event: "+percentage__event.toFixed(2)+"%")


	ul1 = document.getElementById("variants-ul");
	ul2 = document.getElementById("case-ul");


	ul1.innerHTML = "";
	ul2.innerHTML = "";
	document.getElementById("eventsTable").innerHTML = "";
	Object.keys(json).forEach((key1) => {

		//----For each variant I see which are the cases corresponding
		//console.log("Variant " + String(key1));
		allCases = json[key1];
		//----allCases.length = Number of cases for each specific Variant (json[key])
		//console.log("Total cases: " + String(allCases.length));
		//console.log(allCases);


		var li = document.createElement('li');
		li.innerHTML = "<span class='text-table' style='border-radius: 4px;'>" + "Variant " + key1 + "</span><br><small class='text-table' style='border-radius: 4px;'>" + "Cases: " + String(allCases.length) + '</small>';
		li.setAttribute("style", "cursor: pointer");
		li.setAttribute("id", "variant"+key1);
		//li.setAttribute("onclick", "alert('Variant "+ key1 +"')");
		li.setAttribute("onclick", "showCases("+li.getAttribute('id')+"," +key1+")");
		ul1.append(li);	

		// calculate min and max dates
			
		allCases.forEach( (c) => {
			allEvents = Object.values(c)[0]; // all events
			allEvents.forEach((e) => {
				dates.push(new Date(e["time:timestamp"]));
			});
		});
			

	});
	
  	if(document.getElementById(selected)!=null){
		document.getElementById(selected).click();
	}
	
	
	maxDate = new Date(Math.max.apply(null,dates));
	minDate = new Date(Math.min.apply(null,dates));
	maxDate = maxDate.toISOString().split(".")[0];
	minDate = minDate.toISOString().split(".")[0];


	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Minimun date: "+"<br> <span style='color:blue;'>"+minDate.replace("T", " ")+"</span>"+"</div> <br>");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Maximun date: "+"<br> <span style='color:blue;'>"+maxDate.replace("T", " ")+"</span>"+"</div>");
	*/
	
}

function initialVariantRequest(){
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", initialVariantListener2);
	oReq.open("GET", frontend+"initialVariantAction", false);
	oReq.send();

}

function initialVariantListener2(){
	var response = this.responseText;
	var dates = [];
	//console.log(response)
	/*
	if (redo_function==true|| filtered_timeframe == true || filtered_perf == true || filtered_attribute == true) { // 
	
		document.getElementById("digraphF").innerHTML = response.split("|||")[0];
		document.getElementById("digraphP").innerHTML = response.split("|||")[1];
		variantsInfo = response.split("|||")[2];
		console.log(variantsInfo)
		document.getElementById("variants_info").innerHTML=variantsInfo
	}
	else{
		*/
	//console.log("i am here")
	variantsInfo = response;
	document.getElementById("variants_info").innerHTML=variantsInfo
	//}

	
	json = JSON.parse(variantsInfo);
	console.log(json)
	
	allVariants = Object.keys(json);

	dictionaryVariantName={}
	//console.log(dictionaryVariantName)
	
	
	partial_trace_number=0
	partial_event_number=0
	partial_max_event_length=0
	partial_min_event_length=Number.MAX_SAFE_INTEGER


	for(var i=0; i<allVariants.length; i++){
		dictionaryVariantName[allVariants[i]]=i
		//console.log(allVariants[i])
		//console.log(json[allVariants[i]].length)
		partial_trace_number=partial_trace_number+json[allVariants[i]].length

		
		for(var j=0; j<json[allVariants[i]].length; j++){
			var chiave=Object.keys(json[allVariants[i]][j])
			//console.log(chiave)
			
			//console.log(json[allVariants[i]][j]["events"].length)
			
			partial_event_number=partial_event_number+json[allVariants[i]][j]["events"].length
			
			if(partial_max_event_length<json[allVariants[i]][j]["events"].length){
				partial_max_event_length=json[allVariants[i]][j]["events"].length
			}

			if(partial_min_event_length>json[allVariants[i]][j]["events"].length){
				partial_min_event_length=json[allVariants[i]][j]["events"].length
			}
			
		}
		
	}

	partial_avarage_trace_length=partial_event_number/partial_trace_number;

	$("#log_detail").text("");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of traces: "+"<span style='color:blue;'>"+partial_trace_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of events: "+"<span style='color:blue;'>"+partial_event_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Avarage trace length: "+"<span style='color:blue;'>"+partial_avarage_trace_length.toFixed(2) +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Max trace length: "+"<span style='color:blue;'>"+partial_max_event_length+"</span>"+"</div> <br>");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Min trace length: "+"<span style='color:blue;'>"+partial_min_event_length+"</span>"+"</div> <br>");

	if(start_iniziale==0){
		total_trace_number=partial_trace_number
		total_event_number=partial_event_number
		avarage_trace_length=partial_avarage_trace_length
		max_event_length=partial_max_event_length
		min_event_length=partial_min_event_length
	}
	start_iniziale=1

	var percentage__trace=partial_trace_number*100/total_trace_number
	$("#percentage_trace").text("Cases: "+percentage__trace.toFixed(2)+"%")
	
	var percentage__event=partial_event_number*100/total_event_number
	$("#percentage_event").text("Event: "+percentage__event.toFixed(2)+"%")


	//*********/
	ul1 = document.getElementById("variants-ul");
	ul2 = document.getElementById("case-ul");

	ul1.innerHTML = "";
	ul2.innerHTML = "";
	document.getElementById("eventsTable").innerHTML = "";

	//console.log(dictionaryVariantName)

	Object.keys(json).forEach((key1) => {

		//----For each variant I see which are the cases corresponding
		//console.log("Variant " + String(key1));
		allCases = json[key1];
		//----allCases.length = Number of cases for each specific Variant (json[key])
		//console.log("Total cases: " + String(allCases.length));
		//console.log(allCases);

		
		var li = document.createElement('li');
		li.innerHTML = "<span class='text-table' style='border-radius: 4px;'>" + "Variant " + dictionaryVariantName[key1] + "</span><br><small class='text-table' style='border-radius: 4px;'>" + "Cases: " + String(allCases.length) + '</small>';
		li.setAttribute("style", "cursor: pointer");
		li.setAttribute("id", "variant"+dictionaryVariantName[key1]);
		//li.setAttribute("onclick", "alert('Variant "+ key1 +"')");
		
		li.setAttribute("onclick", "showCases2("+li.getAttribute('id')+"," +dictionaryVariantName[key1]+")");
		ul1.append(li);	
		
		// calculate min and max dates
			
		allCases.forEach( (c) => {
			allEvents = Object.values(c)[1]; // all events
			//console.log(allEvents)
			allEvents.forEach((e) => {
				dates.push(new Date(e["time:timestamp"]));
			});
		});
		

	});

	dictionaryVariantNameReverse = swap(dictionaryVariantName);
	//console.log(dictionaryVariantNameReverse)
	
  	if(document.getElementById(selected)!=null){
		document.getElementById(selected).click();
	}
	
	
	
	maxDate=new Date(Math.max.apply(null,dates));
	minDate=new Date(Math.min.apply(null,dates));
	
	if(maxDate instanceof Date && !isNaN(maxDate.valueOf())){
		maxDate = maxDate.toISOString().split(".")[0];
		$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Maximun date: "+"<br> <span style='color:blue;'>"+maxDate.replace("T", " ")+"</span>"+"</div>");
	
	}else{
		console.log("invalid date")
	}
	if(minDate instanceof Date && !isNaN(minDate.valueOf())){
		minDate = minDate.toISOString().split(".")[0];
		$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Minimun date: "+"<br> <span style='color:blue;'>"+minDate.replace("T", " ")+"</span>"+"</div> <br>");
	}else{
		console.log("invalid date")
	}	
	change_filter_type()

	//getMap(false);
}

function initialVariantListener(){
	
	var response = this.responseText;
	//console.log(response)
	
	if (redo_function==true|| filtered_timeframe == true || filtered_perf == true || filtered_attribute == true) { // 
	
		document.getElementById("digraphF").innerHTML = response.split("|||")[0];
		document.getElementById("digraphP").innerHTML = response.split("|||")[1];
		variantsInfo = response.split("|||")[2];
		//console.log(variantsInfo)
		document.getElementById("variants_info").innerHTML=variantsInfo
	}
	else{
		//console.log("i am here")
		variantsInfo = response;
		//console.log(variantsInfo)
		document.getElementById("variants_info").innerHTML=variantsInfo
	}
  
	//console.log(variantsInfo)
	json = JSON.parse(variantsInfo.replace(/'/g,"\""));
	//console.log(json)
	//console.log("off")

	allVariants = Object.keys(json);
	//console.log("Total variants: " +String(allVariants.length));
	//console.log("tutte varianti: "+allVariants)
	
	
	total_trace_number=0
	total_event_number=0
	max_event_length=0
	min_event_length=Number.MAX_SAFE_INTEGER

	for(var i=0; i<allVariants.length; i++){
		//console.log(allVariants[i])
		//console.log(allVariants[i]['events'])
		total_trace_number=total_trace_number+json[allVariants[i]].length

		for(var j=0; j<json[allVariants[i]].length; j++){
			var chiave=Object.keys(json[allVariants[i]][j])
			
			//console.log(json[allVariants[i]][j]['events'][0])
			//console.log(json[allVariants[i]][j]['events'][1])
			//console.log(json[allVariants[i]][j]['events'][2])
			//console.log(json[allVariants[i]][j][chiave].length)
			total_event_number=total_event_number+json[allVariants[i]][j][chiave].length
			
			if(max_event_length<json[allVariants[i]][j][chiave].length){
				max_event_length=json[allVariants[i]][j][chiave].length
			}

			if(min_event_length>json[allVariants[i]][j][chiave].length){
				min_event_length=json[allVariants[i]][j][chiave].length
			}
		}
	}
	avarage_trace_length=total_event_number/total_trace_number;
	
	// $("#log_detail").val("");
	$("#log_detail").text("");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of traces: "+"<span style='color:blue;'>"+total_trace_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of events: "+"<span style='color:blue;'>"+total_event_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Avarage trace length: "+"<span style='color:blue;'>"+avarage_trace_length.toFixed(2) +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Max trace length: "+"<span style='color:blue;'>"+max_event_length+"</span>"+"</div> <br>");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Min trace length: "+"<span style='color:blue;'>"+min_event_length+"</span>"+"</div> <br>");
	// $("#log_detail").append("Number of activities involved: "+"&#13;&#10;" +"&#13;&#10;");
	// $('#log_detail').prop('readonly', true);


	// console.log("number of trace: "+total_trace_number)
	// console.log("number of event: "+total_event_number)
	// console.log("max event: "+max_event_length)
	// console.log("min event: "+min_event_length)

	// console.log("variant 1: " +(json[2].length));
	// console.log("variant 2: " +(json[1]).length);
	// console.log((json[1][0]['190'].length));
	//allVariants.lenght = Number of variants
	//Object.values(allVariants) = array with Variants ID

	ul1 = document.getElementById("variants-ul");
	ul2 = document.getElementById("case-ul");


	ul1.innerHTML = "";
	ul2.innerHTML = "";
	document.getElementById("eventsTable").innerHTML = "";
	Object.keys(json).forEach((key1) => {

		//----For each variant I see which are the cases corresponding
		//console.log("Variant " + String(key1));
		allCases = json[key1];
		//----allCases.length = Number of cases for each specific Variant (json[key])
		//console.log("Total cases: " + String(allCases.length));
		//console.log(allCases);


		var li = document.createElement('li');
		li.innerHTML = "<span class='text-table' style='border-radius: 4px;'>" + "Variant " + key1 + "</span><br><small class='text-table' style='border-radius: 4px;'>" + "Cases: " + String(allCases.length) + '</small>';
		li.setAttribute("style", "cursor: pointer");
		li.setAttribute("id", "variant"+key1);
		//li.setAttribute("onclick", "alert('Variant "+ key1 +"')");
		li.setAttribute("onclick", "showCases("+li.getAttribute('id')+"," +key1+")");
		ul1.append(li);	

		// calculate min and max dates
			
		allCases.forEach( (c) => {
			allEvents = Object.values(c)[0]; // all events
			allEvents.forEach((e) => {
				dates.push(new Date(e["time:timestamp"]));
			});
		});
			

	});
	
  	if(document.getElementById(selected)!=null){
		document.getElementById(selected).click();
	}
	
	
	maxDate=new Date(Math.max.apply(null,dates));
	minDate=new Date(Math.min.apply(null,dates));
	maxDate = maxDate.toISOString().split(".")[0];
	minDate = minDate.toISOString().split(".")[0];

	// console.log("max date: "+maxDate)
	// console.log("min date: "+ minDate)
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Minimun date: "+"<br> <span style='color:blue;'>"+minDate.replace("T", " ")+"</span>"+"</div> <br>");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Maximun date: "+"<br> <span style='color:blue;'>"+maxDate.replace("T", " ")+"</span>"+"</div>");
	// $('#log_detail').prop('readonly', true);

	//getMap(false);
	

}


function initialRequest() {
	start_iniziale=0
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", initialListener);
	oReq.open("GET", frontend+"initialAction", false);
	oReq.send();
}


function initialListener () {
	var dates = [];
	var response=(this.responseText);
	
	var arrayResponse=response.split("£")
	document.getElementById("digraphF").innerHTML = arrayResponse[0];
	document.getElementById("digraphP").innerHTML = arrayResponse[1];
	document.getElementById("usedVariable").innerHTML = arrayResponse[2];
	document.getElementById("allDt").innerHTML = arrayResponse[3];
	document.getElementById("allEdgeDt").innerHTML = arrayResponse[5-1];
	document.getElementById("allFq").innerHTML = arrayResponse[6-1];
	document.getElementById("allEdgeFq").innerHTML = arrayResponse[7-1];
	// document.getElementById("variants_alternative").innerHTML = arrayResponse[8];
	// console.log(arrayResponse[7])
	/*
	var response = arrayResponse[4];
	if (redo_function==true|| filtered_timeframe == true || filtered_perf == true || filtered_attribute == true) { // 
	
		document.getElementById("digraphF").innerHTML = response.split("|||")[0];
		document.getElementById("digraphP").innerHTML = response.split("|||")[1];
		variantsInfo = response.split("|||")[2];
		console.log(variantsInfo)
		document.getElementById("variants_info").innerHTML=variantsInfo
	}
	else{
		console.log("i am here")
		variantsInfo = response;
		console.log(variantsInfo)
		document.getElementById("variants_info").innerHTML=variantsInfo
	}
  
	//console.log(variantsInfo)
	json = JSON.parse(variantsInfo);
	//console.log("Total response log: "+variantsInfo.length);

	allVariants = Object.keys(json);
	//console.log("Total variants: " +String(allVariants.length));
	//console.log("tutte varianti: "+allVariants)

	total_trace_number=0
	total_event_number=0
	max_event_length=0
	min_event_length=Number.MAX_SAFE_INTEGER

	for(var i=0; i<allVariants.length; i++){
		// console.log(allVariants[i])
		total_trace_number=total_trace_number+json[allVariants[i]].length

		for(var j=0; j<json[allVariants[i]].length; j++){
			var chiave=Object.keys(json[allVariants[i]][j])
			// console.log(chiave)
			// console.log(json[allVariants[i]][j][chiave].length)
			total_event_number=total_event_number+json[allVariants[i]][j][chiave].length
			
			if(max_event_length<json[allVariants[i]][j][chiave].length){
				max_event_length=json[allVariants[i]][j][chiave].length
			}

			if(min_event_length>json[allVariants[i]][j][chiave].length){
				min_event_length=json[allVariants[i]][j][chiave].length
			}
		}
	}
	avarage_trace_length=total_event_number/total_trace_number;
	
	// $("#log_detail").val("");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of traces: "+"<span style='color:blue;'>"+total_trace_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of events: "+"<span style='color:blue;'>"+total_event_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Avarage trace length: "+"<span style='color:blue;'>"+avarage_trace_length.toFixed(2) +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Max trace length: "+"<span style='color:blue;'>"+max_event_length+"</span>"+"</div> <br>");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Min trace length: "+"<span style='color:blue;'>"+min_event_length+"</span>"+"</div> <br>");
	// $("#log_detail").append("Number of activities involved: "+"&#13;&#10;" +"&#13;&#10;");
	// $('#log_detail').prop('readonly', true);


	// console.log("number of trace: "+total_trace_number)
	// console.log("number of event: "+total_event_number)
	// console.log("max event: "+max_event_length)
	// console.log("min event: "+min_event_length)

	// console.log("variant 1: " +(json[2].length));
	// console.log("variant 2: " +(json[1]).length);
	// console.log((json[1][0]['190'].length));
	//allVariants.lenght = Number of variants
	//Object.values(allVariants) = array with Variants ID

	ul1 = document.getElementById("variants-ul");
	ul2 = document.getElementById("case-ul");


	ul1.innerHTML = "";
	ul2.innerHTML = "";
	document.getElementById("eventsTable").innerHTML = "";
	Object.keys(json).forEach((key1) => {

		//----For each variant I see which are the cases corresponding
		//console.log("Variant " + String(key1));
		allCases = json[key1];
		//----allCases.length = Number of cases for each specific Variant (json[key])
		//console.log("Total cases: " + String(allCases.length));
		//console.log(allCases);


		var li = document.createElement('li');
		li.innerHTML = "<span class='text-table' style='border-radius: 4px;'>" + "Variant " + key1 + "</span><br><small class='text-table' style='border-radius: 4px;'>" + "Cases: " + String(allCases.length) + '</small>';
		li.setAttribute("style", "cursor: pointer");
		li.setAttribute("id", "variant"+key1);
		//li.setAttribute("onclick", "alert('Variant "+ key1 +"')");
		li.setAttribute("onclick", "showCases("+li.getAttribute('id')+"," +key1+")");
		ul1.append(li);	

		// calculate min and max dates
			
		allCases.forEach( (c) => {
			allEvents = Object.values(c)[0]; // all events
			allEvents.forEach((e) => {
				dates.push(new Date(e["time:timestamp"]));
			});
		});
			

	});
	
  	if(document.getElementById(selected)!=null){
		document.getElementById(selected).click();
	}
	
	
	maxDate=new Date(Math.max.apply(null,dates));
	minDate=new Date(Math.min.apply(null,dates));
	maxDate = maxDate.toISOString().split(".")[0];
	minDate = minDate.toISOString().split(".")[0];

	// console.log("max date: "+maxDate)
	// console.log("min date: "+ minDate)
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Minimun date: "+"<br> <span style='color:blue;'>"+minDate.replace("T", " ")+"</span>"+"</div> <br>");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Maximun date: "+"<br> <span style='color:blue;'>"+maxDate.replace("T", " ")+"</span>"+"</div>");
	// $('#log_detail').prop('readonly', true);

	//getMap(false);
	*/
	
}


function reqListener91 () {
	document.getElementById("allFq").innerHTML = this.responseText;
	// console.log(this.responseText)
	//getMap(false);
}

function csmRequest() {
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", reqListener91);
	oReq.open("GET", frontend+"allfrequency", false);
	oReq.send();
}


function reqListener92 () {
	document.getElementById("allEdgeFq").innerHTML = this.responseText;
	// console.log(this.responseText)
	//getMap(false);
}

function csmEdgeRequest() {
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", reqListener92);
	oReq.open("GET", frontend+"alledgefrequency", false);
	oReq.send();
}


function reqListener1 () {
	document.getElementById("digraphF").innerHTML = this.responseText;

	//getMap(false);
}

function freqRequest() {
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", reqListener1);
	oReq.open("GET", frontend+"dfgFrequency", false);
	oReq.send();
}


function reqListener2 () {
	document.getElementById("digraphP").innerHTML = this.responseText;
	//   document.getElementById("loadingMessage").style.visibility = "hidden";	
	// console.log("digraphP");
  	//getMap(false);
}

function perfRequest() {
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", reqListener2);
	oReq.open("GET", frontend+"dfgPerformance", false);
	oReq.send();
}


function reqListener3 () {
	var dates = [];
	
	//var arrayResponse=this.responseText.split("£")
	//console.log(response)
	//document.getElementById("variants_alternative").innerHTML = arrayResponse[1];
	
	change_filter_type()

	var response=this.responseText

	if (redo_function==true|| filtered_timeframe == true || filtered_perf == true || filtered_attribute == true || filtered_scan == true) { // 
	
		//console.log("inside if dei filtri")
		document.getElementById("digraphF").innerHTML = response.split("|||")[0];
		document.getElementById("digraphP").innerHTML = response.split("|||")[1];
		//variantsInfo = response.split("|||")[2];
		// console.log(variantsInfo)
		//document.getElementById("variants_info").innerHTML=variantsInfo
	}else{
		//console.log("sto nella parte giusta")
		//variantsInfo = response;
		//document.getElementById("variants_info").innerHTML=variantsInfo
	}
  
	//console.log(variantsInfo)
	/*
	json = JSON.parse(variantsInfo);

	allVariants = Object.keys(json);

	var partial_trace_number=0
	var partial_event_number=0
	var partial_max_event_length=0
	var partial_min_event_length=Number.MAX_SAFE_INTEGER

	for(var i=0; i<allVariants.length; i++){
		// console.log(allVariants[i])
		partial_trace_number=partial_trace_number+json[allVariants[i]].length

		for(var j=0; j<json[allVariants[i]].length; j++){
			var chiave=Object.keys(json[allVariants[i]][j])
			// console.log(chiave)
			// console.log(json[allVariants[i]][j][chiave].length)
			partial_event_number=partial_event_number+json[allVariants[i]][j][chiave].length
			
			if(partial_max_event_length<json[allVariants[i]][j][chiave].length){
				partial_max_event_length=json[allVariants[i]][j][chiave].length
			}

			if(partial_min_event_length>json[allVariants[i]][j][chiave].length){
				partial_min_event_length=json[allVariants[i]][j][chiave].length
			}
		}
	}

	var partial_avarage_trace_length=partial_event_number/partial_trace_number;
	
	$("#log_detail").text("");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of traces: "+"<span style='color:blue;'>"+partial_trace_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Total number of events: "+"<span style='color:blue;'>"+partial_event_number +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Avarage trace length: "+"<span style='color:blue;'>"+partial_avarage_trace_length.toFixed(2) +"</span>"+"</div> <br> ");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Max trace length: "+"<span style='color:blue;'>"+partial_max_event_length+"</span>"+"</div> <br>");
	$("#log_detail").append("<div style='color: rgb(38, 38, 38);'> "+"Min trace length: "+"<span style='color:blue;'>"+partial_min_event_length+"</span>"+"</div> <br>");
	
	// $("#log_detail").append("Number of activities involved: "+"&#13;&#10;" +"&#13;&#10;");
	
	// console.log("number of trace: "+partial_trace_number)
	// console.log("number of event: "+partial_event_number)
	// console.log("max event: "+partial_max_event_length)
	// console.log("min event: "+partial_min_event_length)

	
	var percentage__trace=partial_trace_number*100/total_trace_number
	// console.log(percentage__trace)
	$("#percentage_trace").text("Cases: "+percentage__trace.toFixed(2)+"%")
	var percentage__event=partial_event_number*100/total_event_number
	// console.log(percentage__event)
	$("#percentage_event").text("Event: "+percentage__event.toFixed(2)+"%")
	// console.log("Total variants: " +String(allVariants.length));
	//allVariants.lenght = Number of variants
	//Object.values(allVariants) = array with Variants ID

	ul1 = document.getElementById("variants-ul");
	ul2 = document.getElementById("case-ul");


	ul1.innerHTML = "";
	ul2.innerHTML = "";
	document.getElementById("eventsTable").innerHTML = "";
	Object.keys(json).forEach((key1) => {

		//----For each variant I see which are the cases corresponding
		//console.log("Variant " + String(key1));
		allCases = json[key1];
		//----allCases.length = Number of cases for each specific Variant (json[key])
		//console.log("Total cases: " + String(allCases.length));
		//console.log(allCases);


		var li = document.createElement('li');
		li.innerHTML = "<span class='text-table' style='border-radius: 4px;'>" + "Variant " + key1 + "</span><br><small class='text-table' style='border-radius: 4px;'>" + "Cases: " + String(allCases.length) + '</small>';
		li.setAttribute("style", "cursor: pointer");
		li.setAttribute("id", "variant"+key1);
		//li.setAttribute("onclick", "alert('Variant "+ key1 +"')");
		li.setAttribute("onclick", "showCases("+li.getAttribute('id')+"," +key1+")");
		ul1.append(li);	

		// calculate min and max dates
			
		allCases.forEach( (c) => {
			allEvents = Object.values(c)[0]; // all events
			allEvents.forEach((e) => {
				dates.push(new Date(e["time:timestamp"]));
			});
		});
			

	});
	
  	if(document.getElementById(selected)!=null){
		document.getElementById(selected).click();
	}
	
	
	maxDate=new Date(Math.max.apply(null,dates));
	minDate=new Date(Math.min.apply(null,dates));
	*/
	
	/*
	if(maxDate instanceof Date && !isNaN(maxDate.valueOf())){
		maxDate = maxDate.toISOString().split(".")[0];
		$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Maximun date: "+"<br> <span style='color:blue;'>"+maxDate.replace("T", " ")+"</span>"+"</div>");
	
	}else{
		console.log("invalid date")
	}
	if(minDate instanceof Date && !isNaN(minDate.valueOf())){
		minDate = minDate.toISOString().split(".")[0];
		$("#log_detail").append("<div style='color: rgb(38, 38, 38);'>"+"Minimun date: "+"<br> <span style='color:blue;'>"+minDate.replace("T", " ")+"</span>"+"</div> <br>");
	}else{
		console.log("invalid date")
	}
	*/
  
  	//getMap(false);
}

function variantRequest() {
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", reqListener3);
	oReq.open("GET", frontend+"variants", false);
	oReq.send();
}


function reqListener70 () {
  document.getElementById("allDt").innerHTML = this.responseText;
//   console.log(this.responseText)
//   getMap(false);
}

function allDurationRequest(){
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", reqListener70);
	oReq.open("GET", frontend+"allduration", false);
	oReq.send();
}


function reqListener71 () {
  document.getElementById("allEdgeDt").innerHTML = this.responseText;
//   console.log(this.responseText)
//   getMap(false);
}

function allEdgeDurationRequest(){
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", reqListener71);
	oReq.open("GET", frontend+"alledgeduration", false);
	oReq.send();
}


function reqListener72 () {
  document.getElementById("usedVariable").innerHTML = this.responseText;
//   console.log(this.responseText)
//   getMap(false);
}

function allUsedVariable(){
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", reqListener72);
	oReq.open("GET", frontend+"usedvariable", false);
	oReq.send();
}