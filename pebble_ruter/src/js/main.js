Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");

    Ruter.SimpleGetOrderedDepatures(3010312, Ruter.REALTIME_TRANSPORT_TYPES.tram, function(err, data) {
      //console.log("got closest stops: " + JSON.stringify(data[0].ID, null, 4));

      console.log(JSON.stringify(data, null, 4));

    });

  }
);