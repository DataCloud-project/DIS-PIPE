function getNameDsl(){

    //console.log("Function: getNameDsl()")

    $("#showRes").attr("onclick","showResultonDFGbis('esterno')");
    document.getElementById("blocker_dsl_name").style.display = "block";
    document.getElementById("popupDslName").style.display = "block";
    
    

    var oReq1 = new XMLHttpRequest();
    oReq1.addEventListener("load", getNameDslWork);
	oReq1.open("POST", frontend+"deleteRemap", false);
	oReq1.send();


}

function getNameDslWork(){
    //console.log("Function: getNameDslWork()")

    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", getNameDslListener);
	oReq.open("GET", frontend+"getDslName", false);
	oReq.send();

    document.getElementById("gridViewBtn").click();

}

function getNameDslListener(){
    //console.log("Function: getNameDslListener()")

    var dslData=JSON.parse( this.responseText )
    //console.log(dslData["data"])
    
    dslNameArray=[]
    dslIdArray=[]
    
    for (let i in dslData["data"]) {
        dslNameArray.push(dslData["data"][i]["name"]);      
        dslIdArray.push(dslData["data"][i]["id"]);    
    }

    //console.log(dslNameArray.length)
    //console.log(dslIdArray.length)
    //var dslNameArraySingle = [...new Set(dslNameArray)];
    //var dslIdArraySingle = [...new Set(dslIdArray)]

    //console.log(dslIdArraySingle.length);
    //console.log(dslNameArraySingle.length);

    for (let j in dslNameArray){
        
        $('#dslNameList').append('<div class="columnNameList" onclick="getDslbyName('+"'"+dslIdArray[j]+"'"+')"><h3>'+dslNameArray[j]+'</h3></div>');   

    }
}


function closePopupDslName(){
    //console.log("Function: closePopupDslName()")

    document.getElementById("blocker_dsl_name").style.display = "none";
    document.getElementById("popupDslName").style.display = "none";
}


// Get the elements with class="column"
var elements = document.getElementsByClassName("columnNameList");

// Declare a loop variable
var i;

