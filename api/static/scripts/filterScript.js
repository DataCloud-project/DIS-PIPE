var history_crono={};

var listToFilter=[]

function change_filter_type(){
    event.stopImmediatePropagation();
    if(document.getElementById("filter_by").value == "resource"){
        var ar_Act=getAllUsedVariables("resources")
        document.getElementById("mfa2").style.visibility = "visible";
        document.getElementById("mode_for_attribute").style.visibility = "visible";
        document.getElementById("attribute_text").style.visibility = "visible";
        
    }else if (document.getElementById("filter_by").value == "costs"){
        var ar_Act=getAllUsedVariables("resources_cost")
        // var ar_Act=getAllUsedVariables("activities")
        document.getElementById("mfa2").style.visibility = "visible";
        document.getElementById("mode_for_attribute").style.visibility = "visible";
        document.getElementById("attribute_text").style.visibility = "visible";
    }else if (document.getElementById("filter_by").value == "variants"){
        var ar_Act=getAllUsedVariables("variant")
        document.getElementById("mfa2").style.visibility = "hidden";
        document.getElementById("mode_for_attribute").style.visibility = "hidden";
        document.getElementById("attribute_text").style.visibility = "hidden";

    }else if (document.getElementById("filter_by").value == "activity"){
        var ar_Act=getAllUsedVariables("activities")
        document.getElementById("mfa2").style.visibility = "visible";
        document.getElementById("mode_for_attribute").style.visibility = "visible";
        document.getElementById("attribute_text").style.visibility = "visible";
        
    }else if (document.getElementById("filter_by").value == "caseID"){
        var ar_Act=getAllUsedVariables("caseid")
        document.getElementById("mfa2").style.visibility = "hidden";	
        document.getElementById("mode_for_attribute").style.visibility = "hidden";
        document.getElementById("attribute_text").style.visibility = "hidden";
    }

    $('#fieldset3').empty();
    listToFilter=[]

    for (const act of ar_Act) {
        // console.log(act);
        addCheckbox(act)
    }
    
}


function addCheckbox(name) {
    var container = $('#fieldset3');
    var inputs = container.find('input');
    var id = inputs.length+1;
 
 //    $('<input />', { type: 'checkbox', id: 'cb'+id, value: name }).appendTo(container);
 //    $('<label />', { 'for': 'cb'+id, text: name }).appendTo('cb'+id);
    $('<label><input class="boxchecked" type="checkbox" id="'+name+'" value="'+name+'" onchange="cb(this.value, this.checked)">' +name+ '</label>').appendTo(container);
 }

 
function cb(valore,controllo){
    if(controllo) {
        // console.log(this.value)
        var valore_s=valore.replace(".", "@");
        listToFilter.push(valore_s)
        console.log(listToFilter)
    }else{
        var valore_s=valore.replace(".", "@");
        const index = listToFilter.indexOf(valore_s);
        if (index > -1) {
            listToFilter.splice(index, 1); // 2nd parameter means remove one item only
        }
        console.log(listToFilter)
    }
}

function showEvents2(varKey, caseKey) {
    //console.log("showEvents start")
    //console.log(varKey)
    //console.log(caseKey)

    //console.log("showevents2")
    
    allCases = json[dictionaryVariantNameReverse[varKey]];
    //console.log(allCases);
    
    allCases.forEach( (c) => {

        var ck = Object.values(c)[0]["concept:name"]; //case key
        //console.log(caseKey +" =?= "+ck);
        if(caseKey == ck){
            allEvents = Object.values(c)[1]; // all events
            //console.log(Object.keys(allEvents[0]))
        }
    });

    var table = document.getElementById("eventsTable");
    var tableHTML = "<tr>";
    
    var i = 0;
    var parameter_dicitionary={}
    var parameter_array=[]
    var param_new={}
    var indx=0
    var remove_index

    allEvents.forEach((e) => {
        var date = e["time:timestamp"].split(" ")[0];
        var time = e["time:timestamp"].split(" ")[1];   
      
        if(i==0){
            Object.entries(e).forEach((attr) => {
                if(!(parameter_array.includes(attr[0]))){
                    parameter_array.push(attr[0])
                } 
            });
        }  
    });
    //console.log(parameter_array)

    var parameter_array_sort=parameter_array.sort(function(x, y) {
        if(x=="concept:name" && y!="concept:name"){
            return -1
        }
        if(x!="concept:name" && y=="concept:name"){
            return 1
        }
        if(x!="concept:name" && y!="concept:name" && x[0]=="@" && y[0]!="@"){
            return 1
        }
        if(x!="concept:name" && y!="concept:name" && x[0]!="@" && y[0]=="@"){
            return -1
        }
        if(x.substring(0,5)=="case:" && y[0]!="@"){
            return 1
        }
        if(y.substring(0,5)=="case:" && x[0]!="@"){
            return -1
        }
        return x.localeCompare(y)
    });

    //console.log(parameter_array_sort)

    for(var pas=0; pas<parameter_array_sort.length; pas++){
        parameter_dicitionary[parameter_array_sort[pas]]=indx
		indx=indx+1
        if(parameter_array_sort[pas]=="Activity"){
            remove_index=indx
        }else if(parameter_array_sort[pas]=="concept:name"){
            tableHTML += "<th><h6>"+ "step" +"</h6><hr><br></th>" ;
        }else{
            tableHTML += "<th><h6>"+ parameter_array_sort[pas] +"</h6><hr><br></th>" ;
        }
    }
    //console.log(parameter_dicitionary)
    //console.log(Object.keys(parameter_dicitionary).length)
    inv_map = swap(parameter_dicitionary)  
    //console.log(inv_map)
    

    tableHTML += "</tr>"

    allEvents.forEach((e) => {
        //dates.push(new Date(e["time:timestamp"]));

        var date = e["time:timestamp"].split(" ")[0];
        var time = e["time:timestamp"].split(" ")[1];
        
        //console.log(Object.keys(inv_map))
        for (var kf = 0; kf < Object.keys(inv_map).length; kf++) {
            //console.log(Object.keys(inv_map))
            //console.log(inv_map[kf]);
            if(inv_map[kf]!="Activity"){
                if(e[inv_map[kf]]!=undefined){
                    tableHTML += "<td class='text-table' style='border-radius: 4px;'>" + e[inv_map[kf]] + "</td>" ;
                    // console.log(e[inv_map[kf]]);
                }else{
                    tableHTML += "<td class='text-table' style='border-radius: 4px;'>" + "N/A" + "</td>" ;
                }                
            }
        }
        tableHTML += "</tr>"
    });

    table.innerHTML = tableHTML;

    var liItems = document.getElementById("case-ul").getElementsByTagName("li"); 

    for (i = 0; i < liItems.length; i++) {
        //console.log(liItems[i].id == li.id);
        if(liItems[i].id == "case"+caseKey){
            liItems[i].setAttribute("style", "background-color: #7a4791; cursor: pointer; color:white; border-radius: 5px;");
            //selected = li.id;
        } else {
            liItems[i].setAttribute("style", "background-color: #dad3f8; cursor: pointer; color: black; border-radius: 5px;");
        }
    }
}


