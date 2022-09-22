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
		menuBtn.checked = false;
		$("#highlight_trace").val('no').change();
		$(".edge").find("path").attr('stroke', "#000000");
		$(".edge").find("polygon").attr('stroke', "#000000");
		// $(".edge").find("text").css({"text-decoration":"revert"});

		
	}
	if (elanalysis.checked == true) {
		mapText.style.display = "none";
		elanalysisText.style.display = "block";
		cCheckingText.style.display = "none";
		menu.style.display = "none";
		content.style.maxWidth = "100%";
		menuBtn.checked = false;
	}
	if (cChecking.checked == true) {
		mapText.style.display = "none";
		elanalysisText.style.display = "none";
		cCheckingText.style.display = "block";
		menu.style.display = "none";
		content.style.maxWidth = "100%";
		menuBtn.checked = false;
	}
}

function cCheckingComputation(){
	$("#loadingMessage").css("visibility", "visible");
	setTimeout(() => {
	//
		petriRequest();
		petri_sample=document.getElementById("stringPetriNet").innerHTML
		petri_sample = petri_sample.replace(/&#34;/g, '"');
		petri_sample = petri_sample.replace(/&gt;/g, ">");
		petri_sample = petri_sample.replace(/&lt;/g, "<");
		//petri_sample = petri_sample.replace(/●/g, "  &#9679;"); //9679
		petri_sample = petri_sample.replace(/●/g, " ");
		//petri_sample = petri_sample.replace(/■/g, '    &#9724;'); //9632
		petri_sample = petri_sample.replace(/■/g, " ");
		// console.log(petri_sample)

		/*
		var petri_options = {
			format: 'svg',
			ALLOW_MEMORY_GROWTH: 1,
			totalMemory: 537395200
			// format: 'png-image-element'
		}
		var petri_image = Viz(petri_sample, petri_options);
		var petri_main = document.getElementById('petriContainer');

		petri_main.innerHTML = petri_image;


		const zvgZoom = document.getElementById("petriContainer");

		//Zooming and paning with mouse

		const svgContainer = document.getElementById("petrinet-content");
		//console.log(svgContainer);

		var viewBox = {x:0,y:0,w:"1920",h:"1080"};
		var attr = String(viewBox.x)+" "+String(viewBox.y)+" "+String(viewBox.w)+" "+String(viewBox.h);
		zvgZoom.setAttribute('viewBox', attr);


		const svgSize = {w:"1920",h:"1080"};
		//console.log("Initial svgSize: "+String(svgSize.w)+" "+String(svgSize.h));
		var isPanning = false;
		var startPoint = {x:0,y:0};
		var endPoint = {x:0,y:0};;
		var scale = 1;

		svgContainer.onmousewheel = function(e) {
			e.preventDefault();
			var w = viewBox.w;
			var h = viewBox.h;
			var mx = e.offsetX;//mouse x  
			var my = e.offsetY;    
			var dw = -w*Math.sign(e.deltaY)*0.05;
			var dh = -h*Math.sign(e.deltaY)*0.05;
			var dx = dw*mx/svgSize.w;
			var dy = dh*my/svgSize.h;
			viewBox = {x:viewBox.x+dx,y:viewBox.y+dy,w:viewBox.w-dw,h:viewBox.h-dh};
			scale = svgSize.w/viewBox.w;
			slider.value = scale*100;
			output.innerHTML = slider.value;
			//   zoomValue.innerText = String(Math.round(scale*100)/100);
			var attr = String(viewBox.x)+" "+String(viewBox.y)+" "+String(viewBox.w)+" "+String(viewBox.h);
			zvgZoom.setAttribute('viewBox', attr);
		}


		svgContainer.onmousedown = function(e){
			isPanning = true;
			startPoint = {x:e.x,y:e.y};   
		}

		svgContainer.onmousemove = function(e){
			if (isPanning){
				endPoint = {x:e.x,y:e.y};
				var dx = (startPoint.x - endPoint.x)/scale;
				var dy = (startPoint.y - endPoint.y)/scale;
				var movedViewBox = {x:viewBox.x+dx,y:viewBox.y+dy,w:viewBox.w,h:viewBox.h};
				var attr = String(movedViewBox.x)+" "+String(movedViewBox.y)+" "+String(movedViewBox.w)+" "+String(movedViewBox.h);
				zvgZoom.setAttribute('viewBox', attr);
			}
		}

		svgContainer.onmouseup = function(e){
		if (isPanning){ 
		endPoint = {x:e.x,y:e.y};
		var dx = (startPoint.x - endPoint.x)/scale;
		var dy = (startPoint.y - endPoint.y)/scale;
		viewBox = {x:viewBox.x+dx,y:viewBox.y+dy,w:viewBox.w,h:viewBox.h};   
		var attr = String(viewBox.x)+" "+String(viewBox.y)+" "+String(viewBox.w)+" "+String(viewBox.h);
		zvgZoom.setAttribute('viewBox', attr);
		isPanning = false;
		}
		}

		svgContainer.onmouseleave = function(e){
		isPanning = false;
		}
		*/

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

