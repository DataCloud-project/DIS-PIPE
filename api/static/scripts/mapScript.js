var color_highlight='#8B008B';

// var nodes_id_association={}

var graphTextF;
var graphTextP;
var graphNodesF;
var graphNodesP;
var graphNodes;
var graphEdgesF;
var graphEdgesP;
var graphEdges;
var dslSteps;

var meanDurationFP;
var meanEdgeDuration;

var $total_mean=0
var $total_min=0
var $total_max=0
var $total_median=0
var $total_total=0

var copia_sample


// function used to update simultaneously the slider and the textarea
function updateInfo(caso){
	if(caso=="PP"){
        if(!($.isNumeric(outputPP.value) && (parseInt(outputPP.value)>=1 && parseInt(outputPP.value)<=100))){
            alert("The value inserted is not valid! Insert a number between 1 and 100")
        }else{
            sliderPP.value = outputPP.value
            sliderPF.value = outputPP.value
            outputPF.value =outputPP.value
            outputPF.innerHTML =outputPP.value

            $(".change_form").css("visibility", "visible");
            $(".dis_submit").addClass("dosubmit");
            $(".dis_submit").attr('disabled', false);
        }

	}else if(caso=="PF"){
        if(!($.isNumeric(outputPF.value) && (parseInt(outputPF.value)>=1 && parseInt(outputPF.value)<=100))){
            alert("The value inserted is not valid! Insert a number between 1 and 100")
        }else{
            sliderPF.value = outputPF.value
            sliderPP.value = outputPF.value
            outputPP.value =outputPF.value
            outputPP.innerHTML =outputPF.value

            $(".change_form").css("visibility", "visible");
            $(".dis_submit").addClass("dosubmit");
            $(".dis_submit").attr('disabled', false);
        }

	}else if(caso=="AF"){
        if(!($.isNumeric(outputAF.value) && (parseInt(outputAF.value)>=1 && parseInt(outputAF.value)<=100))){
            alert("The value inserted is not valid! Insert a number between 1 and 100")
        }else{
            sliderAF.value = outputAF.value
            sliderAP.value = outputAF.value
            outputAP.value =outputAF.value
            outputAP.innerHTML =outputAF.value

            $(".change_form").css("visibility", "visible");
            $(".dis_submit").addClass("dosubmit");
            $(".dis_submit").attr('disabled', false);
        }

	}else if(caso=="AP"){
        if(!($.isNumeric(outputAP.value) && (parseInt(outputAP.value)>=1 && parseInt(outputAP.value)<=100))){
            alert("The value inserted is not valid! Insert a number between 1 and 100")
        }else{
            sliderAP.value = outputAP.value
            sliderAF.value = outputAP.value
            outputAF.value =outputAP.value
            outputAF.innerHTML =outputAP.value

            $(".change_form").css("visibility", "visible");
            $(".dis_submit").addClass("dosubmit");
            $(".dis_submit").attr('disabled', false);
        }
        
	}
}


//function to update the map, based on the slider change
function pre_request(p_check){
	document.getElementById("loadingMessage").style.visibility = "visible";	
	request(p_check)
}
//function called by pre_request(p_check)
function request(p_check) {

	document.getElementById("loadingMessage").style.visibility = "hidden";

	var pf = document.getElementById("myPathF").value;
	var af = document.getElementById("myActF").value;
	var df = "dfgFreqReduced";

	var pp = document.getElementById("myPathP").value;
	var ap = document.getElementById("myActP").value;
	var dp = "dfgPerfReduced"
	//console.log(String(pf) + " " + String(af) + " " +String(pp)+ " "+String(ap) + " "+String(p_check));
	
	var oReq1 = new XMLHttpRequest();
	var oReq2 = new XMLHttpRequest();
	// var oReq12

	// if (p_check == "false") {
		oReq1.addEventListener("load", reqListener1);
		oReq1.open("GET", frontend+df+"?myPathF="+String(pf)+"&myActF="+String(af)+"&myPathP="+String(pp)+"&myActP="+String(ap)+"&perf_checked="+p_check, false);
	// }
	// else {
		oReq2.addEventListener("load", reqListener2);
		oReq2.open("GET", frontend+dp+"?myPathF="+String(pf)+"&myActF="+String(af)+"&myPathP="+String(pp)+"&myActP="+String(ap)+"&perf_checked="+p_check, false);
	// }
	
	oReq1.send();
	oReq2.send();
	getMap(false);
	$(".change_form").css("visibility", "hidden");
	$(".dis_submit").removeClass("dosubmit");
	$(".dis_submit").attr('disabled', true);
	
	document.getElementById("myChange").style.display = "block";
	document.getElementById("blocker5").style.display = "block";
}

//function used to close popup using ❌
function closePP(){
    $("#myPopup").attr("class","popuptext")
    $(".node").find("polygon").attr('stroke', "#000000");
    $(".node").find("text").css({"text-decoration":"revert"});

    $(".edge").find("path").attr('stroke', "#000000");
    $(".edge").find("polygon").attr('stroke', "#000000");
    $(".edge").find("text").css({"text-decoration":"revert"});

    selected_nodes={}
    selected_edges={}

    $total_mean=0
    $total_min=0 
    $total_max=0 
    $total_median=0 
    $total_total=0
}

