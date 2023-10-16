function callSegmentator(){
    var oReq = new XMLHttpRequest();
	oReq.addEventListener("load", callSegmentatorListener);
	oReq.open("GET", frontend+"segmentator", false);
	oReq.send();
}


function callSegmentatorListener(){
    console.log(this.responseText)
}

function reloadLikeForm(){
	window.location.href = frontend+"indexSeg"
	sessionStorage.setItem('macroTask', 'YES');
}

function startSegmentator(){
	console.log("chiamata start segmentator")

	if(total_trace_number==1 && segmemtator_array.length > 0){
		//document.getElementById('updatedSeg').value = true;
		/*
		console.log(segmemtator_array)
		var oReq = new XMLHttpRequest();
		oReq.open("POST", frontend+"startSegmentator", true);
		//oReq.setRequestHeader("Content-Type", "application/json");
		oReq.setRequestHeader("Array-Data", jsonData);
		oReq.addEventListener("load",reloadLikeForm())
		oReq.send();*/

		var jsonData = JSON.stringify(segmemtator_array);
		$.ajax({
			url: "/startSegmentator",
			type: "POST",
			contentType: "application/json",
			xhrFields: {
				withCredentials: true
			},
			beforeSend: function (xhr) {
				xhr.setRequestHeader('Array-Data', jsonData);
			},
			success: function (data) {
				console.log("XD")
				reloadLikeForm()
			},
			error: function (result) {
				console.log("ko")
			}
		})
	
	}else{
		alert("It is not possible to apply segmentator")
		closechoseEndSeg()
	}

}

function openCheckSegmentatorPopup(){
	console.log(total_trace_number)
	if(total_trace_number==1){
		document.getElementById("blocker_checkSeg").style.display = "block"
		document.getElementById("checkSeg").style.display = "block"
	}
}

function closeCheckSeg(){
	document.getElementById("blocker_checkSeg").style.display = "none"
	document.getElementById("checkSeg").style.display = "none"
}

function openMacroTaskPopup(){
	document.getElementById("blocker_macroTask").style.display = "block"
	document.getElementById("macroTask").style.display = "block"
	
	$.ajax({
        url: "/getMacroTask",
        type: "GET",
        contentType: "application/json",
        xhrFields: {
            withCredentials: true
        },
        success: function (data) {
            console.log(data)
			const tableBody = document.getElementById('table-body');

			const column1 = document.createElement("th")
			const column2 = document.createElement("th")

			column1.style.overflowX = 'scroll';
			column1.style.maxWidth = '60vw';
			column1.style.fontWeight = 'normal';

			column2.style.overflowX = "scroll"
			column2.style.fontWeight = 'normal';

			for (const eventSequence in data) {
				if (data.hasOwnProperty(eventSequence)) {
					
					const count = data[eventSequence];
					/*
					const row = document.createElement('tr');
					
					const eventSequenceCell = document.createElement('td');
					eventSequenceCell.textContent = eventSequence;
					eventSequenceCell.style.overflowX = 'scroll';
					eventSequenceCell.style.maxWidth = '60vw';

					const countCell = document.createElement('td');
					countCell.textContent = count;

					row.appendChild(eventSequenceCell);
					row.appendChild(countCell);

					tableBody.appendChild(row);
					*/
					
					const row1 = document.createElement('tr');
					const eventSequenceCell = document.createElement('td');
					eventSequenceCell.textContent = eventSequence;
					eventSequenceCell.style.border = 'none';
					row1.style.fontWeight = 'normal';
					row1.style.border = 'none';
					row1.appendChild(eventSequenceCell)
					column1.appendChild(row1)


					const row2 = document.createElement('tr');
					const countCell = document.createElement('td');
					countCell.textContent = count;
					countCell.style.border = 'none';
					row2.style.border = 'none';
					row2.style.fontWeight = 'normal';
					row2.style.justifyContent = 'center';
					row2.style.display = 'flex';
					row2.style.alignItems = 'center';
					row2.appendChild(countCell)
					column2.appendChild(row2)
				}
			}
			tableBody.appendChild(column1);
			tableBody.appendChild(column2);
			
			



        },
        error: function (result) {
            console.log("ko")
        }
    })
}


function closeMacroTask(){
	document.getElementById("blocker_macroTask").style.display = "none"
	document.getElementById("macroTask").style.display = "none"
	sessionStorage.setItem('macroTask', 'NO');
}