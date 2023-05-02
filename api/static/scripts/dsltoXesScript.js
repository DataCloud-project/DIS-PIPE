function getNameDsl(){

    $("#showRes").attr("onclick","showResultonDFGbis('esterno')");
    document.getElementById("blocker_dsl_name").style.display = "block";
    document.getElementById("popupDslName").style.display = "block";
    
    

    var oReq1 = new XMLHttpRequest();
    oReq1.addEventListener("load", getNameDslWork);
	oReq1.open("POST", frontend+"deleteRemap", false);
	oReq1.send();


}

function getNameDslWork(){

    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", getNameDslListener);
	oReq.open("GET", frontend+"getDslName", false);
	oReq.send();

    document.getElementById("listViewBtn").click();

}

function getNameDslListener(){
    var dslData=JSON.parse( this.responseText )
    console.log(dslData["data"])
    
    dslNameArray=[]
    
    for (let i in dslData["data"]) {
        dslNameArray.push(dslData["data"][i]["name"]);        
    }

    console.log(dslNameArray)
    var dslNameArraySingle = [...new Set(dslNameArray)];

    console.log(dslNameArraySingle);

    for (let j in dslNameArraySingle){
        
        $('#dslNameList').append('<div class="columnNameList" onclick="getDslbyName('+"'"+dslNameArraySingle[j]+"'"+')"><h3>'+dslNameArraySingle[j]+'</h3></div>');   

    }
}


function closePopupDslName(){
    document.getElementById("blocker_dsl_name").style.display = "none";
    document.getElementById("popupDslName").style.display = "none";
}


// Get the elements with class="column"
var elements = document.getElementsByClassName("columnNameList");

// Declare a loop variable
var i;

// List View
function listView() {
    for (i = 0; i < elements.length; i++) {
        elements[i].style.width = "100%";
    }
    /* Optional: Add active class to the current button (highlight it) */
    var container = document.getElementById("btnContainer");
    var btns = container.getElementsByClassName("btn");
    for (var i = 0; i < btns.length; i++) {
    btns[i].addEventListener("click", function() {
        var current = document.getElementsByClassName("active");
        current[0].className = current[0].className.replace(" active", "");
        this.className += " active";
    });
    }
}

// Grid View
function gridView() {
  for (i = 0; i < elements.length; i++) {
    elements[i].style.width = "50%";
  }
  /* Optional: Add active class to the current button (highlight it) */
    var container = document.getElementById("btnContainer");
    var btns = container.getElementsByClassName("btn");
    for (var i = 0; i < btns.length; i++) {
    btns[i].addEventListener("click", function() {
        var current = document.getElementsByClassName("active");
        current[0].className = current[0].className.replace(" active", "");
        this.className += " active";
    });
    }
}



function getDslbyName(nomeDsl){
    console.log(nomeDsl)

    $("#loadingMessage").css("visibility", "visible");
    $("#back_slide_conf").css('visibility','hidden');
    $("#info_check_conformance").css('visibility','hidden');
    $("#btn_conformance").css('visibility','hidden');
    $("#blocker_dsl_name").click();


	document.getElementById("formConformanceChecking0").style.display = "none";
	document.getElementById("formConformanceChecking").style.display = "block";
    document.getElementById("formConformanceChecking2").style.display = "none";
    document.getElementById("map2-content").style.display = "none";

    setTimeout(() => {

        var oReq = new XMLHttpRequest();
        oReq.addEventListener("load", getDslbyNameListener);
        oReq.open("GET", frontend+"getDslStructure?dslName="+nomeDsl, false);
        oReq.send();
		

		$("#loadingMessage").css("visibility", "hidden");

	}, 10);

}


