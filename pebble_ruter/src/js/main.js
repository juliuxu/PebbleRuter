Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");

  }
);

Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + JSON.stringify(e.payload, null, 4));

    if(e.payload.GET_STOPS) {
      console.log("GET_STOPS");
      putStops(e.payload["GET_STOPS"]);
    }
    if(e.payload.GET_DEPATURES) {
      console.log("GET_DEPATURES");
      //putDepatures(e.payload["GET_DEPATURES"]);
    }

  }
);