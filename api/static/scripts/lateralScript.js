
function logout() {
    //console.log("bode")
    //var oReq = new XMLHttpRequest();
	//oReq.open("GET", frontend+"logout", false);
	//oReq.send();
    var url = "/logout";
    window.location.assign(url);
}


function exportXesRequest() {

	$("#loadingMessage").css("visibility", "visible");
	setTimeout(() => {
   
		var oReq = new XMLHttpRequest();
		oReq.open("GET", frontend+"exportXes", false);
		oReq.send();

		$("#loadingMessage").css("visibility", "hidden");

	}, 10);    
}

function saveRequest() {

	$("#loadingMessage").css("visibility", "visible");
	setTimeout(() => {
   
		var oReq = new XMLHttpRequest();
        oReq.addEventListener("load", saveListener);
		oReq.open("GET", frontend+"exportXes", false);
		oReq.send();

		$("#loadingMessage").css("visibility", "hidden");

	}, 10);    
}

function saveListener(){
    console.log(this.responseText)
}


function saveProjectXes(){
	var oReq1 = new XMLHttpRequest();
	oReq1.addEventListener("load", saveListener);
	oReq1.open("GET", frontend+"saveProject", false);
	oReq1.send();
}

function saveProjectDsl(){
	console.log("dslrequest")

    var dsl = createDsl();
	
    var oReq = new XMLHttpRequest();
    //oReq.addEventListener("load", jarListener);
	oReq.open("POST", frontend+"dslPost", true);
	payload = {"pipeline": dsl}
	
    oReq.setRequestHeader('Dsl', JSON.stringify(payload));
    oReq.send()
}


function changeFolder(){
	var folder = document.getElementById("uploadFolder");
	var files = folder.files,
		len = files.length,
		i;
	var regex_expression_txt=/^[\w\.,\s-]+\.txt$/
	var regex_expression_xes=/^[\w\.,\s-]+\.xes$/

	var name_log, name_dsl;

	console.log(files)

	for(i=0;i<len;i+=1){
		console.log(files[i].name);
		if(files[i].name.match(regex_expression_txt)!=null){
			console.log(files[i].name.match(regex_expression_txt))
			console.log("txt")
			name_dsl=files[i].name
		}
		if(files[i].name.match(regex_expression_xes)!=null){
			console.log(files[i].name.match(regex_expression_xes))
			console.log("xes")
			name_log=files[i].name
		}		
	}
	var oReq1 = new XMLHttpRequest();
	
	oReq1.open("POST", frontend+"loadProject?namelog="+name_log.replace(".xes",""), false);
	oReq1.send();

	window.location.href = "http://127.0.0.1:8080/";
	//location.reload();
}

function load(){
	document.getElementById('uploadFolder').click();
}

function sendDslListenerBis(){
	console.log(this.responseText)
	
	var obj = JSON.parse(this.responseText);
	console.log(obj.success)
	if(obj.success==true){
		alert("DSL sent to DEF-PIPE")
	}else{
		alert("error sending dsl to DEF-PIPE")
	}
	console.log(obj)
	// Expected output: 42

	/*
	document.getElementById("dslFeedBack").style.display = "block";
	document.getElementById("blocker6").style.display = "block";
	*/
}

function sendDslListenerReq2(){
	console.log(this.responseText)

	var oReq1 = new XMLHttpRequest();
	oReq1.addEventListener("load", sendDslListenerBis);
	oReq1.open("GET", frontend+"sendDsl", false);
	oReq1.send();

}

function sendDslRequest(posizione){

	console.log("sendDslRequest")
	
	var pipelineName = document.getElementById('mapTitle').innerHTML.replace('.xes', '');
	
    var dsl = createDsl();
	
    var oReq = new XMLHttpRequest();
	if(posizione!="start"){
		oReq.addEventListener("load", sendDslListenerReq2);
	}
	oReq.open("POST", frontend+"dslPost", true);
	payload = {"pipeline": dsl}
	
    oReq.setRequestHeader('Dsl', JSON.stringify(payload));
    oReq.send()
	

	//alert("DSL sent to DEF-PIPE")
	/*
	document.getElementById("dslFeedBack").style.display = "block";
	document.getElementById("blocker6").style.display = "block";
	*/

	//saveProjectDsl();
	//var pipelineName = document.getElementById('mapTitle').innerHTML.replace('.xes', '');
	//var dsl = createDsl();

}

//function used to compute the DSL
function createDsl(){
	// set path slider to 0 to have a sequence
	document.getElementById("myPathF").value = 0;
	document.getElementById("pathF").value = "0";
	request(0);
	getMap(1);
	// translation from DFG to matrix
	console.log("Translating from map to matrix");
	// get graph in an array
	graphTextF = getGraphText('frequency');
	graphTextP = getGraphText();
	// get nodes in an array 
	graphNodesF = getGraphNodes(graphTextF);
	graphNodesP = getGraphNodes(graphTextP);
	graphNodes = getCombinedNodes(graphNodesF, graphNodesP);
	// get edges in an array
	graphEdgesF = getGraphEdges(graphTextF, false);
	graphEdgesP = getGraphEdges(graphTextP, true);
	graphEdges = getCombinedEdges(graphEdgesF, graphEdgesP);
	getLabeledGraphEdges(graphNodes, graphEdges);
	// get final matrix ready for dsl conversion
	dslSteps = getDslSteps(graphNodes, graphEdges);
	// translating from matrix to DSL
	console.log("Translating from matrix to DSL");
	//translate to DSL
	var parameters = [];
	var count = [];
	var pipelineName = document.getElementById('mapTitle').innerHTML.replace('.xes', '');
	//print pipeline title + fixed line about communicationMedium
	var dsl = 'Pipeline ' + pipelineName +' {\n\tcommunicationMedium: medium WEB_SERVICE\n\tsteps:\n' ;
	//iterate on the steps
	for (var i=0; i<dslSteps.length; i++){
		// if the step is start or end, just skip
		if ( dslSteps[i][0].replaceAll(' ', '_') == 'start' || dslSteps[i][0].replaceAll(' ', '_') == 'end')
			continue;
		// first iteration no \n
		if (i != 0)
			dsl = dsl + '\n\n';
		count[i] = 0;
		// get parameters of the current step in a string
		parameters[i] = "\t\t\t\tFrequency: '" + dslSteps[i][1] + "',\n\t\t\t\tDuration: '" + dslSteps[i][2] +"'";
		// print the whole string for each step
		dsl = dsl + '\t\t- data-processing step ' + dslSteps[i][0].replaceAll(' ', '_');
		// --------------------------------------------------------------------------------------------*/
		dsl = dsl + "\n\t\t\timplementation: container-implementation image: ''\n\t\t\tenvironmentParameters: {\n" + parameters[i] + '\n\t\t\t}\n\t\t\tresourceProvider: Accesspoint\n\t\t\texecutionRequirement:\n\t\t\t\thardRequirements:\n';
	}
	dsl = dsl + '\}';
	//return the dsl
	return dsl;
}
// function used to export the DSL to file
function exportDsl(){
	console.log("Exporting DSL");
	var dsl = createDsl();
	download(dsl, document.getElementById('mapTitle').innerHTML.replace('.xes', ''), '.txt');
}