function showEvents(varKey, caseKey) {

    allCases = json[varKey];
    allCases.forEach( (c) => {
        var ck = Object.keys(c)[0]; //case key
        // console.log(caseKey +" =?= "+ck);
        if(caseKey == ck){
            allEvents = Object.values(c)[0]; // all events
        }
    });
    //  console.log(allEvents);
        
    var table = document.getElementById("eventsTable");
    //var tableHTML = "<tr><th>Activity</th><th>Resource</th><th>Date</th><th>Time</th><th>Duration</th><th>Role</th></tr>"
    var tableHTML = "<tr>";
    var i = 0;
    
//  console.log("fps")
//  console.log(allEvents[0]['concept:name'])
    var parameter_dicitionary={}
    var indx=0
    var remove_index

    allEvents.forEach((e) => {
    //dates.push(new Date(e["time:timestamp"]));

    var date = e["time:timestamp"].split(" ")[0];
    var time = e["time:timestamp"].split(" ")[1];
        
        
        //console.log(e);
        //console.log(Object.entries(e));
        
        
        if(i==0){
            // tableHTML += "<th><h6>Activity</h6><hr><br></th><th><h6>Resource</h6><hr><br></th>" ;
            Object.entries(e).forEach((attr) => {
            
                // console.log(attr) 
                // if(attr[0] == "time:timestamp") {
                // 	tableHTML += "<th><h6>Date</h6><hr><br></th><th><h6>Time</h6><hr><br></th>" ;
                // } else if (attr[0] == "Activity" || attr[0] == "Resource") {

                if(attr[0] in parameter_dicitionary){

                }else{
                    parameter_dicitionary[attr[0]]=indx
                    indx=indx+1
                    if(attr[0]=="Activity"){
                        remove_index=indx
                    }else if(attr[0]=="concept:name"){
                        tableHTML += "<th><h6>"+ "step" +"</h6><hr><br></th>" ;
                    }else{
                        tableHTML += "<th><h6>"+ attr[0] +"</h6><hr><br></th>" ;
                    }
                    
                }
                // }else {
                
                // }
            });
            // tableHTML += "<tr>";
            // i++;
        }
        inv_map = swap(parameter_dicitionary)
        // console.log(inv_map)
        
        // tableHTML += "<tr>"
        // tableHTML += "<td class='text-table' style='border-radius: 4px;'>" + e["Activity"] + "</td>"
        // tableHTML += "<td class='text-table' style='border-radius: 4px;'>" + e["Resource"] + "</td>"
        
        
        
        /*tableHTML += "<td>" + e["Activity"] + "</td>"
        tableHTML += "<td>" + e["Resource"] + "</td>"
        tableHTML += "<td>" + date + "</td>"
        tableHTML += "<td>" + time + "</td>"
        tableHTML += "<td>" + "Duration" + "</td>"
        tableHTML += "<td>" + "Role" + "</td>"*/
        // tableHTML += "</tr>"
    });
        
    tableHTML += "</tr>"

    allEvents.forEach((e) => {
        //dates.push(new Date(e["time:timestamp"]));

        var date = e["time:timestamp"].split(" ")[0];
        var time = e["time:timestamp"].split(" ")[1];
        
        
        // console.log(e)
        for (var kf = 0; kf < Object.keys(inv_map).length; kf++) {
            // console.log(inv_map[kf]);
            if(inv_map[kf]!="Activity"){
                if(e[inv_map[kf]]!=undefined){
                    tableHTML += "<td class='text-table' style='border-radius: 4px;'>" + e[inv_map[kf]] + "</td>" ;
                    // console.log(e[inv_map[kf]]);
                }else{
                    tableHTML += "<td class='text-table' style='border-radius: 4px;'>" + "N/A" + "</td>" ;
                }                
            }

            
            
        }

        // Object.entries(e).forEach((attr) => { 
            // if(attr[0] == "time:timestamp") {
            // 		tableHTML += "<td class='text-table' style='border-radius: 4px;'>"+date+"</td><td class='text-table' style='border-radius: 4px;'>"+time+"</td>" ;
            // } else if (attr[0] == "Activity" || attr[0] == "Resource") {

            // }else {
                // console.log(attr) 
                // tableHTML += "<td class='text-table' style='border-radius: 4px;'>" + attr[1] + "</td>" ;
            // }
        // });
        
        
        
        /*tableHTML += "<td>" + e["Activity"] + "</td>"
        tableHTML += "<td>" + e["Resource"] + "</td>"
        tableHTML += "<td>" + date + "</td>"
        tableHTML += "<td>" + time + "</td>"
        tableHTML += "<td>" + "Duration" + "</td>"
        tableHTML += "<td>" + "Role" + "</td>"*/
        tableHTML += "</tr>"
    });
        
    table.innerHTML = tableHTML;

    var liItems = document.getElementById("case-ul").getElementsByTagName("li"); 
    //console.log(liItems);
    for (i = 0; i < liItems.length; i++) {
        //console.log(liItems[i].id == li.id);
        if(liItems[i].id == "case"+caseKey){
            //console.log(liItems[i].id);
            //console.log(li.id);
            liItems[i].setAttribute("style", "background-color: #7a4791; cursor: pointer; color:white; border-radius: 5px;");
            //selected = li.id;
        }
        else {
            liItems[i].setAttribute("style", "background-color: #dad3f8; cursor: pointer; color: black; border-radius: 5px;");
        }
    }

}

