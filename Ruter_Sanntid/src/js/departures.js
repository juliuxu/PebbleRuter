function putDepartures(stopid, ttype, successCb, failureCb) {
  console.log("putDepartures " + ttype);

  Ruter.GetRealTimeData(stopid, ttype,
    function(data) {
      // console.log("putDepartures: " + JSON.stringify(data, null, 4));

      data = data.filterByTransport(ttype);
      data = data.groupByLineDestination();

      var departuresdata = [];
      for (var key in data) {
        if (data.hasOwnProperty(key)) {
          departuresdata.push(
            [
              data[key].PublishedLineName,
              data[key].DestinationName,
              data[key].DepartureTimes.join(" ")
            ].join("~")
          );
          console.log(data[key].PublishedLineName + " " + data[key].DestinationDisplay);
        }
      }

      if (departuresdata.length === 0) {
        // Send empty message?
        MessageQueue.sendAppMessage({"PUT_DEPARTURE_EMPTY": ttype}, successCb, failureCb);
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

    },
    function(e) {
      console.log("An error occured getting departures");
      MessageQueue.sendAppMessage({"PUT_DEPARTURE_ERROR": ttype});
      failureCb(e);
    }
  });

}