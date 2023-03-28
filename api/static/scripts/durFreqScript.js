const levenshteinDistance = (str1 = '', str2 = '') => {
    const track = Array(str2.length + 1).fill(null).map(() =>
    Array(str1.length + 1).fill(null));
    for (let i = 0; i <= str1.length; i += 1) {
        track[0][i] = i;
    }
    for (let j = 0; j <= str2.length; j += 1) {
        track[j][0] = j;
    }
    for (let j = 1; j <= str2.length; j += 1) {
        for (let i = 1; i <= str1.length; i += 1) {
            const indicator = str1[i - 1] === str2[j - 1] ? 0 : 1;
            track[j][i] = Math.min(
                track[j][i - 1] + 1, // deletion
                track[j - 1][i] + 1, // insertion
                track[j - 1][i - 1] + indicator, // substitution
            );
        }
    }
    return track[str2.length][str1.length];
};

// Function to take min/max/mean/median/total duration of nodes

function getAllDuration(variable){

	var duration_dictionary=document.getElementById("allDt").innerHTML
	// console.log(document.getElementById("allDt").innerHTML);

	duration_dictionary=duration_dictionary.split("*")

	var mean_duration=duration_dictionary[0];
	var total_duration=duration_dictionary[1];
	var median_duration=duration_dictionary[2];
	var max_duration=duration_dictionary[3];
	var min_duration=duration_dictionary[4];
	

	// var mDFP=document.getElementById("allDt").innerHTML;
	var mDFP=""
	if(variable=="mean"){
		mDFP=JSON.parse(mean_duration.replace(/'/g,"\""));
	}else if(variable=="total"){
		mDFP=JSON.parse(total_duration.replace(/'/g,"\""));
	}else if(variable=="median"){
		mDFP=JSON.parse(median_duration.replace(/'/g,"\""));
	}else if(variable=="max"){
		mDFP=JSON.parse(max_duration.replace(/'/g,"\""));
	}else if(variable=="min"){
		mDFP=JSON.parse(min_duration.replace(/'/g,"\""));
	}

	return mDFP
	
}


function getAllFrequency(variable){

	var duration_dictionary=document.getElementById("allFq").innerHTML
	// console.log(document.getElementById("allDt").innerHTML);

	duration_dictionary=duration_dictionary.split("*")

	var absolute_freq=duration_dictionary[0];
	var case_freq=duration_dictionary[1];
	var max_freq=duration_dictionary[2];



	// var mDFP=document.getElementById("allDt").innerHTML;
	var mDFP=""
	if(variable=="absolute"){
		mDFP=JSON.parse(absolute_freq.replace(/'/g,"\""));
	}else if(variable=="case"){
		mDFP=JSON.parse(case_freq.replace(/'/g,"\""));
	}else if(variable=="max"){
		mDFP=JSON.parse(max_freq.replace(/'/g,"\""));
	}

	return mDFP

}


function getAllEdgeDuration(variable){

	var duration_dictionary=document.getElementById("allEdgeDt").innerHTML
	// console.log(document.getElementById("allDt").innerHTML);

	duration_dictionary=duration_dictionary.split("*")

	var mean_duration=duration_dictionary[0];
	var total_duration=duration_dictionary[1];
	var median_duration=duration_dictionary[2];
	var max_duration=duration_dictionary[3];
	var min_duration=duration_dictionary[4];


	// var mDFP=document.getElementById("allDt").innerHTML;
	var mDFP=""
	if(variable=="mean"){
		mDFP=JSON.parse(mean_duration.replace(/'/g,"\""));
	}else if(variable=="total"){
		mDFP=JSON.parse(total_duration.replace(/'/g,"\""));
	}else if(variable=="median"){
		mDFP=JSON.parse(median_duration.replace(/'/g,"\""));
	}else if(variable=="max"){
		mDFP=JSON.parse(max_duration.replace(/'/g,"\""));
	}else if(variable=="min"){
		mDFP=JSON.parse(min_duration.replace(/'/g,"\""));
	}

	return mDFP

}

function getAllEdgeFrequency(variable){

	var duration_dictionary=document.getElementById("allEdgeFq").innerHTML
	// console.log(document.getElementById("allDt").innerHTML);

	duration_dictionary=duration_dictionary.split("*")

	var absolute_freq=duration_dictionary[0];
	var case_freq=duration_dictionary[1];
	var max_freq=duration_dictionary[2];



	// var mDFP=document.getElementById("allDt").innerHTML;
	var mDFP=""
	if(variable=="absolute"){
		mDFP=JSON.parse(absolute_freq.replace(/'/g,"\""));
	}else if(variable=="case"){
		mDFP=JSON.parse(case_freq.replace(/'/g,"\""));
	}else if(variable=="max"){
		mDFP=JSON.parse(max_freq.replace(/'/g,"\""));
	}

	return mDFP

}


function getAllUsedVariables(variable){
	var uv_dictionary=document.getElementById("usedVariable").innerHTML
	
	uv_dictionary = uv_dictionary.replace("'", '"');
	uv_dictionary=uv_dictionary.split("*")

	// activities=uv_dictionary[0]
	// resources=uv_dictionary[1]
	// resources_cost=uv_dictionary[2]
	// caseid=uv_dictionary[3]
	// variant=uv_dictionary[4]

	
	var activities = Object.keys(JSON.parse(uv_dictionary[0]));
	var resources = Object.keys(JSON.parse(uv_dictionary[1]));
	// var resources_cost = Object.keys(JSON.parse(uv_dictionary[2]));
	// console.log(uv_dictionary[2])
	// var rs_cost=JSON.stringify(uv_dictionary[2]);
	// console.log(rs_cost)
	var resources_cost=Object.keys(JSON.parse(uv_dictionary[2]));
	// console.log(resources_cost)

	var caseid = Object.keys(JSON.parse(uv_dictionary[3]));
	var variant = Object.keys(JSON.parse(uv_dictionary[4]));
	document.getElementById("variants_original").innerHTML=String(variant.length)
	
	if(variant.length==0){
		console.log("inside")
		variant=document.getElementById("variants_alternative").innerHTML.replace("[","").replace("]","").split(",")
	}

	document.getElementById("variants_original").innerHTML=String(0)
	variant=Object.keys(dictionaryVariantNameReverse)
	//console.log(variant)

	// console.log(obj[0]);
	// var mean_duration=duration_dictionary[0];
	// var total_duration=duration_dictionary[1];
	// var median_duration=duration_dictionary[2];
	// var max_duration=duration_dictionary[3];
	// var min_duration=duration_dictionary[4];


	// var mDFP=document.getElementById("allDt").innerHTML;
	// var mDFP=""
	if(variable=="activities"){
		result=activities
	}else if(variable=="resources"){
		result=resources
	}else if(variable=="resources_cost"){
		result=resources_cost
	}else if(variable=="caseid"){
		result=caseid
	}else if(variable=="variant"){
		result=variant
	}

	return result
}