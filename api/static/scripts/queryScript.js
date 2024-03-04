function queryDbListener() {
    console.log("queryDb listener")
}

function queryDbRequest() {

	$("#loadingMessage").css("visibility", "visible");

	

	setTimeout(() => {
   
		var oReq = new XMLHttpRequest();
		oReq.addEventListener("load", queryDbListener);
		oReq.open("GET", frontend+"queryDb", false);
		oReq.send();

		$("#loadingMessage").css("visibility", "hidden");

	}, 10);
    
}

function initializeDBListener(){
	//console.log(this.responseText)
	document.getElementById("resultQuery").innerHTML=""
	//document.getElementById("resultQuery").innerHTML=this.responseText
	var ciao=this.responseText.split("\n")
	//console.log(ciao.length)
	for(var i=0; i<ciao.length;i++){
		document.getElementById("resultQuery").innerHTML=document.getElementById("resultQuery").innerHTML+ciao[i]+"<br>"
	}

}

function initializeDBRequest(){

	var percentageFilter=document.getElementById("queryPercentage").value
	//console.log(percentageFilter)

	$("#loadingMessage").css("visibility", "visible");

	

	setTimeout(() => {

		var oReq = new XMLHttpRequest();
		oReq.addEventListener("load", initializeDBListener);
		oReq.open("GET", frontend+"initializeQuery?queryPercentage="+percentageFilter, false);
		oReq.send();

		$("#loadingMessage").css("visibility", "hidden");

	}, 10);


}

function databasePresenceListener(){
	
	var db_response =JSON.parse(this.responseText)["presence"]
	if(db_response==="yes"){
		//console.log("databse presence")
		$("#response_database").text('A database with the name of this log is already present, do you want to redo translation? it may takes some time');
		
			var YesButton = document.createElement("button");
			YesButton.innerHTML = "Yes";
			YesButton.style = "font-size: 16px; width: 20%;"
			YesButton.id = "yesbuttonPresence";
			var div = document.getElementById('querydivfacose');
			if(document.getElementById("yesbuttonPresence") == null) {
				div.appendChild(YesButton);
			}
			
			
			YesButton.addEventListener ("click", function() {
				//chiama la funzione per la traduzione e metti un login page
				//alert("Button was clicked");
				translationRequest();
				document.getElementById("formQuery").style.display = "none";
				document.getElementById("formQuery2").style.display = "block";


				//if(CheckTranslationRequest()=="false"){
				//	takeListRequest();
				//}
				//var listBrand =['LEXUS','AUDI','MAYBACK','FERRARI','TOYOTA'];   
                //the array
                
				//printBtn(listBrand);
			});

			var NoButton = document.createElement("button");
			NoButton.innerHTML = "No";
			NoButton.style = "font-size: 16px; width: 20%;"
			var div = document.getElementById('querydivfacose');
			NoButton.id="nobuttonPresence"
			
			if(document.getElementById("nobuttonPresence") == null) {
				div.appendChild(NoButton);
			}

			NoButton.addEventListener ("click", function() {
				//alert("Button was clicked");
				//chiama la next funzione e usa il db presente
				document.getElementById("formQuery").style.display = "none";
				document.getElementById("formQuery2").style.display = "block";

				if(CheckTranslationRequest()=="false"){
					takeListRequest();
				}
				var listBrand =['LEXUS','AUDI','MAYBACK','FERRARI','TOYOTA'];   
                //the array
                
				//printBtn(listBrand);

			});

	
	}else{
		//console.log("databse not presence")
		$("#response_database").text("You have to translate the log, Do you want to continue? It may take some times");
		var YesButton = document.createElement("button");
			YesButton.innerHTML = "Yes";
			YesButton.style = "font-size: 16px; width: 20%;"
			var div = document.getElementById('querydivfacose');

			YesButton.id="yesbuttonTranslate"
			
			if(document.getElementById("yesbuttonTranslate") == null) {
				div.appendChild(YesButton);
			}

			YesButton.addEventListener ("click", function() {
				//chiama la funzione per la traduzione e metti un login page
				//alert("Button was clicked");
				translationRequest2();
				document.getElementById("formQuery").style.display = "none";
				document.getElementById("formQuery2").style.display = "block";

				//if(CheckTranslationRequest()=="false"){
				//	takeListRequest();
				//}
				//var listBrand =['LEXUS','AUDI','MAYBACK','FERRARI','TOYOTA'];   
                //the array
                
				//printBtn(listBrand);

			});

			var NoButton = document.createElement("button");
			NoButton.innerHTML = "No";
			NoButton.style = "font-size: 16px; width: 20%;"
			var div = document.getElementById('querydivfacose');
			div.appendChild(NoButton);

			NoButton.id="nobuttonTranslate"
			
			if(document.getElementById("nobuttonTranslate") == null) {
				div.appendChild(NoButton);
			}

			NoButton.addEventListener ("click", function() {
				alert("You have to click yes, to perform query");
				//ritorna to map
				//document.getElementById("formQuery").style.display = "none";
				//document.getElementById("formQuery2").style.display = "block";

				//if(CheckTranslationRequest()=="false"){
					//takeListRequest();
				//}
				//var listBrand =['LEXUS','AUDI','MAYBACK','FERRARI','TOYOTA'];   
                //the array
                
				//printBtn(listBrand);
			});
	}
}
var log_numer=1
var element_list_select=[]
var from_list=[]
var where_list=[]


