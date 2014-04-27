function putStops(ttype) {
	console.log("putStops " + ttype);

	// Convert the realtime type to travel type
	var travel_type = Ruter.REALTIME_TO_TRAVEL[ttype];
	Ruter.GetClosestStopsByTransportType(travel_type, function(err, data) {

		if (err === null) { 
			//console.log("putStops: " + JSON.stringify(data, null, 4));

			stopsdata = [];
			for (var stop in data) {
				console.log(data[stop].Name + data[stop].ID);
				stopsdata.push(data[stop].ID);
				stopsdata.push(data[stop].Name);

			}
			stopsdata.unshift(data.length);
			// 3242424~Bislett~432424~Dalsberg~2334324~Majorstuen
			Pebble.sendAppMessage({"PUT_STOPS": stopsdata.join("|")});

		}
		else {
			console.log("An error occured getting stops");
			console.log(err);
		}
	});

}