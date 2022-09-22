var color_highlight='#8B008B';
//##FFD23F
//#e9_ca1b
var disalignment_dictionary={}
var inizio=0

var tr_name=""
var log_name=""

var change_in_pnml=0

var color_node_highlight={}

var array_complete_activity=[] 
var array_moveinthemodel= [] 
var array_moveinthelog = [] 

function textareaInsert(){
    
}

function backPopUp(){
    
    var innerHTMLcode=""
    document.getElementById("myPopup_cChecking").innerHTML="<i class='fa-solid fa-square-pen'></i> Detail" +"<span class='close_pp' onclick='closePP2()'>❌</span>"+"<hr> <br>"+
                                                        "<table style='width: 100%;'>"+
                                                        
                                                        "<tr> <th style='text-decoration: underline;'>Activity</th> <th>times</th> <th>in traces</th> </tr>"+
                                                        "<tr> <th>skipped</th> <td style='border: revert;'> <span id='skip_act'> X2 </span> </td> <td style='border: revert;'> <span id='mean_skip_act'> X2 </span> of "+total_trace_number+" </td> </tr>"+
                                                        "<tr> <th>inserted</th> <td style='border: revert;'> <span id='ins_act'> X3 </span> </td> <td style='border: revert;'> <span id='mean_ins_act'> X2 </span> of "+total_trace_number+"</td> </tr>"+
                                                        "</table>"

                                                        
    $("#skip_act").text(r1[name_activity][1] )
    $("#ins_act").text(r1[name_activity][2] )
    $("#mean_skip_act").text(r2[name_activity][1] ).click(function(){
        
        console.log(r3[name_activity][1]) 
        
        innerHTMLcode=innerHTMLcode+"<i class='fa-solid fa-square-pen'></i> Acrivity skipped in:" +"<span class='close_pp_before'> <span style='border: 3px solid #a18df0;border-radius: 6px; cursor: pointer;' onclick='backPopUp()'>🔙</span> <span class='close_pp_inside' onclick='closePP2()'>❌</span></span>"+"<hr> <br>"
        
    
        for (var i = 0; i < r3[name_activity][1].length; i++) {
            innerHTMLcode=innerHTMLcode+"<div>"+r3[name_activity][1][i]+"</div>" 
        }

        document.getElementById("myPopup_cChecking").innerHTML=innerHTMLcode
    
    });
    $("#mean_ins_act").text(r2[name_activity][2] ).click(function(){ 
        
        console.log(r3[name_activity][0]) 
        
        innerHTMLcode=innerHTMLcode+"<i class='fa-solid fa-square-pen'></i> Acrivity inserted in:" +"<span class='close_pp_before'> <span style='border: 3px solid #a18df0;border-radius: 6px; cursor: pointer;' onclick='backPopUp()'>🔙</span> <span class='close_pp_inside' onclick='closePP2()'>❌</span></span>"+"<hr> <br>"
        
    
        for (var i = 0; i < r3[name_activity][0].length; i++) {
            innerHTMLcode=innerHTMLcode+"<div>"+r3[name_activity][0][i]+"</div>" 
        }

        document.getElementById("myPopup_cChecking").innerHTML=innerHTMLcode
    });
} 
var color_dictionary={}
var r1;
var r2;
var r3;
var name_activity;

