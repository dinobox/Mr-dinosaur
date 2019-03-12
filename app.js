const express = require('express');
const app = express();
const say = require('./say.js');
var bodyParser = require('body-parser');
var urlencodedParser = bodyParser.urlencoded({ extended: false })
app.get('/', (req, res) => res.send('Hello World!'));
app.post('/say', urlencodedParser, (req, res) => {
    text = req.body.text;
    say.play(text);
    res.end(text);
});

app.listen(3000, () => console.log('Example app listening on port 3000!'));