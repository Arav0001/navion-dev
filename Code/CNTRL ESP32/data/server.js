let socket = null;

function initSocket() {
	socket = new WebSocket(`ws://${location.host}/socket`);

	socket.onopen = () => {
		console.log("WebSocket connected");
	};

	socket.onmessage = (event) => {
		alert("Message from NAVION: " + event.data);
		console.log("Message from NAVION:", event.data);
	};

	socket.onclose = () => {
		console.log("WebSocket closed. Reconnecting...");
		setTimeout(initSocket, 3000);
	};

	socket.onerror = (err) => {
		console.error("WebSocket error:", err);
		socket.close();
	};
}

function sendCommand(command) {
	if (socket && socket.readyState === WebSocket.OPEN) {
		socket.send(command);
	} else {
		console.warn("WebSocket not connected yet.");
	}
}

window.onload = initSocket;
