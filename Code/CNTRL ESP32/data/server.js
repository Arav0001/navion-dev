let socket = null;

function initSocket() {
	socket = new WebSocket(`ws://${location.host}/socket`);

	socket.onopen = () => {
		console.log("WebSocket connected");
	};

	socket.onmessage = (event) => {
		console.log("Message from NAVION:", event.data);

		// TODO: json isnt being updated for some reason
		try {
			const data = JSON.parse(event.data);
			processRocketData(data);
		} catch (e) {
			console.warn("Invalid JSON:", e, event.data);
		}
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
	if (currentTime - lastSendTime < updateInterval) return;

	const roundedX = joystickDeflectX.toFixed(2);
	const roundedY = joystickDeflectY.toFixed(2);

	if (roundedX === lastJoystickDeflectX && roundedY === lastJoystickDeflectY) return;

	sendCommand(`tvc_deflect:${roundedX},${roundedY}`);

	lastSendTime = currentTime;
	lastJoystickDeflectX = roundedX;
	lastJoystickDeflectY = roundedY;
}

setInterval(sendTVCDeflection, 50);

// state graphs stuff

const graphTimeSpanSecElem = document.getElementById('time-span-slider');
const graphTimeSpanValueElem = document.getElementById('time-span-value');

let graphTimeSpanSec = parseInt(graphTimeSpanSecElem.value);

graphTimeSpanSecElem.addEventListener('input', () => {
	graphTimeSpanSec = parseInt(graphTimeSpanSecElem.value);
	graphTimeSpanValueElem.textContent = graphTimeSpanSec;
	
	pruneAllBuffers();
});

const stateContainer = document.getElementById('state-container');

const stateVars = {};

const multiComponentVars = {
	acc: ['x', 'y', 'z'],
	gyr: ['x', 'y', 'z'],
	mag: ['x', 'y', 'z'],
	quat: ['w', 'x', 'y', 'z'],
	tvc: ['x', 'y'],
	pyro: ['motor', 'parachute']
};

const colors = ['#ff6384', '#36a2eb', '#cc65fe', '#ffce56', '#4bc0c0', '#9966ff'];

const pyroStates = [
	"PYRO_STANDBY",
	"PYRO_FIRING",
	"PYRO_CONFIRMED",
	"PYRO_TIMEOUT",
	"PYRO_BROKEN"
];

function createStateCard(name, isMulti=false, components=null) {
	const card = document.createElement('div');
	card.classList.add('state-card');

	const labelDiv = document.createElement('div');
	labelDiv.classList.add('state-label');
	labelDiv.textContent = name;
	card.appendChild(labelDiv);

	const valueSpan = document.createElement('div');
	valueSpan.classList.add('state-value');
	card.appendChild(valueSpan);

	const canvas = document.createElement('canvas');
	card.appendChild(canvas);

	let chart = null;

	if (isMulti && components) {
		const datasets = components.map((comp, idx) => ({
		label: comp,
		borderColor: colors[idx % colors.length],
		backgroundColor: 'transparent',
		data: [],
		fill: false,
		cubicInterpolationMode: 'monotone',
		tension: 0.3,
		pointRadius: 0,
		borderWidth: 2,
		spanGaps: true
		}));

		chart = new Chart(canvas.getContext('2d'), {
		type: 'line',
		data: {
			datasets
		},
		options: {
			animation: false,
			maintainAspectRatio: false,
			scales: {
			x: {
				type: 'linear',
				title: { display: true, text: 'T_plus (sec)' },
				min: 0,
				max: graphTimeSpanSec,
				reverse: true
			},
			y: {
				beginAtZero: true,
				title: { display: true, text: name }
			}
			},
			plugins: {
			legend: { position: 'top' }
			}
		}
		});
	} else if (name === 'pyro') {
		valueSpan.textContent = 'N/A';
	} else {
		chart = new Chart(canvas.getContext('2d'), {
		type: 'line',
		data: {
			datasets: [{
			label: name,
			borderColor: colors[0],
			backgroundColor: 'transparent',
			data: [],
			fill: false,
			cubicInterpolationMode: 'monotone',
			tension: 0.3,
			pointRadius: 0,
			borderWidth: 2,
			spanGaps: true
			}]
		},
		options: {
			animation: false,
			maintainAspectRatio: false,
			scales: {
			x: {
				type: 'linear',
				title: { display: true, text: 'T_plus (sec)' },
				min: 0,
				max: graphTimeSpanSec,
				reverse: true
			},
			y: {
				beginAtZero: false,
				title: { display: true, text: name }
			}
			},
			plugins: {
			legend: { display: false }
			}
		}
		});
  }

  	labelDiv.style.cursor = 'pointer';
	labelDiv.onclick = () => {
		if (card.classList.contains('expanded')) {
			card.classList.remove('expanded');
			if (valueSpan) valueSpan.style.display = 'block';
			if (canvas) canvas.style.display = 'none';
		} else {
			card.classList.add('expanded');
			if (valueSpan) valueSpan.style.display = 'none';
			if (canvas) canvas.style.display = 'block';
		}
	};

  	stateContainer.appendChild(card);

  	return { card, labelDiv, valueSpan, chart, data: [], expanded: false, name, isMulti, components };
}

function pruneAllBuffers() {
	const now = Date.now() / 1000;
	const cutoff = now - graphTimeSpanSec;

	for (const key in stateVars) {
		const sv = stateVars[key];
		if (sv.data.length > 0) {
		
			if (sv.isMulti) {
			for (let i=0; i < sv.chart.data.datasets.length; i++) {
			sv.chart.data.datasets[i].data = sv.chart.data.datasets[i].data.filter(p => p.x >= cutoff);
			}
		} else {
			sv.chart.data.datasets[0].data = sv.chart.data.datasets[0].data.filter(p => p.x >= cutoff);
		}
		
		sv.data = sv.data.filter(p => p.t >= cutoff);
		sv.chart.update('none');
		}
	}
}

function updateStateVar(name, t_plus, val) {
	let sv = stateVars[name];
	if (!sv) {
		const isMulti = multiComponentVars.hasOwnProperty(name);
		const components = isMulti ? multiComponentVars[name] : null;
		sv = createStateCard(name, isMulti, components);
		stateVars[name] = sv;
	}

	if (sv.isMulti && typeof val === 'object') {
		sv.components.forEach((comp, idx) => {
		const compVal = val[comp];
			if (compVal !== undefined) {
				sv.chart.data.datasets[idx].data.push({ x: t_plus, y: compVal });
			}
		});
		sv.data.push({ t: t_plus, val });
		sv.chart.options.scales.x.max = t_plus;
		sv.chart.options.scales.x.min = t_plus - graphTimeSpanSec;
		sv.chart.update('none');

		if (!sv.card.classList.contains('expanded')) {
			const vals = sv.components.map(c => `${c}: ${val[c]?.toFixed(2) ?? 'NaN'}`).join(', ');
			sv.valueSpan.textContent = vals;
		}
	} else if (name === 'pyro') {
		// TODO: fix pyro parsing
		if (val && typeof val === 'object') {
			let motorStr = pyroStates[val.motor] || `Unknown(${val.motor})`;
			let parachuteStr = pyroStates[val.parachute] || `Unknown(${val.parachute})`;
			sv.valueSpan.textContent = `Motor: ${motorStr}, Parachute: ${parachuteStr}`;
		}
	} else {
		sv.chart.data.datasets[0].data.push({ x: t_plus, y: val });
		sv.data.push({ t: t_plus, val });
		sv.chart.options.scales.x.max = t_plus;
		sv.chart.options.scales.x.min = t_plus - graphTimeSpanSec;
		sv.chart.update('none');
		if (!sv.card.classList.contains('expanded')) {
			sv.valueSpan.textContent = val.toFixed(2);
		}
	}

	pruneAllBuffers();
}

const orderedKeys = [
	'vbat', 'temperature', 'pressure',
	'acc', 'gyr', 'mag',
	'altitude', 'v_velocity', 'v_accel',
	'quat', 'tvc', 'pyro'
];

function processRocketData(obj) {
	if (!obj || typeof obj !== 'object') return;

	const t_plus = obj.T_plus;

	// TODO: json isnt being updated for some reason
	for (const key of orderedKeys) {
		if (key in obj && key !== 'T_plus') {
			const val = obj[key];
			updateStateVar(key, t_plus, val);
		}
	}
}

window.onload = initSocket;
