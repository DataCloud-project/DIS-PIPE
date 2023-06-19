var color_highlight='#8B008B';
//##FFD23F
//#e9_ca1b
var disalignment_dictionary={}
var inizio=0

//var tr_name=""
//var log_name=""

var change_in_pnml=0

var cambio_mapping=0

var color_dictionary={}
//var r1;
var r2;
var r3;
//var name_activity;


var array_complete_activity=[] 
var array_moveinthemodel= [] 
var array_moveinthelog = [] 


function backPopUp(){
    console.log("Function: backPopUp()")
    
    var innerHTMLcode=""
    document.getElementById("myPopup_cChecking").innerHTML="<i class='fa-solid fa-square-pen'></i> Detail" +"<span class='close_pp' onclick='closePP2()'>❌</span>"+"<hr> <br>"+
                                                        "<table style='width: 100%;'>"+
                                                        
                                                        "<tr> <th style='text-decoration: underline;'>Activity</th> <th>times</th> <th>in traces</th> </tr>"+
                                                        "<tr> <th>skipped</th> <td style='border: revert;'> <span id='skip_act'> X2 </span> </td> <td style='border: revert;'> <span id='mean_skip_act'> X2 </span> </td> </tr>"+ //of "+total_trace_number+"
                                                        "<tr> <th>inserted</th> <td style='border: revert;'> <span id='ins_act'> X3 </span> </td> <td style='border: revert;'> <span id='mean_ins_act'> X2 </span> </td> </tr>"+ //of "+total_trace_number+"
                                                        "</table>"
                                                        
    if(name_activity in r1){
        console.log("name activity sta in r1")
        console.log(r1[name_activity][1])
        console.log(r1[name_activity][2])
        $("#skip_act").text(r1[name_activity][1] )
        $("#ins_act").text(r1[name_activity][2] )
    }else if(name_activity_end in r1){
        console.log(r1[name_activity_end][1])
        console.log(r1[name_activity_end][2])
        $("#skip_act").text(r1[name_activity_end][1] )
        $("#ins_act").text(r1[name_activity_end][2] )
    }else{
        $("#skip_act").text("0" )
        $("#ins_act").text("0" )
    }

    var arraySkipIns=findNumberSkipIns(r3)
    console.log("sto stampando arraySkipIns")
    console.log(arraySkipIns)

    if(name_activity in r2){

        console.log("name activity sta in r2")
        try {
            console.log(arraySkipIns[name_activity][1])
            console.log(arraySkipIns[name_activity][0])
            var r2_name_Act_1=arraySkipIns[name_activity][1]
            var r2_name_Act_2=arraySkipIns[name_activity][0]
        }catch(error){
            console.error(error);
            var r2_name_Act_1="0"
            var r2_name_Act_2="0"
        }

    }else if(name_activity_end in r2){
        try {
            console.log(arraySkipIns[name_activity_end][1])
            console.log(arraySkipIns[name_activity_end][0])
            $("#skip_act").text(arraySkipIns[name_activity_end][1] )
            $("#ins_act").text(arraySkipIns[name_activity_end][0] )
            var r2_name_Act_1=arraySkipIns[name_activity_end][1]
            var r2_name_Act_2=arraySkipIns[name_activity_end][0]
        }catch(error){
            console.error(error);
            var r2_name_Act_1="0"
            var r2_name_Act_2="0"
        }
    }else{
        var r2_name_Act_1="0"
        var r2_name_Act_2="0"
    }
    $("#mean_skip_act").text(r2_name_Act_1).click(function(){
        innerHTMLcode=innerHTMLcode+"<i class='fa-solid fa-square-pen'></i> Acrivity skipped in:" +"<span class='close_pp_before'> <span style='border: 3px solid #a18df0;border-radius: 6px; cursor: pointer;' onclick='backPopUp()'>🔙</span> <span class='close_pp_inside' onclick='closePP2()'>❌</span></span>"+"<hr> <br>"
        if(name_activity in r2){
            var temp1=(r3[name_activity][1]).sort(function(a,b){
                a=a.replace("Trace ","")
                b=b.replace("Trace ","")
                return parseInt(a)-parseInt(b)})
        
            for (var i = 0; i < temp1.length; i++) {
                innerHTMLcode=innerHTMLcode+"<div>"+temp1[i]+"</div>" 
            }
        }else if(name_activity_end in r2){
            var temp1=(r3[name_activity_end][1]).sort(function(a,b){
                a=a.replace("Trace ","")
                b=b.replace("Trace ","")
                return parseInt(a)-parseInt(b)})
        
            for (var i = 0; i < temp1.length; i++) {
                innerHTMLcode=innerHTMLcode+"<div>"+temp1[i]+"</div>" 
            }
        }


        document.getElementById("myPopup_cChecking").innerHTML=innerHTMLcode
    
    });
    $("#mean_ins_act").text(r2_name_Act_2 ).click(function(){ 
        innerHTMLcode=innerHTMLcode+"<i class='fa-solid fa-square-pen'></i> Acrivity inserted in:" +"<span class='close_pp_before'> <span style='border: 3px solid #a18df0;border-radius: 6px; cursor: pointer;' onclick='backPopUp()'>🔙</span> <span class='close_pp_inside' onclick='closePP2()'>❌</span></span>"+"<hr> <br>"
        
        if(name_activity in r2){
            temp2=(r3[name_activity][0]).sort(function(a,b){
                a=a.replace("Trace ","")
                b=b.replace("Trace ","")
                return parseInt(a)-parseInt(b)})
        
            for (var i = 0; i < temp2.length; i++) {
                innerHTMLcode=innerHTMLcode+"<div>"+temp2[i]+"</div>" 
            }
        }else if(name_activity_end in r2){
            temp2=(r3[name_activity_end][0]).sort(function(a,b){
                a=a.replace("Trace ","")
                b=b.replace("Trace ","")
                return parseInt(a)-parseInt(b)})
        
            for (var i = 0; i < temp2.length; i++) {
                innerHTMLcode=innerHTMLcode+"<div>"+temp2[i]+"</div>" 
            }
        }

        document.getElementById("myPopup_cChecking").innerHTML=innerHTMLcode
    });

} 