function printBtn_iniziale(listBrand) {
	

	
	if(document.getElementById("log1") == null) {
		
	

	var myDiv = document.getElementById("query_filter_div");
	var iDiv = document.createElement('div');
	iDiv.id = 'log'+log_numer;
	iDiv.style = "padding-bottom: 20px;"
	
	myDiv.appendChild(iDiv);
	

	from_list.push('log'+log_numer)
	//console.log(from_list)

	for (var i = 0; i < listBrand.length; i++) {
		var checkBox = document.createElement("input");
		var label = document.createElement("label");
		checkBox.type = "checkbox";
		checkBox.id = 'log'+log_numer+"."+listBrand[i];
		checkBox.value = 'log'+log_numer+"."+listBrand[i];
		checkBox.class = "sel_check"

		checkBox.style = "font-size: 16px;"
		label.style = "font-size: 16px; padding-right: 10px;"

		checkBox.onclick = function(e) {
			//alert(e.target.id); 
			
			
			if (e.target.checked) {
				element_list_select.push(e.target.id)
				//console.log(element_list_select)
			}else{
				index = element_list_select.indexOf(e.target.id);
				x= element_list_select.splice(index, 1);
				//console.log(element_list_select)
			}
		};
		
		iDiv.appendChild(checkBox);
		iDiv.appendChild(label);
		label.appendChild(document.createTextNode('log'+log_numer+"."+listBrand[i]));
	}

	var PlusButton = document.createElement("button");
	PlusButton.innerHTML = "Add log";
	PlusButton.style = "font-size: 13px; width: 13%;"

	var div = document.getElementById('query_filter_div');
	iDiv.appendChild(PlusButton);
	PlusButton.addEventListener ("click", function() {
		printBtn(listBrand)
	})

	var LessButton = document.createElement("button");
	LessButton.style = "font-size: 13px; width: 13%;"
	LessButton.innerHTML = "Remove log";
	LessButton.class = 'log'+log_numer;
	
	iDiv.appendChild(LessButton);
	LessButton.addEventListener ("click", function(e) {

		const queryFilterDiv = document.getElementById('query_filter_div');
		const divCount = queryFilterDiv.getElementsByTagName('div').length;
		if(divCount>1){
			console.log(e.target.class)
			index = from_list.indexOf(e.target.class);
			x= from_list.splice(index, 1);
			//log_numer=log_numer-1
			console.log(from_list)
			e.target.closest('div').remove()
		}else{
			alert("It is not possible to remove this log")
		}


	})

	var select1 = document.getElementById("select_first_element");
	for(var i = 0; i < listBrand.length; i++) {
		var opt = 'log'+log_numer+"."+listBrand[i];
		var el = document.createElement("option");
		el.textContent = opt;
		el.value = opt;
		select1.appendChild(el);
	}

	var select2 = document.getElementById("select_second_element");
	for(var i = 0; i < listBrand.length; i++) {
		var opt = 'log'+log_numer+"."+listBrand[i];
		var el = document.createElement("option");
		el.textContent = opt;
		el.value = opt;
		select2.appendChild(el);
	}
	log_numer=log_numer+1;

	if (document.getElementsByClassName('remember').checked) {
		console.log("checked");
	} else {
		console.log("You didn't check it! Let me check it for you.");
	}

	}

}



