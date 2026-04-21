const express = require('express');
const mqtt = require('mqtt');
const path = require('path');
const util = require('util');
const fs = require('fs');
const bodyParser = require('body-parser');
const app = express();

const readFile = util.promisify(fs.readFile);
const writeFile = util.promisify(fs.writeFile);
// Create variables for MQTT use here

const broker = 'mqtt://xx:1883';
const options = {
    username: '//',
    password: '//'
};


const topic = 'suzy/button';
const client = mqtt.connect(broker, options);


app.use(bodyParser.json());
function read(filePath = './message.json') {
    return readFile(path.resolve(__dirname, filePath)).then(data => JSON.parse(data));
}
function write(data, filePath = './message.json') {
    return writeFile(path.resolve(__dirname, filePath), JSON.stringify(data));
}

// create an MQTT instance
//const client = mqtt.connect(broker)

// Check that you are connected to MQTT and subscribe to a topic (connect event)
client.on('connect',()=>{
    console.log('client connected');
    client.subscribe(topic);
});

// handle instance where MQTT will not connect (error event)
client.on('error',(err)=>{
    console.error('Connection Error:', err);
})
// Handle when a subscribed message comes in (message event)
client.on('message', async (topic, payload) => {
    try {
        const text = payload.toString().replace(/\0/g, '').trim();
        const incoming = { id: Date.now().toString(), topic: topic, msg: text };
        const messages = await read();
        messages.push(incoming);
        await write(messages);
    } catch (err) {
        console.error('Error handling message:', err);
    }
});

// Route to serve the home page
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname,'index.html'));
})
// route to serve the JSON array from the file message.json when requested from the home page
app.get('/messages', async (req, res) => {
    const message = await read();
    res.json(message);
})
// Route to serve the page to add a message
app.get('/add', (req, res) => {
    res.sendFile(path.join(__dirname, 'message.html'));
})

//Route to show a selected message. Note, it will only show the message as text. No html needed
app.get('/:id', async (req, res) => {
    const messages = await read();
    const found = messages.find(msg => msg.id === req.params.id);
    if (found) {
        res.json(found);
    } else {
        res.sendStatus(404);
    }
});

// Route to CREATE a new message on the server and publish to mqtt broker
app.post('/', async (req, res) => {
    const id = Date.now().toString();
    const new_topic = req.body.topic;
    const new_message = { id: id, topic: new_topic, msg: req.body.msg };

    if (new_topic !== topic) {
        // to myself
        console.log("Publishing...")
        client.publish(new_topic, new_message.msg);
        //client.publish(new_topic, req.body.msg);

    }
    const messages = await read();
    messages.push(new_message);
    await write(messages);

    res.sendStatus(200);
});

// Route to delete a message by id (Already done for you)
app.delete('/:id', async (req, res) => {
    try {
        const messages = await read();
        write(messages.filter(c => c.id !== req.params.id));
        res.sendStatus(200);
    } catch (e) {
        res.sendStatus(200);
    }
});

// listen to the port
app.listen(3000);