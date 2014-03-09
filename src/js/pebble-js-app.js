function HTTPGET(url) {
    var req = new XMLHttpRequest();
    req.open("GET", url, false);
    req.send(null);
    return req.responseText;
}

var sett = function() {
  var response, response2;
  var json, json2;
  var bprice, dprice, lprice;
  
  // Bitcoin
  response = HTTPGET("https://api.bitcoinaverage.com/all");
  json = JSON.parse(response);
  bprice = json.USD.averages.last;
  
  // Dogecoin
  response2 = HTTPGET("http://www.cryptocoincharts.info/v2/api/listCoins");
  json2 = JSON.parse(response2);
  dprice = json2[61].price_btc * bprice * 10000;
  dprice = dprice.toFixed(4);
  
  // Litecoin
  lprice = json2[123].price_btc * bprice;
  lprice = lprice.toFixed(4);
  
  var dict = {"KEY_BPRICE" : bprice + "", "KEY_DPRICE" : dprice + "", "KEY_LPRICE" : lprice + ""};
     
  // Send data to watch for display
  Pebble.sendAppMessage(dict);
};

Pebble.addEventListener("ready",
  function(e) {
    sett();
  }
);

Pebble.addEventListener("appmessage",
  function(e) {
    //Watch wants new data!
    sett();
  }
);