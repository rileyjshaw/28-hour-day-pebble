var ZZZ_KEY = 0;

var msg = {};
msg[ZZZ_KEY] = localStorage.getItem('zzz');

if (msg[ZZZ_KEY] !== null) {
  Pebble.sendAppMessage(msg);
}

Pebble.addEventListener('showConfiguration', function () {
  var url = 'http://rileyjshaw.com/28-hour-day-pebble/';
  if (msg[ZZZ_KEY] === '1') {
    url += '#enabled';
  }
  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function (e) {
  var res = e.response;
  if (res !== '') {
    localStorage.setItem('zzz', res);
    msg[ZZZ_KEY] = res;
    Pebble.sendAppMessage(msg, ack, nack);
  }
});

// TODO: Don't really need these...
function ack (e) {
  console.log("Sent message with transactionId = " + e.data.transactionId);
}

function nack (e) {
  console.log("Unable to send message with transactionId = " +
              e.data.transactionId + ". Error is: " + e.error.message); 
}
