/**
 * Dev Signaling Server (Node.js 0-dependency / WebSocket fallback)
 * Runs out-of-the-box with Node.js: `node server.js`
 */
const http = require('http');
const crypto = require('crypto');

const PORT = process.env.PORT || 8080;

// session_id -> Set of socket clients
const sessions = new Map();

function parseQuery(url) {
  const query = {};
  const qIdx = url.indexOf('?');
  if (qIdx === -1) return query;
  const pairs = url.slice(qIdx + 1).split('&');
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

  // WebSocket Handshake Key Accept
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

  // Store client metadata
  socket.role = role;
  socket.sessionId = sessionId;

  if (!sessions.has(sessionId)) {
    sessions.set(sessionId, new Set());
  }
  sessions.get(sessionId).add(socket);

  console.log(`[SignalingServer] Client connected: role=${role}, session_id=${sessionId}`);

  // Raw frame parser
  socket.on('data', (buf) => {
    // Basic unmasking for incoming WebSocket text frames
    const len = buf[1] & 0x7f;
    let offset = 2;
    if (len === 126) offset += 2;
    else if (len === 127) offset += 8;

    const masks = buf.slice(offset, offset + 4);
    offset += 4;

    const payload = buf.slice(offset);
    for (let i = 0; i < payload.length; i++) {
      payload[i] ^= masks[i % 4];
    }

    // Forward raw payload to other clients in same session_id
    const room = sessions.get(sessionId);
    if (room) {
      for (const client of room) {
        if (client !== socket && !client.destroyed) {
          sendWsFrame(client, payload);
        }
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
  console.log(`[SignalingServer] Starting Dev Signaling Server on ws://localhost:${PORT}/ws`);
});