//function to handle all the map
function getMap(bool) {
		
    // DARIO START
    if (bool){
        // get graph in an array
        graphTextF = getGraphText('frequency');
        graphTextP = getGraphText();
        // get nodes in an array 
        graphNodesF = getGraphNodes(graphTextF);
        graphNodesP = getGraphNodes(graphTextP);
        graphNodes = getCombinedNodes(graphNodesF, graphNodesP);
        // get edges in an array
        graphEdgesF = getGraphEdges(graphTextF, false);
        graphEdgesP = getGraphEdges(graphTextP, true);
        graphEdges = getCombinedEdges(graphEdgesF, graphEdgesP);
        getLabeledGraphEdges(graphNodes, graphEdges);
        // get final matrix ready for dsl conversion
        dslSteps = getDslSteps(graphNodes, graphEdges);
        console.log(dslSteps)

        meanDurationFP= getAllDuration("mean");
        meanEdgeDuration= getAllEdgeDuration("mean");
        // getAllUsedVariables("activities")
        var array_Activities=getAllUsedVariables("activities")
        for (const act of array_Activities) {
            // console.log(act);
            addCheckbox(act)
        }
        $(document).ready(function(){
            $('#filter_by').on('change', function(e) {
                e.stopImmediatePropagation();
                var typeD=this.value
                // console.log(typeD)
                
            });
        });
    }
    //DARIO END
    
    var selected = document.querySelector('input[name="mytabs"]:checked').value;
    var sample;
         
        
        if (selected == 'frequency') {
            sample = document.getElementById("digraphF").innerHTML;
        }
        else {
            sample = document.getElementById("digraphP").innerHTML;
        }
    
    //var sample = 'digraph g{ a -> b}';
    sample = sample.replace(/&#34;/g, '"');
    sample = sample.replace(/&gt;/g, ">");
    sample = sample.replace(/&lt;/g, "<");
    //sample = sample.replace(/●/g, "  &#9679;"); //9679
    sample = sample.replace(/●/g, " ");
    //sample = sample.replace(/■/g, '    &#9724;'); //9632
    sample = sample.replace(/■/g, " ");
    copia_sample=sample
    //console.log(sample)
    //sample=sample.replace("digraph {", 'digraph { \nrankdir="LR";')
    
    var options = {
      format: 'svg',
      ALLOW_MEMORY_GROWTH: 1,
      totalMemory: 537395200,
      //rankdir:"LR",
      //splines:ortho,
      // format: 'png-image-element'
    }

    var image = Viz(sample, options);

    

    var main = document.getElementById('graphContainer');

    main.innerHTML = image;		// SVG

    // document.getElementById('graphContainer2').innerHTML=image
        
    // Optional - resize the SVG element based on the contents.
    /*var svg = document.querySelector('#graphContainer');
    var bbox = svg.getBBox();
    svg.style.width = bbox.width + 10.0 + "px";
    svg.style.height = bbox.height + 10.0 + "px";*/
    
    var allGs = document.getElementsByTagName('g');
    var firstG = allGs[0];
    
    var figli = main.children;
    //console.log(figli[0].attributes.width.value);
    
    var graph = figli[0].children[0];
    //console.log(figli[0].children[0].children[3].children[3].innerHTML);
    //console.log(figli[0].children[0].children[3]);
    //console.log(figli[0]);
    //console.log(firstG.viewBox);
    //firstG.style.shape = "circle";
    //console.log(firstG.style.shape);


    var $totale = 0
    var $totale_edge=0
    var $make_somma=0
        
    var nodes = document.getElementById('graphContainer').getElementsByClassName("node");
    var edges = document.getElementById('graphContainer').getElementsByClassName("edge");
    
    var nodesNames = [];
    var names;
    
    var pp=0;



    $(document).ready(function(){
        for(var i=0; i<nodes.length; i++){
            
            // console.log(nodes[i].children[2].innerHTML);
            nodesNames[i] = nodes[i].children[2].innerHTML;
            // console.log(nodes[i].children[2].innerHTML);
            // console.log(nodes[i].children[0].innerHTML)
            if(nodes[i].id!=undefined){
                var use=nodes[i].id
                $("#"+use).append("<label class='label_node' hidden>"+nodes[i].children[0].innerHTML+"</label>");
            }

            nodes[i].children[0].innerHTML = nodesNames[i];
            // console.log(nodes[i].children[0].innerHTML)

            names = nodesNames[i].split(' (')[0];
            names = '"'+ names +'"';
            //console.log(names);
            
            nodes[i].addEventListener("click", function(e) {
                // When the user clicks call function to open the popup
                openInfo(e);
            }, false);
            if(nodes[i].children[2].getAttribute("text-anchor") != "start"){
                if(nodes[i].children[0].innerHTML != ""){
                    nodes[i].children[2].setAttribute("font-size", "10");
                }
                else{
                    nodes[i].children[2].setAttribute("fill", "red");
                }
            }
            else{
                nodes[i].children[1].setAttribute("fill", "green");
            }
            
        }
    });


    var selected_nodes={}  //used to memoryze the selected nodes

    

    lista_ora=['w','h','s','m','d','y']
    number_selected_activity=0


    // Apply functions on node*************************************************************
    $(document).ready(function(){
        $("#graphContainer").find(".node").on('click', function(event){
            
            $used_id=this.id
            $temp= ($("#"+$used_id).find("polygon").attr('stroke'))
            $check_node=($("#"+$used_id).find("polygon")).length

            
            if($temp=="#000000" && $check_node!=0){
                
                var $titolo = $("#"+$used_id+" title").html(); 				
                $("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                $("#"+$used_id).find("text").css({"text-decoration":"underline"});

                $titolo_name=$titolo.split(' (')[0].trim()
                console.log($titolo_name)

                $titolo_time=$titolo.split(' (')[1].trim()
                $statistica=$titolo_time.substring(0,$titolo_time.length-1)
                console.log($statistica)
                
                $myduration_mean=getAllDuration("mean");
                $myduration_total=getAllDuration("total");
                $myduration_min=getAllDuration("min")
                $myduration_max=getAllDuration("max")
                $myduration_median=getAllDuration("median");
                // console.log($myduration_mean)
                $myfrequency_absolute=getAllFrequency("absolute");
                $myfrequency_case=getAllFrequency("case")
                $myfrequency_max=getAllFrequency("max")
                //___________________________________________________
                
                $resultduration_mean=$myduration_mean[$titolo_name]
                $resultduration_total=$myduration_total[$titolo_name]
                $resultduration_min=$myduration_min[$titolo_name]
                $resultduration_max=$myduration_max[$titolo_name]
                $resultduration_median=$myduration_median[$titolo_name]

                $resultfrequency_absolute=$myfrequency_absolute[$titolo_name]
                $resultfrequency_case=$myfrequency_case[$titolo_name]
                $resultfrequency_max=$myfrequency_max[$titolo_name]
                
                $total_mean=$total_mean+$resultduration_mean
                $total_min=$total_min+$resultduration_min
                $total_max=$total_max+$resultduration_max
                $total_median=$total_median+$resultduration_median
                $total_total=$total_total+$resultduration_total



                var t_name=$titolo_name

                if(document.getElementById("tabPerf").checked==false){
                    selected_edges={}
                    selected_nodes={}
                    $("#graphContainer").find(".node").find("polygon").attr('stroke', "#000000");
                    $("#graphContainer").find(".node").find("text").css({"text-decoration":"revert"});

                    $("#graphContainer").find(".edge").find("path").attr('stroke', "#000000");
                    $("#graphContainer").find(".edge").find("polygon").attr('stroke', "#000000");
                    $("#graphContainer").find(".edge").find("text").css({"text-decoration":"revert"});
                    

                    $("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                    $("#"+$used_id).find("text").css({"text-decoration":"underline"});
                }


                selected_nodes[t_name]=$resultduration_mean
                // console.log(selected_nodes)
                // console.log(sum_dict(selected_nodes))


                if((Object.keys(selected_nodes).length>1 || Object.keys(selected_edges).length>1) && document.getElementById("tabPerf").checked==false){

                    $("#graphContainer").find(".node").find("polygon").attr('stroke', "#000000");
                    $("#graphContainer").find(".node").find("text").css({"text-decoration":"revert"});

                    $("#graphContainer").find(".edge").find("path").attr('stroke', "#000000");
                    $("#graphContainer").find(".edge").find("polygon").attr('stroke', "#000000");
                    $("#graphContainer").find(".edge").find("text").css({"text-decoration":"revert"});

                    $("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                    $("#"+$used_id).find("text").css({"text-decoration":"underline"});

                }
                

                var bool_switch = document.getElementById("tabPerf").checked;
                if ((bool_switch) == true){
                    
                    if(Object.keys(selected_nodes).length>1 || Object.keys(selected_edges).length>1 || (Object.keys(selected_nodes).length>0 && Object.keys(selected_edges).length>0)){
                        popup.innerHTML = "<i class='fas fa-clock'></i> Selected values duration"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+" <br />"
                                +"Mean duration: "+  "<br />" +secondsToHms($total_mean) + "<br /> <br />" 
                                +"Median duration: " +  "<br />"+ secondsToHms($total_median) + "<br /> <br />" 
                                +"Total duration: " +  "<br />"+secondsToHms($total_total) + "<br /> <br />"  
                                +"Min duration: " +  "<br />"+secondsToHms($total_min) + "<br /> <br />"  
                                +"Max duration: " +  "<br />"+secondsToHms($total_max) 

                    }else if(Object.keys(selected_nodes).length>0){
                        popup.innerHTML ="<i class='fas fa-stopwatch'></i> Duration"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+"<br>"+"Mean duration: "+  "<br />" +secondsToHms($resultduration_mean) + "<br /> <br />" 
                                +"Median duration: " +  "<br />"+ secondsToHms($resultduration_median) + "<br /> <br />" 
                                +"Total duration: " +  "<br />"+secondsToHms($resultduration_total) + "<br /> <br />"  
                                +"Min duration: " +  "<br />"+secondsToHms($resultduration_min) + "<br /> <br />"  
                                +"Max duration: " +  "<br />"+secondsToHms($resultduration_max) 
                    }

                }else if(Object.keys(selected_nodes).length>0){
                    popup.innerHTML ="<i class='fa fa-signal' aria-hidden='true'></i> Frequency"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+"<br>"+"Absolute frequency: "+  "<br />" + $resultfrequency_absolute + "<br /> <br />" 
                                +"Case frequency: " +  "<br />"+ $resultfrequency_case + "<br /> <br />" 
                                +"Max repetitions: " +  "<br />"+ $resultfrequency_max + "<br /> <br />"  
                                
                }
                // popup.innerHTML = "Median duration: " + secondsToHms($resultduration_median) + "<br /> <br />" + "Total duration: " + secondsToHms($resultduration_total)+ "<br /> <br />"  +"Selected duration node: <br />"+ secondsToHms($totale) + "<br /> <br />" +"Selected duration edge: <br />"+ secondsToHms($totale_edge);
                
                
                $("#myPopup").attr("class","popuptext show")

                if(Object.keys(selected_nodes).length==0 && Object.keys(selected_edges).length==0){
                    $("#myPopup").attr("class","popuptext")
                }

                /*
                if(Object.keys(selected_nodes).length>0){

                    $(".edge").find("polygon").attr('stroke', "#000000");
                    $(".edge").find("path").attr('stroke', '#000000');
                    $(".edge").find("text").css({"text-decoration":"revert"});
                    $totale_edge=0
                }*/

                
                // if(number_selected_activity==0){
                // 	$("#myPopup").attr("class","popuptext")
                // }

            }else if($check_node!=0){

                var $titolo = $("#"+$used_id+" title").html(); 				
                $("#"+$used_id).find("polygon").attr('stroke', "#000000");
                $("#"+$used_id).find("text").css({"text-decoration":"revert"});
                
                $titolo_name=$titolo.split(' (')[0].trim()
                // console.log($titolo_name)

                
                $titolo_time=$titolo.split(' (')[1].trim()
                $statistica=$titolo_time.substring(0,$titolo_time.length-1)
                // console.log($statistica)

                // number_selected_activity=number_selected_activity-1
                
                $myduration_mean=getAllDuration("mean");
                $myduration_total=getAllDuration("total");
                $myduration_min=getAllDuration("min")
                $myduration_max=getAllDuration("max")
                $myduration_median=getAllDuration("median");

                $resultduration_mean=$myduration_mean[$titolo_name]
                $resultduration_total=$myduration_total[$titolo_name]
                $resultduration_min=$myduration_min[$titolo_name]
                $resultduration_max=$myduration_max[$titolo_name]
                $resultduration_median=$myduration_median[$titolo_name]


                // $totale=$totale-$resultduration_mean
                $total_mean=$total_mean-$resultduration_mean
                $total_min=$total_min-$resultduration_min
                $total_max=$total_max-$resultduration_max
                $total_median=$total_median-$resultduration_median
                $total_total=$total_total-$resultduration_total

                $myfrequency_absolute=getAllFrequency("absolute");
                $myfrequency_case=getAllFrequency("case")
                $myfrequency_max=getAllFrequency("max")
                                                        
                $resultfrequency_absolute=$myfrequency_absolute[$titolo_name]
                $resultfrequency_case=$myfrequency_case[$titolo_name]
                $resultfrequency_max=$myfrequency_max[$titolo_name]



                delete selected_nodes[$titolo_name]
                // console.log(selected_nodes)
                // console.log(sum_dict(selected_nodes))
                // if(document.getElementById("tabPerf").checked==false){
                // 	selected_edges={}
                // 	selected_nodes={}
                // 	$(".node").find("polygon").attr('stroke', "#000000");
                // 		$(".node").find("text").css({"text-decoration":"revert"});

                // 		$(".edge").find("path").attr('stroke', "#000000");
                // 		$(".edge").find("polygon").attr('stroke', "#000000");
                // 		$(".edge").find("text").css({"text-decoration":"revert"});
                        

                // 		$("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                // 		$("#"+$used_id).find("text").css({"text-decoration":"underline"});
                // }

                if(Object.keys(selected_nodes).length>1 && document.getElementById("tabPerf").checked==false){

                    
                    selected_nodes={}
            
                }

                if($total_mean<0){	$total_mean=0 }
                if($total_min<0){	$total_min=0 }
                if($total_max<0){	$total_max=0 }
                if($total_median<0){	$total_median=0 }
                if($total_total<0){	$total_total=0 }

                if((Object.keys(selected_nodes).length>1 || Object.keys(selected_edges).length>1) && document.getElementById("tabPerf").checked==false){

                    $("#graphContainer").find(".node").find("polygon").attr('stroke', "#000000");
                    $("#graphContainer").find(".node").find("text").css({"text-decoration":"revert"});

                    $("#graphContainer").find(".edge").find("path").attr('stroke', "#000000");
                    $("#graphContainer").find(".edge").find("polygon").attr('stroke', "#000000");
                    $("#graphContainer").find(".edge").find("text").css({"text-decoration":"revert"});

            


                    $("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                    $("#"+$used_id).find("text").css({"text-decoration":"underline"});
                }


                var bool_switch = document.getElementById("tabPerf").checked;
                if ((bool_switch) == true){
                    if(Object.keys(selected_nodes).length>1 || Object.keys(selected_edges).length>1 || (Object.keys(selected_nodes).length>0 && Object.keys(selected_edges).length>0)){
                        popup.innerHTML = "<i class='fas fa-clock'></i> Selected values duration"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+" <br />"
                                +"Mean duration: "+  "<br />" +secondsToHms($total_mean) + "<br /> <br />" 
                                +"Median duration: " +  "<br />"+ secondsToHms($total_median) + "<br /> <br />" 
                                +"Total duration: " +  "<br />"+secondsToHms($total_total) + "<br /> <br />"  
                                +"Min duration: " +  "<br />"+secondsToHms($total_min) + "<br /> <br />"  
                                +"Max duration: " +  "<br />"+secondsToHms($total_max) 
    

                    }else if(Object.keys(selected_nodes).length>0 || Object.keys(selected_edges).length>0){
                        popup.innerHTML ="<i class='fas fa-stopwatch'></i> Duration"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+"<br>"+"Mean duration: "+  "<br />" +secondsToHms($total_mean) + "<br /> <br />" 
                                +"Median duration: " +  "<br />"+ secondsToHms($total_median) + "<br /> <br />" 
                                +"Total duration: " +  "<br />"+secondsToHms($total_total) + "<br /> <br />"  
                                +"Min duration: " +  "<br />"+secondsToHms($total_min) + "<br /> <br />"  
                                +"Max duration: " +  "<br />"+secondsToHms($total_max) 

                    }
                    
                }else if(Object.keys(selected_nodes).length>0){
                    popup.innerHTML ="<i class='fa fa-signal' aria-hidden='true'></i> Frequency"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+"<br>"+"Absolute frequency: "+  "<br />" +$resultfrequency_absolute + "<br /> <br />" 
                                +"Case frequency: " +  "<br />"+ $resultfrequency_case + "<br /> <br />" 
                                +"Max repetitions: " +  "<br />"+ $resultfrequency_max + "<br /> <br />"  
                                
                }



                if(Object.keys(selected_nodes).length==0 && Object.keys(selected_edges).length==0){
                    $("#myPopup").attr("class","popuptext")
                }
                // if(number_selected_activity==0){
                // 	$("#myPopup").attr("class","popuptext")
                // }
                // 
            }else{
                // $("#myPopup").attr("class","popuptext")
            }
            
            // console.log($("#"+$used_id).html())
            // console.log(this.id.innerHTML)
            // console.log($totale)
            // console.log(secondsToHms($totale))				
        });
    });
    // ****************************************************************************************************

    // analyze edge****************************************************************************************
    $totale_edge=0

    var selected_edges={}

    $(document).ready(function(){
        $("#graphContainer").find(".edge").on('click', function(event){
            
            $used_id=this.id
            $temp_polygon= ($("#"+$used_id).find("polygon").attr('stroke'))
            $temp_path= ($("#"+$used_id).find("path").attr('stroke'))

            $check_edge= ($("#"+$used_id).find("polygon")).length
            var $titolo_1 = $("#"+$used_id+" text").html();
            // if(lista_ora.includes(String($titolo_1[$titolo_1.length-1]))){
            
            if($temp_polygon=="#000000" && $check_edge!=0 && $titolo_1!=undefined){
                            
                $("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                $("#"+$used_id).find("path").attr('stroke', color_highlight);
                $("#"+$used_id).find("text").css({"text-decoration":"underline"});
                

                $statistica=$titolo_1.substring(0,$titolo_1.length-1)

                $myduration_edge_mean=getAllEdgeDuration("mean");
                $myduration_edge_total=getAllEdgeDuration("total");
                $myduration_edge_min=getAllEdgeDuration("min")
                $myduration_edge_max=getAllEdgeDuration("max")
                $myduration_edge_median=getAllEdgeDuration("median");

                $myfrequency_edge_absolute=getAllEdgeFrequency("absolute");
                $myfrequency_edge_case=getAllEdgeFrequency("case")
                $myfrequency_edge_max=getAllEdgeFrequency("max")
                                                        


                // $titolo=$titolo_1.split(' (')[0].trim()
                var $label_title_edge= $("#"+$used_id).find("label").text();
                
                // console.log($label_title_edge)
                if($label_title_edge!=undefined && $label_title_edge!=""){
                    // console.log($label_title_edge)
                    var $source_edge= $label_title_edge.split('->')[0].trim()
                    // console.log($source_edge)
                    // console.log(nodes_id_association[$source_edge])
                    var $target_edge= $label_title_edge.split('->')[1].trim()
                    // console.log($target_edge)
                    // console.log(nodes_id_association[$target_edge])

                    $resultduration_edge_mean=$myduration_edge_mean[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    $resultduration_edge_total=$myduration_edge_total[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    $resultduration_edge_min=$myduration_edge_min[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    $resultduration_edge_max=$myduration_edge_max[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    $resultduration_edge_median=$myduration_edge_median[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]

                    // console.log(nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge])
                    // $resultfrequency_edge_absolute=$myfrequency_edge_absolute[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    // $resultfrequency_edge_case=$myfrequency_edge_case[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    // $resultfrequency_edge_max=$myfrequency_edge_max[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    
                    if($source_edge=="@@startnode"){
                        $resultfrequency_edge_absolute=$myfrequency_edge_absolute["@@startnode"+"#"+nodes_id_association[$target_edge]]
                        $resultfrequency_edge_case=$myfrequency_edge_case["@@startnode"+"#"+nodes_id_association[$target_edge]]
                        $resultfrequency_edge_max=1//$myfrequency_edge_max[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    }else if($target_edge=="@@endnode"){
                        $resultfrequency_edge_absolute=$myfrequency_edge_absolute[nodes_id_association[$source_edge]+"#"+"@@endnode"]
                        $resultfrequency_edge_case=$myfrequency_edge_case[nodes_id_association[$source_edge]+"#"+"@@endnode"]
                        $resultfrequency_edge_max=1//$myfrequency_edge_max[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    }else{
                        $resultfrequency_edge_absolute=$myfrequency_edge_absolute[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                        $resultfrequency_edge_case=$myfrequency_edge_case[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                        $resultfrequency_edge_max=$myfrequency_edge_max[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    }


                    $total_mean=$total_mean+$resultduration_edge_mean
                    $total_min=$total_min+$resultduration_edge_min
                    $total_max=$total_max+$resultduration_edge_max
                    $total_median=$total_median+$resultduration_edge_median
                    $total_total=$total_total+$resultduration_edge_total

                    if(document.getElementById("tabPerf").checked==false){
                        selected_edges={}
                        selected_nodes={}
                        $("#graphContainer").find(".node").find("polygon").attr('stroke', "#000000");
                        $("#graphContainer").find(".node").find("text").css({"text-decoration":"revert"});

                        $("#graphContainer").find(".edge").find("path").attr('stroke', "#000000");
                        $("#graphContainer").find(".edge").find("polygon").attr('stroke', "#000000");
                        $("#graphContainer").find(".edge").find("text").css({"text-decoration":"revert"});
                        
                        $("#"+$used_id).find("path").attr('stroke', color_highlight);
                        $("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                        $("#"+$used_id).find("text").css({"text-decoration":"underline"});
                    }

                    var edge_extended_name=[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    selected_edges[edge_extended_name]=$resultduration_edge_mean

                    if(Object.keys(selected_edges).length>1 && document.getElementById("tabPerf").checked==false){
                        selected_edges={}
                    }

                    if((Object.keys(selected_nodes).length>1 || Object.keys(selected_edges).length>1) && document.getElementById("tabPerf").checked==false){

                        $("#graphContainer").find(".node").find("polygon").attr('stroke', "#000000");
                        $("#graphContainer").find(".node").find("text").css({"text-decoration":"revert"});

                        $("#graphContainer").find(".edge").find("path").attr('stroke', "#000000");
                        $("#graphContainer").find(".edge").find("polygon").attr('stroke', "#000000");
                        $("#graphContainer").find(".edge").find("text").css({"text-decoration":"revert"});


                        $("#"+$used_id).find("path").attr('stroke', color_highlight);
                        $("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                        $("#"+$used_id).find("text").css({"text-decoration":"underline"});
                    }

                    var bool_switch = document.getElementById("tabPerf").checked;
                    if ((bool_switch) == true){
                        
                        if(Object.keys(selected_edges).length>1 || Object.keys(selected_nodes).length>1 || (Object.keys(selected_nodes).length>0 && Object.keys(selected_edges).length>0)){
                            popup.innerHTML = "<i class='fas fa-clock'></i> Selected values duration"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+" <br />"
                                    +"Mean duration: "+  "<br />" +secondsToHms($total_mean) + "<br /> <br />" 
                                    +"Median duration: " +  "<br />"+ secondsToHms($total_median) + "<br /> <br />" 
                                    +"Total duration: " +  "<br />"+secondsToHms($total_total) + "<br /> <br />"  
                                    +"Min duration: " +  "<br />"+secondsToHms($total_min) + "<br /> <br />"  
                                    +"Max duration: " +  "<br />"+secondsToHms($total_max) 

                        }else if(Object.keys(selected_edges).length>0){
                            popup.innerHTML ="<i class='fas fa-stopwatch'></i> Duration"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+"<br>"+"Mean duration: "+  "<br />" +secondsToHms($resultduration_edge_mean) + "<br /> <br />" 
                                    +"Median duration: " +  "<br />"+ secondsToHms($resultduration_edge_median) + "<br /> <br />" 
                                    +"Total duration: " +  "<br />"+secondsToHms($resultduration_edge_total) + "<br /> <br />"  
                                    +"Min duration: " +  "<br />"+secondsToHms($resultduration_edge_min) + "<br /> <br />"  
                                    +"Max duration: " +  "<br />"+secondsToHms($resultduration_edge_max) 
                        }

                    }else if(Object.keys(selected_edges).length>0){ //
                        
                        popup.innerHTML ="<i class='fa fa-signal' aria-hidden='true'></i> Frequency"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+"<br>"+"Absolute frequency: "+  "<br />" + $resultfrequency_edge_absolute + "<br /> <br />" 
                                    +"Case frequency: " +  "<br />"+ $resultfrequency_edge_case + "<br /> <br />" 
                                    +"Max repetitions: " +  "<br />"+ $resultfrequency_edge_max + "<br /> <br />"  
                                    
                    }


                    




                }

                // $statistica=$titolo_1.substring(0,$titolo_1.length-1)
                // console.log(lista_ora.includes(String($titolo_1[$titolo_1.length-1])))
                // console.log($statistica)
                // console.log($titolo_1)
                // console.log(String($titolo_1[$titolo_1.length-1]))
                //$myduration=getAllDuration("mean");
                //$resultduration=$myduration[$titolo]
                // if(String($titolo_1[$titolo_1.length-1])=="m"){
                // 	$make_somma=parseInt($statistica)*60
                // }
                // if(String($titolo_1[$titolo_1.length-1])=="h"){
                // 	$make_somma=parseInt($statistica)*60*60
                // }
                // if(String($titolo_1[$titolo_1.length-1])=="D" || String($titolo_1[$titolo_1.length-1])=="d"){
                // 	$make_somma=parseInt($statistica)*60*60*24
                // }
                // if(String($titolo_1[$titolo_1.length-1])=="s"){
                // 	$make_somma=parseInt($statistica)
                // }
                
                // $totale_edge=$totale_edge+$make_somma
                // number_selected_activity=number_selected_activity+1

                /* commento utile forse
                if(Object.keys(selected_edges).length>0){
                    $(".node").find("polygon").attr('stroke', "#000000");
                    $(".node").find("text").css({"text-decoration":"revert"});
                    selected_nodes={}
                }
                */
                
                // popup.innerHTML = "Selected edge duration: <br />"+ secondsToHms($totale_edge);
                // $("#myPopup").attr("class","popuptext show")
                // if(number_selected_activity==0){
                // 	$("#myPopup").attr("class","popuptext")
                // }
                $("#myPopup").attr("class","popuptext show")

                if(Object.keys(selected_edges).length==0 && Object.keys(selected_nodes).length==0){
                    $("#myPopup").attr("class","popuptext")
                }
                
            }else if($check_edge!=0 && $titolo_1!=undefined){

                // var $titolo = $("#"+$used_id+" title").html(); 
                $("#"+$used_id).find("polygon").attr('stroke', "#000000");
                $("#"+$used_id).find("path").attr('stroke', '#000000');
                $("#"+$used_id).find("text").css({"text-decoration":"revert"});
                // $("#"+$used_id).find("text").attr('fill',"#000000");
                // $titolo=$titolo.split(' (')[0].trim()
                number_selected_activity=number_selected_activity-1
                $statistica=$titolo_1.substring(0,$titolo_1.length-1)
                // console.log($statistica)

                $myduration_edge_mean=getAllEdgeDuration("mean");
                $myduration_edge_total=getAllEdgeDuration("total");
                $myduration_edge_min=getAllEdgeDuration("min")
                $myduration_edge_max=getAllEdgeDuration("max")
                $myduration_edge_median=getAllEdgeDuration("median");

                $myfrequency_edge_absolute=getAllEdgeFrequency("absolute");
                $myfrequency_edge_case=getAllEdgeFrequency("case")
                $myfrequency_edge_max=getAllEdgeFrequency("max")

                var $label_title_edge= $("#"+$used_id).find("label").text();
                if($label_title_edge!=undefined && $label_title_edge!=""){
                    // console.log($label_title_edge)
                    var $source_edge= $label_title_edge.split('->')[0].trim()
                    // console.log($source_edge)
                    var $target_edge= $label_title_edge.split('->')[1].trim()
                    // console.log($target_edge)

                    $resultduration_edge_mean=$myduration_edge_mean[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    $resultduration_edge_total=$myduration_edge_total[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    $resultduration_edge_min=$myduration_edge_min[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    $resultduration_edge_max=$myduration_edge_max[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    $resultduration_edge_median=$myduration_edge_median[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]

                    
                    $total_mean=$total_mean-$resultduration_edge_mean
                    $total_min=$total_min-$resultduration_edge_min
                    $total_max=$total_max-$resultduration_edge_max
                    $total_median=$total_median-$resultduration_edge_median
                    $total_total=$total_total-$resultduration_edge_total

                    


                    if($source_edge=="@@startnode"){
                        $resultfrequency_edge_absolute=$myfrequency_edge_absolute["@@startnode"+"#"+nodes_id_association[$target_edge]]
                        $resultfrequency_edge_case=$myfrequency_edge_case["@@startnode"+"#"+nodes_id_association[$target_edge]]
                        $resultfrequency_edge_max=1//$myfrequency_edge_max[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    }else if($target_edge=="@@endnode"){
                        $resultfrequency_edge_absolute=$myfrequency_edge_absolute[nodes_id_association[$source_edge]+"#"+"@@endnode"]
                        $resultfrequency_edge_case=$myfrequency_edge_case[nodes_id_association[$source_edge]+"#"+"@@endnode"]
                        $resultfrequency_edge_max=1//$myfrequency_edge_max[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    }else{
                        $resultfrequency_edge_absolute=$myfrequency_edge_absolute[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                        $resultfrequency_edge_case=$myfrequency_edge_case[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                        $resultfrequency_edge_max=$myfrequency_edge_max[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    }



                    var edge_extended_name2=[nodes_id_association[$source_edge]+"#"+nodes_id_association[$target_edge]]
                    delete selected_edges[edge_extended_name2]

                    // if(document.getElementById("tabPerf").checked==false){
                    // selected_edges={}
                    // selected_nodes={}
                    // $(".node").find("polygon").attr('stroke', "#000000");
                    // 	$(".node").find("text").css({"text-decoration":"revert"});

                    // 	$(".edge").find("path").attr('stroke', "#000000");
                    // 	$(".edge").find("polygon").attr('stroke', "#000000");
                    // 	$(".edge").find("text").css({"text-decoration":"revert"});
                        

                    // 	$("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                    // 	$("#"+$used_id).find("text").css({"text-decoration":"underline"});
                    // }

                    if(Object.keys(selected_nodes).length>1 && document.getElementById("tabPerf").checked==false){							
                        // selected_nodes={}
                    }
                    if($total_mean<0){	$total_mean=0 }
                    if($total_min<0){	$total_min=0 }
                    if($total_max<0){	$total_max=0 }
                    if($total_median<0){	$total_median=0 }
                    if($total_total<0){	$total_total=0 }

                    if((Object.keys(selected_nodes).length>1 || Object.keys(selected_edges).length>1) && document.getElementById("tabPerf").checked==false){
    
                        $("#graphContainer").find(".node").find("polygon").attr('stroke', "#000000");
                        $("#graphContainer").find(".node").find("text").css({"text-decoration":"revert"});

                        $("#graphContainer").find(".edge").find("path").attr('stroke', "#000000");
                        $("#graphContainer").find(".edge").find("polygon").attr('stroke', "#000000");
                        $("#graphContainer").find(".edge").find("text").css({"text-decoration":"revert"});
                        
                        $("#"+$used_id).find("path").attr('stroke', color_highlight);
                        $("#"+$used_id).find("polygon").attr('stroke', color_highlight);
                        $("#"+$used_id).find("text").css({"text-decoration":"underline"});
                        
                    }

                    var bool_switch = document.getElementById("tabPerf").checked;
                    if ((bool_switch) == true){
                        if(Object.keys(selected_edges).length>1 || Object.keys(selected_nodes).length>1 || (Object.keys(selected_nodes).length>0 && Object.keys(selected_edges).length>0)){
                            popup.innerHTML = "<i class='fas fa-clock'></i> Selected values duration"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+"<br />"
                                    +"Mean duration: "+  "<br />" +secondsToHms($total_mean) + "<br /> <br />" 
                                    +"Median duration: " +  "<br />"+ secondsToHms($total_median) + "<br /> <br />" 
                                    +"Total duration: " +  "<br />"+secondsToHms($total_total) + "<br /> <br />"  
                                    +"Min duration: " +  "<br />"+secondsToHms($total_min) + "<br /> <br />"  
                                    +"Max duration: " +  "<br />"+secondsToHms($total_max) 
        

                        }else if(Object.keys(selected_edges).length>0 || Object.keys(selected_nodes).length>0){
                            popup.innerHTML ="<i class='fas fa-stopwatch'></i> Duration"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+"<br>"+"Mean duration: "+  "<br />" +secondsToHms($total_mean) + "<br /> <br />" 
                                    +"Median duration: " +  "<br />"+ secondsToHms($total_median) + "<br /> <br />" 
                                    +"Total duration: " +  "<br />"+secondsToHms($total_total) + "<br /> <br />"  
                                    +"Min duration: " +  "<br />"+secondsToHms($total_min) + "<br /> <br />"  
                                    +"Max duration: " +  "<br />"+secondsToHms($total_max) 

                        }
                        
                    }else if(Object.keys(selected_edges).length>0){ //
                        popup.innerHTML ="<i class='fa fa-signal' aria-hidden='true'></i> Frequency"+"<span class='close_pp' onclick='closePP()'>❌</span>"+"<hr>"+"<br>"+"Absolute frequency: "+  "<br />" +$resultfrequency_edge_absolute + "<br /> <br />" 
                                    +"Case frequency: " +  "<br />"+ $resultfrequency_edge_case + "<br /> <br />" 
                                    +"Max repetitions: " +  "<br />"+ $resultfrequency_edge_max + "<br /> <br />"  
                                                }

                    if(Object.keys(selected_edges).length==0 && Object.keys(selected_nodes).length==0){
                        $("#myPopup").attr("class","popuptext")
                    }
                
                
                // console.log($titolo_1)
                
                // if(String($titolo_1[$titolo_1.length-1])=="m"){
                // 	$make_somma=parseInt($statistica)*60
                // }
                // if(String($titolo_1[$titolo_1.length-1])=="h"){
                // 	$make_somma=parseInt($statistica)*60*60
                // }
                // if(String($titolo_1[$titolo_1.length-1])=="D" || String($titolo_1[$titolo_1.length-1])=="d"){
                // 	$make_somma=parseInt($statistica)*60*60*24
                // }
                // if(String($titolo_1[$titolo_1.length-1])=="s"){
                // 	$make_somma=parseInt($statistica)
                // }
                
                // $totale_edge=$totale_edge-$make_somma
                
                
                // if($totale_edge<0){
                // 	$totale_edge=0
                // }
                // if($totale_edge!=0){
                // 	popup.innerHTML = "Selected edge duration: <br />"+ secondsToHms($totale_edge);
                // }
                
                // if($totale_edge==0){
                // 	$("#myPopup").attr("class","popuptext")
                // }
                // if(number_selected_activity==0){
                // 	$("#myPopup").attr("class","popuptext")
                // }
                }	
            }
                 
            // }else{
                // $("#myPopup").attr("class","popuptext")
            // }
            
            
        });
    });
    // *****************************************************************************************************

    var edgesNames = [];
    var popup = document.getElementById("myPopup");
    var popup_cChecking=document.getElementById("myPopup_cChecking");
    $(document).ready(function(){
                   

        for( i=0; i<edges.length; i++){
            // edgesNames[i] = edges[i].children[3].innerHTML;
            // edges[i].children[0].innerHTML = edgesNames[i];
            
            if(edges[i].children[3]!= undefined){
                
                
                if(edges[i].id!=undefined){
                    var use=edges[i].id
                    $("#"+use).append("<label class='label_edge' hidden>"+edges[i].children[0].innerHTML+"</label>");
                }
                
                edgesNames[i] = edges[i].children[3].innerHTML;
                edges[i].children[0].innerHTML = edgesNames[i];
                

                

                edges[i].addEventListener("click", function(e) {
                // When the user clicks call function to open the popup
                    openInfo(e);
                }, false);

                // console.log($label)



            }
        


            // edges[i].addEventListener("click", function(e) {
            // 	// When the user clicks call function to open the popup
            //     openInfo(e);
            // }, false);
        }
    });


    // Switch Duration**************************************************************************
    $(document).ready(function(){

        $(".dis_submit").on('click', function(event){
            $("#myPopup").attr("class","popuptext");
            number_selected_activity=0
            // console.log("change-do")
        })
        $("#tabFreq").on('click', function(event){
            $("#myPopup").attr("class","popuptext");
            number_selected_activity=0
            // console.log("change-do")
        })
        $("#tabPerf").on('click', function(event){
            $("#myPopup").attr("class","popuptext");
            number_selected_activity=0
            // console.log("change-do")
        })


        // ################################################################



        var bool_switch = document.getElementById("tabPerf").checked;
        if ((bool_switch) == true){
            // if($('select[name=perfs] option').filter(':selected').val()=="totDur"){
                var typeD=$('select[name=perfs] option').filter(':selected').val()
                //var classList = $('.node');
                var classList = document.getElementById('graphContainer').getElementsByClassName("node");
                
                $.each(classList, function(index, item) {
                    var node_id=item.id
                    if(node_id!="@@S" && node_id!="@@E" && $("#"+node_id).find(".label_node").text()!="@@startnode" && $("#"+node_id).find(".label_node").text()!="@@endnode"){
                        
                        if(typeD=="totDur"){
                            $duration_values=getAllDuration("total");
                        }else if (typeD=="medDur"){
                            $duration_values=getAllDuration("median");
                        }else if (typeD=="meanDur"){
                            $duration_values=getAllDuration("mean");
                        }else if (typeD=="maxDur"){
                            $duration_values=getAllDuration("max");
                        }else if (typeD=="minDur"){
                            $duration_values=getAllDuration("min");
                        }
                        
                        // console.log($myduration_total)

                        var $title = $("#"+node_id+" title").html(); 				
                        
                        $titolo_name=$title.split(' (')[0].trim()
                        $("#"+node_id).find("text").text($titolo_name);
                        // $move_y=$clonare.attr("y")
                        // console.log($titolo_name)
                        
                        $("#"+node_id).find("text").attr("class","text_name")
                        $("#"+node_id).find("title").text($titolo_name+" ("+timeToHms($duration_values[$titolo_name])+")");
                    
                        $label_id=$("#"+node_id).find("label").text()
                        // console.log()
                        nodes_id_association[$label_id]=$titolo_name

                        $clonare=$("#"+node_id).find("text").clone()
                        $move_y=$clonare.attr("y")
                        $clonare.attr("y",(parseFloat($move_y)+10)+"")
                        $clonare.attr("class","stat_name")
                        $clonare.text("("+timeToHms($duration_values[$titolo_name])+")")
                        // console.log($clonare.attr("y"))
                        $("#"+node_id).find("text").attr("y",(parseFloat($move_y)-5)+"")
                        $clonare.appendTo("#"+node_id)
                        // console.log($("#"+node_id).find("text").text())
                        // console.log($duration_values[$titolo_name])
                        
                    }

                    if($("#"+node_id).find(".label_node").text()=="@@startnode" || $("#"+node_id).find(".label_node").text()=="@@endnode"){
                        $("#"+node_id).find("title").text(" ")
                    }
                    
                    
                    
                });


                //var classList = $('.edge');
                var classList = document.getElementById('graphContainer').getElementsByClassName("edge"); 
                $.each(classList, function(index, item) {
                    var edge_id=item.id
                    // if(node_id!="@@S" && node_id!="@@E"){
                        
                        if(typeD=="totDur"){
                            $duration_edge_values=getAllEdgeDuration("total");
                        }else if (typeD=="medDur"){
                            $duration_edge_values=getAllEdgeDuration("median");
                        }else if (typeD=="meanDur"){
                            $duration_edge_values=getAllEdgeDuration("mean");
                        }else if (typeD=="maxDur"){
                            $duration_edge_values=getAllEdgeDuration("max");
                        }else if (typeD=="minDur"){
                            $duration_edge_values=getAllEdgeDuration("min");
                        }
                        
                        // console.log($myduration_total)

                        var $title = $("#"+edge_id+" title").html(); 	
                        var $label_title= $("#"+edge_id+" label").text(); 
                        // console.log($title)	
                        // console.log($label_title)
                        
                        if($label_title!=undefined && $label_title!=""){
                            var $source= $label_title.split('->')[0].trim()
                            var $target= $label_title.split('->')[1].trim()

                            
                            // console.log(nodes_id_association[$source])
                            // console.log(nodes_id_association[$target])
                            // console.log(secondsToHms($duration_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]))
                            // secondsToHms
                            
                            // $titolo_name=$title.split(' (')[0].trim()
                            // $("#"+node_id).find("text").text($titolo_name);
                            // // $move_y=$clonare.attr("y")
                            
                            // $("#"+node_id).find("text").attr("class","text_name")
                            $("#"+edge_id).find("text").text(edgetimeconvert($duration_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));
                            // $("#"+edge_id).find("title").text(timeToHms($duration_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));

                            $clone=$("#"+edge_id).find("text").clone()
                            $move_x=$clone.attr("x")
                            // $clonare.attr("y",(parseFloat($move_y)+10)+"")
                            // $clonare.attr("class","stat_name")
                            // $clonare.text("("+timeToHms($duration_values[$titolo_name])+")")
                            // // console.log($clonare.attr("y"))
                            $("#"+edge_id).find("text").attr("x",(parseFloat($move_x)+7)+"")
                            // $clonare.appendTo("#"+node_id)
                            // console.log($("#"+node_id).find("text").text())
                            // console.log($duration_values[$titolo_name])
                            
                        // }
                        }
                    
                    
                    
                });

                
            // }
        }
        
        
        var bool_switch2 = document.getElementById("tabFreq").checked;
        if(bool_switch2){
                var typeF=$('select[name=freqs] option').filter(':selected').val()
                
                //var classList = $('.node');
                var classList = document.getElementById('graphContainer').getElementsByClassName("node");
                
                $.each(classList, function(index, item) {
                    var node_id=item.id
                    if(node_id!="@@S" && node_id!="@@E" && $("#"+node_id).find(".label_node").text()!="@@startnode" && $("#"+node_id).find(".label_node").text()!="@@endnode"){
                        
                        if(typeF=="absFreq"){
                            $frequency_values=getAllFrequency("absolute");
                        }else if (typeF=="caseFreq"){
                            $frequency_values=getAllFrequency("case");
                        }else if (typeF=="maxRep"){
                            $frequency_values=getAllFrequency("max");
                        }
                        
                        // console.log($myduration_total)

                        // var $title = $("#"+node_id+" title").html(); 				
                        
                        // $titolo_name=$title.split(' (')[0].trim()
                        // $("#"+node_id).find("text").text($titolo_name);
                        // // $move_y=$clonare.attr("y")
                        // // console.log($titolo_name)
                        
                        // $("#"+node_id).find("text").attr("class","text_name")
                        // $("#"+node_id).find("title").text($titolo_name+" ("+timeToHms($duration_values[$titolo_name])+")");
                    
                        
                        var $title = $("#"+node_id+" title").html(); 				
                            
                        $titolo_name=$title.split(' (')[0].trim()

                        $label_id=$("#"+node_id).find("label").text()
                        nodes_id_association[$label_id]=$titolo_name

                        $("#"+node_id).find("text").text($titolo_name+" ("+($frequency_values[$titolo_name])+")");
                        // $("#"+node_id).find(".stat_name").text("("+timeToHms($duration_values[$titolo_name])+")");
        
                        $("#"+node_id).find("title").text($titolo_name+" ("+($frequency_values[$titolo_name])+")");

                        
                    }

                    if($("#"+node_id).find(".label_node").text()=="@@startnode" || $("#"+node_id).find(".label_node").text()=="@@endnode"){
                        $("#"+node_id).find("title").text(" ")
                    }
                    
                    
                    
                });


                //var classEdge = $('.edge');
                var classEdge = document.getElementById('graphContainer').getElementsByClassName("edge");
                $.each(classEdge, function(index, item) {
                    var edge_id=item.id
                    
                    // if(node_id!="@@S" && node_id!="@@E"){
                        
                    if(typeF=="absFreq"){
                        $frequency_edge_values=getAllEdgeFrequency("absolute");
                    }else if (typeF=="caseFreq"){
                        $frequency_edge_values=getAllEdgeFrequency("case");
                    }else if (typeF=="maxRep"){
                        $frequency_edge_values=getAllEdgeFrequency("max");
                    }
                    

                    var $title = $("#"+edge_id+" title").html(); 
                    // console.log($title)	
                    // var $label_title= $("#"+edge_id).find("label").text(); 
                    // console.log($label_title)	
                    
                    var $title = $("#"+edge_id+" title").html(); 	
                    var $label_title= $("#"+edge_id+" label").text(); 
                    


                    
                    if($label_title!=undefined && $label_title!=""){
                        // console.log($label_title)
                        var $source= $label_title.split('->')[0].trim()
                        // console.log($source)
                        var $target= $label_title.split('->')[1].trim()
                        // console.log($target)
                        // var $source= $title.split('-&gt;')[0].trim()
                        // var $target= $title.split('-&gt;')[1].trim()
                        
                        // console.log(nodes_id_association)
                        // console.log(nodes_id_association[$source])
                        // console.log(nodes_id_association[$target])
                        // console.log(secondsToHms($duration_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]))
                        
                        if($source=="@@startnode"){
                            $("#"+edge_id).find("text").text(($frequency_edge_values["@@startnode"+"#"+nodes_id_association[$target]]));
                            $("#"+edge_id).find("title").text(($frequency_edge_values["@@startnode"+"#"+nodes_id_association[$target]]));

                        }else if($target=="@@endnode"){
                            $("#"+edge_id).find("text").text(($frequency_edge_values[nodes_id_association[$source]+"#"+"@@endnode"]));
                            $("#"+edge_id).find("title").text(($frequency_edge_values[nodes_id_association[$source]+"#"+"@@endnode"]));
                        
                        }else{
                            $("#"+edge_id).find("text").text(($frequency_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));
                            $("#"+edge_id).find("title").text(($frequency_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));
                        }
                        
                        // $("#"+edge_id).find("text").text(($frequency_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));
                        // $("#"+edge_id).find("title").text(($frequency_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));

                        if($source=="@@startnode" && typeF=="maxRep"){
                            $("#"+edge_id).find("text").text("");
                            $("#"+edge_id).find("title").text("")
                            // TESTTEST
                        }
                        if($target=="@@endnode" && typeF=="maxRep"){
                            $("#"+edge_id).find("text").text("");
                            $("#"+edge_id).find("title").text("")
                            // TESTTEST
                        }
                    

                    }
                    
                    
                    
                });


        
        }

            
            // console.log(nodes_id_association)

        $('#performanceDuration').on('change', function(e) {
            e.stopImmediatePropagation();

            var typeD=this.value

            //var classList = $('.node');
            var classList = document.getElementById('graphContainer').getElementsByClassName("node");
            
            // console.log(classList)
            $.each(classList, function(index, item) {
                var node_id=item.id
                if(node_id!="@@S" && node_id!="@@E" && $("#"+node_id).find(".label_node").text()!="@@startnode" && $("#"+node_id).find(".label_node").text()!="@@endnode"){
                    if(typeD=="totDur"){
                        $duration_values=getAllDuration("total");
                        $('#performanceDuration').prop('title', $('#pd_totDur').prop('title'));
                    }else if (typeD=="medDur"){
                        $duration_values=getAllDuration("median");
                        $('#performanceDuration').prop('title', $('#pd_medDur').prop('title'));
                    }else if (typeD=="meanDur"){
                        $duration_values=getAllDuration("mean");
                        $('#performanceDuration').prop('title', $('#pd_meanDur').prop('title'));
                    }else if (typeD=="maxDur"){
                        $duration_values=getAllDuration("max");
                        $('#performanceDuration').prop('title', $('#pd_maxDur').prop('title'));
                    }else if (typeD=="minDur"){
                        $duration_values=getAllDuration("min");
                        $('#performanceDuration').prop('title', $('#pd_minDur').prop('title'));
                    }
                    
                    var $title = $("#"+node_id+" title").html(); 				
                            
                            $titolo_name=$title.split(' (')[0].trim()
                            $("#"+node_id).find(".text_name").text($titolo_name);
                            $("#"+node_id).find(".stat_name").text("("+timeToHms($duration_values[$titolo_name])+")");
            
                            $("#"+node_id).find("title").text($titolo_name+" ("+timeToHms($duration_values[$titolo_name])+")");


                    if($("#"+node_id).find("title").text()=="@@E (instant)" || $("#"+node_id).find("title").text()=="@@S (instant)"){
                        $("#"+node_id).find("title").text(" ")
                    }	
                }

            });


            var classedge = $('.edge');
            var classedge = document.getElementById('graphContainer').getElementsByClassName("edge");
            $.each(classedge, function(index, item) {
                var edge_id=item.id
                // if(node_id!="@@S" && node_id!="@@E"){
                    
                    if(typeD=="totDur"){
                        $duration_edge_values=getAllEdgeDuration("total");
                        // console.log("total")
                    }else if (typeD=="medDur"){
                        $duration_edge_values=getAllEdgeDuration("median");
                        // console.log("median")
                    }else if (typeD=="meanDur"){
                        $duration_edge_values=getAllEdgeDuration("mean");
                        // console.log("mean")
                    }else if (typeD=="maxDur"){
                        $duration_edge_values=getAllEdgeDuration("max");
                        // console.log("max")
                    }else if (typeD=="minDur"){
                        $duration_edge_values=getAllEdgeDuration("min");
                        // console.log("min")
                    }
                    

                    var $title = $("#"+edge_id+" title").html(); 	
                    var $label_title= $("#"+edge_id).find("label").text(); 
                    // console.log($label_title)	
                    
                    if($label_title!=undefined && $label_title!=""){
                        // console.log($label_title)
                        var $source= $label_title.split('->')[0].trim()
                        // console.log($source)
                        var $target= $label_title.split('->')[1].trim()
                        // console.log($target)

                        
                        // console.log(nodes_id_association)
                        // console.log(nodes_id_association[$source])
                        // console.log(nodes_id_association[$target])
                        // console.log(secondsToHms($duration_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]))
                        
                    
                    
                        $("#"+edge_id).find("text").text(edgetimeconvert($duration_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));
                        $("#"+edge_id).find("title").text(timeToHms($duration_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));

                        if($source=="@@startnode"){
                            $("#"+edge_id).find("text").text("");
                            $("#"+edge_id).find("title").text("")
                        }
                        if($target=="@@endnode"){
                            $("#"+edge_id).find("text").text("");
                            $("#"+edge_id).find("title").text("")
                        }

                    }
                    
                    
                // }
                
                
                
            });
            
        });

       
        $('#perfromanceFrequency').on('change', function(e) {
            e.stopImmediatePropagation();

            var typeF=this.value
            var titoloCheck= this.title

            //var classList = $('.node');
            var classList = document.getElementById('graphContainer').getElementsByClassName("node");
            
            // console.log(classList)
            $.each(classList, function(index, item) {
                var node_id=item.id
                if(node_id!="@@S" && node_id!="@@E" && $("#"+node_id).find(".label_node").text()!="@@startnode" && $("#"+node_id).find(".label_node").text()!="@@endnode"){
                    if(typeF=="absFreq"){
                        $frequency_values=getAllFrequency("absolute");
                        $('#perfromanceFrequency').prop('title', $('#pf_absFreq').prop('title'));
                    }else if (typeF=="caseFreq"){
                        $frequency_values=getAllFrequency("case");
                        $('#perfromanceFrequency').prop('title', $('#pf_caseFreq').prop('title'));
                    }else if (typeF=="maxRep"){
                        $frequency_values=getAllFrequency("max");
                        $('#perfromanceFrequency').prop('title', $('#pf_maxRep').prop('title'));
                    }
                    
                    var $title = $("#"+node_id+" title").html(); 				
                            
                    $titolo_name=$title.split(' (')[0].trim()
                    $("#"+node_id).find("text").text($titolo_name+" ("+($frequency_values[$titolo_name])+")");
                    // $("#"+node_id).find(".stat_name").text("("+timeToHms($duration_values[$titolo_name])+")");
    
                    $("#"+node_id).find("title").text($titolo_name+" ("+($frequency_values[$titolo_name])+")");


                    if($("#"+node_id).find("title").text()=="@@E (instant)" || $("#"+node_id).find("title").text()=="@@S (instant)"){
                        $("#"+node_id).find("title").text(" ")
                    }	
                } 
            });

            
            //var classedge = $('.edge');
            var classedge = document.getElementById('graphContainer').getElementsByClassName("edge");
            $.each(classedge, function(index, item) {
                var edge_id=item.id
                // if(node_id!="@@S" && node_id!="@@E"){
                 
                    if(typeF=="absFreq"){
                        $frequency_edge_values=getAllEdgeFrequency("absolute");
                        $('#perfromanceFrequency').prop('title', $('#pf_absFreq').prop('title'));
                    }else if (typeF=="caseFreq"){
                        $frequency_edge_values=getAllEdgeFrequency("case");
                        $('#perfromanceFrequency').prop('title', $('#pf_caseFreq').prop('title'));
                    }else if (typeF=="maxRep"){
                        $frequency_edge_values=getAllEdgeFrequency("max");
                        $('#perfromanceFrequency').prop('title', $('#pf_maxRep').prop('title'));
                    }
                    

                    var $title = $("#"+edge_id+" title").html(); 	
                    var $label_title= $("#"+edge_id).find("label").text(); 
                    // console.log($label_title)	
                    
                    if($label_title!=undefined && $label_title!=""){
                        // console.log($label_title)
                        var $source= $label_title.split('->')[0].trim()
                        // console.log($source)
                        var $target= $label_title.split('->')[1].trim()
                        // console.log($target)

                        // console.log(nodes_id_association)
                        // console.log(nodes_id_association[$source])
                        // console.log(nodes_id_association[$target])
                        // console.log(secondsToHms($duration_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]))
 
                        if($source=="@@startnode"){
                            $("#"+edge_id).find("text").text(($frequency_edge_values["@@startnode"+"#"+nodes_id_association[$target]]));
                            $("#"+edge_id).find("title").text(($frequency_edge_values["@@startnode"+"#"+nodes_id_association[$target]]));
                        }else if($target=="@@endnode"){
                            $("#"+edge_id).find("text").text(($frequency_edge_values[nodes_id_association[$source]+"#"+"@@endnode"]));
                            $("#"+edge_id).find("title").text(($frequency_edge_values[nodes_id_association[$source]+"#"+"@@endnode"]));
                        }else{
                            $("#"+edge_id).find("text").text(($frequency_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));
                            $("#"+edge_id).find("title").text(($frequency_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));
                        }
        
                        // $("#"+edge_id).find("text").text(($frequency_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));
                        // $("#"+edge_id).find("title").text(($frequency_edge_values[nodes_id_association[$source]+"#"+nodes_id_association[$target]]));

                        if($source=="@@startnode" && typeF=="maxRep"){
                            $("#"+edge_id).find("text").text("");
                            $("#"+edge_id).find("title").text("")
                        }
                        if($target=="@@endnode" && typeF=="maxRep"){
                            $("#"+edge_id).find("text").text("");
                            $("#"+edge_id).find("title").text("")
                        }
                    }
                // } 
            }); 
        });
    });
    // *******************************************************************************************************************************
    
    var median = document.getElementById('median').innerHTML;
    var total = document.getElementById('total').innerHTML;
    
    //function to convert seconds to year,day,hour,minute,seconds



    function secondsToHms(d) {
        d = Number(d);
        
        var result=""
        if(d>=1){
            var year = Math.floor(d / (3600*24*365));
            var day = Math.floor(d % (3600*24*365) /(3600*24));
            var h = Math.floor(d % (3600*24*365) % (3600*24) /3600);
            var m = Math.floor(d % (3600*24*365) % (3600*24) % 3600 / 60);
            var s = Math.floor(d % (3600*24*365) % (3600*24) % 3600 % 60);

            var yearDisplay = year > 0 ? year + (year == 1 ? "year, " : "years, ") : "";
            var dayDisplay = day > 0 ? day + (day == 1 ? " day, " : " days, ") : "";
            var hDisplay = h > 0 ? h + (h == 1 ? " hour, " : " hours, ") : "";
            var mDisplay = m > 0 ? m + (m == 1 ? " minute, " : " minutes, ") : "";
            var sDisplay = s > 0 ? s + (s == 1 ? " second" : " seconds") : "";
            result=yearDisplay+dayDisplay+hDisplay + mDisplay + sDisplay;
            
        }else if(d>=0 && d<1){
            var milliDisplay= d*1000;
            result= Math.round(milliDisplay)+ " milliseconds"
        }
        if(result==""){
                result="instant"
            }
        return result; 
    }

    //function similar to secondsToHms used inside node
    function timeToHms(d) {
        d = Number(d);
    
        var result=""
        if(d>=1){
            var year = Math.floor(d / (3600*24*365));
            var day = Math.floor(d % (3600*24*365) /(3600*24));
            var h = Math.floor(d % (3600*24*365) % (3600*24) /3600);
            var m = Math.floor(d % (3600*24*365) % (3600*24) % 3600 / 60);
            var s = Math.floor(d % (3600*24*365) % (3600*24) % 3600 % 60);

            var yearDisplay = year > 0 ? year + (year == 1 ? "y" : "y") : "";
            var dayDisplay = day > 0 ? day + (day == 1 ? "d" : "d") : "";
            var hDisplay = h > 0 ? h + (h == 1 ? "h" : "h") : "";
            var mDisplay = m > 0 ? m + (m == 1 ? "m" : "m") : "";
            var sDisplay = s > 0 ? s + (s == 1 ? "s" : "s") : "";
            result=yearDisplay+dayDisplay+hDisplay + mDisplay + sDisplay;
            
        }else if(d>=0 && d<1){
            var milliDisplay= d*1000;
            result= Math.round(milliDisplay)+ "ms"
        }
        if(result==""){
                result="0ms"
            }
        return result; 
    }

    //function to convert edge time
    function edgetimeconvert(d) {
        d = Number(d);
    
        var result=""
        if(d>=1){
            var year= Math.floor(d/(365*24*60*60))
            if(year>=1){ return year+"y"}
            var w = Math.floor(d/(7*24*60*60))
            if(w>1){ return w+"w"}
            var days= Math.floor(d/(24*60*60))
            if(days>1){ return days+"d"}
            var h = Math.floor(d / 3600);
            if(h>1){ return h+"h"}
            var m = Math.floor(d / 60);
            if(m>1){ return m+"m"}
            var s = Math.floor(d);
            result=s+"s";
            
        }else if(d>=0 && d<1){
            var milliDisplay= d*1000;
            result= Math.round(milliDisplay)+ "ms"
        }
        // if(result==""){
        // 		result="instant"
        // 	}
        return result; 
    }

    var x = secondsToHms(median);
    var y = secondsToHms(total);
    // popup.innerHTML = "Median duration: " + x + "<br /> <br />" + "Total duration: " + y;
    // popup.innerHTML = "Median duration: " + x + "<br /> <br />" + "Total duration: " + y+ "<br /> <br />"  +"Selected duration node: "+ secondsToHms($totale) + "<br /> <br />" +"Selected duration edge: <br />"+ secondsToHms($totale_edge);    

    // function to do dictionary sum
    function sum_dict( obj ) {
        var sum = 0;
        for( var el in obj ) {
            if( obj.hasOwnProperty( el ) ) {
            sum += parseFloat( obj[el] );
            }
        }
        return sum;
    }


    // When the user clicks, open the popup
    function openInfo(e) {
        // popup.classList.toggle("show");
        popup.style.left = e.clientX-100+200 + "px";
        popup.style.top = (e.clientY-280) + "px";
        //popup.style.top= "10%"
        //popup.style.left= "47%"
        popup.style.zIndez = "15";
    }	



}