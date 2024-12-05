console.log("Running program —");

// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 

// // establish connection to UDP server
// const dgram = require('dgram'); //
// const udpServer = dgram.createSocket('udp4'); // 

// establish serial communication
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

// create websocket to communicate with Wick
const http = require('http');
const WebSocket = require('ws');
const httpServer = http.createServer();
const wss = new WebSocket.Server({ server: httpServer });

// // when there is an error in UDP server 
// udpServer.on('error', (err) => { // error in UDP server
//     console.log(`Server error:\n${err.stack}`);
//     udpServer.close();
// });

// udpServer.on('message', (msg, rinfo) => {
//     console.log(`Server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
//     // Broadcast the UDP message to all connected WebSocket clients
//     wss.clients.forEach(client => {
//         if (client.readyState === WebSocket.OPEN) {
//             client.send(msg.toString());
//         }
//     });
// });

// udpServer.on('listening', () => {
//     const address = udpServer.address();
//     console.log(`UDP Server listening on ${address.address}:${address.port}`);
// });

// udpServer.bind(12347); // udpServer.bind(12347, '0.0.0.0');

// making sure websocket is connected
wss.on('connection', (ws) => {
    console.log('WebSocket client connected');
});

httpServer.listen(8080, () => {
    console.log('WebSocket server listening on port 8080');
});






// Configure the serial port
const port = new SerialPort({
    path: '/dev/tty.usbmodemMG3500011', // serial port
    baudRate: 115200, // Set your baud rate
});

// Create a parser to handle incoming data
const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }));

// Event: Open the port
port.on('open', () => {
    console.log('Serial port is open.');
});

// Event: Data received
parser.on('data', (data) => {
    console.log('Received:', data);

    // send data through websocket when recieved
    wss.clients.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(data.toString());
        }
    });

});

// Send data to the serial device
port.write('Hello, serial device!\n', (err) => {
    if (err) {
        return console.error('Error writing to port:', err.message);
    }
    console.log('Message sent.');
});