function getDslbyNameListener(){
    
    var response=this.responseText.split("£")

    tr_name=response[4]
    log_name=response[3]
    nuovo_grafo=response[5]
    

    document.getElementById("new_digraph").innerHTML = nuovo_grafo

    console.log("it's a test")
    console.log(tr_name, log_name)

    addTransitionName(tr_name,log_name);
    
    document.getElementById("stringPetriNet").innerHTML = response[0];

    petri_sample = document.getElementById("stringPetriNet").innerHTML
    petri_sample = petri_sample.replace(/&#34;/g, '"');
    petri_sample = petri_sample.replace(/&gt;/g, ">");
    petri_sample = petri_sample.replace(/&lt;/g, "<");
    petri_sample = petri_sample.replace(/●/g, " ");
    petri_sample = petri_sample.replace(/■/g, " ");
}







function showResultonDFGbis(tipologia){
 

    if(checkPlannerChosen() && checkDisalignmentMove() && checkTraceLengthFilter()) {

        $("#loadingMessage").css("visibility", "visible");
        $('html, body').animate({ scrollTop: 0 }, 'fast');

        setTimeout(() => {

            document.getElementById("slidercontainer_cChecking").style.display = "block"

            $('#trace_selected').find('option').remove()

            $('html, body').animate({ scrollTop: 0 }, 'fast');
            document.getElementById("formConformanceChecking").style.display = "none";
            document.getElementById("formConformanceChecking2").style.display = "none";
            document.getElementById("map2-content").style.display = "block";
            // var gpContainer=$( "#graphContainer" ).clone()

            $("#btn_conformance").css('visibility', 'visible');
            $("#back_slide_conf").css('visibility', 'visible');
            $("#info_check_conformance").css('visibility','visible');
        
            var response2 = ""

            if(tipologia=="esterno"){
                response2 = document.getElementById("new_digraph").innerHTML
            }else{
                response2 = document.getElementById("digraphF").innerHTML
            }

            //response2 = document.getElementById("digraphF").innerHTML
            console.log("response 2")
            console.log(response2)
            

            response2 = response2.replace(/&#34;/g, '"');
            response2 = response2.replace(/&gt;/g, ">");
            response2 = response2.replace(/&lt;/g, "<");
            //response2 = response2.replace(/●/g, "  &#9679;"); //9679
            response2 = response2.replace(/●/g, " ");
            //response2 = response2.replace(/■/g, '    &#9724;'); //9632
            response2 = response2.replace(/■/g, " ");

            console.log(response2)

            var options2 = {
                format: 'svg',
                ALLOW_MEMORY_GROWTH: 1,
                totalMemory: 537395200    
            }

            var image2 = Viz(response2, options2);	

            var main2 = document.getElementById('graphContainer2');

            main2.innerHTML = image2;
            // console.log(image2)		
            
            
            $("#graphContainer2").find(".node").find("polygon").attr('stroke', "#000000");
            $("#graphContainer2").find(".node").find("text").css({"text-decoration":"revert"});

            $("#graphContainer2").find(".edge").find("path").attr('stroke', "#000000");
            $("#graphContainer2").find(".edge").find("polygon").attr('stroke', "#000000");
            $("#graphContainer2").find(".edge").find("text").css({"text-decoration":"revert"});

            //main2.innerHTML = document.getElementById('graphContainer').innerHTML;

            
            var prova=$('#graphContainer2')
            var prova2=prova.find("svg")
            var prova3=prova2.find("#graph0")
        
            var classList = prova3.find('.node');
            
                
            
            $.each(classList, function(index, item) {
                var node_id=item.id
                // console.log(node_id)
                item.id=node_id+"_"
                
                //ADDEDTEST cambiare forse 1
                
                var change_title=$("#"+item.id).find("text").html()
                console.log(change_title)
                
                refactor_title=(change_title.split("(")[0])
                console.log(refactor_title)

                $("#"+item.id).find("text").html(refactor_title)
                var font_size = $("#"+item.id).find("text").attr("font-size")
                $("#"+item.id).find("text").attr("font-size",font_size-2)
                
                $("#"+item.id).find("title").html(refactor_title)
                
                $("#"+item.id).find("polygon").attr('fill','#fdfdff')
                
                //ADDEDTEST cambiare forse 1

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
    
                
                $('#'+item.id).click(function(e) {

                    $temp= ($("#"+item.id).find("polygon").attr('stroke'))
                    $check_node=($("#"+item.id).find("polygon")).length

                    if(($temp=="#686868" || $temp=="#FFD23F" || $temp=="#000000" || $temp=="#187F00" || $temp=="#FF9292" || $temp=="#FF5C5C" || $temp=="#FF3838" || $temp=="#E20000") && $check_node!=0){

                        if(($temp=="#FFD23F" || $temp=="#686868")  && ($("#highlight_trace option:selected").val()=="yes")){

                            highlightTraceNode();
                            $("#"+item.id).find("polygon").attr('stroke', color_highlight);

                        }else{

                            for (var i=0;i<Object.keys(color_node_highlight).length;i++) {
                                var id_node=Object.keys(color_node_highlight)[i]
                                $("#"+id_node).find("polygon").attr('stroke', color_node_highlight[id_node]);
                                console.log(id_node)
                                console.log(color_node_highlight[id_node])
                            }

                        

                        $("#graphContainer2").find(".node").find("polygon").attr('stroke', color_node_highlight[item.id]);

                        $("#graphContainer2").find(".node").find("text").css({"text-decoration":"revert"});

                        $("#graphContainer2").find(".edge").find("path").attr('stroke', "#000000");
                        $("#graphContainer2").find(".edge").find("polygon").attr('stroke', "#000000");
                        $("#graphContainer2").find(".edge").find("text").css({"text-decoration":"revert"});


                        if($temp=="red"){
                            $("#"+item.id).find("polygon").attr('stroke', '#f57e07');
                            console.log("red")
                        }else if($temp=="green"){
                            $("#"+item.id).find("polygon").attr('stroke', '#23f507');
                            console.log("green")
                        }else{
                            $("#"+item.id).find("polygon").attr('stroke', color_highlight);
                            console.log("funziona")
                        }
                        
                        }

                        $("#"+item.id).find("text").css({"text-decoration":"underline"});

                        document.getElementById("myPopup_cChecking").innerHTML= "<i class='fa-solid fa-square-pen'></i> Detail" +"<span class='close_pp' onclick='closePP2()'>❌</span>"+"<hr> <br>"+
                                                                                
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
            
                    }

                    
                });
                
            })

            

            /* TESTARE ELIMINAZIONE 4

            var classList = prova3.find('.edge');
                        
            $.each(classList, function(index, item) {
                var edge_id=item.id
                // console.log(edge_id)
                item.id=edge_id+"_"
                //ADDEDTEST
                $("#"+item.id).find("text").html("")
                $("#"+item.id).find("title").html("")
                //ADDEDTEST
            })

            */

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

            

            jarRequest(lengthMin,lengthMax,plannerChoose,duplicateChoose);
            traceRequest();
            updateInfoRequest();

            
            var class3 = prova3.find('.node');
       
                    
            $.each(class3, function(index, item) {
                var node_id=item.id
                var title_id=$("#"+item.id).find("title").html()
                var titolo=title_id.replaceAll(" ","").replaceAll("-","_").toLowerCase()
        
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
            
            console.log(color_dictionary)
            console.log(color_node_highlight)
            console.log(color_node_highlight["node3_"])
            console.log($("#node3_").find("polygon").attr('stroke'))

            
            for (var i=0;i<Object.keys(color_node_highlight).length;i++) {
                var id_node=Object.keys(color_node_highlight)[i]
                $("#"+id_node).find("polygon").attr('stroke', color_node_highlight[id_node]);
                console.log(id_node)
                console.log(color_node_highlight[id_node])
            }

            console.log("it's a test")


            var traceSelectedName=String($("#trace_selected").val());
            var oReq = new XMLHttpRequest();
            oReq.addEventListener("load", updateTraceListener);
            oReq.open("GET", frontend+"updateTraceDetail?nameTrace="+traceSelectedName, false);
            oReq.send();

            $('html, body').animate({ scrollTop: 0 }, 'fast');
            $("#loadingMessage").css("visibility", "hidden");

            for (var i=0;i<Object.keys(color_node_highlight).length;i++) {
                var id_node=Object.keys(color_node_highlight)[i]
                $("#"+id_node).find("polygon").attr('stroke', color_node_highlight[id_node]);
                console.log(id_node)
                console.log(color_node_highlight[id_node])
            }

        }, 10);
       
    }

    function openInfoCc(e) {
        // When the user clicks, open the popup
        // When the user clicks, open the popup
        var pop=document.getElementById("myPopup_cChecking")
        // popup.classList.toggle("show");
        pop.style.left = e.clientX+50 + "px";
        pop.style.top = (e.clientY-150) + "px";
        //popup.style.top= "10%"
        //popup.style.left= "47%"
        pop.style.zIndez = "15";
    }
}	



