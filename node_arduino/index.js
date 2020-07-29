const five = require("johnny-five");
//Cria objeto para conectar a placa
const board = new five.Board();

//Quando conseguir se conectar a placa, realiza os comandos dentro da 'function()'
board.on("ready", () => {
  // Cria um led no pino digital 13 do Arduino
  const led = new five.Led(6);
  // Pisca o led a cada 1000ms (1 segundo)
  led.blink(1000);
}); 