function showCases2(li, key1) {
    //console.log("showcases2")
	var liItems = document.getElementById("variants-ul").getElementsByTagName("li"); 
    
	for (i = 0; i < liItems.length; i++) {
		//console.log(liItems[i].id == li.id);
        
		if(liItems[i].id == li.id){
			li.setAttribute("style", "background-color: #7a4791; cursor: pointer; color:white; ; border-radius: 5px;");
			selected = li.id;
		}else {
			liItems[i].setAttribute("style", "background-color: #dad3f8; cursor: pointer; color:black; ; border-radius: 5px;");
		}
        
	}

    
	allCases = json[dictionaryVariantNameReverse[key1]];
	ul2.innerHTML = "";
	//console.log(allCases)
    
    allCases.forEach( (c) => {
		//var caseKey = Object.keys(c)[0]; //case key old
        var caseKey = Object.values(c)[0]["concept:name"] //case key
		allEvents = Object.values(c)[1]; // all events
		
		var li2 = document.createElement('li');
        
		if("variant"+key1 == selected){
			li2.innerHTML = "<span class='text-table' style='border-radius: 4px;'>" + "Case " + caseKey + "</span><br><small class='text-table' style='border-radius: 4px;'>" + "Events: " + allEvents.length + '</small>';
			li2.setAttribute("style", "cursor: pointer; border-radius: 5px;");
			li2.setAttribute("id", "case"+ caseKey);

			li2.setAttribute("onclick", "showEvents2("+key1+",'"+caseKey+"')");
			ul2.append(li2);
		}
        
	});
    

};


function showCases(li, key1) {
    //console.log("showCases start")
	var liItems = document.getElementById("variants-ul").getElementsByTagName("li"); 
	//console.log(liItems);
	for (i = 0; i < liItems.length; i++) {
		//console.log(liItems[i].id == li.id);
		if(liItems[i].id == li.id){
			//console.log(liItems[i].id);
			//console.log(li.id);
			li.setAttribute("style", "background-color: #7a4791; cursor: pointer; color:white; ; border-radius: 5px;");
			selected = li.id;
		}
		else {
			liItems[i].setAttribute("style", "background-color: #dad3f8; cursor: pointer; color:black; ; border-radius: 5px;");
		}
	}
	//console.log(selected);
	
	allCases = json[key1];
	ul2.innerHTML = "";
	allCases.forEach( (c) => {
		var caseKey = Object.keys(c)[0]; //case key
		allEvents = Object.values(c)[0]; // all events
		
		
		var li2 = document.createElement('li');
		if("variant"+key1 == selected){
			li2.innerHTML = "<span class='text-table' style='border-radius: 4px;'>" + "Case " + caseKey + "</span><br><small class='text-table' style='border-radius: 4px;'>" + "Events: " + allEvents.length + '</small>';
			li2.setAttribute("style", "cursor: pointer; border-radius: 5px;");
			li2.setAttribute("id", "case"+ caseKey);
			//alert("ok");
			//li2.setAttribute("onclick", "alert('Variant "+ e +"')");
			li2.setAttribute("onclick", "showEvents("+key1+",'"+caseKey+"')");
			ul2.append(li2);
		}
	});


	/*
	
	
		
		Object.values(allEvents)[0].forEach((key3) => {
		  //----For each event I see its details
		  
			//----singleEvent = allEvents[key3];
			//console.log(singleEvent); //?
			//console.log(key3); //Contains details regarding signle case
			
			//console.log("Event: " + Object.keys(singleEvent)[key]);
			//console.log(Object.values(singleEvent)[key]);
		});
	  });*/
};

