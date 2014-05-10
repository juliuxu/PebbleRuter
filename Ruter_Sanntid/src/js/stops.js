function putStops(ttype, successCb, failureCb) {
  console.log("putStops " + ttype);

  // Convert the realtime type to travel type
  var travel_type = Ruter.REALTIME_TO_TRAVEL[ttype];

  // Get current location
  Ruter.GetCurrentUTM32Location(
    function(X, Y) {

      // Notify that we got the location
      MessageQueue.sendAppMessage({"PUT_STOPS_LOCATION_SUCCESS": ttype});

      // Get the closest stops
      Ruter.GetClosestStopsAdvancedByCoordinates(X, Y, 20, 1200, 
        function(data) {
          //console.log("putStops: " + JSON.stringify(data, null, 4));

          // Filter by transport
          data = data.filterByTransport(travel_type);

          var stopsdata = [];
          for (var i=0;i<data.length;i++) {
            console.log(data[i].ID + " " + data[i].Name);
            stopsdata.push(data[i].ID);
            stopsdata.push(data[i].Name);

          }

          if (stopsdata.length === 0) {
            // Notify that we got no stops
            MessageQueue.sendAppMessage({"PUT_STOPS_EMPTY": ttype}, successCb, failureCb);
          }
          else {
            // Append length of data
            stopsdata.unshift(data.length);

            // 3~3242424~Bislett~432424~Dalsberg~2334324~Majorstuen
            MessageQueue.sendAppMessage({"PUT_STOPS": stopsdata.join("~")}, successCb, failureCb);
          }

      },

      function(e) {
        console.log("An error occured getting stops");

        // Notify that an error occurred
        MessageQueue.sendAppMessage({"PUT_STOPS_ERROR": ttype});
        failureCb(e);
      });
    },

    function(e) {
      console.log("An error occured getting location");

      // Notify that we didn't get the current location
      MessageQueue.sendAppMessage({"PUT_STOPS_LOCATION_ERROR": ttype});
      failureCb(e);
    }
  );

}