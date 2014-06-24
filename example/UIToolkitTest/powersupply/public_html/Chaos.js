var maxarray=600;
var cus=[];
           
function CU(name){
    this.name =name;
    this.state=0; //on, stdby
    this.initialized=0;
    this.timestamp=0;
    this.oldtimestamp=0;

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

        console.log("device:" + this.name + " command:" + command + " param:" + parm);
        request.open("GET", "/cgi-bin/cu.cgi?dev=" + this.name + "&cmd=" + command + "&param=" + parm, true);
        request.send();
    }
    this.setCurrent=function setCurrent(val){
        var curr = Number(val).toFixed(3);
       
        this.sendCommand("sett", "{\"sett_cur\":" + curr + "}");
    }
    this.setPolarity=function setPolarity(val){
        if(this.state&0x2){
            alert("Powersupply:"+this.name+ " you should be in standby to change polarity");
            return;
        }
        this.sendCommand("pola", "{\"pola_value\":" + val + "}");
    }
    this.powerSupplyToggle=function powerSupplyToggle(val) {
        if(val=="On"){
            if(this.current>1.0 || this.current<-1.0){
                alert("Powersupply:"+this.name+" current should at 0 to change to standby");
                return;
            }
            this.sendCommand("mode", "{\"mode_type\": 0 }");
        } else {
            this.sendCommand("mode", "{\"mode_type\": 1 }");
        }
    }
    this.powerSupplyClrAlarms=function powerSupplyClrAlarms(){
        this.sendCommand("rset","");
    }
    
    this.update=function update(){
       var request = new XMLHttpRequest();
       var my=this;
        request.open("GET", "/cgi-bin/cu.cgi?dev=" + this.name, true);
        request.send();
        
        request.onreadystatechange = function() {
	if (request.readyState == 4) {
	    var json_answer = request.responseText;
	    
	    console.log("answer ("+my.name+"):" + json_answer);
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
		  //  console.log("processing:"+key+ " val:"+val);

		    if (key == "cs|csv|timestamp") {
			
			if(my.firsttimestamp==0){
                            my.firsttimestamp=val;
                        }
                         my.oldtimestamp=my.timestamp;
			my.timestamp = val;
		    } else if (key == "current") {
                            //var index = npoints%maxarray;
			    var curr = Number(json[key]).toFixed(3);
			   
                            my.points.push([(my.timestamp-my.firsttimestamp)/1000,curr]);
                            
                            npoints++;
                            if(npoints>maxarray){
                                my.points.pop();
                            }
			    my.current=curr;
                            system.log("current:"+my.current);
			} else if(key == "polarity"){
                            my.polarity=val;
			} else if(key == "status_id"){
                            my.state = val;
                     } else if(key=="current_sp"){
                         my.current_sp = val;
                     }
                } catch(err) {
		    // console.error(key + " does not exist:" + err);
		}
	    });
	}
    } 
} 
}

 function initializePowerSupply(cunames){
     for(var i=0;i<cunames.length;i++){
          cus.push(new CU(cunames[i]));
     }
 }
 function powerSupplyUpdateArrayInterface(){
                for(var i = 0;i<cus.length;i++){
                    cus[i].update();
                    console.log("updating \""+cus[i].name + " current:"+cus[i].current + " polarity:"+cus[i].polarity);
                    document.getElementById("readoutcurrent_"+i).innerHTML=cus[i].current;
                    document.getElementById("spcurrent_"+i).innerHTML=cus[i].current_sp;
                    document.getElementById("cuname_"+i).innerHTML=cus[i].name;
                    document.getElementById("timestamp_"+i).innerHTML=Number((cus[i].timestamp -cus[i].firsttimestamp))*1.0/1000;
                    if(cus[i].state&0x2){
                        document.getElementById("onstby_"+i).value="On";
                        document.getElementById("neg_"+i).disabled=true;
                        document.getElementById("pos_"+i).disabled=true;
                        document.getElementById("open_"+i).disabled=true;
                    }
                    if(cus[i].state&0x8){
                        document.getElementById("onstby_"+i).value="StandBy";
                        document.getElementById("neg_"+i).disabled=false;
                        document.getElementById("pos_"+i).disabled=false;
                        document.getElementById("open_"+i).disabled=false;
                    }
                    document.getElementById("open_"+i).className="";
                    document.getElementById("neg_"+i).className="";
                    document.getElementById("pos_"+i).className="";
                    if((cus[i].polarity>0)){
                        document.getElementById("pos_"+i).className="active";
                    }
                    if((cus[i].polarity<0)){
                       document.getElementById("neg_"+i).className="active";
                    }
                     if((cus[i].polarity==0)){
                       document.getElementById("open_"+i).className="active";
                    }
                    if((cus[i].timestamp!=cus[i].oldtimestamp)){
                        document.getElementById("cuname_"+i).style.color="green";
                    } else {
                        document.getElementById("cuname_"+i).style.color="red";
                    }
                }
            }
    