function swap(json){
    var ret = {};
    for(var key in json){
      ret[json[key]] = key;
    }
    return ret;
}


function openForm(id) {
	document.getElementById("blocker").style.display = "block";
	switch (id) {
	    case 'timeframe_filter':
            document.getElementById("filter_title").innerHTML = "Timeframe";
            document.getElementById("filter_info").style.visibility = "hidden";
            document.getElementById("timeframe_filter_div").style.display = "block";
            document.getElementById("perf_filter_div").style.display = "none";
            document.getElementById("endpoints_filter_div").style.display = "none";
            document.getElementById("attribute_filter_div").style.display = "none";
            document.getElementById("follower_filter_div").style.display = "none";
            document.getElementById("all_filters_button").style.display = "block";
            $("#myForm").removeClass("form-popup-scan");
            break;
	    case 'perf_filter':
            document.getElementById("filter_title").innerHTML = "Performance";
            document.getElementById("filter_info").style.visibility = "hidden";
            document.getElementById("perf_filter_div").style.display = "block";
            document.getElementById("timeframe_filter_div").style.display = "none";
            document.getElementById("endpoints_filter_div").style.display = "none";
            document.getElementById("attribute_filter_div").style.display = "none";
            document.getElementById("follower_filter_div").style.display = "none";
            document.getElementById("all_filters_button").style.display = "block";
            $("#myForm").removeClass("form-popup-scan");
            break;
	    case 'endpoints_filter':
            document.getElementById("filter_title").innerHTML = "Endpoints";
            document.getElementById("filter_info").style.visibility = "hidden";
            document.getElementById("endpoints_filter_div").style.display = "block";
            document.getElementById("timeframe_filter_div").style.display = "none";
            document.getElementById("perf_filter_div").style.display = "none";
            document.getElementById("attribute_filter_div").style.display = "none";
            document.getElementById("follower_filter_div").style.display = "none";
            document.getElementById("all_filters_button").style.display = "block";
            $("#myForm").removeClass("form-popup-scan");
            break;
	    case 'attribute_filter':
            document.getElementById("filter_title").innerHTML = "Attribute";
            document.getElementById("filter_info").style.visibility = "visible";
            document.getElementById("filter_info").title = "You can use it to split your log \nbased on a particular attribute, but also to \nfilter cases based on the presence or absence of activities, \nto filter case IDs and variants, or to remove individual \nactivities or attribute values"
            document.getElementById("attribute_filter_div").style.display = "block";
            document.getElementById("timeframe_filter_div").style.display = "none";
            document.getElementById("perf_filter_div").style.display = "none";
            document.getElementById("endpoints_filter_div").style.display = "none";
            document.getElementById("follower_filter_div").style.display = "none";
            document.getElementById("all_filters_button").style.display = "block";
            $("#myForm").removeClass("form-popup-scan");
            break;
	    case 'follower_filter':
            document.getElementById("filter_title").innerHTML = "Rule";
            document.getElementById("filter_info").style.visibility = "visible";
            document.getElementById("filter_info").title = "You can use it to filter your log based \non the defintion on a declare constraint"
            document.getElementById("follower_filter_div").style.display = "block";
            document.getElementById("timeframe_filter_div").style.display = "none";
            document.getElementById("perf_filter_div").style.display = "none";
            document.getElementById("endpoints_filter_div").style.display = "none";
            document.getElementById("attribute_filter_div").style.display = "none";
            document.getElementById("all_filters_button").style.display = "none";
            $("#myForm").addClass("form-popup-scan");

            scanRequest();
            break;
	    default:
		    document.getElementById("filter_title").innerHTML = "Sorry, there has been an error. No filter has been applied yet.";
	}
  document.getElementById("myForm").style.display = "block";
}

function closeForm() {
    deleteBtn()
    document.getElementById("myForm").style.display = "none";
    document.getElementById("blocker").style.display = "none";
    document.getElementById("check_filter").checked = false;
  }
  
function closeLoading(){
    document.getElementById("myLoading").style.display = "none";
    document.getElementById("blocker2").style.display = "none";
}
function closeRemove(){
    document.getElementById("myRemove").style.display = "none";
    document.getElementById("blocker3").style.display = "none";
}
function closeOrder(){
    document.getElementById("myOrder").style.display = "none";
    document.getElementById("blocker4").style.display = "none";
}
function closeChangement(){
    document.getElementById("myChange").style.display = "none";
    document.getElementById("blocker5").style.display = "none";
}

function dateMachRegex(date) {
	const regex = /^(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2})$/g;
	if (date.match(regex)) {
		return date;
	} else if ((date+":00").match(regex)) {
		return date+":00";
	}
	return "error";
}

