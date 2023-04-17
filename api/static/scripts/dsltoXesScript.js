function getNameDsl(){
    
    document.getElementById("blocker_dsl_name").style.display = "block";
    document.getElementById("popupDslName").style.display = "block";

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


    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", getDslbyNameListener);
	oReq.open("GET", frontend+"getDslStructure?dslName="+nomeDsl, false);
	oReq.send();

}



function getDslbyNameListener(){
    console.log(this.responseText)
}