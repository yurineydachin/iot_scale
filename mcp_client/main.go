package main

import (
	"log"
	"net/http"
)

func main() {
	serverURL := "http://localhost:8080/tools/unlock/call"
	log.Printf("Sending request to %s", serverURL)

	resp, err := http.Post(serverURL, "application/json", nil)
	if err != nil {
		log.Fatalf("Failed to send request: %v", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode == http.StatusOK {
		log.Println("Successfully sent unlock command.")
	} else {
		log.Printf("Failed to send unlock command. Status: %s", resp.Status)
	}
}
