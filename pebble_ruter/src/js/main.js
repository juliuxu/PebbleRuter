Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");

var options = {
  enableHighAccuracy: false,
  timeout: 5000,
  maximumAge: 0
};

function success(pos) {
  var crd = pos.coords;

  console.log('Your current position is:');
  console.log('Latitude : ' + crd.latitude);
  console.log('Longitude: ' + crd.longitude);
  console.log('More or less ' + crd.accuracy + ' meters.');


    var latLng = new LatLng(crd.latitude, crd.longitude); 
    var utm = latLng.toUTMRef();
        
    var X = utm.easting.toString().split(".")[0];   
    var Y = utm.northing.toString().split(".")[0];

 	console.log('X: ' + X + ' Y: ' + Y);

};

function error(err) {
  console.warn('ERROR(' + err.code + '): ' + err.message);
};

navigator.geolocation.getCurrentPosition(success, error, options);

  }
);