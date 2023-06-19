function callSegmentator(){
    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", callSegmentatorListener);
	oReq.open("GET", frontend+"segmentator", false);
	oReq.send();
}


function callSegmentatorListener(){
    console.log(this.responseText)
}