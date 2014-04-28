/**
 * Ruter Javascript library
 *
 * Depends on jscoord for lat/lon to UTM conversion: http://www.jstott.me.uk/jscoord/
 *
 * Author: Julian Jark <julianj@ifi.uio.no>
 */

var Ruter = (function() {
    var my = {};
    
    var base_url = 'http://api.ruter.no/ReisRest/';
    
    var location_options = {
      enableHighAccuracy: false,
      timeout: 5000,
      maximumAge: 0
    };

    my.REALTIME_TRANSPORT_TYPES = {
        'bus': 0,
        'ferry': 1,
        'rail': 2, 'train': 2,
        'tram': 3,
        'metro': 4,
    };

    my.TRAVEL_TRANSPORT_TYPES = {
        'walking': 0,
        'airportbus': 1,
        'bus': 2,
        'dummy value': 3,
        'airporttrain': 4,
        'boat': 5,
        'train': 6,
        'tram': 7,
        'metro': 8,
    };

    my.REALTIME_TO_TRAVEL = {
      0: 2,
      1: 5,
      2: 6,
      3: 7,
      4: 8,
    };

    /**
     * Get json data from an url + parameters
     */
    function get(url, query, callback) {

      console.log("Get url: " + url + " + query: " + JSON.stringify(query, null, 4));

      function serialize(obj) {
        var str = [];
        for(var p in obj) {
          if (obj.hasOwnProperty(p)) {
            str.push(encodeURIComponent(p) + '=' + encodeURIComponent(obj[p]));
          }
        }
        return str.join('&');
      }

      var req = new XMLHttpRequest();
      url += '?' + serialize(query);
      req.open('GET', url, true);
      req.setRequestHeader('Connection', 'close');
      req.onload = function () {
        if (req.readyState === 4 && req.status === 200) {
          var response = JSON.parse(req.responseText);
          return callback(null, response);
        }
        else {
          return callback(new Error(req.status));
        }
      };
      req.onerror = function () {
        switch (req.status) {
        case 0:
          return callback(new Error('NOT_CONNECTED'));
        case 404:
          return callback(new Error('NOT_FOUND'));
        }
      };
      req.send();
    }

    /**
     * Convert a timestamp to ruter time estimate
     */
    my.GetRuterTime = function(timestamp) {
      remaining = timestamp - Date.now();
      remaining = remaining / 1000;

      if (remaining < 0) {
        return "";
      }
      else if (remaining < 44) {
        return "now";
          //return "nå";
      }
      else if (remaining > 584) {
        var date = new Date(timestamp);
        var hourminute;
        hourminute = (date.getHours()<10?'0':'') + date.getHours();
        hourminute += ":";
        hourminute += (date.getMinutes()<10?'0':'') + date.getMinutes();

        return hourminute;
      }
      else {
        var start = 45;
        for (var i=1;i<10;i++) {
          if (remaining < (start + (i * 60)) ) {
            return i + " min";
          }
        }
      }
    };

    /**
     * Get realtime data for stopid
     */
    my.GetRealTimeData = function (stopid, callback) {
      console.log("GetRealTimeData " + stopid);
      get(base_url + 'RealTime/GetRealTimeData/' + stopid, {}, callback)
    };

    /**
     * Get a list of stops by UTM32 coordinates
     */
    my.GetClosestStopsByCoordinates = function (X, Y, callback) {
      var parameters = {
        'coordinates': '(X='+X+',Y='+Y+')',
        'proposals': '7',

      };
      get(base_url + 'Stop/GetClosestStopsByCoordinates/', parameters, callback);
    };

    /**
     * Get current location, convert to UTM32 and get the closest stops
     */
    my.GetClosestStops = function(callback) {

      // Get location
      function success(position) {

        // Convert to UTM32 using jscoord
        var latLng = new LatLng(position.coords.latitude, position.coords.longitude);
        var utm = latLng.toUTMRef();
        var X = utm.easting.toString().split(".")[0];
        var Y = utm.northing.toString().split(".")[0];

        console.log('UTM32 X: ' + X + ' Y: ' + Y);

        my.GetClosestStopsByCoordinates(X, Y, callback);

      };

      function error(err) {
        console.warn('ERROR(' + err.code + '): ' + err.message);
        callback('ERROR(' + err.code + '): ' + err.message);
      };

      console.log('Get Location');
      navigator.geolocation.getCurrentPosition(success, error, location_options);

    };

    /**
     * Get Closest stops by transportation type
     * Use types from TRAVEL_TRANSPORT_TYPES
     */
    my.GetClosestStopsByTransportType = function(ttype, callback) {

      my.GetClosestStops(function(err, data) {

        var stops = [];
        for (var stop in data) {
          //console.log("stop: " + stop + " Name: " + data[stop].Name);

          // Go trough all lines to determine if this stop has any lines with ttype transport type
          for (var line in data[stop].Lines) {
            if (data[stop].Lines[line].Transportation == ttype) {
              stops.push(data[stop]);
              break;
            }
          }

        }

        callback(err, stops);

      });

    };

    /**
    * Get Realtime Data from by transport type
    * Use REALTIME_TRANSPORT_TYPES
    */
    my.GetRealTimeDataByTransportType = function(stopid, ttype, callback) {

      my.GetRealTimeData(stopid, function(err, data) {

        var departures = [];
        for (var departure in data) {
          if (data[departure].VehicleMode == ttype) {
            departures.push(data[departure]);
          }
        }

        callback(err, departures);

      });

    };

    /**
    * Group realtime departures by line and direction
    * If ttype is not null, only departures with ttype transport type will be returned
    */
    my.GetRealTimeDataGrouped = function(stopid, ttype, callback) {
      console.log("GetRealTimeDataGrouped " + stopid);

      if (ttype === null) {
        my.GetRealTimeData(stopid, group);          
      }
      else {
        my.GetRealTimeDataByTransportType(stopid, ttype, group)
      }

      function group(err, data) {
        //console.log("Group data: " + JSON.stringify(data, null, 4));
        var departures = {};
        for (var departure in data) {

          // Concat direction linename and destination to create a unique key
          var key = data[departure].DirectionRef + data[departure].PublishedLineName + data[departure].DestinationDisplay;

          console.log(key);

          if (departures.hasOwnProperty(key)) {
            departures[key].push(data[departure]);
          }
          else {
            departures[key] = [data[departure]];            
          }
        }
        callback(err, departures);
      }

    };

    /**
    * Group Realtime Data by Direction (and lines)
    */
    my.GetRealTimeDataGroupedByDirection = function(stopid, ttype, callback) {
      console.log("GetRealTimeDataGroupedByDirection " + stopid);

      my.GetRealTimeDataGrouped(stopid, ttype, function(err, data){
        var departures = {};
        for (var line in data) {

          var key = data[line][0].DirectionRef;

          console.log(key);

          if (departures.hasOwnProperty(key)) {
            departures[key].push(data[line]);
          }
          else {
            departures[key] = [data[line]];            
          }
        }

        callback(err, departures);
      });

    };

    /**
     * Get Ordered Departures in a simple format
     */
    my.SimpleGetOrderedDepartures = function(stopid, ttype, callback) {
      console.log("SimpleGetOrderedDepartures " + stopid);

      my.GetRealTimeDataGroupedByDirection(stopid, ttype, function(err, data) {
        //console.log(JSON.stringify(data, null, 4));
        var res = {};
        for (var dir in data) {
          res[dir] = [];

          for (var line in data[dir]) {

            console.log(data[dir][line][0].PublishedLineName + " " + data[dir][line][0].DestinationDisplay);

            departure_times = [];
            departure_timestamps = [];

            for (var departure in data[dir][line]) {
              // Convert asp.net date to js date
              var server_date = data[dir][line][departure].ExpectedDepartureTime;
              var d = new Date(parseFloat(server_date.replace("/Date(", "").replace(")/", "")));

              departure_times.push(my.GetRuterTime(d.getTime()))
              departure_timestamps.push(d.getTime());
            }
            console.log(JSON.stringify(departure_times, null, 4));

            res[dir].push(
              {
                'PublishedLineName': data[dir][line][0].PublishedLineName,
                'DestinationDisplay': data[dir][line][0].DestinationDisplay,
                'DepartureTimes': departure_times,
                'DepartureTimestamps': departure_timestamps,

                // Extra
                'DestinationName': data[dir][line][0].DestinationName,
                'LineRef': data[dir][line][0].LineRef,
              }
            );

          }

        }
        callback(err, res);


      });

    };

    return my;

}());

