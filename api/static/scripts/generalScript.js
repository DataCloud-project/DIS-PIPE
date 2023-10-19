var petri_sample


function renameProject(){
	
	let nuovoNome = ""
	nuovoNome = prompt('Please enter a new name:');

	if(document.getElementById("mapTitle").innerText==="Example.xes"){
		alert("You can't rename the demo project.");
	}else if(!nuovoNome) {
		alert("You can't enter an empty name.");
	}else if(nuovoNome==="Example" || nuovoNome==="example") {
		alert("You can't use this name.");
	}else{
		// Verifica se la stringa contiene caratteri speciali
		var regex = /[!@#$%^&£*()_+\-=\[\]{};':"\\|,.<>\/?]+/;
		if (regex.test(nuovoNome)) {
			alert("La stringa contiene caratteri speciali.");
		} else {
			console.log("La stringa è valida.");
			var oReq1 = new XMLHttpRequest();
			oReq1.addEventListener("load", renameProjectListener);
			oReq1.open("POST", frontend+"renameProject/"+nuovoNome, false);
			oReq1.send();
		}
	}
}

function renameProjectListener(){
	console.log(this.responseText)
	document.getElementById('mapTitle').innerHTML = this.responseText
}

/////



////



function update(){
	console.log("update normale")
	document.getElementById('updated').value = true;
	document.getElementById('file').click();
	sessionStorage.setItem('segmentatore', 'NO');

}
function updateSegmentator(){
	console.log("update segmentator")
	document.getElementById('updated').value = true;
	document.getElementById('file').click();
	sessionStorage.setItem('segmentatore', 'YES');
}

function openDecisionMakingSegementator(){
	document.getElementById("blocker_choseEndSeg").style.display = "block";
    document.getElementById("choseEndSeg").style.display = "block";
	const svgContent = document.getElementById('graphContainer').innerHTML;
	console.log(svgContent)
	// Assuming you have the SVG content in a variable called 'svgContent'
	const parser = new DOMParser();
	const doc = parser.parseFromString(svgContent, "image/svg+xml");

	// Get all elements with class "node"
	const nodes = doc.getElementsByClassName("node");

	// Iterate over the nodes and extract the text values
	const nodeTexts = [];
	for (const node of nodes) {
	const textElement = node.querySelector("text");
	if (textElement) {
		const text = textElement.textContent.trim();
		if (text !== "") {
			const modifiedText = text.replace(/\([^)]*\)/g, "").trim();
			nodeTexts.push(modifiedText);
		}
	}

	}
	console.log(nodeTexts);

	const select = document.getElementById("endSegAct");

    // Step 3-5: Add options to the select element
    nodeTexts.forEach((option) => {
      const optionElement = document.createElement("option");
      optionElement.text = option;
      optionElement.value = option;
      select.appendChild(optionElement);
    });
}

function wrapOpenDecisionMakingSegmentator(){
	openDecisionMakingSegementator()
	sessionStorage.setItem('segmentatore', 'NO');
}

function closechoseEndSeg(){
	document.getElementById("blocker_choseEndSeg").style.display = "none";
    document.getElementById("choseEndSeg").style.display = "none";
}

var selectCount = 1; 
function addNewEndAct(){
	console.log("ora fai add")
	var containerEnd = document.getElementById("containerEnd");

	// Get the addEnd0 div
	//var addEnd0 = document.getElementById("addEnd0");
	var button = event.target;
  
	// Get the parent div to be removed
	var addEnd0 = button.parentNode;
	
	// Clone the addEnd0 div
	var clonedDiv = addEnd0.cloneNode(true);

	/********/
	
	var selectElement = addEnd0.querySelector("select");
  
	var selectedOption = selectElement.options[selectElement.selectedIndex].text;
	
	const isPresent = segmemtator_array.includes(selectedOption);
	if(isPresent){
		alert("already present")
	}else{
	segmemtator_array.push(selectedOption)
	var spanElement = document.createElement("span");
	spanElement.textContent = selectedOption;


	/**********/
	
	// Generate a new ID for the cloned div
	//var newDivId = "addEnd" + containerEnd.children.length;
	var newDivId = "addEnd" + selectCount;
	selectCount=selectCount+1
	
	// Set the new ID for the cloned div
	clonedDiv.id = newDivId;
  
	// Append the cloned div to the containerEnd element
	containerEnd.appendChild(clonedDiv);

	selectElement.parentNode.replaceChild(spanElement, selectElement);
	
	}
	
	
}

