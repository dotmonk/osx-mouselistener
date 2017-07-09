# POC Mouse Listener for Mac OS X 10.5+

## Introduction
By using native bindings to IOKit we can talk directly with the Mouse/Touchpad/Touchscreen. We can talk directly with the hardware so this should work regardless of which state the rest of the system is in.

## Usage example
```javascript

const mouseListener = require('osx-mouselistener');

mouseListener.listen((button) => {
  console.log(`User clicked button: ${button}`);
});

```
Writes click information to console

## Installation
via npm
```
npm install osx-mouselistener
```

## Tests
Sorry, I didn't feel like it. It's a POC

## Why?
When I was writing a osx-keylogger POC I was intrigued by the possibility of listening for mouse clicks. Would it be easier? I was determined to find out.

## License
MIT, see LICENSE file
