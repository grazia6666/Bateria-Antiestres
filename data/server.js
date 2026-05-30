const http = require('http');
const fs = require('fs');
const path = require('path');
const WebSocket = require('ws');

// Servidor HTTP para servir el HTML
const httpServer = http.createServer((req, res) => {
  res.writeHead(200, { 'Content-Type': 'text/html' });

  const htmlPath = path.join(__dirname, 'index.html');
  fs.createReadStream(htmlPath).pipe(res);
});

// WebSocket simulando el ESP32
const wss = new WebSocket.Server({ server: httpServer });

wss.on('connection', (ws) => {
  console.log('Celular conectado');

  ws.on('message', (msg) => {
    const data = JSON.parse(msg);
    console.log('Recibido:', data);

    if (data.accion === 'get_scores') {
      ws.send(JSON.stringify({ scores: [] }));
    }

    if (data.accion === 'iniciar_juego') {
      // Simula el ESP32 arrancando el juego
      ws.send(
        JSON.stringify({
          evento: 'juego_iniciado',
          vidas: 3,
          modo: data.modo
        })
      );

      setTimeout(() => {
        ws.send(
          JSON.stringify({
            evento: 'pad_objetivo',
            pad: 2,
            ronda: 1
          })
        );
      }, 2000);

      setTimeout(() => {
        ws.send(
          JSON.stringify({
            evento: 'hit_correcto',
            pad: 2,
            puntos_ganados: 100,
            puntos_total: 100,
            combo: 1
          })
        );
      }, 4000);

      setTimeout(() => {
        ws.send(
          JSON.stringify({
            evento: 'juego_terminado',
            puntos_final: 100
          })
        );
      }, 6000);
    }
  });

  ws.on('close', () => {
    console.log('Cliente desconectado');
  });
});

httpServer.listen(3000, () => {
  console.log('Abre http://localhost:3000');
});