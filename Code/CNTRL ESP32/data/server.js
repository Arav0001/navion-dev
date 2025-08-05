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

// servo control

function sendServoAngle(axis) {
	const angle = document.getElementById(`${axis}-angle`);
	const value = parseFloat(angle.value);
	
	if (isNaN(value)) {
		alert("Invalid angle");
		return;
	}

	const cmd = `servo_${axis}:${value}`;

	sendCommand(cmd);
}

// joystick control
const joystickBox = document.getElementById("joystick-box");
const joystickDot = document.getElementById("joystick-dot");
const output = document.getElementById("tvc-output");

const boxSize = 200;
const dotSize = 20;
const boxCenter = boxSize / 2 - dotSize / 2;

let isDragging = false;
let joystickCurrentX = boxCenter;
let joystickCurrentY = boxCenter;
let joystickDeflectX = 0;
let joystickDeflectY = 0;
let lastSendTime = 0;

let updateInterval = 50;

let lastJoystickDeflectX = null;
let lastJoystickDeflectY = null;

joystickDot.addEventListener("mousedown", () => {
	isDragging = true;
	joystickDot.style.transition = "none";
});

document.addEventListener("mouseup", () => {
	if (!isDragging) return;

	isDragging = false;
	joystickDot.style.transition = "left 0.2s ease, top 0.2s ease";
	
	moveJoystickDotToCenter();
});

document.addEventListener("mousemove", (event) => {
	if (!isDragging) return;

	const rect = joystickBox.getBoundingClientRect();
	let x = event.clientX - rect.left - dotSize / 2;
	let y = event.clientY - rect.top - dotSize / 2;

	x = Math.max(0, Math.min(boxSize - dotSize, x));
	y = Math.max(0, Math.min(boxSize - dotSize, y));

	joystickCurrentX = x;
	joystickCurrentY = y;

	updateJoystickDotPosition();
});

function updateJoystickDotPosition() {
	joystickDot.style.left = `${joystickCurrentX}px`;
	joystickDot.style.top = `${joystickCurrentY}px`;

	const dotCenterX = joystickCurrentX + dotSize / 2;
	const dotCenterY = joystickCurrentY + dotSize / 2;

	// flip the axes to signify rotation

	const offsetX = dotCenterY - boxSize / 2;
	const offsetY = dotCenterX - boxSize / 2;

	const range = (boxSize - dotSize) / 2;

	joystickDeflectX = -(offsetX / range) * 5;
	joystickDeflectY = (offsetY / range) * 5;

	const roundedX = joystickDeflectX.toFixed(2);
	const roundedY = joystickDeflectY.toFixed(2);

	output.textContent = `TVC X: ${roundedX}, Y: ${roundedY}`;
}

function moveJoystickDotToCenter() {
	const snapInterval = setInterval(() => {
		const dx = boxCenter - joystickCurrentX;
		const dy = boxCenter - joystickCurrentY;

		const distance = Math.sqrt(dx * dx + dy * dy);

		if (distance < 1) {
			joystickCurrentX = boxCenter;
			joystickCurrentY = boxCenter;

			updateJoystickDotPosition();
			clearInterval(snapInterval);

			return;
		}

		joystickCurrentX += dx * 0.2;
		joystickCurrentY += dy * 0.2;

		updateJoystickDotPosition();
	}, 20);
}

function sendTVCDeflection() {
	const currentTime = Date.now();
	if (currentTime - lastSendTime < updateInterval) return; // 20Hz

	const roundedX = joystickDeflectX.toFixed(2);
	const roundedY = joystickDeflectY.toFixed(2);

	if (roundedX === lastJoystickDeflectX && roundedY === lastJoystickDeflectY) return;

	sendCommand(`tvc_deflect:${roundedX},${roundedY}`);

	lastSendTime = currentTime;
	lastJoystickDeflectX = roundedX;
	lastJoystickDeflectY = roundedY;
}

setInterval(sendTVCDeflection, 50);

window.onload = initSocket;
