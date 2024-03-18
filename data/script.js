function updateActivations() {
    var activationsInput = document.getElementById("activationsInput").value;
    // Send the value to the ESP8266 server (you'll need to implement this)
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            alert("Activations per day updated: " + activationsInput);
        }
        else{
            alert("Unable to update element");
        }
    };
    xhttp.open("GET", "setActivations?Activations=" +activationsInput, true);
    xhttp.send();
    
}

function updateActiveTime() {
    //var hoursInput = document.getElementById("hoursInput").value;
    var minutesInput = document.getElementById("minutesInput").value;
    // Send the values to the ESP8266 server (you'll need to implement this)
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            alert("Active time per activation updated: " + minutesInput + " minutes");
            //alert("Active time per activation updated: " + hoursInput + " hours " + minutesInput + " minutes");
        }
        else{
            alert("Unable to update element");
        }
    };
    //xhttp.open("GET", "setActiveTime?ActiveTime=" +[hoursInput,minutesInput], true);
    xhttp.open("GET", "setActiveTime?ActiveTime=" +minutesInput, true);
    xhttp.send();
    //Prompt alert
    
}

function manualTrigger(led) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("ValveState").innerHTML =
                this.responseText;
        }
    };
    xhttp.open("GET", "setValve?ValveState=" + led, true);
    xhttp.send();
    alert("Manual trigger activated");
}
setInterval(function () {
    // Call a function repetatively with 2 Second interval
    getTemperature();
    getValve();
}, 2000); //2000mSeconds update rate

function getTemperature() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("Temperature").innerHTML =
                this.responseText;
        }
    };
    xhttp.open("GET", "temperature", true);
    xhttp.send();
}

function getValve() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("ValveState").innerHTML =
                this.responseText;
        }
    };
    xhttp.open("GET", "getValve", true);
    xhttp.send();
}