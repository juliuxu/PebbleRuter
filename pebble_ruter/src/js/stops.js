function putStops(ttype) {
	console.log("putStops " + ttype);

	// Convert the realtime type to travel type
	var travel_type = Ruter.REALTIME_TO_TRAVEL[ttype];
	Ruter.GetClosestStopsByTransportType(travel_type, function(err, data) {

		if (err == null) { 
			//console.log("putStops: " + JSON.stringify(data, null, 4));
			for (var stop in data) {
				console.log(data[stop].Name + data[stop].ID);
			}
			if (data.length == 0) {
				// No elements returned
			}
			else if {
				Pebble.sendAppMessage({"PUT_STOPS"});
			}

		}
		else {
			console.log("An error occured getting stops");
			console.log(err);
		}
	});

}