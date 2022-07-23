var color_highlight='#8B008B';

var disalignment_dictionary={}
var inizio=0

var tr_name=""
var log_name=""

function textareaInsert(){
    
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
}

function backToPersonalize(){
    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "none";
    document.getElementById("formConformanceChecking2").style.display = "block";
    document.getElementById("map2-content").style.display = "none";

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
        log_array.push("InvisibleT");
        for (let i = 0; i < tr_array.length; i++) {
        // console.log(tr_array[i][1])
        if(tr_array[i][1]=="None"){
            tr_array[i][1]="InvisibleT"
            $("#transition_pnml").append("<p style='font-style: italic;'>"+tr_array[i][1]+"</p>","<br>");
        }else{
            $("#transition_pnml").append("<p>"+tr_array[i][1]+"</p>","<br>");
        }
           // A
        // sel.append($("<option>").attr('value',1).text(tr_array[i][1]));
        if(i==0){
            $('#disalignment_log').append($("<option>").attr('value',tr_array[i][1]).text(tr_array[i][1]).attr('selected','selected'))
        }else{
            $('#disalignment_log').append($("<option>").attr('value',tr_array[i][1]).text(tr_array[i][1]))
        }
        
        $('#move_log').text("0")
        $('#move_model').text("0")
        var sel = $('<select>').attr('id',tr_array[i][1])

        disalignment_dictionary[tr_array[i][1]]=[1,1]
        if(tr_array[i][1]=='None'){
            disalignment_dictionary[tr_array[i][1]]=[0,0]
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
            if(selected_elem==log_array[k]){

                sel.append($("<option>").attr('value',log_array[k]).text(log_array[k]).attr('selected','selected'));
            }else{
                sel.append($("<option>").attr('value',log_array[k]).text(log_array[k]));
            }
            
        }
        $('#transition_log').append(sel,"<br>","<br>");
        // console.log(String(i))
        }

        changedisalignment_select()
    

        // console.log(disalignment_dictionary)
        inizio=1
    
    }
    // console.log(typeof tr_array);
    

    
    
}

function showResultonDFG(){
    $('html, body').animate({ scrollTop: 0 }, 'fast');
    document.getElementById("formConformanceChecking").style.display = "none";
    document.getElementById("formConformanceChecking2").style.display = "none";
    document.getElementById("map2-content").style.display = "block";
    // var gpContainer=$( "#graphContainer" ).clone()

    
    
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
    
    var color_node_highlight={}

    var classList = prova3.find('.node');
    // console.log(classList)
                
    $.each(classList, function(index, item) {
        var node_id=item.id
        // console.log(node_id)
        item.id=node_id+"_"

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
        $('#'+item.id).click(function() {
            // alert('ho ho ho1');

            $temp= ($("#"+item.id).find("polygon").attr('stroke'))
            $check_node=($("#"+item.id).find("polygon")).length

            if(($temp=="#000000" || $temp=="red" || $temp=="green") && $check_node!=0){

                

                // for (const element in Object.keys(color_node_highlight)) {
                //     $("#"+element).find("polygon").attr('stroke', color_node_highlight[element]);
                //     console.log(element)
                // }

                for (var i=0;i<Object.keys(color_node_highlight).length;i++) {
                    var id_node=Object.keys(color_node_highlight)[i]
                    $("#"+id_node).find("polygon").attr('stroke', color_node_highlight[id_node]);
                    console.log(id_node)
                    console.log(color_node_highlight[id_node])
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
                }
                
                $("#"+item.id).find("text").css({"text-decoration":"underline"});

                document.getElementById("myPopup_cChecking").innerHTML= "<i class='fa-solid fa-square-pen'></i> Detail" +"<hr> <br>"+"Move log + model: "+"<br /> <br />"+
                                                                        "Traces where move log + model occur: " +"<br /> <br />"+
                                                                        "Move model only (in all traces): " +"<br /> <br />"+
                                                                        "Traces where move model only occur: "
                $("#myPopup_cChecking").attr("class","popuptext show")
                // console.log("you have selected a node")

            }else if(($temp==color_highlight || $temp=='#23f507' || $temp=='#f57e07') && $check_node!=0){
                $("#"+item.id).find("polygon").attr('stroke', color_node_highlight[item.id]);
                $("#"+item.id).find("text").css({"text-decoration":"revert"});
                $("#myPopup_cChecking").attr("class","popuptext")
                // console.log("you have deselected a node")
            }


        });
        // $("#"+node_id).attr("id","ciao")
    })

    console.log(color_node_highlight)

    var classList = prova3.find('.edge');
    // console.log(classList)
                
    $.each(classList, function(index, item) {
        var edge_id=item.id
        // console.log(edge_id)
        item.id=edge_id+"_"
        // console.log(item.id)
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

            
        });
        // $("#"+edge_id).attr("id","ciao")
    })
    // console.log(targetNode)
    // console.log(targetEdge)

    // document.getElementById('graphContainer2').innerHTML = document.getElementById('graphContainer').innerHTML

    // $(document).ready(function(){
    
    // 	$('#node1_').click(function() {
    // 		alert('ho ho ho');
    // 	});
    // });

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
        // console.log(changed_disalign)
        // console.log($("#move_model").val())
        // disalignment_dictionary[tr_array[i][1]]=[0,0]
        var temp=disalignment_dictionary[changed_disalign]
        temp[1]=parseInt($("#move_log").val())
        disalignment_dictionary[changed_disalign]=temp

    }else if(valore=="model"){
        var changed_disalign=$('#disalignment_log option:selected').val();
        // console.log(changed_disalign)
        // console.log($("#move_model").val())
        var temp=disalignment_dictionary[changed_disalign]
        temp[0]=parseInt($("#move_model").val())
        disalignment_dictionary[changed_disalign]=temp

    }
    // console.log(disalignment_dictionary)

}

function displayTracePopUp(){
    if(document.getElementById("check_conformance").checked){
        document.getElementById("tabTrace").style.visibility = "visible";
        document.getElementById("mytabsConformance").style.display = "block";
        // console.log("it work on true")
    }else{
        document.getElementById("mytabsConformance").style.display = "none";
        // console.log("it work on false")
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