function removeEndAct(){
	// Get the button element that triggered the function
	var button = event.target;
  
	// Get the parent div to be removed
	var divToRemove = button.parentNode;
	var spanElement = divToRemove.querySelector("span");
	console.log(spanElement.innerHTML)
	elementToRemove=spanElement.innerHTML

	segmemtator_array = segmemtator_array.filter((element) => element !== elementToRemove);
	console.log(segmemtator_array); // Output: [1, 2, 4, 5]
	
	// Get the parent element of the div
	var parent = divToRemove.parentNode;
	
	// Remove the div from its parent
	parent.removeChild(divToRemove);
}

function xesORcsv(){
	document.getElementById("blocker_exportMenu").style.display = "block";
    document.getElementById("exportMenu").style.display = "block";
}

function normalORsegmentator(){
	//document.getElementById("blocker_importMenu").style.display = "block";
    //document.getElementById("importMenu").style.display = "block";
	updateSegmentator()
}

function closeExportMenu(){
	document.getElementById("blocker_exportMenu").style.display = "none";
    document.getElementById("exportMenu").style.display = "none";
}

function closeImportMenu(){
	document.getElementById("blocker_importMenu").style.display = "none";
    document.getElementById("importMenu").style.display = "none";
}

function changeConstraint(){
	console.log($("#selectFun option:selected").attr("title"));
	var constraint_definition=$("#selectFun option:selected").attr("title");
	var constraint_title=$("#selectFun option:selected").text();
	$('#definitions_constraint').text(constraint_title+": "+constraint_definition);
}

function mainMenu(){
	if (menuBtn.checked == false) {
		menu.style.display = "none";
		content.style.maxWidth = "100%";
		//console.log(document.getElementById("file").files[0]);
	}
	else {
		menu.style.display = "flex";
		content.style.maxWidth = "80%";
	}
}