function updateInfoListener(){


    console.log(this.responseText)
    var response = this.responseText.split("#")

    r1 = response[0]
    r1 = JSON.parse(r1.replace(/'/g,"\""));
  
    r2 = response[1]
    r2 = JSON.parse(r2.replace(/'/g,"\""));
  

    r3 = response[2]
    r3 = JSON.parse(r3.replace(/'/g,"\""));

    

    
    for (const [key, value] of Object.entries(r1)) {
        console.log(key, (value[1]+value[2])/(value[1]+value[2]+value[0]));
        color_dictionary[key]=(value[1]+value[2])/(value[1]+value[2]+value[0])  
    }

    
    

    $(".node").on('click', function(event){
        event.stopPropagation();
        var element_id=$(this).attr('id')

        const lst = element_id.slice(-1);
        var innerHTMLcode=""

        if(lst=="_"){
           
            //console.log(element_id);
            var selected_element=$(this).find("text").text().trim()
            name_activity=selected_element.replaceAll(" ","").toLowerCase()
            //console.log(name_activity)
            //console.log(r1[name_activity])
            $("#skip_act").text(r1[name_activity][1] )
            $("#ins_act").text(r1[name_activity][2] )
            $("#mean_skip_act").text(r2[name_activity][1] ).click(function(){
                
                //console.log(r3[name_activity][1]) 
                
                innerHTMLcode=innerHTMLcode+"<i class='fa-solid fa-square-pen'></i> Acrivity skipped in:" +"<span class='close_pp_before'> <span style='border: 3px solid #a18df0;border-radius: 6px; cursor: pointer;' onclick='backPopUp()'>🔙</span> <span class='close_pp_inside' onclick='closePP2()'>❌</span></span>"+"<hr> <br>"
                
            
                for (var i = 0; i < r3[name_activity][1].length; i++) {
                    innerHTMLcode=innerHTMLcode+"<div>"+r3[name_activity][1][i]+"</div>" 
                }

                document.getElementById("myPopup_cChecking").innerHTML=innerHTMLcode
            
            });
            $("#mean_ins_act").text(r2[name_activity][2] ).click(function(){ 
                
                //console.log(r3[name_activity][0]) 
                
                innerHTMLcode=innerHTMLcode+"<i class='fa-solid fa-square-pen'></i> Acrivity inserted in:" +"<span class='close_pp_before'> <span style='border: 3px solid #a18df0;border-radius: 6px; cursor: pointer;' onclick='backPopUp()'>🔙</span> <span class='close_pp_inside' onclick='closePP2()'>❌</span></span>"+"<hr> <br>"
                
            
                for (var i = 0; i < r3[name_activity][0].length; i++) {
                    innerHTMLcode=innerHTMLcode+"<div>"+r3[name_activity][0][i]+"</div>" 
                }

                document.getElementById("myPopup_cChecking").innerHTML=innerHTMLcode
             });
            
            
        }
        
    });

    
}   

function updateInfoRequest(){
    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", updateInfoListener);
	oReq.open("GET", frontend+"generalTraceInfo", false);
	oReq.send();
}




function highlightTrace(){
    if($("#highlight_trace option:selected").val()=="yes"){
        highlightTraceNode()
        
    }else {
        
        closePP2()
        closeEdge()
        
    } 
    

} 

function highlightTraceNode(){
    
    closePP2()
    closeEdge()

 
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

    

}


function updateTraceListener(){
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
            $('#aligner_space').append($('<span>').attr('style',"color: blue;").text(activity_name)) 
            matchbis = regex.exec(activity_name);
            if (matchbis) {
                response_matchbis=matchbis[1];
                //console.log(response_matchbis)
                //console.log(activity_name.replaceAll("("+response_matchbis+")", "").trim())
                array_complete_activity.push(activity_name.replaceAll("("+response_matchbis+")", "").trim())
                array_moveinthemodel.push(activity_name.replaceAll("("+response_matchbis+")", "").trim())
            }
        }else if(movetype=="moveinthelog"){
            array_complete_activity.push(activity_name)
            array_moveinthelog.push(activity_name)
            match = regex.exec(activity_name_array[2]);
            if (match) {
                response_match=match[1];
            }

            $('#aligner_space').append($('<span>').attr('style',"text-decoration: line-through; color:red;").text(activity_name+" ("+response_match+")" ) )
            
        
        }else if(movetype=="movesync"){
            array_complete_activity.push(activity_name)
            $('#aligner_space').append($('<span>').text(activity_name))
        } 

          

        $('#aligner_space').append($('<br>'))
    }

    highlightTraceNode();
 
}

function updateTraceRequest(){
    var traceSelectedName=String($("#trace_selected").val());
 
    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", updateTraceListener);
	oReq.open("GET", frontend+"updateTraceDetail?nameTrace="+traceSelectedName, false);
	oReq.send();
}

function jarListener() {
	// document.getElementById("allFq").innerHTML = this.responseText;
    console.log("jar listener")
}

function jarRequest(minLen,maxLen,chPlanner,chDuplicate) {
   
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", jarListener);
	oReq.open("GET", frontend+"jarCalling?minLen="+minLen+"&maxLen="+maxLen+"&planner="+chPlanner+"&duplicate="+chDuplicate, false);
	oReq.send();
}

function traceListener() {
	document.getElementById("traceDt").innerHTML = this.responseText;
    var traceNameList=this.responseText.split("#")
    
    listSort=traceNameList.sort()

    $('#selectBox').append($('<option>').val("out"+traceNameList[0]+".txt").text("out"+traceNameList[0]+".txt"))
   
    for (let i = 0; i < traceNameList.length; i++) {
          
        $('#trace_selected').append($('<option>').attr('value',"out"+traceNameList[i]+".txt").text("Trace "+traceNameList[i]))
    }
    
    
}

function traceRequest() {
   
	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", traceListener);
	oReq.open("GET", frontend+"traceDetail", false);
	oReq.send();
}

function updatePnml(first,after){

    var oReq = new XMLHttpRequest();
    var ciao=1
    //oReq.addEventListener("load", jarListener);
	oReq.open("POST", frontend+"mapPnml", true);
    
    oReq.setRequestHeader('Replace_content', first+"#"+after);
    oReq.send()
}

function checkPnmlExistence(){
    var oReq = new XMLHttpRequest();

    //oReq.addEventListener("load", jarListener);
	oReq.open("POST", frontend+"createRemap", true);
    oReq.send()
}

function getPnmlExistenceListener(){
    if(String(this.responseText)=="False"){
        checkPnmlExistence()
    }
} 


function getPnmlExistenceRequest(){
    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", getPnmlExistenceListener);
	oReq.open("GET", frontend+"getPnmlExistence", false);
	oReq.send();
} 

function updateCostFile(){
    
    var oReq = new XMLHttpRequest();
    var ciao=1
    //oReq.addEventListener("load", jarListener);
	oReq.open("POST", frontend+"costFile", true);
    
    oReq.setRequestHeader('Contenuto', JSON.stringify(disalignment_dictionary));
    oReq.send()
}


function backDivForm(){
    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "block";
    document.getElementById("formConformanceChecking2").style.display = "none";
    document.getElementById("map2-content").style.display = "none";
}
function backPrecDiv(){
    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "none";
    document.getElementById("formConformanceChecking2").style.display = "block";
    document.getElementById("map2-content").style.display = "none";
    // document.getElementById('graphContainer2').innerHTML = document.getElementById('graphContainer').innerHTML

}

function nextDivForm(){
    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "none";
    document.getElementById("formConformanceChecking2").style.display = "block";
    document.getElementById("map2-content").style.display = "none";

    getPnmlExistenceRequest();
}

function backToPersonalize(){
    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "none";
    document.getElementById("formConformanceChecking2").style.display = "block";
    document.getElementById("map2-content").style.display = "none";

    $("#back_slide_conf").css('visibility','hidden');
    $("#btn_conformance").css('visibility','hidden');

}

function petriRequest(){

    var oReq = new XMLHttpRequest();
    oReq.addEventListener("load", petriListener);
    oReq.open("GET", frontend+"conformanceChecking", false);
    oReq.send();

}

function petriListener(){
    var response=this.responseText.split("£")
    // console.log(response[0])
    // console.log(response[1])
    // console.log(response[2])
    // console.log(response[3])
    // console.log(response[4])
    tr_name=response[4]
    log_name=response[3]
    addTransitionName();
    document.getElementById("stringPetriNet").innerHTML = response[0];
    /*
    document.getElementById("InitialMarking").innerHTML = "Initial Marking: "+response[1];
    document.getElementById("FinalMarking").innerHTML = "Final Marking: "+response[2];
    */
}


    

function addTransitionName(){
    // Create text with HTML
    // console.log(tr_name)
    
    var tr_array = JSON.parse(tr_name.replace(/'/g,"\"") );
    var log_array=JSON.parse(log_name.replace(/'/g,"\"") );

    // console.log(log_array)
    // console.log(tr_array[0][1]);
    backDivForm();
    
    if(inizio==0){
        log_array.push("None");
        for (let i = 0; i < tr_array.length; i++) {

            if(tr_array[i][1]=="None"){
                $("#pnml_log_table").append("<tr style='background-color: revert; border: revert'> <td style='border: revert'>"+tr_array[i][0]+"</td> <td style='width: 40%; border: revert'></td> <td style='border: revert'> <select id="+tr_array[i][0].replaceAll(" ","_bis")+"_bis"+ " class='pnmlRemap'></select></td></tr>");
            }else{
                $("#pnml_log_table").append("<tr style='background-color: revert; border: revert'> <td style='border: revert'>"+tr_array[i][1]+"</td> <td style='width: 40%; border: revert'></td> <td style='border: revert'> <select id="+tr_array[i][1].replaceAll(" ","_bis")+"_bis"+ " class='pnmlRemap'></select></td></tr>");
            } 
            

            // console.log(tr_array[i][1])
            /*
            if(tr_array[i][1]=="None"){
                //tr_array[i][1]="InvisibleT"
                $("#transition_pnml").append("<p style='font-style: italic;'>"+tr_array[i][0]+"</p>","<br>");
            }else{
                $("#transition_pnml").append("<p>"+tr_array[i][1]+"</p>","<br>");
            }*/
            // A
            // sel.append($("<option>").attr('value',1).text(tr_array[i][1]));
            if(i==0){
                if(tr_array[i][1]=="None"){
                    $('#disalignment_log').append($("<option>").attr('value',tr_array[i][0]).text(tr_array[i][0]).attr('selected','selected'))
                }else{
                    $('#disalignment_log').append($("<option>").attr('value',tr_array[i][1]).text(tr_array[i][1]).attr('selected','selected'))
                } 
            }else{
                if(tr_array[i][1]=="None"){
                    $('#disalignment_log').append($("<option>").attr('value',tr_array[i][0]).text(tr_array[i][0]))
                }else{
                    $('#disalignment_log').append($("<option>").attr('value',tr_array[i][1]).text(tr_array[i][1]))
                } 
                
            }
            
            
            $('#move_log').text("0")
            $('#move_model').text("0")

            /*
            if(tr_array[i][1]=='None'){
                //disalignment_dictionary[tr_array[i][1]]=[0,0]
                var sel = $('<select>').attr('id',tr_array[i][0].replaceAll(" ","_bis")+"_bis"+"bode").attr("class","pnmlRemap")
            }else{
                var sel = $('<select>').attr('id',tr_array[i][1].replaceAll(" ","_bis")+"_bis"+"bode").attr("class","pnmlRemap")
            }
            */

            
        

            

            
            if(tr_array[i][1]=='None'){
                disalignment_dictionary[tr_array[i][0]]=[0,0]
                //disalignment_dictionary[tr_array[i][1]]=[0,0]
            }else{
                disalignment_dictionary[tr_array[i][1]]=[1,1]
            }

            var selected_elem=""
            var minimo=5000
            for (let j = 0; j < log_array.length; j++) {
                if(levenshteinDistance(tr_array[i][1],log_array[j])<minimo){
                    minimo=levenshteinDistance(tr_array[i][1],log_array[j])
                    selected_elem=log_array[j]
                }
            }

            for (let k = 0; k < log_array.length; k++) {

                if(tr_array[i][1]=="None"){
                    if(selected_elem==log_array[k]){
                        //sel.append($("<option>").attr('value',log_array[k]).text(log_array[k]).attr('selected','selected'));
                        $("#"+tr_array[i][0].replaceAll(" ","_bis")+"_bis").append($("<option>").attr('value',log_array[k]).text(log_array[k]).attr('selected','selected'));
                    }else{
                        //sel.append($("<option>").attr('value',log_array[k]).text(log_array[k]));
                        $("#"+tr_array[i][0].replaceAll(" ","_bis")+"_bis").append($("<option>").attr('value',log_array[k]).text(log_array[k]));
                    }  

                }else{
                    if(selected_elem==log_array[k]){
                        //sel.append($("<option>").attr('value',log_array[k]).text(log_array[k]).attr('selected','selected'));
                        $("#"+tr_array[i][1].replaceAll(" ","_bis")+"_bis").append($("<option>").attr('value',log_array[k]).text(log_array[k]).attr('selected','selected'));
                    }else{
                        //sel.append($("<option>").attr('value',log_array[k]).text(log_array[k]));
                        $("#"+tr_array[i][1].replaceAll(" ","_bis")+"_bis").append($("<option>").attr('value',log_array[k]).text(log_array[k]));
                    }  
                } 
                
                
            }
            //$('#transition_log').append(sel,"<br>","<br>");
            // console.log(String(i))
        }

        changedisalignment_select()
    

        // console.log(disalignment_dictionary)
        inizio=1
        $( ".pnmlRemap" ).change(function() {
            
            var first_1=this.id.replaceAll("_bis", " ").trim()
            //console.log(first_1);
            var prova=$('#'+this.id).find(":selected").val();
            var after_1=prova
            //console.log(after_1)
            updatePnml(first_1,after_1)
        });
    
    }
    // console.log(typeof tr_array);
    

    
    
}

function closeEdge(){

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

function closePP2(){
    $("#myPopup_cChecking").attr("class","popuptext")
   
    var prova=$('#graphContainer2')
    var prova2=prova.find("svg")
    var prova3=prova2.find("#graph0")

    var class3 = prova3.find('.node');
                
    $.each(class3, function(index, item) {
        var node_id=item.id
        var title_id=$("#"+item.id).find("title").html()
        var titolo=title_id.replaceAll(" ","").toLowerCase()
   
        //console.log(titolo)
        //console.log(color_dictionary[titolo])

        if(color_dictionary[titolo]==0){
            $("#"+item.id).find("polygon").attr('stroke',"#187F00")
            color_node_highlight[item.id]="#187F00"
        }else if(color_dictionary[titolo]<=0.1){
            $("#"+item.id).find("polygon").attr('stroke',"#FF9292")
            color_node_highlight[item.id]="#FF9292"
        }else if(color_dictionary[titolo]<=0.3){
            $("#"+item.id).find("polygon").attr('stroke',"#FF5C5C")
            color_node_highlight[item.id]="#FF5C5C"
        }else if(color_dictionary[titolo]<=0.5){
            $("#"+item.id).find("polygon").attr('stroke',"#FF3838")
            color_node_highlight[item.id]="#FF3838"
        }else{
            $("#"+item.id).find("polygon").attr('stroke',"#E20000")
            color_node_highlight[item.id]="#E20000"
        }
        $("#"+item.id).find("text").css({"text-decoration":"revert"});
        
    }) 
} 



function showResultonDFG(){

    $("#loadingMessage").css("visibility", "visible");
    $('html, body').animate({ scrollTop: 0 }, 'fast');
	setTimeout(() => {

    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "none";
    document.getElementById("formConformanceChecking2").style.display = "none";
    document.getElementById("map2-content").style.display = "block";
    // var gpContainer=$( "#graphContainer" ).clone()

    $("#btn_conformance").css('visibility', 'visible');
    $("#back_slide_conf").css('visibility', 'visible');
    
    
    var response2 = document.getElementById("digraphF").innerHTML

    response2 = response2.replace(/&#34;/g, '"');
    response2 = response2.replace(/&gt;/g, ">");
    response2 = response2.replace(/&lt;/g, "<");
    //response2 = response2.replace(/●/g, "  &#9679;"); //9679
    response2 = response2.replace(/●/g, " ");
    //response2 = response2.replace(/■/g, '    &#9724;'); //9632
    response2 = response2.replace(/■/g, " ");

    var options2 = {
      format: 'svg',
      ALLOW_MEMORY_GROWTH: 1,
      totalMemory: 537395200
      // format: 'png-image-element'
    }

    var image2 = Viz(response2, options2);

    var main2 = document.getElementById('graphContainer2');

    // main2.innerHTML = image2;
    // console.log(image2)		
    $(".node").find("polygon").attr('stroke', "#000000");
    $(".node").find("text").css({"text-decoration":"revert"});

    $(".edge").find("path").attr('stroke', "#000000");
    $(".edge").find("polygon").attr('stroke', "#000000");
    $(".edge").find("text").css({"text-decoration":"revert"});

    main2.innerHTML = document.getElementById('graphContainer').innerHTML;

    var prova=$('#graphContainer2')
    // console.log(prova)
    var prova2=prova.find("svg")
    // console.log(prova2.attr("width"))
    var prova3=prova2.find("#graph0")
    // console.log(prova3.attr("id","graph1"))
    // var targetNode = document.getElementById("graph1").getElementsByClassName("node")
    // var targetEdge = document.getElementById("graph1").getElementsByClassName("edge")
    // targetDiv.textContent = "Goodbye world!";

    // document.getElementById("node1").onclick= function(event) {
      // 	console.log("edge");
    // };
    


    var classList = prova3.find('.node');
    // console.log(classList)
                
    $.each(classList, function(index, item) {
        var node_id=item.id
        // console.log(node_id)
        item.id=node_id+"_"
        
        //ADDEDTEST
        var change_title=$("#"+item.id).find("title").html()
        // console.log(change_title)
        refactor_title=(change_title.split("(")[0])
        $("#"+item.id).find("text").html(refactor_title)
        $("#"+item.id).find("title").html(refactor_title)
        
        $("#"+item.id).find("polygon").attr('fill','#fdfdff')
        //ADDEDTEST

        $("#"+item.id).find("polygon").attr('stroke-width',4)
        var node_number = item.id
        var node_number = node_number.replace("node","");
        // console.log(node_number)
        var node_number= node_number.replace("_","");

        if(parseInt(node_number)%2==0){
            $("#"+item.id).find("polygon").attr('stroke',"green")
            color_node_highlight[item.id]="green"
        }else{
            $("#"+item.id).find("polygon").attr('stroke',"red")
            color_node_highlight[item.id]="red"
        }
        // console.log(node_number)
        // console.log(item.id)
        $('#'+item.id).click(function(e) {
            // alert('ho ho ho1');

            $temp= ($("#"+item.id).find("polygon").attr('stroke'))
            $check_node=($("#"+item.id).find("polygon")).length

            if(($temp=="#686868" || $temp=="#FFD23F" || $temp=="#000000" || $temp=="#187F00" || $temp=="#FF9292" || $temp=="#FF5C5C" || $temp=="#FF3838" || $temp=="#E20000") && $check_node!=0){

                

                // for (const element in Object.keys(color_node_highlight)) {
                //     $("#"+element).find("polygon").attr('stroke', color_node_highlight[element]);
                //     console.log(element)
                // }

                if(($temp=="#FFD23F" || $temp=="#686868")  && ($("#highlight_trace option:selected").val()=="yes")){

                    highlightTraceNode();
                    $("#"+item.id).find("polygon").attr('stroke', color_highlight);

                }else{

        

                for (var i=0;i<Object.keys(color_node_highlight).length;i++) {
                    var id_node=Object.keys(color_node_highlight)[i]
                    $("#"+id_node).find("polygon").attr('stroke', color_node_highlight[id_node]);
                    // console.log(id_node)
                    // console.log(color_node_highlight[id_node])
                }

                

                // $(".node").find("polygon").attr('stroke', color_node_highlight[item.id]);

                $(".node").find("text").css({"text-decoration":"revert"});

                $(".edge").find("path").attr('stroke', "#000000");
                $(".edge").find("polygon").attr('stroke', "#000000");
                $(".edge").find("text").css({"text-decoration":"revert"});


                if($temp=="red"){
                    $("#"+item.id).find("polygon").attr('stroke', '#f57e07');
                }else if($temp=="green"){
                    $("#"+item.id).find("polygon").attr('stroke', '#23f507');
                }else{
                    $("#"+item.id).find("polygon").attr('stroke', color_highlight);
                }
                
                }

                $("#"+item.id).find("text").css({"text-decoration":"underline"});

                document.getElementById("myPopup_cChecking").innerHTML= "<i class='fa-solid fa-square-pen'></i> Detail" +"<span class='close_pp' onclick='closePP2()'>❌</span>"+"<hr> <br>"+
                                                                        
                                                                        /*
                                                                        "Activity skipped <span id='skip_act'> X2 </span> times" +"<br /> <br />"+
                                                                        "Activity inserted <span id='ins_act'> X3 </span> times" +"<br /> <br />"+
                                                                        "Activity skipped in <span id='mean_skip_act'> X2 </span> traces"+"<br /> <br />"+
                                                                        "Activity inserted in <span id='mean_ins_act'> X2 </span> traces"+"<br /> <br />"*/
                                                                        "<table style='width: 100%;'>"+
                                                                        
                                                                        "<tr> <th style='text-decoration: underline;'>Activity</th> <th>times</th> <th>in traces</th> </tr>"+
                                                                        "<tr> <th>skipped</th> <td style='border: revert;'> <span id='skip_act'> X2 </span> </td> <td style='border: revert;'> <span id='mean_skip_act'> X2 </span> of "+total_trace_number+" </td> </tr>"+
                                                                        "<tr> <th>inserted</th> <td style='border: revert;'> <span id='ins_act'> X3 </span> </td> <td style='border: revert;'> <span id='mean_ins_act'> X2 </span> of "+total_trace_number+"</td> </tr>"+
                                                                        "</table>"
                $("#myPopup_cChecking").attr("class","popuptext show")
                openInfoCc(e)
                // console.log("you have selected a node")
                
            }else if(($("#highlight_trace option:selected").val()=="yes") && ($temp==color_highlight || $temp=='#23f507' || $temp=='#f57e07') && $check_node!=0){
                
                var title_id2=$("#"+item.id).find("title").html()
                var titolo2=title_id2.replaceAll(" ","").toLowerCase()
    
                if(array_complete_activity.includes(titolo2)) {
                    $("#"+item.id).find("polygon").attr('stroke', "#FFD23F");
                }else{
                    $("#"+item.id).find("polygon").attr('stroke', "#686868");
                } 
                
                
                $("#"+item.id).find("text").css({"text-decoration":"revert"});
                $("#myPopup_cChecking").attr("class","popuptext")
            }else if(($temp==color_highlight || $temp=='#23f507' || $temp=='#f57e07') && $check_node!=0){
                $("#"+item.id).find("polygon").attr('stroke', color_node_highlight[item.id]);
                $("#"+item.id).find("text").css({"text-decoration":"revert"});
                $("#myPopup_cChecking").attr("class","popuptext")
                // console.log("you have deselected a node")
            }

            
        });
        // $("#"+node_id).attr("id","ciao")
    })

    //console.log(color_node_highlight)

    var classList = prova3.find('.edge');
    // console.log(classList)
                
    $.each(classList, function(index, item) {
        var edge_id=item.id
        // console.log(edge_id)
        item.id=edge_id+"_"
        //ADDEDTEST
        $("#"+item.id).find("text").html("")
        $("#"+item.id).find("title").html("")
        //ADDEDTEST

        // console.log(item.id)
        /*
        $('#'+item.id).click(function() {
            // alert('ho ho ho2');

            $temp_polygon= ($("#"+item.id).find("polygon").attr('stroke'))
            $temp_path= ($("#"+item.id).find("path").attr('stroke'))
            $check_edge= ($("#"+item.id).find("polygon")).length
            var $titolo_1 = $("#"+item.id+" text").html();
    

            if($temp_polygon=="#000000" && $check_edge!=0 && $titolo_1!=undefined){

                $(".node").find("polygon").attr('stroke', "#000000");
                $(".node").find("text").css({"text-decoration":"revert"});

                $(".edge").find("path").attr('stroke', "#000000");
                $(".edge").find("polygon").attr('stroke', "#000000");
                $(".edge").find("text").css({"text-decoration":"revert"});

                $("#"+item.id).find("polygon").attr('stroke', color_highlight);
                $("#"+item.id).find("path").attr('stroke', color_highlight);
                $("#"+item.id).find("text").css({"text-decoration":"underline"});


                document.getElementById("myPopup_cChecking").innerHTML="You have selected an edge"
                $("#myPopup_cChecking").attr("class","popuptext show")
                // console.log("you have selected an edge")

            }else if($temp_polygon==color_highlight && $check_edge!=0 && $titolo_1!=undefined){

                $("#"+item.id).find("path").attr('stroke', "#000000");
                $("#"+item.id).find("polygon").attr('stroke', "#000000");
                $("#"+item.id).find("text").css({"text-decoration":"revert"});

                $("#myPopup_cChecking").attr("class","popuptext")
                // console.log("you have deselected an edge")
            }

            
        });*/
        // $("#"+edge_id).attr("id","ciao")
    })

    var jarPath;
    var pnmlPath;
    var xesPath;
    var costPath;
    
    var lengthMin=String(min_event_length);
    var lengthMax=String(max_event_length);
    if($('#trace_length').is(":checked")){
        var message_min = $('#minlen').val();
        if(message_min!="" && message_min!=null){
            //console.log(message_min)
            lengthMin=String(message_min)
        }

        var message_max = $('#maxlen').val();
        if(message_max!="" && message_max!=null){
            //console.log(message_max)
            lengthMax=String(message_max)
        }
    }

    

    var duplicateChoose;
    //console.log("duplicateChoose: "+$('#discard_check').is(":checked"))
    if($('#discard_check').is(":checked")){
        duplicateChoose="true"
    }else{
        duplicateChoose="false"    
    }
    
    
    var plannerChoose;
    if($('#FD').is(":checked")){
        plannerChoose="FD";
    }else if($('#SYMBA').is(":checked")){
        plannerChoose="SYMBA";
    }else if($('#Planner3').is(":checked")){
        plannerChoose="FD";
    }else {
        plannerChoose="FD";
    } 




    // document.getElementById('graphContainer2').innerHTML = document.getElementById('graphContainer').innerHTML

    // $(document).ready(function(){
    
    // 	$('#node1_').click(function() {
    // 		alert('ho ho ho');
    // 	});
    // });
    //updateCostFile();
    jarRequest(lengthMin,lengthMax,plannerChoose,duplicateChoose);
    traceRequest();
    updateInfoRequest();

    var class3 = prova3.find('.node');
    // console.log(class3)
                
    $.each(class3, function(index, item) {
        var node_id=item.id
        var title_id=$("#"+item.id).find("title").html()
        var titolo=title_id.replaceAll(" ","").toLowerCase()
   
        //console.log(titolo)
        //console.log(color_dictionary[titolo])

        if(color_dictionary[titolo]==0){
            $("#"+item.id).find("polygon").attr('stroke',"#187F00")
            color_node_highlight[item.id]="#187F00"
        }else if(color_dictionary[titolo]<=0.1){
            $("#"+item.id).find("polygon").attr('stroke',"#FF9292")
            color_node_highlight[item.id]="#FF9292"
        }else if(color_dictionary[titolo]<=0.3){
            $("#"+item.id).find("polygon").attr('stroke',"#FF5C5C")
            color_node_highlight[item.id]="#FF5C5C"
        }else if(color_dictionary[titolo]<=0.5){
            $("#"+item.id).find("polygon").attr('stroke',"#FF3838")
            color_node_highlight[item.id]="#FF3838"
        }else{
            $("#"+item.id).find("polygon").attr('stroke',"#E20000")
            color_node_highlight[item.id]="#E20000"
        }
        
    }) 





    var traceSelectedName=String($("#trace_selected").val());
    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", updateTraceListener);
	oReq.open("GET", frontend+"updateTraceDetail?nameTrace="+traceSelectedName, false);
	oReq.send();

    $('html, body').animate({ scrollTop: 0 }, 'fast');
    $("#loadingMessage").css("visibility", "hidden");

	}, 10);


}

function openInfoCc(e) {
// When the user clicks, open the popup
// When the user clicks, open the popup
    var pop=document.getElementById("myPopup_cChecking")
    // popup.classList.toggle("show");
    pop.style.left = e.clientX-100+200 + "px";
    pop.style.top = (e.clientY-280) + "px";
    //popup.style.top= "10%"
    //popup.style.left= "47%"
    pop.style.zIndez = "15";
    
}	


function trace_length_filter(){
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
    var changed_disalign=$('#disalignment_log option:selected').val();
    
    
    $("#move_model").val(disalignment_dictionary[changed_disalign][0])
    $("#move_log").val(disalignment_dictionary[changed_disalign][1])

}

function changedisalignment_dictionary(valore){
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

function displayTracePopUp(){
    if(document.getElementById("check_conformance").checked){
        document.getElementById("tabTrace").style.visibility = "visible";
        document.getElementById("mytabsConformance").style.display = "block";
    }else{
        document.getElementById("mytabsConformance").style.display = "none";
    }
}

window.onchange = function() {
	var regex = new RegExp(/^[\w\.,\s-]+\.xes$/);

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
		
}