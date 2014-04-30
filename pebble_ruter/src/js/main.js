Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");

  }
);

/**
 * Boolean to make sure we are only processing one command at the time
 */
var sendingInProgress = false;

Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + JSON.stringify(e.payload, null, 4));

    if (sendingInProgress) {
      console.log("sendingInProgress is true, cannot process two commands at the same time");
      return;
    }
    else {
      sendingInProgress = true;
    }

    if(e.payload.hasOwnProperty("GET_STOPS")) {
      console.log("GET_STOPS");
      putStops(e.payload["GET_STOPS"]);
    }
    if(e.payload.hasOwnProperty("GET_DEPARTURES")) {
      console.log("GET_DEPARTURES");
      putDepartures(e.payload["GET_DEPARTURES_STOPID"], e.payload["GET_DEPARTURES"]);
    }

    sendingInProgress = false;

  }
);