Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");

    Ruter.GetRealTimeDataGroupedByDirection(3010312, null, function(err, data) {
      console.log("Got these depatures: " +  JSON.stringify(data, null, 4));
    });

  }
);