function printBtn(listBrand) {
	

	var myDiv = document.getElementById("query_filter_div");
	var iDiv = document.createElement('div');
	iDiv.id = 'log'+log_numer;
	iDiv.style = "padding-bottom: 20px;"
	
	myDiv.appendChild(iDiv);
	

	from_list.push('log'+log_numer)
	//console.log(from_list)

	for (var i = 0; i < listBrand.length; i++) {
		var checkBox = document.createElement("input");
		var label = document.createElement("label");
		checkBox.type = "checkbox";
		checkBox.id = 'log'+log_numer+"."+listBrand[i];
		checkBox.value = 'log'+log_numer+"."+listBrand[i];
		checkBox.class = "sel_check"

		checkBox.style = "font-size: 16px;"
		label.style = "font-size: 16px; padding-right: 10px;"

		checkBox.onclick = function(e) {
			//alert(e.target.id); 
			
			
			if (e.target.checked) {
				element_list_select.push(e.target.id)
				//console.log(element_list_select)
			}else{
				index = element_list_select.indexOf(e.target.id);
				x= element_list_select.splice(index, 1);
				//console.log(element_list_select)
			}
		};
		
		iDiv.appendChild(checkBox);
		iDiv.appendChild(label);
		label.appendChild(document.createTextNode('log'+log_numer+"."+listBrand[i]));
	}

	var PlusButton = document.createElement("button");
	PlusButton.innerHTML = "Add log";
	PlusButton.style = "font-size: 13px; width: 13%;"

	var div = document.getElementById('query_filter_div');
	iDiv.appendChild(PlusButton);
	PlusButton.addEventListener ("click", function() {
		printBtn(listBrand)
	})

	var LessButton = document.createElement("button");
	LessButton.style = "font-size: 13px; width: 13%;"
	LessButton.innerHTML = "Remove log";
	LessButton.class = 'log'+log_numer;
	
	iDiv.appendChild(LessButton);
	LessButton.addEventListener ("click", function(e) {

		const queryFilterDiv = document.getElementById('query_filter_div');
		const divCount = queryFilterDiv.getElementsByTagName('div').length;
		if(divCount>1){
			console.log(e.target.class)
			index = from_list.indexOf(e.target.class);
			x= from_list.splice(index, 1);
			//log_numer=log_numer-1
			console.log(from_list)
			e.target.closest('div').remove()
		}else{
			alert("It is not possible to remove this log")
		}

	})

	var select1 = document.getElementById("select_first_element");
	for(var i = 0; i < listBrand.length; i++) {
		var opt = 'log'+log_numer+"."+listBrand[i];
		var el = document.createElement("option");
		el.textContent = opt;
		el.value = opt;
		select1.appendChild(el);
	}

	var select2 = document.getElementById("select_second_element");
	for(var i = 0; i < listBrand.length; i++) {
		var opt = 'log'+log_numer+"."+listBrand[i];
		var el = document.createElement("option");
		el.textContent = opt;
		el.value = opt;
		select2.appendChild(el);
	}
	log_numer=log_numer+1;

	if (document.getElementsByClassName('remember').checked) {
		//alert("checked");
		console.log("checked")
	} else {
		//alert("You didn't check it! Let me check it for you.");
		console.log("You didn't check it! Let me check it for you.");
	}


}

function removeBtn(){
	e.target.closest('div.image').remove();
}





function databasePresenceRequest(){

	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", databasePresenceListener);
	oReq.open("GET", frontend+"checkDatabasePresence", false);
	oReq.send();

}


