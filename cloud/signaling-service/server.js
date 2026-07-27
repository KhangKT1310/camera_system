/**
 * Dev Signaling Server (Node.js 0-dependency / Pure Built-in Modules)
 * Runs out-of-the-box anywhere without npm dependencies: `node server.js`
 */
const http = require('http');
const crypto = require('crypto');

const PORT = process.env.PORT || 8080;

// session_id -> Set of socket clients
const sessions = new Map();

function parseQuery(urlStr) {
  const query = {};
  const qIdx = urlStr.indexOf('?');
  if (qIdx === -1) return query;
  const pairs = urlStr.slice(qIdx + 1).split('&');
  for (const pair of pairs) {
    const [k, v] = pair.split('=');
    if (k) query[decodeURIComponent(k)] = decodeURIComponent(v || '');
  }
  return query;
}

const server = http.createServer((req, res) => {
  if (req.url === '/health') {
    res.writeHead(200, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ status: 'UP', service: 'signaling-service-dev' }));
    return;
  }
  res.writeHead(200, { 'Content-Type': 'text/plain' });
  res.end('WebRTC Signaling Service (Dev WS Server)');
});

server.on('upgrade', (req, socket, head) => {
  const query = parseQuery(req.url);
  const role = query.role || 'viewer';
  const sessionId = query.session_id || 'stream1';

  const secKey = req.headers['sec-websocket-key'];
  if (!secKey) {
    socket.destroy();
    return;
  }

  const hash = crypto
    .createHash('sha1')
    .update(secKey + '258EAFA5-E914-47DA-95CA-C5AB0DC85B11')
    .digest('base64');

  const headers = [
    'HTTP/1.1 101 Switching Protocols',
    'Upgrade: websocket',
    'Connection: Upgrade',
    `Sec-WebSocket-Accept: ${hash}`,
    '\r\n'
  ];

  socket.write(headers.join('\r\n'));

  socket.role = role;
  socket.sessionId = sessionId;

  if (!sessions.has(sessionId)) {
    sessions.set(sessionId, new Set());
  }
  sessions.get(sessionId).add(socket);

  console.log(`[SignalingServer] Client connected: role=${role}, session_id=${sessionId}`);

  let rawBuffer = Buffer.alloc(0);

  socket.on('data', (chunk) => {
    rawBuffer = Buffer.concat([rawBuffer, chunk]);

    while (rawBuffer.length >= 2) {
      const opcode = rawBuffer[0] & 0x0f;
      const isMasked = (rawBuffer[1] & 0x80) !== 0;
      let payloadLen = rawBuffer[1] & 0x7f;

      let headerLen = 2;
      if (payloadLen === 126) {
        if (rawBuffer.length < 4) break;
        payloadLen = rawBuffer.readUInt16BE(2);
        headerLen = 4;
      } else if (payloadLen === 127) {
        if (rawBuffer.length < 10) break;
        payloadLen = Number(rawBuffer.readBigUInt64BE(2));
        headerLen = 10;
      }

      const maskLen = isMasked ? 4 : 0;
      const totalFrameLen = headerLen + maskLen + payloadLen;

      if (rawBuffer.length < totalFrameLen) {
        break; // Wait for full WebSocket frame chunk
      }

      const frameData = rawBuffer.slice(0, totalFrameLen);
      rawBuffer = rawBuffer.slice(totalFrameLen);

      let payload = frameData.slice(headerLen + maskLen);
      if (isMasked) {
        const maskKey = frameData.slice(headerLen, headerLen + 4);
        payload = Buffer.from(payload);
        for (let i = 0; i < payload.length; i++) {
          payload[i] ^= maskKey[i % 4];
        }
      }

      if (opcode === 0x1 || opcode === 0x2) { // Text or Binary frame
        const msgStr = payload.toString('utf8');
        console.log(`[SignalingServer] Received from role=${role}, session_id=${sessionId}: ${msgStr.slice(0, 80)}...`);

        const room = sessions.get(sessionId);
        if (room) {
          for (const client of room) {
            if (client !== socket && !client.destroyed) {
              sendWsFrame(client, payload);
              console.log(`[SignalingServer] Forwarded message to client role=${client.role}`);
            }
          }
        }
      } else if (opcode === 0x8) { // Close frame
        socket.end();
      }
    }
  });

  socket.on('close', () => {
    const room = sessions.get(sessionId);
    if (room) {
      room.delete(socket);
      if (room.size === 0) sessions.delete(sessionId);
    }
    console.log(`[SignalingServer] Client disconnected: role=${role}, session_id=${sessionId}`);
  });

  socket.on('error', (err) => {
    console.error(`[SignalingServer] Socket error:`, err.message);
  });
});

function sendWsFrame(socket, payload) {
  const len = payload.length;
  let header;
  if (len <= 125) {
    header = Buffer.from([0x81, len]);
  } else if (len <= 65535) {
    header = Buffer.alloc(4);
    header[0] = 0x81;
    header[1] = 126;
    header.writeUInt16BE(len, 2);
  } else {
    header = Buffer.alloc(10);
    header[0] = 0x81;
    header[1] = 127;
    header.writeBigUInt64BE(BigInt(len), 2);
  }
  socket.write(Buffer.concat([header, payload]));
}

server.listen(PORT, () => {
  console.log(`[SignalingServer] Starting 0-dependency Dev Signaling Server on ws://localhost:${PORT}/ws`);
});
