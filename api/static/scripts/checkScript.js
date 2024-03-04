

function checkPercentage(caso) {
    if(caso=="PP"){
		if(!($.isNumeric(outputPP.value) && (parseInt(outputPP.value)>=1 && parseInt(outputPP.value)<=100))){
            alert("The value inserted is not valid! Insert a number between 1 and 100")
        }
	}else if(caso=="PF"){
	    if(!($.isNumeric(outputPF.value) && (parseInt(outputPF.value)>=1 && parseInt(outputPF.value)<=100))){
            alert("The value inserted is not valid! Insert a number between 1 and 100")
        }
	}else if(caso=="AF"){
		if(!($.isNumeric(outputAF.value) && (parseInt(outputAF.value)>=1 && parseInt(outputAF.value)<=100))){
            alert("The value inserted is not valid! Insert a number between 1 and 100")
        }
	}else if(caso=="AP"){
		if(!($.isNumeric(outputAP.value) && (parseInt(outputAP.value)>=1 && parseInt(outputAP.value)<=100))){
            alert("The value inserted is not valid! Insert a number between 1 and 100")
        }
	}
}

function checkPerformanceFilter(caso) {
    var min = document.getElementById("input_min").value;
	var max = document.getElementById("input_max").value;
    if(caso=="max"){
        if(!($.isNumeric(max))){
            alert("The value inserted is not valid! Insert a number")
        }
    }else if(caso=="min"){
        if(!($.isNumeric(min))){
            alert("The value inserted is not valid! Insert a number")
        }
    }

}

function checkPlannerChosen() {
    
    var planner_value=document.querySelector('input[name="planner_list"]:checked');
    if(planner_value==null){
        alert("Choose a planner")
        return false
    }else{
        return true
    }
}

function checkDisalignmentMove() {
    var moveModel = document.getElementById("move_model").value;
	var moveLog = document.getElementById("move_log").value;
    if(!($.isNumeric(moveModel))){
        alert("The disalignment model value inserted is not valid! Insert a number")
        return false
    }
    if(!($.isNumeric(moveLog))){
        alert("The disalignment log value inserted is not valid! Insert a number")
        return false
    }
    return true

}

function checkTraceLengthFilter() {
 
    var trace_length_value=document.querySelector('input[id="trace_length"]:checked');
    
    var minTrace = document.getElementById("minlen").value;
	var maxTrace = document.getElementById("maxlen").value;
    if(trace_length_value!=null){
        if(!($.isNumeric(minTrace))){
            alert("The min trace length value inserted is not valid! Insert a number")
            return false
        }
        if(!($.isNumeric(maxTrace))){
            alert("The max trace length value inserted is not valid! Insert a number")
            return false
        }
        if(maxTrace<minTrace){
            alert("The max trace length value should be bigger than max trace length value!")
            return false
        }
    }

    return true

}

function do_check_conformance(){
    if(document.getElementById("check_conformance").checked){
        //console.log("false")
        $('#check_conformance').prop('checked', false);
        displayTracePopUp()
    }else{
        //console.log("true")
        $('#check_conformance').prop('checked', true);
        displayTracePopUp()
    }

}