/*
  {
    "OperatorRef": "Sporvognsdrift", 
    "VisitNumber": 19, 
    "AimedDepartureTime": "/Date(1398376380000+0200)/", 
    "Delay": "PT37S", 
    "DestinationRef": 3012324, 
    "PublishedLineName": "18", 
    "Monitored": true, 
    "ArrivalBoardingActivity": null, 
    "AimedArrivalTime": "/Date(1398376380000+0200)/", 
    "TrainBlockPart": null, 
    "ExpectedDepartureTime": "/Date(1398376417000+0200)/", 
    "LineRef": "18", 
    "FramedVehicleJourneyRef": {
      "DataFrameRef": "2014-04-24", 
      "DatedVehicleJourneyRef": "18003"
    }, 
    "DirectionRef": "2", 
    "ExpectedArrivalTime": "/Date(1398376417000+0200)/", 
    "InCongestion": false, 
    "DestinationDisplay": "Rikshospitalet", 
    "VehicleAtStop": false, 
    "DestinationName": "Rikshospitalet", 
    "DeparturePlatformName": "12", 
    "Extensions": {
      "Deviations": [
        {
          "Header": "Trikk 18 og 19: Buss for trikk Ljabru\u2013Kastellet-Ljabru", 
          "ID": 25457
        }
      ], 
      "OccupancyData": {
        "OccupancyAvailable": false, 
        "OccupancyPercentage": 20
      }
    }, 
    "OriginName": "Br\u00e5ten [trikk]", 
    "ViaName1": null, 
    "OriginRef": "3010850", 
    "VehicleFeatureRef": "lowFloor", 
    "VehicleRef": "200144", 
    "BlockRef": "1717", 
    "StopVisitNote": null, 
    "DirectionName": "2", 
    "DepartureBoardingActivity": null, 
    "VehicleMode": 3, 
    "VehicleJourneyName": ""
  }, 

*/