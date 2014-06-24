var initialized = "";
var timestamp = 0;
var firsttimestamp=0;
var points = new Array();
var onswitched=0;
var maxarray=600;
var npoints=0;

function CU(name){
    this.name =name;
    this.state=0; //on, stdby
    this.initialized=0;
    this.timestamp=0;
    this.firsttimestamp=0;
    this.points = new Array();
    this.npoints=0;
    this.current=0;
    this.polarity=0;
    this.initCU=function initCU(){
        if(this.initialized==0){
        var request = new XMLHttpRequest();
        
	
        request.open("GET", "/cgi-bin/cu.cgi?InitId=" + this.name, true);
        request.send();
        
	this.initialized=1;
        
        }
    }
    
    this.sendCommand=function sendCommand(command, parm) {
        var request = new XMLHttpRequest();

        console.log("device:" + this.name + "command:" + command + " param:" + parm);
        request.open("GET", "/cgi-bin/cu.cgi?dev=" + initialized + "&cmd=" + command + "&param=" + parm, true);
        request.send();
    }
    this.setCurrent=function setCurrent(val){
        var curr = Number(val).toFixed(3);
        this.sendCommand("sett", "{\"sett_cur\":" + curr + "}");
    }
    this.setPolarity=function setPolarity(val){
        this.sendCommand("pola", "{\"pola_value\":" + val + "}");
    }
    this.powerSupplyToggle=function powerSupplyToggle(val) {
    
        this.sendCommand("mode", "{\"mode_type\":" + val + "}");
    }
    this.powerSupplyClrAlarms=function powerSupplyClrAlarms(){
        this.sendCommand("rset","");
    }
    
    this.update=function update(){
       var request = new XMLHttpRequest();
    
        request.open("GET", "/cgi-bin/cu.cgi?dev=" + this.name, true);
        request.send();
    
        request.onreadystatechange = function() {
	if (request.readyState == 4) {
	    var json_answer = request.responseText;
	    
	    console.log("answer ("+this.name+"):" + json_answer);
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
			
			if(this.firsttimestamp==0){
                            this.firsttimestamp=val;
                        }
			this.timestamp = val;
		    } else {
			if (key == "current") {
                            //var index = npoints%maxarray;
			    var curr = Number(json[key]).toFixed(3);
			   
                            this.points.push([(timestamp-firsttimestamp)/1000,curr]);
                            
                            this.npoints++;
                            if(this.npoints>maxarray){
                                this.points.pop();
                            }
			    this.current=curr;
			} else if(key == "polarity"){
                            this.polarity=val;
			} else if(key == "status_id"){
                            this.state = val;
                        }
			 
			
		    }
		} catch(err) {
		    // console.error(key + " does not exist");
		}
	    });
	}
    } 
} 
    }
    

	$(function() {
	var cnt;
		
        points=[];
		var plot = $.plot("#powersupply-graph",  [ ], {
			series: {
				shadowSize: 0	// Drawing is faster without shadows
			},
			yaxis: {
                            
				min: 0,
				max: 100
			},
			xaxis: {
				show: false
			}
		});
                plot.draw();

		
	});
        
function initCU() {
    var cuname = document.forms["init"]["InitID"].value;
    
    if (cuname != "") {
	var request = new XMLHttpRequest();
        
	console.log("answer:" + cuname);
        request.open("GET", "../cgi-bin/cu.cgi?InitId=" + cuname, true);
        request.send();
        
	initialized = cuname;
	
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
function powerSupplyClrAlarms(){
    sendCommand("rset","");
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
			if(firsttimestamp==0){
                            firsttimestamp=val;
                        }
			timestamp = val;
		    } else {
			if (key == "current") {
                            //var index = npoints%maxarray;
			    var curr = Number(json[key]).toFixed(3);
			   
                           points.push([(timestamp-firsttimestamp)/1000,curr]);
                            
                            npoints++;
                            if(npoints>maxarray){
                                points.pop();
                            }
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
    /*
    currentGraph.off();
    console.log("loading graph: " + points);
    currentGraph.channel[0].setPoints(points);
    currentGraph.on();
    */
   console.log("plotting: "+ points);
   
   //$.plot("#placeholder",[points]);
   $.plot("#powersupply-graph",[points]);
}

function onload() {
    if (initialized != "") {
	queryInterface();
    }
}