async function asyncCall() {
    if(document.getElementById("perf_filter_div").style.display == "block" && (!($.isNumeric($("#input_max").val())) || !($.isNumeric($("#input_min").val())))){
        if(!($.isNumeric($("#input_max").val()))){
            alert("The max value inserted is not valid! Insert a number")
        }
        if(!($.isNumeric($("#input_min").val()))){
            alert("The min value inserted is not valid! Insert a number")
        }
    }else{
        //console.log('calling');
        //await $("#disappear_loading").css("visibility", "visible");
        await $("#loadingMessage").css("visibility", "visible");
        //document.getElementById("loadingMessage").style.visibility = "visible";	
        //const result = await applyFilter();
        $("#triggerFilterSpan").click()
        // expected output: "resolved"
    }
}

function triggerInvisibleFunction(){
	
	setTimeout(() => {
		applyFilter()
	}, 1);
	
}
  
function applyFilter() {


    // let p = new Promise((resolve,reject) =>{
    // 	document.getElementById("disappear_loading").style.visibility = "visible";	

    // // setTimeout(function(){ console.log("Hello"); }, 3000);

    //     resolve("Success")   
    // }).then((message) =>  {
    var tf_selected = document.getElementById("keep_timeframe").value;
    var pf_selected = document.getElementById("filter_by_performance").value;
    var af_selected = document.getElementById("filter_by").value;
    var af_selected_mode = document.getElementById("mode_for_attribute").value;

    var start_tf = document.getElementById("starting-time").value;
    start_tf=dateMachRegex(start_tf)
    // console.log(start_tf)
    var end_tf = document.getElementById("ending-time").value;
    end_tf=dateMachRegex(end_tf)

    start_tf = start_tf.replace("T", " ");
    end_tf = end_tf.replace("T", " ");
    // console.log(start_tf)
    // console.log(end_tf)




    var time1 = document.getElementById("select_time_1").value;
    var min = document.getElementById("input_min").value;
    var min_sec;
    switch(time1){
        case "mills": min_sec = (min / 1000); break;
        case "seconds": min_sec = min; break;
        case "minutes": min_sec = (min *60); break;
        case "hours": min_sec = (min *3600); break;
        case "days": min_sec = (min *86400); break;
        case "weeks": min_sec = (min *604800); break;
        case "years": min_sec = (min *3,154e+7); break; 
    }

    var time2 = document.getElementById("select_time_2").value;
    var max = document.getElementById("input_max").value;
    var max_sec;
    switch(time2){
        case "mills": max_sec = (max / 1000); break;
        case "seconds": max_sec = max; break;
        case "minutes": max_sec = (max *60); break;
        case "hours": max_sec = (max *3600); break;
        case "days": max_sec = (max *86400); break;
        case "weeks": max_sec = (max *604800); break;
        case "years": max_sec = (max *3,154e+7); break;
    }		

    var oReq = new XMLHttpRequest();
    //oReq.addEventListener("load", reqListenerFilter);
    oReq.addEventListener("load", reqListener3);

    if(document.getElementById("timeframe_filter_div").style.display == "block"){
        // document.getElementById("timeframe_filter").setAttribute("style", "background-color: #7a4791;");
        // document.getElementById("timeframe_remove").style.visibility = "visible";
        filtered_timeframe = true;
        //oReq.open("GET", frontend+"filterTimeframe"+"?start="+start_tf+"&end="+end_tf+"&timeframe="+tf_selected, false);
    }

    if(document.getElementById("perf_filter_div").style.display == "block"){		
        // document.getElementById("perf_filter").setAttribute("style", "background-color: #7a4791;");
        // document.getElementById("perf_remove").style.visibility = "visible";
        filtered_perf = true;
        //oReq.open("GET", frontend+"filterPerformance"+"?min="+String(min_sec)+"&max="+String(max_sec), false);
    }

    if(document.getElementById("attribute_filter_div").style.display == "block"){		
        // document.getElementById("attribute_filter").setAttribute("style", "background-color: #7a4791;");
        // document.getElementById("attribute_remove").style.visibility = "visible";
        filtered_attribute = true;
        //oReq.open("GET", frontend+"filterPerformance"+"?min="+String(min_sec)+"&max="+String(max_sec), false);
    }


    // console.log(end_tf)																		//min_event max_event				
    // console.log(start_tf)
    var listToFilter_plus=[]
    var plus_mode="0"
    var filtro=""

    if(af_selected=='variants' && document.getElementById("variants_original").innerHTML==String(0)){
        plus_mode="1"
        //var real_filter=document.getElementById("variants_info").innerHTML
        //data_filter_attr = JSON.parse(real_filter);
        
        for(var key_variant in listToFilter) {
            //console.log("la variante considerata è "+listToFilter[key_variant])
            var key_variant_used=listToFilter[key_variant].trim()
            //console.log(dictionaryVariantNameReverse[key_variant_used])
            var array_var_singolar=dictionaryVariantNameReverse[key_variant_used].split(",")
            //console.log(array_var_singolar)
            listToFilter_plus.push(array_var_singolar)
            /*
            for(var index in dictionaryVariantNameReverse[key_variant_used]){

                console.log(dictionaryVariantNameReverse[key_variant_used][index])
                console.log(String(Object.keys(dictionaryVariantNameReverse[key_variant_used][index])))
                listToFilter_plus.push(String(Object.keys(dictionaryVariantNameReverse[key_variant_used][index])))
                //console.log(listToFilter_plus)
            }
            */
        }
        //console.log(listToFilter_plus)
        listToFilter_plus=JSON.stringify(listToFilter_plus)
        

        filtro="filter"+ "?"+"filterTime="+filtered_timeframe+	"&timeframe="+tf_selected+	"&start="+start_tf+				"&end="+end_tf+

        "&filterPerf="+filtered_perf+		"&perfFrame="+pf_selected+	"&min="+String(min_sec)+		"&max="+String(max_sec)+
                                                                    "&minevent="+String(min)+		"&maxevent="+String(max)+
                                        
        "&filterAttr="+filtered_attribute +	"&attrFrame="+af_selected +	"&attrFilt="+af_selected_mode+	"&listattr="+listToFilter_plus + "&plusmode="+plus_mode


        
    }else{

    filtro="filter"+ "?"+"filterTime="+filtered_timeframe+	"&timeframe="+tf_selected+	"&start="+start_tf+				"&end="+end_tf+

                                        "&filterPerf="+filtered_perf+		"&perfFrame="+pf_selected+	"&min="+String(min_sec)+		"&max="+String(max_sec)+
                                                                                                    "&minevent="+String(min)+		"&maxevent="+String(max)+
                                                                        
                                        "&filterAttr="+filtered_attribute +	"&attrFrame="+af_selected +	"&attrFilt="+af_selected_mode+	"&listattr="+listToFilter + "&plusmode="+plus_mode
                                    }
                                  
    history_crono[String(indice)]=filtro

    listToFilter=[]
    listToFilter_plus=[]
    // console.log(history_crono)
    // console.log(history_crono.length)
    oReq.open("GET",frontend+filtro, false);
    oReq.send();
    initialVariantRequest();

    getMap(false);

    closeForm();
    closeLoading();

    allDurationRequest()
    allEdgeDurationRequest()
    csmEdgeRequest()
    csmRequest()

    var container = $('#history_div');
    if(filtered_timeframe){
        $('#history_span').attr("hidden", false); 
        // var inputs = container.find('input');
        // var id = inputs.length+1;

    //    $('<input />', { type: 'checkbox', id: 'cb'+id, value: name }).appendTo(container);
    //    $('<label />', { 'for': 'cb'+id, text: name }).appendTo('cb'+id);
        var idvalue="valore"+String(indice)
        var element=$("<div class='history_element' id="+idvalue+ "></div>")
        element.appendTo(container)
        $("<span class='element_title' value="+String(indice)+">"+String(indice)+") Timeframe:     \n" +tf_selected+' '+ '</span>').appendTo(element);
        $('<br>').appendTo(element);
        $("<button class='history_bottone' value="+String(indice)+' onclick="filterRemove(this.value)"'+'> ❌' +' '+ '</button>').appendTo(element);
        $('<span>&zwnj;</span>').appendTo(element);
        $("<button class='history_bottone' value="+String(indice)+' onclick="filterSwipeUp(this.value)"'+'>⬆️' + '</button>').appendTo(element);
        $('<span>&zwnj;</span>').appendTo(element);
        $("<button class='history_bottone' value="+String(indice)+' onclick="filterSwipeDown(this.value)"'+'>⬇️' + '</button>').appendTo(element);
        $('<div> &zwnj; </div>').appendTo(element);
        $('#'+idvalue).prop('title', start_tf +  "\n"+ end_tf);


    }
    if(filtered_perf){

        if(pf_selected=="caseDuration"){
            $('#history_span').attr("hidden", false); 	
            var idvalue="valore"+String(indice)
            var element=$("<div class='history_element' id="+idvalue+ "></div>")
            element.appendTo(container)
            $("<span class='element_title' value="+String(indice)+">"+String(indice)+ ") Perforamance: \n" +pf_selected+ '</span>').appendTo(element);
            $('<br>').appendTo(element);
            $("<button class='history_bottone' value="+String(indice)+' onclick="filterRemove(this.value)"'+'> ❌' +' '+ '</button>').appendTo(element);
            $('<span>&zwnj;</span>').appendTo(element);
            $("<button class='history_bottone' value="+String(indice)+' onclick="filterSwipeUp(this.value)"'+'>⬆️' + '</button>').appendTo(element);
            $('<span>&zwnj;</span>').appendTo(element);
            $("<button class='history_bottone' value="+String(indice)+' onclick="filterSwipeDown(this.value)"'+'>⬇️' + '</button>').appendTo(element);
            $('<div> &zwnj; </div>').appendTo(element);
            $('#'+idvalue).prop('title', min_sec +  "\n"+ max_sec);

        }else if(pf_selected=="eventsNumber"){
        
            $('#history_span').attr("hidden", false); 
            var idvalue="valore"+String(indice)
            var element=$("<div class='history_element' id="+idvalue+ "></div>")
            element.appendTo(container)
            $("<span class='element_title' value="+String(indice)+">"+String(indice)+ ") Performance: \n" +pf_selected+ '</span>').appendTo(element);
            $('<br>').appendTo(element);
            $("<button class='history_bottone' value="+String(indice)+' onclick="filterRemove(this.value)"'+'> ❌' +' '+ '</button>').appendTo(element);
            $('<span>&zwnj;</span>').appendTo(element);
            $("<button class='history_bottone' value="+String(indice)+' onclick="filterSwipeUp(this.value)"'+'>⬆️' + '</button>').appendTo(element);
            $('<span>&zwnj;</span>').appendTo(element);
            $("<button class='history_bottone' value="+String(indice)+' onclick="filterSwipeDown(this.value)"'+'>⬇️' + '</button>').appendTo(element);
            $('<div> &zwnj; </div>').appendTo(element);
            $('#'+idvalue).prop('title', min +  "\n"+ max);

        }
        
    }
    if(filtered_attribute){
        $('#history_span').attr("hidden", false);  
        var idvalue="valore"+String(indice)
        var element=$("<div class='history_element' id="+idvalue+ "></div>")
        element.appendTo(container)
        $("<span class='element_title' value="+String(indice)+">"+String(indice)+") Attribute:    \n " +af_selected+'\n '+"-"+ af_selected_mode+""+ '</span>').appendTo(element);
        $('<br>').appendTo(element);
        $("<button class='history_bottone' value="+String(indice)+' onclick="filterRemove(this.value)"'+'> ❌' +' '+ '</button>').appendTo(element);
        $('<span>&zwnj;</span>').appendTo(element);
        $("<button class='history_bottone' value="+String(indice)+' onclick="filterSwipeUp(this.value)"'+'>⬆️' + '</button>').appendTo(element);
        $('<span>&zwnj;</span>').appendTo(element);
        $("<button class='history_bottone' value="+String(indice)+' onclick="filterSwipeDown(this.value)"'+'>⬇️' + '</button>').appendTo(element);
        $('<div> &zwnj; </div>').appendTo(element);
        $('#'+idvalue).prop('title', listToFilter);
        
        
    }
    indice=indice+1;
    
    filtered_timeframe=false
    filtered_perf=false
    filtered_attribute=false


    // // setTimeout(() => , 1000);
    // // resolve("gtttt")
    // }).then((message) =>  {
    // 	console.log("this is the then "+message)
    // 	// document.getElementById("loadingMessage").style.visibility = "hidden";	

    $("#loadingMessage").css("visibility", "hidden");
    document.getElementById("myLoading").style.display = "block";
    document.getElementById("blocker2").style.display = "block";

    $("#myPathP").val("100");
    $("#myActP").val("100");
    $("#myPathF").val("100");
    $("#myActF").val("100");
    document.getElementById("pathP").innerHTML="100"
    document.getElementById("actP").innerHTML="100"
    document.getElementById("pathF").innerHTML="100"
    document.getElementById("actF").innerHTML="100"
    outputPF.innerHTML = sliderPF.value;
    outputPP.innerHTML = sliderPP.value;
    outputAF.innerHTML = sliderAF.value;
    outputAP.innerHTML = sliderAP.value;

    outputPF.value = sliderPF.value;
    outputPP.value = sliderPP.value;
    outputAF.value = sliderAF.value;
    outputAP.value = sliderAP.value;
    // })



}