// List View
function listView() {
    console.log("Function: listView()")

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
    //console.log("Function: gridView()")
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
    //console.log("Function: getDslbyName("+nomeDsl+")")

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

/*
function getDslbyNameListener(){
    console.log("Function: getDslbyNameListener()")
    
    var response=this.responseText.split("£")

    tr_name=response[4]
    log_name=response[3]
    nuovo_grafo=response[5]
    

    document.getElementById("new_digraph").innerHTML = nuovo_grafo

    //console.log("it's a test")
    //console.log(tr_name, log_name)

    addTransitionName(tr_name,log_name);
    
    document.getElementById("stringPetriNet").innerHTML = response[0];

    petri_sample = document.getElementById("stringPetriNet").innerHTML
    petri_sample = petri_sample.replace(/&#34;/g, '"');
    petri_sample = petri_sample.replace(/&gt;/g, ">");
    petri_sample = petri_sample.replace(/&lt;/g, "<");
    petri_sample = petri_sample.replace(/●/g, " ");
    petri_sample = petri_sample.replace(/■/g, " ");
}*/



function getDslbyNameListener() {

    
    const p1 = new Promise((resolve, reject) => {
        console.log("Function: getDslbyNameListener()");
          
        var response=this.responseText.split("£")
        
        tr_name=response[4]
        console.log(tr_name)
        log_name=response[3]
        console.log(log_name)
        nuovo_grafo=response[5]

        document.getElementById("new_digraph").innerHTML = nuovo_grafo;
        document.getElementById("stringPetriNet").innerHTML = response[0];

        petri_sample = document.getElementById("stringPetriNet").innerHTML;
        petri_sample = petri_sample.replace(/&#34;/g, '"');
        petri_sample = petri_sample.replace(/&gt;/g, ">");
        petri_sample = petri_sample.replace(/&lt;/g, "<");
        petri_sample = petri_sample.replace(/●/g, " ");
        petri_sample = petri_sample.replace(/■/g, " ");

        // Resolve the promise with the result
        resolve({ tr_name, log_name }); 
    });
    p1.then(
        (value) => {
            console.log(value); // Success!
            const { tr_name, log_name } = value
            addTransitionName(tr_name, log_name);
        },
        (reason) => {
          console.error(reason); // Error!
        },
      ); 
}



/*
async function addTransitionNameAsync() {
    try {
        const { tr_name, log_name } = await getDslbyNameListener();
        addTransitionName(tr_name, log_name);
    } catch (error) {
        console.error(error);
    }
}*/




// Call addTransitionNameAsync function to start the process
//addTransitionNameAsync();










function showResultonDFGbis(tipologia){
    console.log("Function: showResultonDFGbis("+tipologia+")")

    //controllare se sono inseriti tutti i dati
    if(checkPlannerChosen() && checkDisalignmentMove() && checkTraceLengthFilter()) {

        //messaggio loading visibile
        $("#loadingMessage").css("visibility", "visible");
        //andare ad inizio pagina
        $('html, body').animate({ scrollTop: 0 }, 'fast');

        setTimeout(() => {

            document.getElementById("slidercontainer_cChecking").style.display = "block"

            //rimuovere le opzioni dalla select che permette di cambiare le tracce
            $('#trace_selected').find('option').remove()

            //andare a inizio pagina e identificare cosa sta nel display
            $('html, body').animate({ scrollTop: 0 }, 'fast');
            document.getElementById("formConformanceChecking").style.display = "none";
            document.getElementById("formConformanceChecking2").style.display = "none";
            document.getElementById("map2-content").style.display = "block";
 
            $("#btn_conformance").css('visibility', 'visible');
            $("#back_slide_conf").css('visibility', 'visible');
            $("#info_check_conformance").css('visibility','visible');
        
            //salvare i dati del grafico
            var response2 = ""
            if(tipologia=="esterno"){
                response2 = document.getElementById("new_digraph").innerHTML
            }else{
                response2 = document.getElementById("digraphF").innerHTML
            }

            //fare cose sui dati del grafico
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
            }

            //creare immagine del grafico usando il valore dei dati con response2 e options2
            var image2 = Viz(response2, options2);	

            //identificare il contenitore del grafico
            var main2 = document.getElementById('graphContainer2');

            //inserire nel contenitore de grafico il grafico
            main2.innerHTML = image2;	
            
            
            /* TOGLIERE COMMENTO SE SERVE
            //modificare lo stroke e la text-decoration dei nodi dentro graphContainer2
            $("#graphContainer2").find(".node").find("polygon").attr('stroke', "#000000");
            $("#graphContainer2").find(".node").find("text").css({"text-decoration":"revert"});
            //modificare lo stroke e il text degli edge dentro graphContainer2
            $("#graphContainer2").find(".edge").find("path").attr('stroke', "#000000");
            $("#graphContainer2").find(".edge").find("polygon").attr('stroke', "#000000");
            $("#graphContainer2").find(".edge").find("text").css({"text-decoration":"revert"});
            */
            
            //main2.innerHTML = document.getElementById('graphContainer').innerHTML;

            
            var prova=$('#graphContainer2')
            var prova2=prova.find("svg")
            var prova3=prova2.find("#graph0")
        
        
            //Impostare lunghezza minima e lunchezza massima del conformance checking
            var lengthMin=String(Math.floor(parseInt(min_event_length)/2));
            var lengthMax=String(parseInt(max_event_length)*3);
            if($('#trace_length').is(":checked")){
                var message_min = $('#minlen').val();
                if(message_min!="" && message_min!=null){
                    lengthMin=String(message_min)
                }
                var message_max = $('#maxlen').val();
                if(message_max!="" && message_max!=null){
                    lengthMax=String(message_max)
                }
            }

            //impostare se i duplicati sono considerati o meno
            var duplicateChoose;
            if($('#discard_check').is(":checked")){
                duplicateChoose="true"
            }else{
                duplicateChoose="false"    
            }
        
            //scegliere il planner da usare            
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

            //eseguire funzioni per il conformance checking
            jarRequest(lengthMin,lengthMax,plannerChoose,duplicateChoose);
            //traceRequest();
            //updateInfoRequest();

            //personalizeNode
            
            for (var i=0;i<Object.keys(color_node_highlight).length;i++) {
                var id_node=Object.keys(color_node_highlight)[i]
                //$("#"+id_node).find("polygon").attr('stroke', color_node_highlight[id_node]);
                console.log(id_node)
                console.log(color_node_highlight[id_node])
            }


            //Aggiorno informazioni sulla traccia
            /*
            var traceSelectedName=String($("#trace_selected").val());
            var oReq = new XMLHttpRequest();
            oReq.addEventListener("load", updateTraceListener);
            oReq.open("GET", frontend+"updateTraceDetail?nameTrace="+traceSelectedName, false);
            oReq.send();
            */

            //scrollo in alto e rimuovo caricamento
            $('html, body').animate({ scrollTop: 0 }, 'fast');
            $("#loadingMessage").css("visibility", "hidden");

        }, 10);
       
    }




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


function findNumberSkipIns(r3){
    var lengthsDict = {};
    for (var key in r3) {
        // Check if the current property is an array
        if (Array.isArray(r3[key])) {
            // Store the lengths of the arrays
            lengthsDict[key] = r3[key].map(function(arr) {
                return arr.length;
            });
        }
    }
    return lengthsDict
}



function personalizeNode(){
    //per ogni nodo dentro graphContainer2 fai
        var prova=$('#graphContainer2')
        var prova2=prova.find("svg")
        var prova3=prova2.find("#graph0")
        var classList = prova3.find('.node');
        $.each(classList, function(index, item) {
            var node_id=item.id
            
            //Modificare id dei nodi aggiungendo _
            item.id=node_id+"_"
            
            //prendere il titolo della stringa del nodo
            var change_title=$("#"+item.id).find("text").html()
            
            //modificare titolo togliendo parentesi
            refactor_title=(change_title.split("(")[0])

            //text dei nodi modificato
            $("#"+item.id).find("text").html(refactor_title)
            //title dei nodi modificato
            $("#"+item.id).find("title").html(refactor_title)
            
            //modificare font-size dei testi
            var font_size = $("#"+item.id).find("text").attr("font-size")
            $("#"+item.id).find("text").attr("font-size",font_size-2)
            
            //modificare colore dentro il nodo
            $("#"+item.id).find("polygon").attr('fill','#fdfdff')
            
            //modificare stroke-width dei nodi
            $("#"+item.id).find("polygon").attr('stroke-width',4)

            //######################
            
            var titolo=refactor_title.replaceAll(" ","").replaceAll("-","_").replaceAll("_End","").replaceAll("_","").toLowerCase()
            var titolo_end = refactor_title.replaceAll(" ","").replaceAll("-","_").toLowerCase()

            console.log("Sono in personilizeNode, il titolo prima è: "+ titolo)
            //#################################

            var selected_elem=""
            var minimo=5000
            for (let j = 0; j < log_array.length; j++) {
                if(levenshteinDistance(refactor_title,log_array[j])<minimo){
                    minimo=levenshteinDistance(refactor_title,log_array[j])
                    selected_elem=log_array[j]
                }
            }

            if(refactor_title.toLowerCase() in color_dictionary){
                titolo=refactor_title.toLowerCase()
                console.log("Sono in personilizeNode, primo if")
            }else if(refactor_title.toLowerCase().replaceAll(" ","").replaceAll("-end","") in color_dictionary){
                titolo=refactor_title.toLowerCase().replaceAll(" ","").replaceAll("-end","")
                console.log("Sono in personilizeNode, secondo if")
            }else if(refactor_title.toLowerCase().replaceAll(" ","").replaceAll("-end","").replaceAll("-","_") in color_dictionary){
                titolo=refactor_title.toLowerCase().replaceAll(" ","").replaceAll("-end","").replaceAll("-","_")
                console.log("Sono in personilizeNode, terzo if")
            }else{
                console.log("Sono in personilizeNode, quarto if")
                var stringa_temporanea=refactor_title.toLowerCase().replaceAll(" ","").replaceAll("-end","").replaceAll("-","_")
                var stringa_comparazione=selected_elem.toLowerCase().replaceAll(" ","")

                var stringa_finale=findPossibleTitolo(stringa_temporanea,stringa_comparazione)
                
                if(stringa_finale in color_dictionary){
                    titolo=stringa_finale
                }
            }

            console.log("Sono in personilizeNode, il titolo dopo è: "+ titolo)

            //################################


            if(titolo in color_dictionary){
                console.log("sto printando il titolo DPG")
                console.log(titolo)
                if(color_dictionary[titolo]==0){
                    //console.log("i am inside the problem")
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
                console.log("sto printando il titolo DPG")
                console.log(titolo_end)
                if(color_dictionary[titolo_end]==0){
                    //console.log("i am inside the problem")
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
                console.log("il titolo nonè è dentro")
                console.log(titolo_end)
                $("#"+item.id).find("polygon").attr('stroke',"#999999")
                color_node_highlight[item.id]="#999999"
            }

            //######################
            
            //estrarre il node_number dall id
            var node_number = item.id
            var node_number = node_number.replace("node","");
            var node_number= node_number.replace("_","");

            //per ogni id dei nodi dentro graphContainer2 fai qualcosa on click
            $('#'+item.id).click(function(e) {

                //prende il valore della stroke
                $temp = ($("#"+item.id).find("polygon").attr('stroke'))
                console.log("colore della stroke del nodo è: "+ $temp)
                $check_node=($("#"+item.id).find("polygon")).length

                if(($temp=="#686868" ||$temp=="#999999" || $temp=="#FFD23F" || $temp=="#000000" || $temp=="#187F00" || $temp=="#FF9292" || $temp=="#FF5C5C" || $temp=="#FF3838" || $temp=="#E20000") && $check_node!=0){

                    /* COMMENTO PER HIGHLIGHT
                    if(($temp=="#FFD23F" || $temp=="#686868")  && ($("#highlight_trace option:selected").val()=="yes")){
                        console.log("stiamo chiamando highlight per le trace")
                        highlightTraceNode();
                        $("#"+item.id).find("polygon").attr('stroke', color_highlight);

                    }else{
                    */
                        /* TOGLIERE COMMENTO SE SERVE
                        //ASSEGNARE STROKE IN BASE AL COLOR NODE HIGHLIGHT ARRAY
                        console.log("non c'è highlight per le trace")
                        for (var i=0;i<Object.keys(color_node_highlight).length;i++) {
                            var id_node=Object.keys(color_node_highlight)[i]
                            $("#"+id_node).find("polygon").attr('stroke', color_node_highlight[id_node]);
                        }
                        */

                        /* TOGLIERE COMMENTO SE SERVE
                        $("#graphContainer2").find(".node").find("polygon").attr('stroke', color_node_highlight[item.id]);
                        $("#graphContainer2").find(".node").find("text").css({"text-decoration":"revert"});
                        $("#graphContainer2").find(".edge").find("path").attr('stroke', "#000000");
                        $("#graphContainer2").find(".edge").find("polygon").attr('stroke', "#000000");
                        $("#graphContainer2").find(".edge").find("text").css({"text-decoration":"revert"});
                        */

                        /* TOGLIERE COMMENTO SE SERVE
                        if($temp=="red"){
                            $("#"+item.id).find("polygon").attr('stroke', '#f57e07');
                        }else if($temp=="green"){
                            $("#"+item.id).find("polygon").attr('stroke', '#23f507');
                        }else{
                            $("#"+item.id).find("polygon").attr('stroke', color_highlight);
                        }
                        */
                    
                    //} COMMENTO HIGHLIGHT

                    for (var i=0;i<Object.keys(color_node_highlight).length;i++) {
                        var id_node=Object.keys(color_node_highlight)[i]
                        $("#"+id_node).find("polygon").attr('stroke', color_node_highlight[id_node]);
                        $("#"+id_node).find("text").css({"text-decoration":"revert"});
                    }

                    //sottolineare testo del nodo cliccato
                    $("#"+item.id).find("text").css({"text-decoration":"underline"});
                    //Coloro per evidenziare nodo cliccato
                    $("#"+item.id).find("polygon").attr('stroke', color_highlight);

                    //dare statistiche del popup
                    document.getElementById("myPopup_cChecking").innerHTML= "<i class='fa-solid fa-square-pen'></i> Detail" +"<span class='close_pp' onclick='closePP2()'>❌</span>"+"<hr> <br>"+
                                                                            "<table style='width: 100%;'>"+
                                                                            "<tr> <th style='text-decoration: underline;'>Activity</th> <th>times</th> <th>in traces</th> </tr>"+
                                                                            "<tr> <th>skipped</th> <td style='border: revert;'> <span id='skip_act'> Gi2 </span> </td> <td style='border: revert;'> <span id='mean_skip_act'> Xk2 </span> </td> </tr>"+ //of "+total_trace_number+"
                                                                            "<tr> <th>inserted</th> <td style='border: revert;'> <span id='ins_act'> Gi3 </span> </td> <td style='border: revert;'> <span id='mean_ins_act'> Xk3 </span> </td> </tr>"+ //of "+total_trace_number+"
                                                                            "</table>"
                    //mostrare popup

                    //Codice che prima era in updateListener
                    const lst = item.id.slice(-1);
                    var innerHTMLcode=""

                    if(lst=="_"){
                        var chosen_element=$(this).find("text").text().trim()
                        name_activity=chosen_element.replaceAll(" ","").replaceAll("-","_").replaceAll("_End","").replaceAll("_","").toLowerCase()
                        name_activity_end=chosen_element.replaceAll(" ","").replaceAll("-","_").toLowerCase()
                        

                        var selected_elem=""
                        var minimo=5000
                        for (let j = 0; j < log_array.length; j++) {
                            if(levenshteinDistance(chosen_element,log_array[j])<minimo){
                                minimo=levenshteinDistance(chosen_element,log_array[j])
                                selected_elem=log_array[j]
                            }
                        }

                        if(chosen_element.toLowerCase() in color_dictionary){
                            name_activity=chosen_element.toLowerCase()
                            console.log("Sono in personilizeNode, primo if")
                        }else if(chosen_element.toLowerCase().replaceAll(" ","").replaceAll("-end","") in color_dictionary){
                            name_activity=chosen_element.toLowerCase().replaceAll(" ","").replaceAll("-end","")
                            console.log("Sono in personilizeNode, secondo if")
                        }else if(chosen_element.toLowerCase().replaceAll(" ","").replaceAll("-end","").replaceAll("-","_") in color_dictionary){
                            name_activity=chosen_element.toLowerCase().replaceAll(" ","").replaceAll("-end","").replaceAll("-","_")
                            console.log("Sono in personilizeNode, terzo if")
                        }else{
                            console.log("Sono in personilizeNode, quarto if")
                            var stringa_temporanea=chosen_element.toLowerCase().replaceAll(" ","").replaceAll("-end","").replaceAll("-","_")
                            var stringa_comparazione=selected_elem.toLowerCase().replaceAll(" ","")

                            var stringa_finale=findPossibleTitolo(stringa_temporanea,stringa_comparazione)
                            
                            if(stringa_finale in color_dictionary){
                                name_activity=stringa_finale
                            }
                        }

                        
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
                            console.log("name activity sta in r2")
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
                
                    $("#myPopup_cChecking").attr("class","popuptext show")
                    openInfoCc(e)
                }
                /* se c'è hoghlight
                else if(($("#highlight_trace option:selected").val()=="yes") && ($temp==color_highlight || $temp=='#23f507' || $temp=='#f57e07') && $check_node!=0){
                    
                    var title_id2=$("#"+item.id).find("title").html()
                    var titolo2=title_id2.replaceAll(" ","").toLowerCase()
        
                    if(array_complete_activity.includes(titolo2)) {
                        $("#"+item.id).find("polygon").attr('stroke', "#FFD23F");
                    }else{
                        $("#"+item.id).find("polygon").attr('stroke', "#686868");
                    } 
                    
                    $("#"+item.id).find("text").css({"text-decoration":"revert"});
                    $("#myPopup_cChecking").attr("class","popuptext")
                
                }*/
                else if(($temp==color_highlight || $temp=='#23f507' || $temp=='#f57e07') && $check_node!=0){
                    $("#"+item.id).find("polygon").attr('stroke', color_node_highlight[item.id]);
                    $("#"+item.id).find("text").css({"text-decoration":"revert"});
                    $("#myPopup_cChecking").attr("class","popuptext")
                }

                
            });
            
        })
    
    }

    
function findPossibleTitolo(str_temporanea, str_comparazione) {
    var stringa_temporanea=str_temporanea
    var stringa_comparazione=str_comparazione
    var original_underscore = findCharPositions(stringa_temporanea, '_');
    var comparison_underscore = findCharPositions(stringa_comparazione, '_');
    
    
    if (comparison_underscore.length == 0) {
        
    } else {
        //console.log(stringa_temporanea);
        //console.log(stringa_comparazione);
        for (const comparison_position in comparison_underscore) {
        var string_c = stringa_comparazione[comparison_underscore[comparison_position] - 1] + stringa_comparazione[comparison_underscore[comparison_position]] + stringa_comparazione[comparison_underscore[comparison_position] + 1];
        for (const original_position in original_underscore) {
            var string_p = stringa_temporanea[original_underscore[original_position] - 1] + stringa_temporanea[original_underscore[original_position]] + stringa_temporanea[original_underscore[original_position] + 1];
    
            if (string_c === string_p) {
            //console.log("_____siamo_if______");
            //console.log(string_c);
            //console.log(string_p);
            //console.log("_____siamo_if______");
            var replacement = "§";
            var originalArray = stringa_temporanea.split("");
    
            originalArray[original_underscore[original_position]] = replacement;
    
            // Convert the array back to a string
            var modifiedStr = originalArray.join("");
            stringa_temporanea = modifiedStr;
            //console.log(stringa_temporanea);
            } else {
    
            }
        }
    
        }
    }
    
    stringa_finale = stringa_temporanea.replaceAll("_", "").replaceAll("§", "_");
    return stringa_finale
}
