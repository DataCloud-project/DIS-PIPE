function callSegmentator(){
    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", callSegmentatorListener);
	oReq.open("GET", frontend+"segmentator", false);
	oReq.send();
}


function callSegmentatorListener(){
    console.log(this.responseText)
}

function reloadLikeForm(){
	window.location.href = frontend+"indexSeg"
}

function startSegmentator(){
	console.log("chiamata start segmentator")

	if(total_trace_number==1 && segmemtator_array.length > 0){
		//document.getElementById('updatedSeg').value = true;
		console.log(segmemtator_array)
		var oReq = new XMLHttpRequest();
		oReq.open("POST", frontend+"startSegmentator", true);
		//oReq.setRequestHeader("Content-Type", "application/json");
	
		var jsonData = JSON.stringify(segmemtator_array);
		oReq.setRequestHeader("Array-Data", jsonData);
		oReq.addEventListener("load",reloadLikeForm())
		oReq.send();
	
	}else{
		alert("It is not possible to apply segmentator")
		closechoseEndSeg()
	}

}