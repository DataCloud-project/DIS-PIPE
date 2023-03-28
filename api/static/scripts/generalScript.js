var petri_sample

function update(){
	document.getElementById('updated').value = true;
	document.getElementById('file').click();
	
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
		document.getElementById("get_dsl").disabled = true;
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
		document.getElementById("get_dsl").disabled = true;
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
		document.getElementById("get_dsl").disabled = false;
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
		document.getElementById("get_dsl").disabled = true;
		document.getElementById("send_dsl").disabled = true; 
		document.getElementById("load_project").disabled = true;
		document.getElementById("save_project").disabled = true; 
		document.getElementById("slidercontainer_cChecking").style.display = "none"

		document.getElementById("link_export_log").classList.remove('link_normal');
		document.getElementById("link_export_log").classList.add('link_disabled');
	}
}

function petriNetCompute(){
	
	$("#loadingMessage").css("visibility", "visible");
	$("#back_slide_conf").css('visibility','hidden');
    $("#info_check_conformance").css('visibility','hidden');
    $("#btn_conformance").css('visibility','hidden');

	setTimeout(() => {

		petriRequest();
		petri_sample=document.getElementById("stringPetriNet").innerHTML
		petri_sample = petri_sample.replace(/&#34;/g, '"');
		petri_sample = petri_sample.replace(/&gt;/g, ">");
		petri_sample = petri_sample.replace(/&lt;/g, "<");
		petri_sample = petri_sample.replace(/●/g, " ");
		petri_sample = petri_sample.replace(/■/g, " ");

		$("#loadingMessage").css("visibility", "hidden");

	}, 10);

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
	//obtain the array of edges with parameter
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
			// check if the current node is the successor
			if (graphEdges[j][1] == graphNodes[i][1]){
				// in that case save the info about the edge
				dslSteps[i].push([graphEdges[j][0], graphEdges[j][2], graphEdges[j][3]]);
			}
		}
	}
	return dslSteps;
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

