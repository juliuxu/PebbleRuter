function putDepartures(stopid, ttype) {
	console.log("putDepartures " + ttype);

	Ruter.SimpleGetOrderedDepartures(stopid, ttype, function(err, data) {

		if (err === null) { 
			console.log("putDepartures: " + JSON.stringify(data, null, 4));

			departuresdata = [0];
			for (var dir in data) {
				for (var departure in data[dir]) {
					console.log(data[dir][departure].PublishedLineName + data[dir][departure].DestinationDisplay);
					departuresdata.push(data[dir][departure].PublishedLineName);
					departuresdata.push(data[dir][departure].DestinationDisplay);
					departuresdata.push(data[dir][departure].DepartureTimes.join(" "));
				}
				departuresdata[0] += data[dir].length;

			}
			message = departuresdata.join("~");
			console.log("Send message("+message.length+"): " + message);
			Pebble.sendAppMessage({"PUT_DEPARTURES": message});

		}
		else {
			console.log("An error occured getting departures");
			console.log(err);
		}
	});

}