function displayContent(){
	if (map.checked == true) {
		mapText.style.display = "block";
		elanalysisText.style.display = "none";
		cCheckingText.style.display = "none";
		menu.style.display = "none";
		content.style.maxWidth = "100%";
		queryText.style.display = "none"
		menuBtn.checked = false;
		$("#highlight_trace").val('no').change();
		$(".edge").find("path").attr('stroke', "#000000");
		$(".edge").find("polygon").attr('stroke', "#000000");
		// $(".edge").find("text").css({"text-decoration":"revert"});
		document.getElementById("import_log").disabled = false;
		document.getElementById("export_log").disabled = false; 
		document.getElementById("export_dsl").disabled = false; 
		//document.getElementById("get_dsl").disabled = false;
		document.getElementById("send_dsl").disabled = false; 
		document.getElementById("load_project").disabled = false;
		document.getElementById("save_project").disabled = false;

		document.getElementById("slidercontainer_cChecking").style.display = "none"		

		document.getElementById("link_export_log").classList.remove('link_disabled');
		document.getElementById("link_export_log").classList.add('link_normal');
	}
	if (elanalysis.checked == true) {
		mapText.style.display = "none";
		elanalysisText.style.display = "block";
		cCheckingText.style.display = "none";
		menu.style.display = "none";
		queryText.style.display = "none"
		content.style.maxWidth = "100%";
		menuBtn.checked = false;
		document.getElementById("import_log").disabled = true;
		document.getElementById("export_log").disabled = false; 
		document.getElementById("export_dsl").disabled = false; 
		//document.getElementById("get_dsl").disabled = false;
		document.getElementById("send_dsl").disabled = false; 
		document.getElementById("load_project").disabled = true;
		document.getElementById("save_project").disabled = false;
		document.getElementById("slidercontainer_cChecking").style.display = "none"

		document.getElementById("link_export_log").classList.remove('link_disabled');
		document.getElementById("link_export_log").classList.add('link_normal');
	}
	if (cChecking.checked == true) {
		mapText.style.display = "none";
		elanalysisText.style.display = "none";
		cCheckingText.style.display = "block";
		menu.style.display = "none";
		queryText.style.display = "none"
		content.style.maxWidth = "100%";
		menuBtn.checked = false;
		document.getElementById("import_log").disabled = true;
		document.getElementById("export_log").disabled = false; 
		document.getElementById("export_dsl").disabled = false; 
		//document.getElementById("get_dsl").disabled = false;
		document.getElementById("send_dsl").disabled = false; 
		document.getElementById("load_project").disabled = true;
		document.getElementById("save_project").disabled = true;
		document.getElementById("slidercontainer_cChecking").style.display = "none"

		document.getElementById("link_export_log").classList.remove('link_disabled');
		document.getElementById("link_export_log").classList.add('link_normal');

	}
	if (query.checked == true) {
		mapText.style.display = "none";
		elanalysisText.style.display = "none";
		cCheckingText.style.display = "none";
		queryText.style.display = "block"
		menu.style.display = "none";
		content.style.maxWidth = "100%";
		menuBtn.checked = false;
		document.getElementById("import_log").disabled = true;
		document.getElementById("export_log").disabled = true; 
		document.getElementById("export_dsl").disabled = true; 
		//document.getElementById("get_dsl").disabled = false;
		document.getElementById("send_dsl").disabled = true; 
		document.getElementById("load_project").disabled = true;
		document.getElementById("save_project").disabled = true; 
		document.getElementById("slidercontainer_cChecking").style.display = "none"

		document.getElementById("link_export_log").classList.remove('link_normal');
		document.getElementById("link_export_log").classList.add('link_disabled');
	}
}


function petriNetComputeInternal(){
		

	$("#showRes").attr("onclick","showResultonDFGbis('interno')");


	$("#loadingMessage").css("visibility", "visible");
	$("#back_slide_conf").css('visibility','hidden');
    $("#info_check_conformance").css('visibility','hidden');
    $("#btn_conformance").css('visibility','hidden');


	document.getElementById("formConformanceChecking0").style.display = "none";
	document.getElementById("formConformanceChecking").style.display = "block";
    document.getElementById("formConformanceChecking2").style.display = "none";
    document.getElementById("map2-content").style.display = "none";

	setTimeout(() => {

		petriRequestInternal();

		$("#loadingMessage").css("visibility", "hidden");

	}, 10);
}



function petriNetComputeExternal(){
	
	/*
	$("#loadingMessage").css("visibility", "visible");
	$("#back_slide_conf").css('visibility','hidden');
    $("#info_check_conformance").css('visibility','hidden');
    $("#btn_conformance").css('visibility','hidden');
	*/

	/*getNameDsl()*/

	/*
	document.getElementById("formConformanceChecking0").style.display = "none";
	document.getElementById("formConformanceChecking").style.display = "block";
    document.getElementById("formConformanceChecking2").style.display = "none";
    document.getElementById("map2-content").style.display = "none";

	setTimeout(() => {

		petriRequestExternal();
		petri_sample = document.getElementById("stringPetriNet").innerHTML
		petri_sample = petri_sample.replace(/&#34;/g, '"');
		petri_sample = petri_sample.replace(/&gt;/g, ">");
		petri_sample = petri_sample.replace(/&lt;/g, "<");
		petri_sample = petri_sample.replace(/●/g, " ");
		petri_sample = petri_sample.replace(/■/g, " ");

		$("#loadingMessage").css("visibility", "hidden");

	}, 10);
	*/
}




