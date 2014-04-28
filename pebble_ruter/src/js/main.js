Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");

  }
);

Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + JSON.stringify(e.payload, null, 4));

    if(e.payload.hasOwnProperty("GET_STOPS")) {
      console.log("GET_STOPS");
      putStops(e.payload["GET_STOPS"]);
    }
    if(e.payload.hasOwnProperty("GET_DEPARTURES")) {
      console.log("GET_DEPARTURES");
      putDepartures(e.payload["GET_DEPARTURES_STOPID"], e.payload["GET_DEPARTURES"]);
    }

  }
);