function filterSwipeUp(id){

	if(Object.keys(history_crono).length==1){
		alert("Impossible to apply this filter")
	}else if(parseInt(id)==0){
		alert("Impossible to apply this filter")
	}else{

	$("#loadingMessage").css("visibility", "visible");
	// console.log(id)
	// console.log(history_crono)
	setTimeout(() => {
		var indice_up= String(parseInt(id)-1)
		var temp=history_crono[id]
		history_crono[id]=history_crono[indice_up]
		history_crono[indice_up]=temp
		// console.log(history_crono)

		console.log(id)
        console.log(indice_up)

		var v1 = $('#valore'+String(id)).html(),
			v2 = $('#valore'+(indice_up)).html();
	

		
		// console.log(v1)
		// console.log('#valore'+String(id))
		
		// console.log(v2)
		// console.log('#valore'+(indice_up))

		$('#valore'+String(id)).html(v2);
		$('#valore'+(indice_up)).html(v1);

		var classList_id = $('#valore'+String(id)+' .history_bottone');
							
		$.each(classList_id, function(index, item) {
			item.value=String(id)
		});

		var classList_id_up = $('#valore'+indice_up+' .history_bottone');
							
		$.each(classList_id_up, function(index, item) {
			item.value=indice_up
		});

		// var classList_title = $('#valore'+String(id)+' .element_title');
		var classList_title = $('#valore'+String(id)+' .element_title');
							
		$.each(classList_title, function(index, item) {
			$(this).value=String(id)
            console.log(String(id))
			var testo=$(this).html()
			var newtext= String(id)+") "+testo.split(")")[1].trim()
			$(this).html(newtext)
		});


		var classList_title_up = $('#valore'+indice_up+' .element_title');
							
		$.each(classList_title_up, function(index, item) {
			$(this).value=indice_up
            console.log(indice_up)
			var testo=$(this).html()
			var newtext= indice_up+") "+testo.split(")")[1].trim()
			$(this).html(newtext)
		});
		
        // $('#valore'+String(id)).html(v2);
        // $('#valore'+String(id)).html(v2);

        var temp = $('#valore'+String(id)).attr('title');
        console.log(temp)
        var temp2 = $('#valore'+indice_up).attr('title');
        console.log(temp2)

        $('#valore'+indice_up).attr('title',temp);
        $('#valore'+String(id)).attr('title',temp2);
		/* check to redesign start part	*/
		// freqRequest();
		// perfRequest();
		// variantRequest();
		swipeRemoveRequest();
        initialVariantRequest();
		apply_crono()

		$("#loadingMessage").css("visibility", "hidden");
		document.getElementById("myOrder").style.display = "block";
		document.getElementById("blocker4").style.display = "block";

		allDurationRequest()
		allEdgeDurationRequest()
		csmEdgeRequest()
		csmRequest()

	},1);
	}
}