function translationListener(){
	console.log(this.responseText)
	if(CheckTranslationRequest()=="false"){
		takeListRequest();
	}

}
function translationRequest(){

	$("#loadingMessage").css("visibility", "visible");
	setTimeout(() => {

		var oReq = new XMLHttpRequest();
		oReq.addEventListener("load", translationListener);
		oReq.open("GET", frontend+"translation1", false);
		oReq.send();

		$("#loadingMessage").css("visibility", "hidden");

	}, 10);

}

function translationListener2(){
	console.log(this.responseText)
	if(CheckTranslationRequest()=="false"){
		takeListRequest();
		//console.log("bode")
	}
}
function translationRequest2(){

	$("#loadingMessage").css("visibility", "visible");

	

	setTimeout(() => {
   
		var oReq = new XMLHttpRequest();
		oReq.addEventListener("load", translationListener2);
		oReq.open("GET", frontend+"translation2", false);
		oReq.send();


		$("#loadingMessage").css("visibility", "hidden");

	}, 10);
    

}




function CheckTranslationListener(){
	console.log(this.responseText)
	if(this.responseText=="false"){
		takeListRequest();
	}
		
}


function CheckTranslationRequest(){

	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", CheckTranslationListener);
	oReq.open("GET", frontend+"checkTranslationEnd", false);
	oReq.send();

}

function takeListListener(){
	var lista_campi=JSON.parse(this.responseText)["campi"]
	console.log(lista_campi)
	console.log(lista_campi[1])
	printBtn_iniziale(lista_campi);
}


function takeListRequest(){

	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", takeListListener);
	oReq.open("GET", frontend+"createEventLog", false);
	oReq.send();

}

function changeSecondTerm(variabile){
	if(variabile=="select"){
		document.getElementById("select_first_element").style.display="block"
		document.getElementById("first_element_textarea").style.display="none"

		document.getElementById("select_operator").style.display="block"
		
		document.getElementById("select_second_element").style.display="block"
		document.getElementById("second_element_textarea").style.display="none"

		document.getElementById("addCondition").disabled = false;

	

	}else if (variabile=="textarea"){
		document.getElementById("select_first_element").style.display="none"
		document.getElementById("first_element_textarea").style.display="block"
		
		document.getElementById("select_operator").style.display="block"
		
		document.getElementById("select_second_element").style.display="none"
		document.getElementById("second_element_textarea").style.display="block"

		document.getElementById("addCondition").disabled = false;

	}else if (variabile=="none"){
		document.getElementById("select_first_element").style.display="none"
		document.getElementById("first_element_textarea").style.display="none"
		
		document.getElementById("select_operator").style.display="none"
		
		document.getElementById("second_element_textarea").style.display="none"
		document.getElementById("select_second_element").style.display="none"

		document.getElementById("addCondition").disabled = true;
	}

}
var number_condition=1

var condition_query="#"

