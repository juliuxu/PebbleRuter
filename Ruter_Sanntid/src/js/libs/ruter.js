/**
 * Ruter Javascript library
 *
 * Depends on jscoord for lat/lon to UTM conversion: http://www.jstott.me.uk/jscoord/
 *
 * Author: Julian Jark <julianj@ifi.uio.no>
 *
 */


var Ruter = (function() {
    var my = {};
    
    var base_url = 'http://api.ruter.no/reisrestnational/';
    
    var location_options = {
      enableHighAccuracy: false,
      timeout: 5000,
      maximumAge: 60000
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
    function get(url, query, successCb, failureCb) {

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
          return successCb(response);
        }
        else {
          return failureCb(new Error(req.status));
        }
      };
      req.onerror = function () {
        switch (req.status) {
        case 0:
          return failureCb(new Error('NOT_CONNECTED'));
        case 404:
          return failureCb(new Error('NOT_FOUND'));
        default:
          return failureCb(new Error('ERROR: ' + req.status));
        }
      };
      req.send();
    }

    /**
     * Convert a timestamp to ruter time estimate
     * Return the converted string
     */
    my.GetRuterTime = function(timestamp, nowString) {
      var remaining = timestamp - Date.now();
      remaining = Math.floor(remaining / 1000);

      if (remaining < 0) {
        return "";
      }
      else if (remaining < 44) {
        if (typeof(nowString) === 'undefined') {
          //return "now";
          return "n\u00E5";
        }
        else {
          return nowString;
        }
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
     * Get the current location in utm32 coordinates
     */
    my.GetCurrentUTM32Location = function(successCb, failureCb) {
      console.log("GetCurrentUTM32Location");

      function success(position) {

        // Convert to UTM32 using jscoord
        var latLng = new LatLng(position.coords.latitude, position.coords.longitude);
        var utm = latLng.toUTMRef();
        var X = utm.easting.toString().split(".")[0];
        var Y = utm.northing.toString().split(".")[0];

        console.log('UTM32 X: ' + X + ' Y: ' + Y);

        successCb(X, Y);
      }

      function error(err) {
        console.warn('getCurrentPosition ERROR(' + err.code + '): ' + err.message);
        failureCb('ERROR(' + err.code + '): ' + err.message);
      }

      navigator.geolocation.getCurrentPosition(success, error, location_options);

    };


    /**
     * Get a list of stops by UTM32 coordinates, Advanced
     */
    my.GetClosestStopsAdvancedByCoordinates = function (X, Y, proposals, walkingDistance, successCb, failureCb) {
      var parameters = {
        'coordinates': '(X='+X+',Y='+Y+')',
        'proposals': proposals,
        'walkingDistance': walkingDistance,

      };
      get(base_url + 'Stop/GetClosestStopsAdvancedByCoordinates/', parameters, function(data) {

        var stops = data;

        /**
         * Be able to filter by transport type
         */
        stops.filterByTransport = function(ttype) {
          return this.filter(function(element) {
            if (element.hasOwnProperty('Lines')) {
              for (var i=0;i<element.Lines.length;i++) {
                if (element.Lines[i].hasOwnProperty('Transportation') && element.Lines[i].Transportation == ttype) {
                  return true;
                }
              }
            }
            return false;
          });
        };
        successCb(stops);

      }, failureCb);
    };

    /**
     * Get a list of stops by UTM32 coordinates
     */
    my.GetClosestStopsByCoordinates = function (X, Y, successCb, failureCb) {
      my.GetClosestStopsAdvancedByCoordinates(X, Y, 7, 1200, successCb, failureCb);
      return;
    };

    /**
     * Get realtime data for stopid
     */
    my.GetRealTimeData = function (stopid, successCb, failureCb) {
      console.log("GetRealTimeData " + stopid);
      get(base_url + 'RealTime/GetRealTimeData/' + stopid, {}, function(data) {

        var departures = data;

        /**
         * Be able to group by line+destination
         */
        departures.groupByLineDestination = function() {

          var groupedDepartures = {};
          for (var i=0;i<this.length;i++) {
            if (!(this[i].hasOwnProperty('DirectionRef') && this[i].hasOwnProperty('PublishedLineName') && this[i].hasOwnProperty('DestinationName'))) {
              continue;
            }

            var key = this[i].DirectionRef + this[i].PublishedLineName + this[i].DestinationName;

            if (groupedDepartures.hasOwnProperty(key)) {
              groupedDepartures[key].push(this[i]);
            }
            else {
              groupedDepartures[key] = [this[i]];
            }

          }

          /**
           * Add Relevant Data
           */
          for (var key in groupedDepartures) {
            if (groupedDepartures.hasOwnProperty(key)) {
              var departureTimes = [];
              var departureTimestamps = [];
              for (var i=0;i<groupedDepartures[key].length;i++) {
                if (groupedDepartures[key][i].hasOwnProperty("ExpectedDepartureTime")) {
                  // Convert asp.net date to js date
                  var server_date = groupedDepartures[key][i].ExpectedDepartureTime;
                  var d = new Date(parseFloat(server_date.replace("/Date(", "").replace(")/", "")));

                  departureTimes.push(my.GetRuterTime(d.getTime()));
                  departureTimestamps.push(d.getTime());
                }
              }
              groupedDepartures[key].PublishedLineName = groupedDepartures[key][0].PublishedLineName;
              groupedDepartures[key].DestinationName = groupedDepartures[key][0].DestinationName;
              groupedDepartures[key].LineRef = groupedDepartures[key][0].LineRef;
              groupedDepartures[key].DirectionRef = groupedDepartures[key][0].DirectionRef;

              groupedDepartures[key].departureTimes = departureTimes;
              groupedDepartures[key].departureTimestamps = departureTimestamps;
            }
          }

          return groupedDepartures;
        };

        /**
         * Be able to filter by transport type
         */
        departures.filterByTransport = function(ttype) {
          var res = this.filter(function(element) {
            return (element.hasOwnProperty('VehicleMode') && element.VehicleMode === ttype);
          });
          res.groupByLineDestination = this.groupByLineDestination;
          return res;
        };

        successCb(departures);

      }, failureCb);
    };

    return my;

}());