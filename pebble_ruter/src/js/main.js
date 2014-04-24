Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");

    Ruter.GetRealTimeDataGrouped(3010312, function(err, data) {
      for(var index in data) {
        console.log( index + " : " + data[index]);
      }
    });

  }
);