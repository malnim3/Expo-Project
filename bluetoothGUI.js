//let blueToothCharacteristic;
let receivedValue = "";

let blueTooth;
let isConnected = false;

function setup() {

  createCanvas(windowWidth, windowHeight);
  
  // Create a p5ble class
  console.log("setting up");
  blueTooth = new p5ble();

  //Creating button which will allow connection of arduino
  const connectButton = createButton('Connect');
  connectButton.mousePressed(connectToBle);
  connectButton.position(15, 15);
  
  //Creating buttons for time zones Pacific, Mountain, Central, and Eastern. 
  const pacificButton = createButton('Pacific');
  pacificButton.mousePressed(zonePacific);
  pacificButton.position(15, 60);

  const mountainButton = createButton('Mountain');
  mountainButton.mousePressed(zoneMountain);
  mountainButton.position(pacificButton.x+pacificButton.width+10, 60);

  const centralButton = createButton('Central');
  centralButton.mousePressed(zoneCentral);
  centralButton.position(mountainButton.x+mountainButton.width+10, 60);

  const easternButton = createButton('Eastern');
  easternButton.mousePressed(zoneEastern);
  easternButton.position(centralButton.x+centralButton.width+10, 60);
}
