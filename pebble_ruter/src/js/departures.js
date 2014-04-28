function putDepartures(stopid, ttype) {
	console.log("putDepartures " + ttype);

	Ruter.SimpleGetOrderedDepartures(stopid, ttype, function(err, data) {

		if (err === null) { 
			console.log("putDepartures: " + JSON.stringify(data, null, 4));

			departuresdata = [];
			for (var dir in data) {
				for (var departure in data[dir]) {
					console.log(data[dir][departure].PublishedLineName + data[dir][departure].DestinationDisplay);
					
					departuresdata.push([
						data[dir][departure].PublishedLineName,
						data[dir][departure].DestinationDisplay,
						data[dir][departure].DepartureTimes.join(" ")
						].join("~")
					);
				}

			}

			/**
			 * Send an array to pebble
			 */
			function sendArray(index) {

				if (index == departuresdata.length) {
					return;
				}

				//console.log("Sending Message(" + index + "): " + departuresdata[index]);

				messageDict = {}
				messageDict["PUT_DEPARTURE"] = departuresdata[index];
				messageDict["PUT_DEPARTURE_INDEX"] = index;
				messageDict["PUT_DEPARTURE_LENGTH"] = departuresdata.length;

				//console.log(JSON.stringify(messageDict));
				Pebble.sendAppMessage(messageDict,
					function(e) {
						sendArray(index+1, departuresdata, appkey, appkey_index, appkey_length);
					},
					function(e) {
						console.log("An error occured trying to send message!");
						// Error occured, try again?
					});

			}
			sendArray(0);

		}
		else {
			console.log("An error occured getting departures");
			console.log(err);
		}
	});

}