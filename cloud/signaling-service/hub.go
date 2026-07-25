package main

import (
	"log"
	"sync"

	"github.com/gorilla/websocket"
)

type Client struct {
	SessionID string
	Role      string // "camera" or "viewer"
	Conn      *websocket.Conn
	Send      chan []byte
}

type SessionHub struct {
	mu       sync.RWMutex
	sessions map[string]map[*Client]bool
}

func NewSessionHub() *SessionHub {
	return &SessionHub{
		sessions: make(map[string]map[*Client]bool),
	}
}

func (h *SessionHub) Register(client *Client) {
	h.mu.Lock()
	defer h.mu.Unlock()

	if _, exists := h.sessions[client.SessionID]; !exists {
		h.sessions[client.SessionID] = make(map[*Client]bool)
	}
	h.sessions[client.SessionID][client] = true
	log.Printf("[SignalingHub] Client registered: role=%s, session_id=%s", client.Role, client.SessionID)
}

func (h *SessionHub) Unregister(client *Client) {
	h.mu.Lock()
	defer h.mu.Unlock()

	if clients, exists := h.sessions[client.SessionID]; exists {
		if _, ok := clients[client]; ok {
			delete(clients, client)
			close(client.Send)
			log.Printf("[SignalingHub] Client unregistered: role=%s, session_id=%s", client.Role, client.SessionID)
		}
		if len(clients) == 0 {
			delete(h.sessions, client.SessionID)
		}
	}
}

func (h *SessionHub) Broadcast(sender *Client, message []byte) {
	h.mu.RLock()
	defer h.mu.RUnlock()

	clients, exists := h.sessions[sender.SessionID]
	if !exists {
		return
	}

	for client := range clients {
		if client != sender {
			select {
			case client.Send <- message:
			default:
				log.Printf("[SignalingHub] Buffer full, dropping message for client role=%s", client.Role)
			}
		}
	}
}
