
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


function saveProject(){
    console.log("dslrequest")

    var predecessors = [];
	var parameters = [];
	var count = [];
	var pipelineName = document.getElementById('mapTitle').innerHTML.replace('.xes', '');
	var dsl = 'Pipeline ' + pipelineName +'{\n';
	
	for (var i=0; i<dslSteps.length; i++){
		predecessors[i] = '';
		count[i] = 0;
		// get parameters of the current step in a string
		parameters[i] = '\t\t\t\tFrequency: ' + dslSteps[i][0][1] + ',\n\t\t\t\tDuration: ' + dslSteps[i][0][2];
		// iterate on the predecessors of the current step and save them in a string
		for (var j=1; j<dslSteps[i].length; j++){
			count[i]++;
			predecessors[i] = predecessors[i] + '\t\t\t\t' + dslSteps[i][j][0].replace(' ', '_');
			if (j<dslSteps[i].length-1)
				predecessors[i] = predecessors[i] +',\n';	
		}
		// print the whole string for each step
		dsl = dsl + '\t-\tStep Step: ' + dslSteps[i][0][0].replaceAll(' ', '_');
		if (predecessors[i] != ''){
			dsl = dsl + '\n\t\t\tPrevious:';
			if (count[i]>1)
				dsl = dsl + '[';
			dsl = dsl + '\n' + predecessors[i];
			if (count[i]>1)
				dsl = dsl + '\n\t\t\t]';
		}
		if (dslSteps[i][0][0] == 'start' || dslSteps[i][0][0] == 'end')
			dsl = dsl + '\n';
		else
			dsl = dsl + '\n\t\t\tEnvironmentParameters:\n' + parameters[i] + '\n';
	}
	dsl = dsl + '\}';

    var oReq = new XMLHttpRequest();
    //oReq.addEventListener("load", jarListener);
	oReq.open("POST", frontend+"dslPost", true);
	payload = {"pipeline": dsl}
	
    oReq.setRequestHeader('Dsl', JSON.stringify(payload));
    oReq.send()

	var oReq1 = new XMLHttpRequest();
	oReq1.addEventListener("load", saveListener);
	oReq1.open("GET", frontend+"saveProject", false);
	oReq1.send();
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

function sendDslListener(){
	console.log(this.responseText)
}

function sendDslRequest(){

	var predecessors = [];
	var parameters = [];
	var count = [];
	var pipelineName = document.getElementById('mapTitle').innerHTML.replace('.xes', '');
	var dsl = 'Pipeline ' + pipelineName +'{\n';
	
	for (var i=0; i<dslSteps.length; i++){
		predecessors[i] = '';
		count[i] = 0;
		// get parameters of the current step in a string
		parameters[i] = '\t\t\t\tFrequency: ' + dslSteps[i][0][1] + ',\n\t\t\t\tDuration: ' + dslSteps[i][0][2];
		// iterate on the predecessors of the current step and save them in a string
		for (var j=1; j<dslSteps[i].length; j++){
			count[i]++;
			predecessors[i] = predecessors[i] + '\t\t\t\t' + dslSteps[i][j][0].replace(' ', '_');
			if (j<dslSteps[i].length-1)
				predecessors[i] = predecessors[i] +',\n';	
		}
		// print the whole string for each step
		dsl = dsl + '\t-\tStep: ' + dslSteps[i][0][0].replaceAll(' ', '_');
		if (predecessors[i] != ''){
			dsl = dsl + '\n\t\t\tPrevious:';
			if (count[i]>1)
				dsl = dsl + '[';
			dsl = dsl + '\n' + predecessors[i];
			if (count[i]>1)
				dsl = dsl + '\n\t\t\t]';
		}
		if (dslSteps[i][0][0] == 'start' || dslSteps[i][0][0] == 'end')
			dsl = dsl + '\n';
		else
			dsl = dsl + '\n\t\t\tEnvironmentParameters:\n' + parameters[i] + '\n';
	}
	dsl = dsl + '\}';

	var oReq1 = new XMLHttpRequest();
	oReq1.addEventListener("load", sendDslListener);
	oReq1.open("GET", frontend+"sendDsl?dsl="+dsl, false);
	oReq1.send();

}


//function used to compute the DSL
function createDsl(){

	var predecessors = [];
	var parameters = [];
	var count = [];
	var pipelineName = document.getElementById('mapTitle').innerHTML.replace('.xes', '');
	//print pipeline title + fixed line about communicationMedium
	var dsl = 'Pipeline ' + pipelineName +' {\n\tcommunicationMedium: medium WEB_SERVICE\n\tsteps:\n' ;
	//iterate on the steps
	for (var i=0; i<dslSteps.length; i++){
		// if the step is start or end, just skip
		if ( dslSteps[i][0][0].replaceAll(' ', '_') == 'start' || dslSteps[i][0][0].replaceAll(' ', '_') == 'end')
			continue;
		// first iteration no \n
		if (i != 0)
			dsl = dsl + '\n\n';
		predecessors[i] = '';
		count[i] = 0;
		// get parameters of the current step in a string
		parameters[i] = "\t\t\t\tFrequency: '" + dslSteps[i][0][1] + "',\n\t\t\t\tDuration: '" + dslSteps[i][0][2] +"'";
		/* -----------------------outdated part about predecessors --------------------------------------
		// iterate on the predecessors of the current step and save them in a string
		for (var j=1; j<dslSteps[i].length; j++){
			count[i]++;
			predecessors[i] = predecessors[i] + '\t\t\t\t' + dslSteps[i][j][0].replace(' ', '_');
			if (j<dslSteps[i].length-1)
				predecessors[i] = predecessors[i] +',\n';	
		}
		// --------------------------------------------------------------------------------------------*/
		// print the whole string for each step
		dsl = dsl + '\t\t- data-processing step ' + dslSteps[i][0][0].replaceAll(' ', '_');
		/* -----------------------outdated part about predecessors --------------------------------------
		if (predecessors[i] != ''){
			dsl = dsl + '\n\t\t\tPrevious:';
			if (count[i]>1)
				dsl = dsl + '[';
			dsl = dsl + '\n' + predecessors[i];
			if (count[i]>1)
				dsl = dsl + '\n\t\t\t]';
		}
		// --------------------------------------------------------------------------------------------*/
		dsl = dsl + "\n\t\t\timplementation: container-implementation image: ''\n\t\t\tenvironmentParameters: {\n" + parameters[i] + '\n\t\t\t}\n\t\t\tresourceProvider: Accesspoint\n\t\t\texecutionRequirement:\n\t\t\t\thardRequirements:\n';
	}
	dsl = dsl + '\}';
	//return the dsl
	return dsl;
}
// function used to export the DSL to file
function exportDsl(){
	var pipelineName = document.getElementById('mapTitle').innerHTML.replace('.xes', '');
	var dsl = createDsl();
	download(dsl, pipelineName, '.txt');
}