function addCondition(){

	document.getElementById("addCondition").disabled = true;
	document.getElementById("addAnd").disabled = false;
	document.getElementById("addOr").disabled = false;

	let select_first = document.getElementById("select_first_element")
	//let select_first_prime = select_first.cloneNode(true)
	//select_first_prime.id="select_first_element_"+number_condition

	let select_operator = document.getElementById("select_operator")
	//let select_operator_prime = select_operator.cloneNode(true)
	//select_operator_prime.id="select_operator_element_"+number_condition

	let select_second= document.getElementById("select_second_element")
	//let select_second_prime = select_second.cloneNode(true)
	//select_second_prime.id="select_second_element_"+number_condition


	let textarea_second = document.getElementById("second_element_textarea")
	let textarea_first = document.getElementById("first_element_textarea")
	//let textarea_second_prime = textarea_second.cloneNode(true)
	//textarea_second_prime.id="second_element_textarea"+number_condition
	
	var myDiv = document.getElementById("qc2");

	//var generalDiv=document.createElement('div');

	var condSpan = document.createElement('span');

	if(document.getElementById("select_second_element").style.display == "block"){
		stringCond = select_first.value+" "+select_operator.value+" "+select_second.value;
	}else if(document.getElementById("second_element_textarea").style.display == "block"){
		stringCond = textarea_first.value.trim()+" "+select_operator.value+" "+textarea_second.value.trim();
	}
	

	condition_query=condition_query+" "+stringCond

	condSpan.innerHTML=stringCond;
	var removeCondButt = document.createElement('button');
	removeCondButt.innerHTML="Remove"
	removeCondButt.class=stringCond
	
	removeCondButt.addEventListener ("click", function(e,stringCond) {
		//console.log(e.target.closest('div').textContent.slice(0, -6))
		//console.log(condition_query)

		if(condition_query.split("#").length == 3){

			//console.log("lunghezza 3")
			//console.log(condition_query.split("#"))

			condition_query=condition_query.replace("# "+e.target.closest('div').textContent.slice(0, -6),"")

			condition_query=condition_query.trim()

			//condition_query=condition_query.replace("or ","")
			//condition_query=condition_query.replace("and ","")
			
			//console.log(condition_query.length)
			if(condition_query.length==0){
				condition_query="#"
			}
		}else if(condition_query.split("#").length == 2){
			condition_query=condition_query.replace("# "+e.target.closest('div').textContent.slice(0, -6),"")
			document.getElementById("addCondition").disabled = false;
			document.getElementById("addAnd").disabled = true;
			document.getElementById("addOr").disabled = true;
			condition_query=condition_query.trim()
			//console.log(condition_query.length)
			if(condition_query.length==0){
				condition_query="#"
			}
		}else{
			condition_query=condition_query.replace("# "+e.target.closest('div').textContent.slice(0, -6),"")
			condition_query=condition_query.trim()
			//console.log(condition_query.length)
			if(condition_query.length==0){
				condition_query="#"
			}
		}
		
		//console.log(condition_query)
		e.target.closest('div').remove()
		console.log(e.target.class)
		//console.log(condition_query.split("#"))

		if(condition_query.split("#").length == 2){
			condition_query=condition_query.replace("# "+e.target.closest('div').textContent.slice(0, -6),"")

			condition_query=condition_query.trim()
			//console.log(condition_query.length)
			if(condition_query.length==0){
				condition_query="#"
			}
		}

	})
	
	
	if($("#" + "Condition"+number_condition).length == 0) {
		//it doesn't exist
		var generalDiv=document.createElement('div');
		generalDiv.id="Condition"+number_condition
	}else{
		var generalDiv = document.getElementById("Condition"+number_condition);
	}

	condSpan.style = "font-size: 16px; padding-right:14px;"
	removeCondButt.style = "font-size: 16px; width=25%"

	generalDiv.appendChild(condSpan)
	generalDiv.appendChild(removeCondButt)

	another_div=document.getElementById("where_condition")
	
	another_div.appendChild(generalDiv)
	//myDiv.appendChild(select_operator_prime)
	//myDiv.appendChild(select_second_prime)
	//myDiv.appendChild(textarea_second_prime)
	
	
	number_condition=number_condition+1
	//console.log(condition_query)
}

function addAnd(){
	document.getElementById("addCondition").disabled = false;
	document.getElementById("addAnd").disabled = true;
	document.getElementById("addOr").disabled = true;
	condition_query=condition_query+" # "+"and";

	var operatorSpan = document.createElement('span');
	operatorSpan.innerHTML="and ";
	operatorSpan.style = "font-size: 16px;"

	var generalDiv=document.createElement('div');
	generalDiv.id = "Condition"+number_condition
	
	//var myDiv = document.getElementById("qc2");
	//myDiv.appendChild(generalDiv)
	another_div=document.getElementById("where_condition")
	another_div.appendChild(generalDiv)

	var div_div = document.getElementById("Condition"+number_condition);
	div_div.appendChild(operatorSpan)

	//console.log(condition_query)
}