function getGraphText(selector){
	// take the graph in text form and save it 
	// F is annotated with frequencies
	// P is annotated with durations
	if ( selector == 'frequency')
		var graphText = document.getElementById("digraphF").innerHTML;
	else
		var graphText = document.getElementById("digraphP").innerHTML;
	// fix formatting
	graphText = graphText.replace(/&#34;/g, '"');
	graphText = graphText.replace(/&gt;/g, ">");
	// transform the string into an array, each line is an element
	// then remove from the array the first 3 and last 2 elements which are useless
	var graphText = graphText.split('\n\t').splice(3);
	graphText.pop();
	if ( selector != 'frequency' )
		graphText.pop();
	return graphText;
}

function getGraphNodes(graphText){
	// obtain the array of nodes with parameter 
	var graphNodes = [];
	var j = 0;
	// cycle throguh the graph array 
	for (var i=0; i<graphText.length; i++){
		// if this element has no '->' in the string, it's a node
		if (graphText[i].search('->') == -1){
			// if the current element is START
			if (graphText[i].search('\"@@startnode\"') != -1){
				// \"@@startnode\" [label=\"@@S\" fillcolor=\"#32CD32\" fontcolor=\"#32CD32\" shape=circle style=filled]"
				graphNodes[j] = graphText[i].split('\"').splice(1,1);
				graphNodes[j][1] = 'start';
				graphNodes[j][2] = '';
				graphNodes[j][3] = '';
				j++;
			}
			// if the current element is END
			else if (graphText[i].search('\"@@endnode\"') != -1){
				graphNodes[j] = graphText[i].split('\"').splice(1,1);
				graphNodes[j][1] = 'end';
				graphNodes[j][2] = '';
				graphNodes[j][3] = '';
				j++;
			}
			// otherwise
			else{
				// save it into the nodes array, remove useless info from the string and increment j
				// final array has two elements, (0):ID and (1):label
				graphNodes[j] = graphText[i].split(' [');
				graphNodes[j][1] = graphNodes[j][1].split('" ').shift().split('"').pop();
				// if additional info about performance of frequency are not useful 
				// add .split(' (').shift() to the previous line of code
				graphNodes[j][2] = graphNodes[j][1].split('(').pop().split(')').shift();
				graphNodes[j][1] = graphNodes[j][1].split('(').shift();
				graphNodes[j][1] = graphNodes[j][1].substring(0, graphNodes[j][1].length-1);
				j++;
			}
		}	
	}
	
	return graphNodes;
}

function getCombinedNodes(graphNodesF, graphNodesP){
	for (var i=0; i<graphNodesF.length; i++)
		graphNodesF[i][3] = graphNodesP[i][2];
	return graphNodesF;
}

function getGraphEdges(graphText, boolFreq){
	//obtain the array of edges with parameters
	var graphEdges = [];
	var j = 0;
	// cycle throguh the graph array 
	for (var i=0; i<graphText.length; i++){
		// if this element has '->' in the string, it's a node
		if (graphText[i].search('->') != -1){
			// save it into the nodes array, remove useless info from the string and increment j
			graphEdges[j] = graphText[i].split(' f').shift().split(' [');
			graphEdges[j][2] = graphEdges[j][1].split('=').pop();
			graphEdges[j][0] = graphEdges[j][0].split(' -> ');
			graphEdges[j][1] = graphEdges[j][0].pop();
			graphEdges[j][0] = graphEdges[j][0].shift();
			if (boolFreq){
				graphEdges[j][2] = graphEdges[j][2].split('\"').splice(1,1);
				graphEdges[j][2] = graphEdges[j][2].shift();
			}
			if (graphEdges[j][0] == '\"@@startnode\"' || graphEdges[j][1] =='\"@@endnode\"'){
				if (graphEdges[j][0].search('\"') != -1)
					graphEdges[j][0] = graphEdges[j][0].split('\"').splice(1,1).shift();
				if (graphEdges[j][1].search('\"') != -1)
					graphEdges[j][1] = graphEdges[j][1].split('\"').splice(1,1).shift();
				graphEdges[j][2] = '';
			}
			j++;
		}
	}
	
	return graphEdges;
}

function getCombinedEdges(graphEdgesF, graphEdgesP){
	for (var i=0; i<graphEdgesF.length; i++){
		graphEdgesF[i][3] = graphEdgesP[i][2];
		
	}
	return graphEdgesF;
}

function getLabeledGraphEdges(graphNodes, graphEdges){
	// for each couple of elements in graphEdges 
	for (var i=0; i<graphEdges.length; i++){
		// for each element in graphNodes
		for (var j=0; j<graphNodes.length; j++){
			// if they are equal, swap ID in edges for the Label in Nodes
			if ( graphEdges[i][0] === graphNodes[j][0])
				graphEdges[i][0] = graphNodes[j][1];
			if ( graphEdges[i][1] === graphNodes[j][0])
				graphEdges[i][1] = graphNodes[j][1];
		}
	}
	return 1;
}


function getDslStepsTemp(graphNodes, graphEdges){
	// from graphNodes and graphEdges get a unique matrix ready for conversion
	var dsl_Steps = [];
	// for each node 
	for (var i=0; i<graphNodes.length; i++){
		// save info about the node
		dsl_Steps[i] = [];
		dsl_Steps[i].push([graphNodes[i][1], graphNodes[i][2], graphNodes[i][3]]);
		// then, for each edge
		for (var j=0; j<graphEdges.length; j++){
			// check if the current node is the successor
			if (graphEdges[j][1] == graphNodes[i][1]){
				// in that case save the info about the edge
				dsl_Steps[i].push([graphEdges[j][0], graphEdges[j][2], graphEdges[j][3]]);
			}
		}
	}
	return dsl_Steps;
}

function getDslSteps(graphNodes, graphEdges){
	// from graphNodes and graphEdges get a unique matrix ready for conversion
	var dslSteps = [];
	// for each node 
	for (var i=0; i<graphNodes.length; i++){
		// save info about the node
		dslSteps[i] = [];
		dslSteps[i].push([graphNodes[i][1], graphNodes[i][2], graphNodes[i][3]]);
		// then, for each edge
		for (var j=0; j<graphEdges.length; j++){
			// check if the current node is the predecessor
			if (graphEdges[j][0] == graphNodes[i][1]){
				//save the info about the edge (successor name, frequency, duration)
				dslSteps[i].push([graphEdges[j][1], graphEdges[j][2], graphEdges[j][3]]);
			}
		}
	}
	// order DSL steps
	console.log(graphNodes);
	console.log(graphEdges);
	console.log(dslSteps);
	var orderedDslSteps = [];
	//populate start and end
	orderedDslSteps[0] = ['start', '', ''];
	orderedDslSteps[dslSteps.length-1] = ['end', '', ''];
	// for each step in ordered dslSteps (except 'end')
	for (var i=0; i<orderedDslSteps.length-1; i++){
		// for each step in dslSteps
		for (var j=0; j<dslSteps.length-1; j++){
			// if the current ordered step is the current step
			if (orderedDslSteps[i][0] == dslSteps[j][0][0]){
				// push the successor of the step in the next ordered step
				orderedDslSteps[i+1] = dslSteps[j][1];
				// remove the current item from the array to reduce iterations
				dslSteps.splice(j,1);
				// exit from the cycle to reduce iterations
				break;
			}
		}
	}
	console.log(orderedDslSteps);
	return orderedDslSteps;
}

// Function to download data to a file
function download(data, filename, type) {
    var file = new Blob([data], {type: type});
    if (window.navigator.msSaveOrOpenBlob) // IE10+
        window.navigator.msSaveOrOpenBlob(file, filename);
    else { // Others
        var a = document.createElement("a"),
                url = URL.createObjectURL(file);
        a.href = url;
        a.download = filename;
        document.body.appendChild(a);
        a.click();
        setTimeout(function() {
            document.body.removeChild(a);
            window.URL.revokeObjectURL(url);  
        }, 0); 
    }
}

