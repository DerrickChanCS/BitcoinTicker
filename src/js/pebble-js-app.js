function HTTPGET(url) {
    var req = new XMLHttpRequest();
    req.open("GET", url, false);
    req.send(null);
    return req.responseText;
}

var sett = function() {
  console.log("#Set method called");
  
  var response = HTTPGET("https://api.bitcoinaverage.com/all");
  
  var json = JSON.parse(response);
  
  var last = json.USD.averages.last;
  console.log("last: " + last);
  
  var bid = json.USD.averages.bid;
  console.log("bid: " + bid);
  
  var dict = {"KEY_LAST" : last, "KEY_BID" : bid};
     
  // Send data to watch for display
  Pebble.sendAppMessage(dict);
};

Pebble.addEventListener("ready",
  function(e) {
    sett();
  }
);