function filterSwipeDown(id){

	if(Object.keys(history_crono).length==1){
		alert("Impossible to apply this filter")
	}else if(parseInt(id)==Object.keys(history_crono).length-1){
		alert("Impossible to apply this filter")
	}else{


	$("#loadingMessage").css("visibility", "visible");

	setTimeout(() => {
		var indice_up= String(parseInt(id)+1)
		var temp=history_crono[id]
		history_crono[id]=history_crono[indice_up]
		history_crono[indice_up]=temp
		// console.log(history_crono)

		var v1 = $('#valore'+String(id)).html(),
			v2 = $('#valore'+(indice_up)).html();
	
		$('#valore'+String(id)).html(v2);
		$('#valore'+(indice_up)).html(v1);


		var classList_id = $('#valore'+String(id)+' .history_bottone');
							
		$.each(classList_id, function(index, item) {
			item.value=String(id)
		});

		var classList_id_up = $('#valore'+indice_up+' .history_bottone');
							
		$.each(classList_id_up, function(index, item) {
			item.value=indice_up
		});

		var classList_title = $('#valore'+String(id)+' .element_title');
							
		$.each(classList_title, function(index, item) {
		
			$(this).value=String(id)
			var testo=$(this).html()
			var newtext= String(id)+") "+testo.split(")")[1].trim()
			$(this).html(newtext)
		});
						
		var classList_title_up = $('#valore'+indice_up+' .element_title');
												
		$.each(classList_title_up, function(index, item) {
		
			$(this).value=indice_up
			var testo=$(this).html()
			
			var newtext= indice_up+") "+testo.split(")")[1].trim()
			$(this).html(newtext)
			
		});


        var temp = $('#valore'+String(id)).attr('title');
        //console.log(temp)
        var temp2 = $('#valore'+indice_up).attr('title');
        //console.log(temp2)

        $('#valore'+indice_up).attr('title',temp);
        $('#valore'+String(id)).attr('title',temp2);
		/* check to redesign start part	*/
		// freqRequest();
		// perfRequest();
		// variantRequest();
		swipeRemoveRequest();
        initialVariantRequest();
		apply_crono()

		$("#loadingMessage").css("visibility", "hidden");
		document.getElementById("myOrder").style.display = "block";
		document.getElementById("blocker4").style.display = "block";

		allDurationRequest()
		allEdgeDurationRequest()
		csmEdgeRequest()
		csmRequest()


	},1);
	}
}





