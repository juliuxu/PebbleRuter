function putStops(ttype, successCb, failureCb) {
	console.log("putStops " + ttype);

	// Convert the realtime type to travel type
	var travel_type = Ruter.REALTIME_TO_TRAVEL[ttype];

	// Get current location
	Ruter.GetUTM32Location(function(err, X, Y) {
		if (err === null) {

			// Get the closest stops
			Ruter.GetClosestStopsByTransportType(X, Y, travel_type, function(err2, data) {

				if (err2 === null) { 
					//console.log("putStops: " + JSON.stringify(data, null, 4));

					var stopsdata = [];
					for (var stop in data) {
						console.log(data[stop].ID + " " + data[stop].Name);
						stopsdata.push(data[stop].ID);
						stopsdata.push(data[stop].Name);

					}
					stopsdata.unshift(data.length);

					if (data.length == 0) {
						// Send empty message?
					}

					// 3~3242424~Bislett~432424~Dalsberg~2334324~Majorstuen
					Pebble.sendAppMessage({"PUT_STOPS": stopsdata.join("~")},
						function(e){
							successCb(e);
						},
						function(e){
							failureCb(e);
						}
					);

				}
				else {
					console.log("An error occured getting stops");
					console.log(err2);
					failureCb(err2)
				}
			});


		}
		else {
			console.log("Ann error occured getting location");
			failureCb(err);
		}

	});

}