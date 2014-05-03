Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");

  }
);

/**
 * Boolean to make sure we are only processing one command at the time
 */

var proccessingCommand = false;

Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + JSON.stringify(e.payload, null, 4));

    if (proccessingCommand) {
      console.log("proccessingCommand is true! cannot do two things at the same time");
      return;
    }

    var successCb = function(res) {
      console.log("Function returned successfully!");
      proccessingCommand = false;
    };
    var failureCb = function(res) {
      console.log("Function returned failed!");
      proccessingCommand = false;
    };


    if(e.payload.hasOwnProperty("GET_STOPS")) {
      console.log("GET_STOPS");
      putStops(e.payload.GET_STOPS, successCb, failureCb);
      proccessingCommand = true;
    }
    if(e.payload.hasOwnProperty("GET_DEPARTURES")) {
      console.log("GET_DEPARTURES");
      putDepartures(e.payload.GET_DEPARTURES_STOPID, e.payload.GET_DEPARTURES, successCb, failureCb);
      proccessingCommand = true;
    }


  }
);