function filterRemove(id){
	$("#loadingMessage").css("visibility", "visible");
	var temp=history_crono[id]
	// console.log(id)
	// console.log(temp)
	
	setTimeout(() => {
		var history_length=(Object.keys(history_crono).length)
		var history_indice=id

		var v1 = $('#valore'+id).html();


		delete history_crono[id]
		// console.log(history_crono)
		$('#valore'+id).remove();
		// 
		// 
		
		
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		for (let i = 0; i < history_length; i += 1) {
			if(i>history_indice){
				var temp = history_crono[String(i)]
				history_crono[String(i-1)]=temp
				
				$('#valore'+String(i)).attr("id","valore"+String(i-1));
				
				var classList_id_up = $('#valore'+String(i-1)+' .history_bottone');	
				$.each(classList_id_up, function(index, item) {
					item.value=String(i-1)
				});


				var classList_title_up = $('#valore'+String(i-1)+' .element_title');
									
				$.each(classList_title_up, function(index, item) {
					$(this).value=String(i-1)
					// item.value=String(i-1)
					var testo=$(this).html()
					var newtext= String(i-1)+") "+testo.split(")")[1].trim()
					$(this).html(newtext)
					// $(this).value="4"
				});


				delete history_crono[String(i)]
			}
		// 	
		}

		// console.log(history_crono)
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		
		
		/* check to redesign start part	*/
		// freqRequest();	
		// perfRequest();
		// variantRequest();
		swipeRemoveRequest();
        initialVariantRequest();
		apply_crono();
		indice=indice-1

		$("#loadingMessage").css("visibility", "hidden");
		document.getElementById("myRemove").style.display = "block";
		document.getElementById("blocker3").style.display = "block";

		allDurationRequest()
		allEdgeDurationRequest()
		csmEdgeRequest()
		csmRequest()
		
	}, 1);

	
}

function apply_crono(){
	redo_function=true
	
	var oReq = new XMLHttpRequest();
	//oReq.addEventListener("load", reqListenerFilter);
	oReq.addEventListener("load", reqListener3);
	
	
	Object.keys(history_crono).forEach(function(key) {
        console.log("applycronoinside")
   		
		oReq.open("GET",frontend+history_crono[key], false);

		oReq.send();
		
	});

    initialVariantRequest();
	getMap(false);
	redo_function=false;
	
	if(Object.keys(history_crono).length === 0){
		
		$('#history_span').attr("hidden", true); 
	}

}