var initialized = "";
var timestamp = 0;
var points = new Array();
var onswitched=0;
var maxarray=1000;
var npoints=0;
function initCU() {
    var cuname = document.forms["init"]["InitID"].value;
    
    if (cuname != "") {
	var request = new XMLHttpRequest();
        
	console.log("answer:" + cuname);
        request.open("GET", "../cgi-bin/cu.cgi?InitId=" + cuname, true);
        request.send();
        
	initialized = cuname;
	//	var cnt;
	//	for(cnt=0;cnt<maxarray;cnt++){
	//	    points[cnt] = 0.0;
	//	}
    } else {
        alert("You must specify a valid ID");
    }
    return false;
}

function sendCommand(command, parm) {
    var request = new XMLHttpRequest();
    
    console.log("device:" + initialized + "command:" + command + " param:" + parm);
    request.open("GET", "../cgi-bin/cu.cgi?dev=" + initialized + "&cmd=" + command + "&param=" + parm, true);
    request.send();
}

function powerSupplyToggle(val) {
    sendCommand("mode", "{\"mode_type\":" + val + "}");
}
function powerSupplySet(val) {
    var curr = Number(val).toFixed(3);
    sendCommand("sett", "{\"sett_cur\":" + curr + "}");
}
function powerSupplyPol(val) {
    sendCommand("pola", "{\"pola_value\":" + val + "}");
}

function queryInterface() {
    var request = new XMLHttpRequest();
    
    request.open("GET", "../cgi-bin/cu.cgi?dev=" + initialized, true);
    request.send();
    
    request.onreadystatechange = function() {
	if (request.readyState == 4) {
	    var json_answer = request.responseText;
	    
	    console.log("answer:" + json_answer);
	    if (json_answer == "") {
		return;
	    }
	    try {
	    var json = JSON.parse(json_answer);
	    } catch (err){
		console.log("exception parsing " + json_answer);
	    }
	    Object.keys(json).forEach(function(key) {
		try {
		    var val = json[key];
		    
		    if (typeof(val) === 'object') {
			if (val.hasOwnProperty('$numberLong')) {
			    val = val['$numberLong'];
			}
		    }
		    
		    if (key == 'cs|csv|timestamp') {
			if (timestamp != val) {
			    document.getElementById('timestamp').innerHTML = val;
			    Drinks.getElementById('connected').toggle();
			}
			
			timestamp = val;
		    } else {
			if (key == "current") {
			    var curr = Number(json[key]).toFixed(3);
			    points[npoints++%maxarray]=curr;
			    Drinks.getElementById("current").value=curr;
			    loadGraph();
			} else if(key == "polarity"){
			    Drinks.getElementById("polPos").value=0;
			    Drinks.getElementById("polNeg").value=0;
			    Drinks.getElementById("polOpen").value=0;
			    if(val == 1){
				Drinks.getElementById("polPos").value=1;
			    } else if(val == -1){
				Drinks.getElementById("polNeg").value=1;
			    } else if(val == 0){
				Drinks.getElementById("polOpen").value=1;
			    } 
			} else if(key == "on"){

			    if(onswitched!=val){
				Drinks.getElementById("ON").value=val;
			    }
			    onswitched= val;
			    Drinks.getElementById("on").value=val;
			} else {
			    Drinks.getElementById(key).value = val;
			}
			
			/*
			var arrClass = Drinks.getElementsByClassName(key);
			for (var i in arrClass ) {
			    var elem = Drinks.displays[i];
			    elem.value = val;
			    found = 1;
			}
			*/
			
			
		    }
		} catch(err) {
		    // console.error(key + " does not exist");
		}
	    });
	}
    } 
}

function loadGraph() {
    currentGraph.off();
    console.log("loading graph: " + points);
    currentGraph.channel[0].setPoints(points);
    currentGraph.on();
}

function onload() {
    if (initialized != "") {
	queryInterface();
    }
}