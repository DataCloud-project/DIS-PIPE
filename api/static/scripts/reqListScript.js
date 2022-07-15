var variantsInfo;
var allVariants;
var dates = [];

function swipeRemoveRequest() {
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", swipeRemoveListener);
	oReq.open("GET", frontend+"swipeRemoveAction", false);
	oReq.send();
}

function swipeRemoveListener () {
	var response=this.responseText;
	var arrayResponse=response.split("£")
	document.getElementById("digraphF").innerHTML = arrayResponse[0];
	document.getElementById("digraphP").innerHTML = arrayResponse[1];
	var response = arrayResponse[2];
	if (redo_function==true|| filtered_timeframe == true || filtered_perf == true || filtered_attribute == true) { // 
	
		document.getElementById("digraphF").innerHTML = response.split("|||")[0];
		document.getElementById("digraphP").innerHTML = response.split("|||")[1];
		variantsInfo = response.split("|||")[2];
		// console.log(variantsInfo)
	}
	else{
		
		variantsInfo = response;
	}
  
	// console.log(variantsInfo)
	json = JSON.parse(variantsInfo);
	//console.log(json);

	allVariants = Object.keys(json);
	//console.log("Total variants: " +String(allVariants.length));
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
	
}


function initialRequest() {
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", initialListener);
	oReq.open("GET", frontend+"initialAction", false);
	oReq.send();
}

function initialListener () {
	var response=this.responseText;
	var arrayResponse=response.split("£")
	document.getElementById("digraphF").innerHTML = arrayResponse[0];
	document.getElementById("digraphP").innerHTML = arrayResponse[1];
	document.getElementById("usedVariable").innerHTML = arrayResponse[2];
	document.getElementById("allDt").innerHTML = arrayResponse[3];
	document.getElementById("allEdgeDt").innerHTML = arrayResponse[5];
	document.getElementById("allFq").innerHTML = arrayResponse[6];
	document.getElementById("allEdgeFq").innerHTML = arrayResponse[7];
	// console.log(arrayResponse[7])
	var response = arrayResponse[4];
	if (redo_function==true|| filtered_timeframe == true || filtered_perf == true || filtered_attribute == true) { // 
	
		document.getElementById("digraphF").innerHTML = response.split("|||")[0];
		document.getElementById("digraphP").innerHTML = response.split("|||")[1];
		variantsInfo = response.split("|||")[2];
		// console.log(variantsInfo)
	}
	else{
		
		variantsInfo = response;
	}
  
	// console.log(variantsInfo)
	json = JSON.parse(variantsInfo);
	//console.log(json);

	allVariants = Object.keys(json);
	//console.log("Total variants: " +String(allVariants.length));
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

	//getMap(false);
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
   
	var response = this.responseText;
	if (redo_function==true|| filtered_timeframe == true || filtered_perf == true || filtered_attribute == true) { // 
	
		document.getElementById("digraphF").innerHTML = response.split("|||")[0];
		document.getElementById("digraphP").innerHTML = response.split("|||")[1];
		variantsInfo = response.split("|||")[2];
		// console.log(variantsInfo)
	}
	else{
		
		variantsInfo = response;
	}
  
	// console.log(variantsInfo)
	json = JSON.parse(variantsInfo);
	//console.log(json);

	allVariants = Object.keys(json);
	//console.log("Total variants: " +String(allVariants.length));
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