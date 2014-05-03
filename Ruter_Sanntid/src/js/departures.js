function putDepartures(stopid, ttype, successCb, failureCb) {
  console.log("putDepartures " + ttype);

  Ruter.SimpleGetOrderedDepartures(stopid, ttype, function(err, data) {

    if (err === null) { 
      // console.log("putDepartures: " + JSON.stringify(data, null, 4));

      var departuresdata = [];
      for (var dir in data) {
        for (var departure in data[dir]) {
          console.log(data[dir][departure].PublishedLineName + " " + data[dir][departure].DestinationDisplay);

          departuresdata.push([
            data[dir][departure].PublishedLineName,
            data[dir][departure].DestinationDisplay,
            data[dir][departure].DepartureTimes.join(" ")
            ].join("~")
          );
        }

      }

      if (departuresdata.length === 0) {
        // Send empty message?
        MessageQueue.sendAppMessage({"PUT_DEPARTURE_EMPTY": ttype});
        successCb(null);
        return;
      }

      /**
       * Send an array to pebble
       */
      var sendArray = function(index) {

        if (index == departuresdata.length) {
          console.log("Reached end of array");
          successCb(null);
          return;
        }

        console.log("Sending Message(" + index + "): " + departuresdata[index]);

        var messageDict = {};
        messageDict.PUT_DEPARTURE = departuresdata[index];
        messageDict.PUT_DEPARTURE_INDEX = index;
        messageDict.PUT_DEPARTURE_LENGTH = departuresdata.length;

        // Add the time on the last departure we send
        if (index == departuresdata.length-1) {
          var hourminuteseconds;
          var date = new Date();
              hourminuteseconds = (date.getHours()<10?'0':'') + date.getHours();
              hourminuteseconds += ":";
              hourminuteseconds += (date.getMinutes()<10?'0':'') + date.getMinutes();
              hourminuteseconds += ":";
              hourminuteseconds += (date.getSeconds()<10?'0':'') + date.getSeconds();

          messageDict.PUT_DEPARTURE_TIME = hourminuteseconds;
          console.log("Time Now: " + hourminuteseconds);

        }

        //console.log(JSON.stringify(messageDict));
        MessageQueue.sendAppMessage(messageDict,
          function(e) {
            console.log("Send next message: " + (index + 1));
            sendArray(index+1);
          },
          function(e) {
            console.log("An error occured trying to send message!");
            failureCb(e);
          });

      };
      sendArray(0);

    }
    else {
      console.log("An error occured getting departures");
      console.log(err);
      MessageQueue.sendAppMessage({"PUT_DEPARTURE_ERROR": ttype});
      failureCb(err);
    }
  });

}