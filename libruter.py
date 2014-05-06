"""
libruter.py: Simple library to talk to the ruter api
"""
__author__ = "Julian Jark"

import urllib2
import json
import re
import time

API_BASE_URL = 'http://api.ruter.no/ReisRest/'

REALTIME_TRANSPORT_TYPES = {
    'bus': 0,
    'ferry': 1,
    'rail': 2,
    'tram': 3,
    'metro': 4,
}

TRAVEL_TRANSPORT_TYPES = {
    'walking': 0,
    'airportbus': 1,
    'bus': 2,
    'dummy value': 3,
    'airporttrain': 4,
    'boat': 5,
    'train': 6,
    'tram': 7,
    'metro': 8,
}


def get_contents(url):
    "Return the content of an url"

    req = urllib2.Request(url)
    response = urllib2.urlopen(req)

    if response.getcode() != 200:
        raise RuntimeError("did not return 200")

    return response.read()


def place_findmatches(name):
    url = API_BASE_URL + "Place/FindMatches/" + name
    return json.loads(get_contents(url))


def place_findplaces(name):
    url = API_BASE_URL + "Place/FindPlaces/" + name
    return json.loads(get_contents(url))


def place_findplacesbycounties(name, counties):
    """
    Get places by conties

    @param name: Name to search for
    @param counties: A list of counties
    """

    url = API_BASE_URL + "Place/FindPlacesByCounties/" + name + "?counties={}".format(",".join(counties))
    return json.loads(get_contents(url))


def stop_getcloseststopsbycoordinates(x, y, propsals=7):
    url = API_BASE_URL + "Stop/GetClosestStopsByCoordinates/" + \
        "?coordinates=(X={0},Y={1})&proposals={2}".format(x, y, propsals)
    return json.loads(get_contents(url))


def stop_getcloseststopsadvancedbycoordinates(x, y, walkingDistance=1200, propsals=7):
    url = API_BASE_URL + "Stop/GetClosestStopsAdvancedByCoordinates/" + \
        "?coordinates=(X={0},Y={1})&proposals={2}&walkingDistance={3}".format(x, y, propsals, walkingDistance)
    return json.loads(get_contents(url))


def realtime_getrealtimedata(stopid):
    url = API_BASE_URL + "RealTime/GetRealTimeData/" + str(stopid)
    return json.loads(get_contents(url))


def realtime_getalldepartures(strstopid):
    url = API_BASE_URL + "RealTime/GetAllDepartures/" + str(strstopid)
    return json.loads(get_contents(url))


def get_nearest_by_transport_type(x, y, ttype):
    """
    Get nearest stops by transport types, i.e filter out all other stops
    """

    stops = stop_getcloseststopsadvancedbycoordinates(x, y, 1200, 25)

    bus_stops = []

    for x in stops:

        # A stop can have multiple transportations
        # So to determine if this stop has 'ttype' transportations we need to check all lines
        for line in x['Lines']:
            if line['Transportation'] == ttype:
                bus_stops.append(x)
                break

    return bus_stops


def get_nearest_buses(x, y):
    return get_nearest_by_transport_type(x, y, TRAVEL_TRANSPORT_TYPES['bus'])


def get_nearest_trams(x, y):
    return get_nearest_by_transport_type(x, y, TRAVEL_TRANSPORT_TYPES['tram'])


def get_nearest_metros(x, y):
    return get_nearest_by_transport_type(x, y, TRAVEL_TRANSPORT_TYPES['metro'])


def get_realtime_by_transport_type(stopid, ttype):
    """
    Get realtime data for a stop, and filter out all other transport types than the one specifed
    returns a stripped down dict of departure information

    TODO: Group together, sort by departure times
    """

    departures = realtime_getrealtimedata(stopid)

    type_departures = []

    for x in departures:
        if x['VehicleMode'] != ttype:
            continue
        else:

            rtmp = re.match(r'/Date\((\d+)([+-]\d+)\)/', x['ExpectedDepartureTime'])
            if not rtmp:
                continue

            # departure_time = datetime.fromtimestamp(int(rtmp.group(1)) // 1000)
            timestamp = int(rtmp.group(1)) // 1000

            type_departures.append(
                {
                    # 'departure_time': departure_time,
                    'timestamp': timestamp,
                    'destinationdisplay': x['DestinationDisplay'],
                    'destinationname': x['DestinationName'],

                    'lineref': x['LineRef'],
                    'publishedlinename': x['PublishedLineName'],

                    'directionref': x['DirectionRef'],
                }
            )

    return type_departures


def get_ruter_time(timestamp):
    """
    Convert a timestamp for display in ruter realtime
    """

    remaining = timestamp - int(time.time())

    if remaining < 0:
        return None
    elif remaining < 44:
        return "Now"
    elif remaining > 584:
        return time.strftime("%H:%M", time.localtime(timestamp))
    else:
        for i, x in enumerate(xrange(105, 645, 60)):
            if remaining < x:
                return "{} min".format(i+1, remaining)


def show_realtime_by_transport_type(stopid, type, outputtype=0):

    departures = get_realtime_by_transport_type(stopid, type)

    if outputtype == 0:
        # Print all
        for x in departures:
            print("{:5} {:18.18} - {:6}".format(x['publishedlinename'], x['destinationdisplay'], get_ruter_time(x['timestamp'])))
    elif outputtype == 1:
        # Print departures for each line
        lines = set([(x['directionref'], x['publishedlinename'], x['destinationdisplay']) for x in departures])
        for line in sorted(lines):
            print "{:5} {:18.18}".format(line[1], line[2])
            linedepartures = [get_ruter_time(x['timestamp']) for x in departures if x['directionref'] == line[0] and x['publishedlinename'] == line[1] and x['destinationdisplay'] == line[2]]
            print " ".join(linedepartures[:3])
            print

##
# Todo:
# * Deviations
# * Ca when not from realtime
#

if __name__ == '__main__':
    pass
    #if len(sys.argv) < 3:
    #    print("Usage: {} <function> <arg2>".format(sys.argv[0]))

    # print json.dumps(place_findplaces("Nationall"), indent=2)

    # print json.dumps(stop_getcloseststopsbycoordinates(596762, 6644420, 5), indent=2)

    stops = stop_getcloseststopsadvancedbycoordinates(596762, 6644420, 1200, 7)
    print json.dumps(stops, indent=2)
    print len(stops)

    # print json.dumps(realtime_getalldepartures(3010030), indent=2)
    # print len(realtime_getalldepartures(3010030))

    # print json.dumps(realtime_getrealtimedata(6274360)[:3], indent=2)
    # print len(realtime_getrealtimedata(3010312))

    # print json.dumps(place_findplacesbycounties("Bislett", ['Oslo']), indent=2)

    # print json.dumps(get_nearest_buses(596762, 6644420), indent=2)

    # print json.dumps(get_nearest_trams(596762, 6644420), indent=2)

    # print json.dumps(get_nearest_metros(596762, 6644420), indent=2)

    # print json.dumps(get_realtime_by_transport_type(3010312, REALTIME_TRANSPORT_TYPES['tram']), indent=2)

    # show_realtime_by_transport_type(3010312, REALTIME_TRANSPORT_TYPES['tram'], 1)
