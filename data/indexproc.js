// Die Javascript-Datei für die index.html

var HeuteTag, HeuteMonat, HeuteJahr;
var JetztMinute, JetztStunde;

function sendData(led) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "setLED?LEDstate="+led, true);
  xhttp.send();
}

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("BlinkLED").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readLED", true);
  xhttp.send();
}

setInterval(function() {
  // Call a function repetatively with 1 Second interval
  getData();
  getJSON();
}, 1000); //1000ms update rate



function getJSON() {
  var xhttp = new XMLHttpRequest();
  var vorname, nachname, foermlich, geschlecht;
  xhttp.onreadystatechange = function() {
	if (this.readyState == 4 && this.status == 200) {
		var ESPObj = JSON.parse(this.responseText);
		
		nachname = ESPObj.nachname;
		geschlecht = ESPObj.geschlecht;
		foermlich = ESPObj.foermlich;
		if (foermlich == 0)
			document.getElementById("name").innerHTML = ESPObj.vorname;
		else {
			if(geschlecht == 1)
				document.getElementById("anrede").innerHTML = "Frau ";
			else
				document.getElementById("anrede").innerHTML = "Herr ";
			document.getElementById("name").innerHTML = ESPObj.nachname;
		}
		document.getElementById("wochentag").innerHTML = ESPObj.wochentag;
		document.getElementById("tag").innerHTML = ESPObj.tag;
		document.getElementById("uhrzeit").innerHTML = ESPObj.uhrzeit;
		document.getElementById("compile").innerHTML = ESPObj.compile;
		document.getElementById("rentein").innerHTML = ESPObj.rentein;
		document.getElementById("temperatur").innerHTML = ESPObj.temperatur;
		document.getElementById("feuchte").innerHTML = ESPObj.feuchte;
		document.getElementById("druck").innerHTML = ESPObj.druck;
		document.getElementById("helligkeit").innerHTML = ESPObj.helligkeit;
		document.getElementById("vcc").innerHTML = ESPObj.vcc;
		console.log(nachname, foermlich, geschlecht);
	}
  };
  xhttp.open("GET", "readJSON", true);
  xhttp.send();
}