function addOr(){
	document.getElementById("addCondition").disabled = false;
	document.getElementById("addAnd").disabled = true;
	document.getElementById("addOr").disabled = true;	
	condition_query=condition_query+" # "+"or";

	var operatorSpan = document.createElement('span');
	operatorSpan.innerHTML="or "
	operatorSpan.style = "font-size: 16px;"

	var generalDiv=document.createElement('div');
	generalDiv.id = "Condition"+number_condition
	
	//var myDiv = document.getElementById("qc2");
	//myDiv.appendChild(generalDiv)
	another_div=document.getElementById("where_condition")
	another_div.appendChild(generalDiv)

	var div_div = document.getElementById("Condition"+number_condition);
	div_div.appendChild(operatorSpan)

	//console.log(condition_query)
}

function backMakeQuery(){
	document.getElementById("formQuery3").style.display = "none"
	document.getElementById("formQuery2").style.display = "block"

}

function applicaFiltro(){

	document.getElementById("formQuery3").style.display = "block"
	document.getElementById("formQuery2").style.display = "none"

	//console.log(condition_query)
	//console.log(from_list)
	//console.log(element_list_select)
	var querySelectString="select distinct "
	var queryFromString="from "
	var queryWhereString="where "

	//const array1 = ['a', 'b', 'c'];

	for (const element of element_list_select) {
		//console.log(element);
		querySelectString=querySelectString+" "+element
		querySelectString=querySelectString+","
	}
	querySelectString=querySelectString.slice(0, -1)
	//console.log(querySelectString)


	for (const element of from_list) {
		//console.log(element);
		queryFromString=queryFromString+" log_db "+element
		queryFromString=queryFromString+","
	}
	queryFromString=queryFromString.slice(0, -1)
	//console.log(queryFromString)

	var cond_q_filt=condition_query;
	if(cond_q_filt.split("#").length == 2){
		cond_q_filt=cond_q_filt.replace(" or","")
		cond_q_filt=cond_q_filt.replace(" and","")
	}

	for (const element of cond_q_filt.split("#")) {
		//console.log(element);
		queryWhereString=queryWhereString+" "+element.trim()
	}
		
	//console.log(queryWhereString)
	cond_q_filt_where=queryWhereString.replace("where","")
	if(!cond_q_filt_where.replace(/\s/g, '').length){
		var totalQuery=querySelectString+" "+queryFromString
	}else{
		var totalQuery=querySelectString+" "+queryFromString+" "+queryWhereString
	}


	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", makeQueryListener);
	oReq.open("GET", frontend+"makeQuery?query="+totalQuery+"&selectpart="+querySelectString, false);
	oReq.send();

}


function makeQueryListener(){
	//console.log(this.responseText)
	var risposta=this.responseText.split("£");

	document.getElementById("result_div").innerHTML = ""
	//console.log("gfr")
	
	var array_query= risposta[0].split("\n")
	//console.log(array_query.length)

	//var headers = ["Title", "Author", "Read?"];
    var table = document.createElement("TABLE");  //makes a table element for the page

	table.setAttribute('class', 'styled-table');
        
	
    for(var i = 0; i < array_query.length; i++) {
        var row = table.insertRow(i);
		//console.log()
		elementi_row=array_query[i].split(",")
		
		for (var j = 0; j < elementi_row.length; j++) {
			
			//d = date_time.strftime("%m/%d/%Y, %H:%M:%S")
			//console.log(typeof elementi_row[j]);
			row.insertCell(j).innerHTML = elementi_row[j];
		}
    }

	
    var header = table.createTHead();
    var headerRow = header.insertRow(0);
	var hed = risposta[1].split(",")
    for(var i = 0; i < hed.length; i++) {
        headerRow.insertCell(i).innerHTML = hed[i];
    }
	
	

    document.getElementById("result_div").append(table);

}

function goback(){
	//console.log("work? or not")
	document.getElementById("formQuery").style.display = "block"
	document.getElementById("formQuery2").style.display = "none"
}


function doQuery1(){
	document.getElementById("formQuery3").style.display = "block"
	document.getElementById("formQuery2").style.display = "none"

	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", makeQueryListener);
	oReq.open("GET", frontend+"doQuery1", false);
	oReq.send();
}



function doQuery2(){
	document.getElementById("formQuery3").style.display = "block"
	document.getElementById("formQuery2").style.display = "none"

	var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", makeQueryListener);
	oReq.open("GET", frontend+"doQuery2", false);
	oReq.send();
}