function updateInfoListener(){
  
    const p_upInfo = new Promise((resolve, reject) => {  
        var response=this.responseText


        console.log("Function: updateInfoListener()")

        //console.log(this.responseText)
        var response = this.responseText.split("#")

        color_dictionary={}
        name_activity=""
        document.getElementById("myPopup_cChecking").innerHTML=""
        r1 = ""
        r1 = response[0]
        r1 = JSON.parse(r1.replace(/'/g,"\""));
    
        r2= ""
        r2 = response[1]
        r2 = JSON.parse(r2.replace(/'/g,"\""));
    

        r3=""
        r3 = response[2]
        r3 = JSON.parse(r3.replace(/'/g,"\""));

        console.log("sto stampando r1")
        console.log(r1)
        
        //console.log(r3);

        
        for (const [key, value] of Object.entries(r1)) {
            console.log(key, (value[1]+value[2])/(value[1]+value[2]+value[0]));
            color_dictionary[key]=(value[1]+value[2])/(value[1]+value[2]+value[0])  
        }
        console.log("sto stampando color_dictionary: "+color_dictionary)


        resolve(response); 
    });
    p_upInfo.then(
    (value) => {
        console.log(value); // Success!
    
        personalizeNode();

        var traceSelectedName=String($("#trace_selected").val());
        var oReq = new XMLHttpRequest();
        oReq.addEventListener("load", updateTraceListener);
        oReq.open("GET", frontend+"updateTraceDetail?nameTrace="+traceSelectedName, false);
        oReq.send();
    },
    (reason) => {
        console.error(reason); // Error!
    },
    ); 


}   

function updateInfoRequest(){
    console.log("Function: updateInfoRequest()")
    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", updateInfoListener);
	oReq.open("GET", frontend+"generalTraceInfo", false);
	oReq.send();
}




function highlightTrace(){
    console.log("Function: highlightTrace()")
    
    if($("#highlight_trace option:selected").val()=="yes"){
        highlightTraceNode()
        
    }else {
        
        closePP2()
        closeEdge()
        
    } 
    

} 

function highlightTraceNode(){
    console.log("Function: highlightTraceNode()")

    
    closePP2()
    closeEdge()

    /*provare commento
    if($("#highlight_trace option:selected").val()=="yes"){

        

        var prova=$('#graphContainer2')
        var prova2=prova.find("svg")
        var prova3=prova2.find("#graph0")


        var classList = prova3.find('.node');
                    
        $.each(classList, function(index, item) {
            var node_id=item.id
            
            var title_id=$("#"+item.id).find("title").html()
            var titolo=title_id.replaceAll(" ","").toLowerCase()
    
            if(array_complete_activity.includes(titolo)) {
                $("#"+item.id).find("polygon").attr('stroke',"#FFD23F")
            }else{
                $("#"+item.id).find("polygon").attr('stroke',"#686868")
            }      

        })

        var classEdge = $('.edge');
        $.each(classEdge, function(index, item) {
            var edge_id=item.id

            var title_id=$("#"+item.id).find(".label_edge").html()

            var source= title_id.split('-&gt;')[0].trim()
            var target= title_id.split('-&gt;')[1].trim()

            if(source=="@@startnode"){
                $("#"+item.id).find("path").attr('stroke', "#FFD23F");
                $("#"+item.id).find("polygon").attr('stroke', "#FFD23F");
            } 

            //console.log(array_complete_activity[array_complete_activity.length-1])
            //console.log(nodes_id_association[source])

            if(nodes_id_association[source]!=undefined && array_complete_activity[array_complete_activity.length-1]==nodes_id_association[source].replaceAll(" ","").toLowerCase() && target=="@@endnode"){
                $("#"+item.id).find("path").attr('stroke', "#FFD23F");
                $("#"+item.id).find("polygon").attr('stroke', "#FFD23F");
            }

            if(nodes_id_association[source]!=undefined && nodes_id_association[target]!=undefined){
        
                var index_array=(array_complete_activity.indexOf(nodes_id_association[source].replaceAll(" ","").toLowerCase()))

                if(index_array!=-1 && nodes_id_association[target].replaceAll(" ","").toLowerCase()==array_complete_activity[index_array+1]){
                    $("#"+item.id).find("path").attr('stroke', "#FFD23F");
                    $("#"+item.id).find("polygon").attr('stroke', "#FFD23F");
                }else if(index_array!=-1 && array_moveinthemodel.includes(nodes_id_association[target].replaceAll(" ","").toLowerCase()) && nodes_id_association[target].replaceAll(" ","").toLowerCase()==array_complete_activity[index_array+2]){
                    $("#"+item.id).find("path").attr('stroke', "#FFD23F");
                    $("#"+item.id).find("polygon").attr('stroke', "#FFD23F");
                } 
            } 

            for(var i=0; i<array_complete_activity.length; i++){

                if(nodes_id_association[source]!=undefined && nodes_id_association[source].replaceAll(" ","").toLowerCase()==array_complete_activity[i]  && nodes_id_association[target]!=undefined ){
                    if(i!=-1 && nodes_id_association[target].replaceAll(" ","").toLowerCase()==array_complete_activity[i+1]){
                        $("#"+item.id).find("path").attr('stroke', "#FFD23F");
                        $("#"+item.id).find("polygon").attr('stroke', "#FFD23F");
                    }else if(i!=-1 && array_moveinthemodel.includes(array_complete_activity[i+1]) && nodes_id_association[target].replaceAll(" ","").toLowerCase()==array_complete_activity[i+2]){
                        $("#"+item.id).find("path").attr('stroke', "#FFD23F");
                        $("#"+item.id).find("polygon").attr('stroke', "#FFD23F");
                    }
                } 
            } 
        })

    }else{
       
    }  
    */
    

}


function updateTraceListener(){
    console.log("Function: updateTraceListener()")

    var analise_response=this.responseText.split("$")
    
    var alignment=String(analise_response[0])
    alignment_array=alignment.split("\n")
    //console.log(alignment_array)

    array_complete_activity=[] 
    array_moveinthemodel= [] 
    array_moveinthelog = [] 

    

    $("#alignment_cost").text(String(analise_response[3]))
    $("#alignment_time").text(String(analise_response[1]))
    $("#plan_length").text(String(analise_response[2]))


    $("#aligner_space").empty();
    $("#aligner_space").append($('<span>').attr('style',"text-decoration: underline; font-size: 15px; color: green").text("-- ALIGNMENT --"))
    $('#aligner_space').append($('<br>'))

    for (let i = 0; i < alignment_array.length; i++) {
        //console.log(i)
        activity_name_array=alignment_array[i].split("#")
        activity_name=activity_name_array[1] 
        movetype=activity_name_array[0]
        
        var regex = /\((.*?)\)/g;
        var match;
        var response_match;

        

        if(movetype=="moveinthemodel"){
               
            matchbis = regex.exec(activity_name);
            var check_word
            if (matchbis) {
                response_matchbis=matchbis[1];
                //console.log(response_matchbis)
                //console.log(activity_name.replaceAll("("+response_matchbis+")", "").trim())
                array_complete_activity.push(activity_name.replaceAll("("+response_matchbis+")", "").trim())
                array_moveinthemodel.push(activity_name.replaceAll("("+response_matchbis+")", "").trim())
                check_word=activity_name.replaceAll("("+response_matchbis+")", "").trim()
            }
            if(!(invisible_steps.includes(check_word))){
                //console.log(check_word)
                $('#aligner_space').append($('<span>').attr('style',"color: blue;").text(activity_name)) 
                $('#aligner_space').append($('<br>'))
            } 
            
        }else if(movetype=="moveinthelog"){
            array_complete_activity.push(activity_name)
            array_moveinthelog.push(activity_name)
            match = regex.exec(activity_name_array[2]);
            if (match) {
                response_match=match[1];
            }
            if(!(invisible_steps.includes(activity_name))){
                //console.log(activity_name)
                $('#aligner_space').append($('<span>').attr('style',"text-decoration: line-through; color:red;").text(activity_name+" ("+response_match+")" ) )
                $('#aligner_space').append($('<br>'))
            }
        
        }else if(movetype=="movesync"){
            array_complete_activity.push(activity_name)
            if(!(invisible_steps.includes(activity_name))){
                //console.log(activity_name)
                $('#aligner_space').append($('<span>').text(activity_name))
                $('#aligner_space').append($('<br>'))
            }
        } 

          

        
    }

    highlightTraceNode();
 
}

function updateTraceRequest(){
    console.log("Function: updateTraceRequest()")

    var traceSelectedName=String($("#trace_selected").val());
 
    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", updateTraceListener);
	oReq.open("GET", frontend+"updateTraceDetail?nameTrace="+traceSelectedName, false);
	oReq.send();
}

function jarListener() {
	// document.getElementById("allFq").innerHTML = this.responseText;
    //console.log("jar listener")
    const p_jar = new Promise((resolve, reject) => {  
        var response=this.responseText
        resolve(response); 
    });
    p_jar.then((value) => {
        console.log(value); // Success!
        traceRequest();
    },(reason) => {
        console.error(reason); // Error!
    },); 
}

function jarRequest(minLen,maxLen,chPlanner,chDuplicate) {
    console.log("Function: jarRequest()")

	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", jarListener);
	oReq.open("GET", frontend+"jarCalling?minLen="+minLen+"&maxLen="+maxLen+"&planner="+chPlanner+"&duplicate="+chDuplicate, false);
	oReq.send();
}

function traceListener() {

    const p_trl = new Promise((resolve, reject) => {  
        var response=this.responseText


        console.log("Function: traceListener()")

        document.getElementById("traceDt").innerHTML = this.responseText;
        var traceNameList=this.responseText.split("#")
        
        var listSort=traceNameList.sort(function(a,b){
            return parseInt(a)-parseInt(b)})

        $('#selectBox').append($('<option>').val("out"+listSort[0]+".txt").text("out"+listSort[0]+".txt"))
    
        for (let i = 0; i < listSort.length; i++) {
            
            $('#trace_selected').append($('<option>').attr('value',"out"+listSort[i]+".txt").text("Trace "+listSort[i]))
        }

        resolve(response); 
    });
    p_trl.then((value) => {
        console.log(value); // Success!
        updateInfoRequest();
    },(reason) => {
        console.error(reason); // Error!
    },); 
    
    
}

function traceRequest() {
    console.log("Function: traceRequest()")

   
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", traceListener);
	oReq.open("GET", frontend+"traceDetail", false);
	oReq.send();
}

function updatePnml(first,after){

    console.log("Function: updatePnml()")

    var oReq = new XMLHttpRequest();
    var ciao=1
    //oReq.addEventListener("load", jarListener);
	oReq.open("POST", frontend+"mapPnml", true);
    
    oReq.setRequestHeader('Replace_content', first+"#"+after);
    oReq.send()
}

function updatePnmlBis(array){

    console.log("Function: updatePnml()")
    console.log(array)
    var oReq = new XMLHttpRequest();
    var ciao=1
    //oReq.addEventListener("load", jarListener);
	oReq.open("POST", frontend+"mapPnmlBis", true);
    
    oReq.setRequestHeader('Replace_content', JSON.stringify(array));
    oReq.send()
}

function checkPnmlExistence(){

    console.log("Function: checkPnmlExistence()")

    var oReq = new XMLHttpRequest();

    //oReq.addEventListener("load", jarListener);
	oReq.open("POST", frontend+"createRemap", true);
    oReq.send()
}

function getPnmlExistenceListener(){
    console.log("Function: getPnmlExistenceListener()")

    if(String(this.responseText)=="False"){
        checkPnmlExistence()
    }
} 


function getPnmlExistenceRequest(){
    console.log("Function: getPnmlExistenceRequest()")

    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", getPnmlExistenceListener);
	oReq.open("GET", frontend+"getPnmlExistence", false);
	oReq.send();
} 

function noMappingListener(){
    console.log(this.responseText)


}

function noMappingRequest(){
    console.log("Function: noMappingRequest()")

    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", noMappingListener);
	oReq.open("GET", frontend+"noMappingExistence", false);
	oReq.send();
}

function updateCostFile(){
    console.log("Function: updateCostFile()")
    
    var oReq = new XMLHttpRequest();
    var ciao=1
    //oReq.addEventListener("load", jarListener);
	oReq.open("POST", frontend+"costFile", true);
    
    oReq.setRequestHeader('Contenuto', JSON.stringify(disalignment_dictionary));
    oReq.send()
}


function backDivForm(){
    console.log("Function: backDivForm()")

    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "block";
    document.getElementById("formConformanceChecking2").style.display = "none";
    document.getElementById("map2-content").style.display = "none";
}
function backPrecDiv(){
    console.log("Function: backPrecDiv()")

    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "none";
    document.getElementById("formConformanceChecking2").style.display = "block";
    document.getElementById("map2-content").style.display = "none";
    // document.getElementById('graphContainer2').innerHTML = document.getElementById('graphContainer').innerHTML

}

function nextDivForm(){
    console.log("Function: nextDivForm()")

    var mapYes = document.getElementById('mapYes');
    var mapNo = document.getElementById('mapNo');
  
    // Check if mapping is allowed
    if (mapYes.checked && !mapNo.checked) {
        console.log("Mapping is allowed")
        // Mapping is allowed
        var confirmation = confirm("Do you want to apply the following mapping?");

        if (confirmation) {
            
            // Define an array to store the select IDs and their associated option values
            var selectData = [];
            var howUpdatePnml = []
            // Get the table element by its ID
            var table = document.getElementById('pnml_log_table');
            // Iterate over each row in the table
            for (var i = 0; i < table.rows.length; i++) {
                // Get the third cell (td) in each row, which contains the select element
                var selectCell = table.rows[i].cells[2];
                // Get the select element within the cell
                var selectElement = selectCell.querySelector('select');
                // Get the select ID and selected option value
                var selectId = selectElement.id;
                var selectedValue = selectElement.value;
    
                cambio_mapping=1
                var prima_1=selectId.replaceAll("_bis", " ").trim()
                console.log(prima_1);
                var dopo_1=selectedValue
                console.log(dopo_1)
                howUpdatePnml.push({ before_text: prima_1, after_text: dopo_1 });
                //updatePnml(prima_1,dopo_1)
                
                // Store the select ID and selected value in the array
                selectData.push({ id: selectId, value: selectedValue });
            }
            
            $('html, body').animate({ scrollTop: 0 }, 'fast');
            document.getElementById("formConformanceChecking").style.display = "none";
            document.getElementById("formConformanceChecking2").style.display = "block";
            document.getElementById("map2-content").style.display = "none";
    
            updatePnmlBis(howUpdatePnml)

            //getPnmlExistenceRequest();
    
        } else {
            
        }
        
    } else if(!mapYes.checked && mapNo.checked) {
        console.log("Mapping is not allowed")

        $('html, body').animate({ scrollTop: 0 }, 'fast');
        document.getElementById("formConformanceChecking").style.display = "none";
        document.getElementById("formConformanceChecking2").style.display = "block";
        document.getElementById("map2-content").style.display = "none";

        //getPnmlExistenceRequest();
        noMappingRequest();
      
    }  
}


function backStart(){
    console.log("Function: backStart()")

    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking0").style.display = "block";
    document.getElementById("formConformanceChecking").style.display = "none";
    document.getElementById("formConformanceChecking2").style.display = "none";
    document.getElementById("map2-content").style.display = "none";

}

function backToPersonalize(){
    console.log("Function: backToPersonalize()")

    document.getElementById("slidercontainer_cChecking").style.display = "none"

    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "none";
    document.getElementById("formConformanceChecking2").style.display = "block";
    document.getElementById("map2-content").style.display = "none";

    $("#back_slide_conf").css('visibility','hidden');
    $("#info_check_conformance").css('visibility','hidden');
    $("#btn_conformance").css('visibility','hidden');

}

function petriRequestInternal(){
    console.log("Function: petriRequestInternal()")

    var oReq = new XMLHttpRequest();
    oReq.addEventListener("load", petriListenerInternal);
    oReq.open("GET", frontend+"conformanceChecking", false);
    oReq.send();

}

function petriListenerInternal(){
    console.log("Function: petriListenerInternal()")
    console.log("non usato forse")

    var response=this.responseText.split("£")

    tr_name=response[4]
    log_name=response[3]

    addTransitionName(tr_name,log_name);
    document.getElementById("stringPetriNet").innerHTML = response[0];

    petri_sample = document.getElementById("stringPetriNet").innerHTML
    petri_sample = petri_sample.replace(/&#34;/g, '"');
    petri_sample = petri_sample.replace(/&gt;/g, ">");
    petri_sample = petri_sample.replace(/&lt;/g, "<");
    petri_sample = petri_sample.replace(/●/g, " ");
    petri_sample = petri_sample.replace(/■/g, " ");
    /*
    document.getElementById("InitialMarking").innerHTML = "Initial Marking: "+response[1];
    document.getElementById("FinalMarking").innerHTML = "Final Marking: "+response[2];
    */



    
    var oReq1 = new XMLHttpRequest();
    oReq1.addEventListener("load", getNameDslWorkBis);
    oReq1.open("POST", frontend+"deleteRemap", false);
    oReq1.send();
    

}

function getNameDslWorkBis(){
    console.log("Function: getNameDslWorkBis()")
    
    var oReq = new XMLHttpRequest();
    oReq.addEventListener("load", getNameDslListener);
    oReq.open("GET", frontend+"getDslName", false);
    oReq.send();

}

function petriRequestExternal(){
    console.log("Function: petriRequestExternal()")

    var oReq = new XMLHttpRequest();
    oReq.addEventListener("load", petriListenerExternal);
    oReq.open("GET", frontend+"conformanceChecking", false);
    oReq.send();

}

function petriListenerExternal(){
    console.log("non usato")
    console.log("Function: petriListenerExternal()")

    var response=this.responseText.split("£")

    tr_name=response[4]
    log_name=response[3]
    addTransitionName();
    document.getElementById("stringPetriNet").innerHTML = response[0];
    /*
    document.getElementById("InitialMarking").innerHTML = "Initial Marking: "+response[1];
    document.getElementById("FinalMarking").innerHTML = "Final Marking: "+response[2];
    */
}




var invisible_steps=[]

function addTransitionName(tr_name,log_name){
    console.log("Function: addTransitionName("+tr_name+","+log_name+")")
    
    //array con i nomi delle transizioni relative al dsl importato
    tr_array = JSON.parse(tr_name.replace(/'/g,"\"") );
    //array con i nomi delle attività del log
    log_array=JSON.parse(log_name.replace(/'/g,"\"") );
    
    backDivForm();
    
    //si può anche togliere l'if 
    //non ci sono problemi dal momento che la variabile inizio non cambia
    if(inizio==0){ 
        
        //rimuovere tutti elementi tabella
        $("#pnml_log_table").empty();
        //resettare le opzioni per il disalinneamento del log
        document.getElementById("disalignment_log").options.length = 0;
        
        //log_array.push("None");   Rimuovere il commento per mappare ad una attività invisible
        
        for (let j = 0; j < log_array.length; j++) {
           
           $('#disalignment_log').append($("<option>").attr('value',log_array[j]).text(log_array[j]))
            
        }

        //ciclo su array che contiene transizioni
        //utile per popolare la tabella per il mapping
        for (let i = 0; i < tr_array.length; i++) {
            if(tr_array[i][1]=="None"){ //caso in cui si ha una transizione invisibile
                //caso non preso in considerazione per l'utente
                $("#pnml_log_table").append("<tr style=' display: none; background-color: revert; border: revert'> <td style='border: revert'>"+tr_array[i][0]+"</td> <td style='width: 40%; border: revert'></td> <td style='border: revert'> <select id="+tr_array[i][0].replaceAll(" ","_bis")+"_bis"+ " class='pnmlRemap'></select></td></tr>");
                invisible_steps.push(tr_array[i][0].replaceAll(" ","").toLowerCase())
            }else{
                //caso in cui inserisco una riga con: 1) nome transizione 2)spazio 3)select da popolare con id=nome della transizione più .replaceAll(" ","_bis")+"_bis"
                $("#pnml_log_table").append("<tr style='background-color: revert; border: revert'> <td style='border: revert'>"+tr_array[i][1]+"</td> <td style='width: 40%; border: revert'></td> <td style='border: revert'> <select id="+tr_array[i][1].replaceAll(" ","_bis")+"_bis"+ " class='pnmlRemap'></select></td></tr>");
            } 
            
            //se stiamo aggiungendo per la prima volta una riga
            if(i==0){
                if(tr_array[i][1]=="None"){ //se transizione invisibile non fare nulla
                    //$('#disalignment_log').append($("<option>").attr('value',tr_array[i][0]).text(tr_array[i][0]).attr('selected','selected'))
                }else{ //altrimenti popolare la select con le opzioni per il disalinneamento del log e selezionare l'elemento
                    $('#disalignment_log').append($("<option>").attr('value',tr_array[i][1]).text(tr_array[i][1]).attr('selected','selected'))
                } 
            }else{
                if(tr_array[i][1]=="None"){ //se transizione invisibile non fare nulla
                    //$('#disalignment_log').append($("<option>").attr('value',tr_array[i][0]).text(tr_array[i][0]))
                }else{ //altrimenti popolare la select con le opzioni per il disalinneamento del log
                    $('#disalignment_log').append($("<option>").attr('value',tr_array[i][1]).text(tr_array[i][1]))
                } 
            }
            
            //Imposto a 0 i valori relativi al disalinneamento del log
            $('#move_log').text("0")
            //Imposto a 0 i valori relativi al disalinneamento del modello
            $('#move_model').text("0")

            //popolazione del dizionario relativo ai disalignment
            if(tr_array[i][1]=='None'){ //se la transizione è invisibile imposto a 0
                disalignment_dictionary[tr_array[i][0]]=[0,0]
            }else{ //altrimenti i valori base sono 1,1
                disalignment_dictionary[tr_array[i][1]]=[1,1]
            }

            var selected_elem=""
            var minimo=5000
            //Uso la distanza di levenshtein per trovare il miglior mapping per la transizione con l'attività del log
            for (let j = 0; j < log_array.length; j++) {
                if(levenshteinDistance(tr_array[i][1],log_array[j])<minimo){
                    minimo=levenshteinDistance(tr_array[i][1],log_array[j])
                    selected_elem=log_array[j]
                }
            }


            //loop per popolar la select che serve a mappare le transizioni con il log nel modello
            for (let k = 0; k < log_array.length; k++) {

                if(tr_array[i][1]=="None"){     //se la transizione è nulla, non so se serve veramente
                    if(selected_elem==log_array[k]){
                        //selezionare il miglior mapping se presente
                        $("#"+tr_array[i][0].replaceAll(" ","_bis")+"_bis").append($("<option>").attr('value',log_array[k]).text(log_array[k]).attr('selected','selected'));
                    }else{
                        $("#"+tr_array[i][0].replaceAll(" ","_bis")+"_bis").append($("<option>").attr('value',log_array[k]).text(log_array[k]));
                    }  

                }else{
                    if(selected_elem==log_array[k]){ 
                        //selezionare il miglior mapping se presente
                        $("#"+tr_array[i][1].replaceAll(" ","_bis")+"_bis").append($("<option>").attr('value',log_array[k]).text(log_array[k]).attr('selected','selected'));
                    }else{
                        $("#"+tr_array[i][1].replaceAll(" ","_bis")+"_bis").append($("<option>").attr('value',log_array[k]).text(log_array[k]));
                    }  
                } 
                
                
            }

        }


        for(let j=0; j< log_array.length; j++){
            disalignment_dictionary[log_array[j]]=[1,1]
        }
        

        changedisalignment_select()
    

        //inizio=1

        //funzione chiamata quando c'è un cambio di mapping
        
        $( ".pnmlRemap" ).change(function() {
            cambio_mapping=1
            /*
            var first_1=this.id.replaceAll("_bis", " ").trim()
            //console.log(first_1);
            var prova=$('#'+this.id).find(":selected").val();
            var after_1=prova
            //console.log(after_1)
            updatePnml(first_1,after_1)
            */
        });
    
    }
    
}

function closeEdge(){
    console.log("Function: closeEdge()")

    var prova=$('#graphContainer2')
    // console.log(prova)
    var prova2=prova.find("svg")
    // console.log(prova2.attr("width"))
    var prova3=prova2.find("#graph0")

    var class3 = prova3.find('.edge');
    // console.log(class3)
                
    $.each(class3, function(index, item) {
        
        var edge_id=item.id

        var title_id=$("#"+item.id).find(".label_edge").html()

        var source= title_id.split('-&gt;')[0].trim()
        var target= title_id.split('-&gt;')[1].trim()

        $("#"+item.id).find("path").attr('stroke', "#000000");
        $("#"+item.id).find("polygon").attr('stroke', "#000000");
         
        
    }) 

} 

function findCharPositions(word, char) {
    var positions = [];
    for (var i = 0; i < word.length; i++) {
      if (word.charAt(i) === char) {
        positions.push(i);
      }
    }
    return positions;
}

function closePP2(){
    console.log("Function: closePP2()")

    $("#myPopup_cChecking").attr("class","popuptext")
   
    var prova=$('#graphContainer2')
    var prova2=prova.find("svg")
    var prova3=prova2.find("#graph0")

    var class3 = prova3.find('.node');
                
    $.each(class3, function(index, item) {
        var node_id=item.id
        var title_id=$("#"+item.id).find("title").html()
        var titolo=title_id.replaceAll(" ","").replaceAll("-","_").replaceAll("_End","").replaceAll("_","").toLowerCase()
   
        var titolo_end = title_id.replaceAll(" ","").replaceAll("-","_").toLowerCase()

        console.log("Sono in closePP2, il titolo prima è:" + titolo)

        ///
        var selected_elem=""
        var minimo=5000
       
        for (let j = 0; j < log_array.length; j++) {
            if(levenshteinDistance(title_id,log_array[j])<minimo){
                minimo=levenshteinDistance(title_id,log_array[j])
                selected_elem=log_array[j]
            }
        }
        if(title_id.toLowerCase() in color_dictionary){
            titolo=title_id.toLowerCase()
            console.log("Sono in closePP2, primo if")
        }else if(title_id.toLowerCase().replaceAll(" ","").replaceAll("-end","") in color_dictionary){
            titolo=title_id.toLowerCase().replaceAll(" ","").replaceAll("-end","")
            console.log("Sono in closePP2, secondo if")
        }else if(title_id.toLowerCase().replaceAll(" ","").replaceAll("-end","").replaceAll("-","_") in color_dictionary){
            titolo=title_id.toLowerCase().replaceAll(" ","").replaceAll("-end","").replaceAll("-","_")
            console.log("Sono in closePP2, terzo if")
        }else{
            var stringa_temporanea=title_id.toLowerCase().replaceAll(" ","").replaceAll("-end","").replaceAll("-","_")
            var stringa_comparazione=selected_elem.toLowerCase().replaceAll(" ","")
            
            var stringa_finale=findPossibleTitolo(stringa_temporanea,stringa_comparazione)
                
            if(stringa_finale in color_dictionary){
                titolo=stringa_finale
            }

        }
        console.log("Sono in closePP2, il titolo dopo è:" + titolo)

        ///


        if(titolo in color_dictionary){
            if(color_dictionary[titolo]==0){
                $("#"+item.id).find("polygon").attr('stroke',"#187F00")
                color_node_highlight[item.id]="#187F00"
            }else if(color_dictionary[titolo]<=0.2){
                $("#"+item.id).find("polygon").attr('stroke',"#FF9292")
                color_node_highlight[item.id]="#FF9292"
            }else if(color_dictionary[titolo]<=0.5){
                $("#"+item.id).find("polygon").attr('stroke',"#FF5C5C")
                color_node_highlight[item.id]="#FF5C5C"
            }else if(color_dictionary[titolo]<=0.8){
                $("#"+item.id).find("polygon").attr('stroke',"#FF3838")
                color_node_highlight[item.id]="#FF3838"
            }else{
                $("#"+item.id).find("polygon").attr('stroke',"#E20000")
                color_node_highlight[item.id]="#E20000"
            }
        }else if(titolo_end in color_dictionary){
            if(color_dictionary[titolo_end]==0){
                $("#"+item.id).find("polygon").attr('stroke',"#187F00")
                color_node_highlight[item.id]="#187F00"
            }else if(color_dictionary[titolo_end]<=0.2){
                $("#"+item.id).find("polygon").attr('stroke',"#FF9292")
                color_node_highlight[item.id]="#FF9292"
            }else if(color_dictionary[titolo_end]<=0.5){
                $("#"+item.id).find("polygon").attr('stroke',"#FF5C5C")
                color_node_highlight[item.id]="#FF5C5C"
            }else if(color_dictionary[titolo_end]<=0.8){
                $("#"+item.id).find("polygon").attr('stroke',"#FF3838")
                color_node_highlight[item.id]="#FF3838"
            }else{
                $("#"+item.id).find("polygon").attr('stroke',"#E20000")
                color_node_highlight[item.id]="#E20000"
            }
        }else{
            $("#"+item.id).find("polygon").attr('stroke',"#999999")
            color_node_highlight[item.id]="#999999"
        }
        /*
        
        */
        $("#"+item.id).find("text").css({"text-decoration":"revert"});
        
    }) 
} 


function trace_length_filter(){
    console.log("Function: trace_length_filter()")
    

    if($('#trace_length').is(":checked")){
        $('#maxlen').prop('disabled', false);
        $('#minlen').prop('disabled', false);
        $('#define_length').css('color','black');
    }else{
        $('#maxlen').prop('disabled', true);
        $('#minlen').prop('disabled', true);
        $('#define_length').css('color','grey');
    }
    
}

function trace_date_filter(){
    console.log("Function: trace_date_filter()")

    if($('#trace_date').is(":checked")){
        $('#maxdate').prop('disabled', false);
        $('#mindate').prop('disabled', false);
        // $('#define_date').css('color','black');
        $('.d_date').css('color','black');
    }else{
        $('#maxdate').prop('disabled', true);
        $('#mindate').prop('disabled', true);
        // $('#define_date').css('color','grey');
        $('.d_date').css('color','grey');
    }
    
}

function changedisalignment_select(){
    console.log("Function: changedisalignment_select()")

    var changed_disalign=$('#disalignment_log option:selected').val();
    
    
    $("#move_model").val(disalignment_dictionary[changed_disalign][0])
    $("#move_log").val(disalignment_dictionary[changed_disalign][1])

}

function changedisalignment_dictionary(valore){
    console.log("Function: changedisalignment_dictionary("+valore+")")

    if(valore=="log"){
        
        
        var changed_disalign=$('#disalignment_log option:selected').val();
        //console.log(changed_disalign)
        //console.log($("#move_model").val())
        //disalignment_dictionary[tr_array[i][1]]=[0,0]
        var temp=disalignment_dictionary[changed_disalign]
        temp[1]=parseInt($("#move_log").val())
        disalignment_dictionary[changed_disalign]=temp
        

    }else if(valore=="model"){
        var changed_disalign=$('#disalignment_log option:selected').val();
        //console.log(changed_disalign)
        //console.log($("#move_model").val())
        var temp=disalignment_dictionary[changed_disalign]
        temp[0]=parseInt($("#move_model").val())
        disalignment_dictionary[changed_disalign]=temp
        

    }
    //console.log(disalignment_dictionary)
    updateCostFile();

}


function cambiaTitolo(){
    console.log("Function: cambiaTitolo()")

    if($('#keep_timeframe').find(":selected").val()=="contained"){
        $('#keep_timeframe').prop('title', $('#kp_contained').prop('title'));
    }else if($('#keep_timeframe').find(":selected").val()=="intersecting"){
        $('#keep_timeframe').prop('title', $('#kp_intersecting').prop('title'));
    }else if($('#keep_timeframe').find(":selected").val()=="started"){
        $('#keep_timeframe').prop('title', $('#kp_started').prop('title'));
    }else if($('#keep_timeframe').find(":selected").val()=="completed"){
        $('#keep_timeframe').prop('title', $('#kp_completed').prop('title'));
    }else if($('#keep_timeframe').find(":selected").val()=="trim"){
        $('#keep_timeframe').prop('title', $('#kp_trim').prop('title'));
    }

    
}


function displayTracePopUp(){
    console.log("Function: displayTracePopUp()")

    if(document.getElementById("check_conformance").checked){
        document.getElementById("tabTrace").style.visibility = "visible";
        document.getElementById("mytabsConformance").style.display = "block";
    }else{
        document.getElementById("mytabsConformance").style.display = "none";
    }
}

window.onchange = function() {
    console.log("Function: window.onchange function()")

	var regex = new RegExp(/^[\(\)\w\.,\s-]+\.xes$/);

	if (document.getElementById("file").files[0] != undefined && regex.test(document.getElementById("file").files[0]['name']) == false) {
		// console.log(document.getElementById("file").files[0])
		document.getElementById("file").value = ""
		alert("File not compatible");
		
		// document.getElementById("loadingMessage").style.visibility = "visible";
		// document.getElementById("updateBtn").click();
        // document.getElementById("loadingMessage").style.visibility = "visible";
	}else if (document.getElementById("file").files[0] != undefined) {
		//document.getElementById("loadingMessage").style.visibility = "visible";
		inizio=0
		document.getElementById("updateBtn").click();
        document.getElementById("loadingMessage").style.visibility = "visible";
	}
	


	var timeframe_selected = document.getElementById("keep_timeframe").value;
	var img = document.getElementById("img");
	var source = "../static/images/"
	img.src = source+timeframe_selected+".jpg";
	

	document.getElementById("starting-time").setAttribute("min", minDate);
	document.getElementById("starting-time").setAttribute("max", maxDate);
	document.getElementById("starting-time").setAttribute("value", minDate);
	document.getElementById("ending-time").setAttribute("min", minDate);	
	document.getElementById("ending-time").setAttribute("max", maxDate);
	document.getElementById("ending-time").setAttribute("value", maxDate);

    document.getElementById("mindate").setAttribute("min", minDate);
	document.getElementById("mindate").setAttribute("max", maxDate);
	document.getElementById("mindate").setAttribute("value", minDate);
	document.getElementById("maxdate").setAttribute("min", minDate);	
	document.getElementById("maxdate").setAttribute("max", maxDate);
	document.getElementById("maxdate").setAttribute("value", maxDate);
	
	var perf_selected = document.getElementById("filter_by_performance").value;
	if (perf_selected == "caseDuration") {
		document.getElementById("select_time_1").style.display = "inline-block";
		document.getElementById("select_time_2").style.display = "inline-block";
		document.getElementById("label_input_min").innerHTML = "Minimum duration: <small>(Use cases running longer than)</small>";
		document.getElementById("label_input_max").innerHTML = "Maximum duration: <small>(Use cases running shorter than)</small>";
	}
	if (perf_selected == "eventsNumber") {
		document.getElementById("select_time_1").style.display = "none";
		document.getElementById("select_time_2").style.display = "none";
		document.getElementById("label_input_min").innerHTML = "Minimum number of events";
		document.getElementById("label_input_max").innerHTML = "Maximum number of events";
	}
	
	var endpoints_selected = document.getElementById("mode_for_endpoints").value;
	if (endpoints_selected == "discard") {

		document.getElementById("endpoints_text").innerHTML = "This filter removes all cases in which the first and last events \r do not have one of the selected values.";
	}
	else {
		document.getElementById("endpoints_text").innerHTML = "This filter trims all cases to the sequence between"
																	+"the first occurence \r of a start value and the last occurrence of an end value.";
	}
	
	var attribute_selected = document.getElementById("mode_for_attribute").value;
	switch (attribute_selected) {
		case "selected":
			document.getElementById("attribute_text").innerHTML = "This filter removes all events that do not have one of the selected values.";
			break;
		case "mandatory":
			document.getElementById("attribute_text").innerHTML = "This filter removes all cases that do not have at least one event with one of the selected values.";
			break;
		case "forbidden":
			document.getElementById("attribute_text").innerHTML = "This filter removes all cases that have at least one event with one of the selected values.";
			break;
		default:
			document.getElementById("attribute_text").innerHTML = "";
	}
	
    /*
	var check_require = document.getElementById("require_for_follower");
	if (check_require.checked) {
		document.getElementById("mode_for_follower").disabled = false;
		document.getElementById("filter_by_follower").disabled = false;
	}
	else {
		document.getElementById("mode_for_follower").disabled = true;
		document.getElementById("filter_by_follower").disabled=true;
	}

	var check_time = document.getElementById("time_for_follower");
	if (check_time.checked) {
		document.getElementById("mode_time_for_follower").disabled = false;
		document.getElementById("select_time_3").disabled = false;
		document.getElementById("input_time_follower").disabled = false;
	}
	else {
		document.getElementById("mode_time_for_follower").disabled = true;
		document.getElementById("select_time_3").disabled = true;
		document.getElementById("input_time_follower").disabled = true;
	}
	*/
}