/* eslint-disable no-console,import/no-unresolved */
const mouseListener = require('./build/Release/osx-mouselistener');

mouseListener.listen((button) => {
  console.log(`User clicked button: ${button}`);
});
