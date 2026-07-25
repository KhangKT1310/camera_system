package main

import (
	"flag"
	"log"
	"net/http"

	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	ReadBufferSize:  65536,
	WriteBufferSize: 65536,
	CheckOrigin: func(r *http.Request) bool {
		return true // Allow all origins for dev environment
	},
}

func handleWebSocket(hub *SessionHub, w http.ResponseWriter, r *http.Request) {
	role := r.URL.Query().Get("role")
	sessionID := r.URL.Query().Get("session_id")

	if role == "" {
		role = "viewer"
	}
	if sessionID == "" {
		sessionID = "stream1"
	}

	conn, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		log.Printf("[SignalingServer] Upgrade error: %v", err)
		return
	}

	client := &Client{
		SessionID: sessionID,
		Role:      role,
		Conn:      conn,
		Send:      make(chan []byte, 256),
	}

	hub.Register(client)

	// Writer goroutine
	go func() {
		defer func() {
			client.Conn.Close()
		}()
		for message := range client.Send {
			if err := client.Conn.WriteMessage(websocket.TextMessage, message); err != nil {
				log.Printf("[SignalingServer] Write error: %v", err)
				break
			}
		}
	}()

	// Reader loop
	defer func() {
		hub.Unregister(client)
		client.Conn.Close()
	}()

	for {
		_, message, err := client.Conn.ReadMessage()
		if err != nil {
			if websocket.IsUnexpectedCloseError(err, websocket.CloseGoingAway, websocket.CloseAbnormalClosure) {
				log.Printf("[SignalingServer] Read error: %v", err)
			}
			break
		}

		// Forward envelope to other clients in room
		hub.Broadcast(client, message)
	}
}

func main() {
	addr := flag.String("addr", ":8080", "HTTP service address")
	flag.Parse()

	hub := NewSessionHub()

	http.HandleFunc("/ws", func(w http.ResponseWriter, r *http.Request) {
		handleWebSocket(hub, w, r)
	})

	http.HandleFunc("/health", func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusOK)
		w.Write([]byte(`{"status":"UP","service":"signaling-service"}`))
	})

	log.Printf("[SignalingServer] Starting Go WebRTC Signaling Server on %s (ws://localhost%s/ws)", *addr, *addr)
	if err := http.ListenAndServe(*addr, nil); err != nil {
		log.Fatalf("[SignalingServer] ListenAndServe error: %v", err)
	}
}
