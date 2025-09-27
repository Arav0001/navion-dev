let socket = null;

const chartBuffer = [];
const bufferMaxSeconds = 3;
const bufferMaxLength = bufferMaxSeconds * 20; // 20Hz
const timeWindowSeconds = 3;

const chartConfigs = {};
const chartInstances = {};
const latestValues = {};

function initSocket() {
	socket = new WebSocket(`ws://${location.host}/socket`);

	socket.onopen = () => {
		console.log("WebSocket connected");
	};

	socket.onmessage = (event) => {
		try {
			const data = JSON.parse(event.data);
			processRocketData(data);
		} catch (e) {
			console.warn("Invalid JSON:", e);
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
		if (command === 'arm') {
			const armButton = document.getElementById('arm-button');
	
			armButton.textContent = 'ARMED';
			armButton.classList.add('armed');
		}
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

const orderedKeys = [
	'vbat', 'temperature', 'pressure',
	'acc', 'gyr', 'mag',
	'altitude', 'v_velocity', 'v_accel',
	'quat', 'tvc', 'pyro'
];

const units = {
    vbat: 'V',
    temperature: '°C',
    pressure: 'kPa',
    acc: 'm/s²',
    gyr: 'rad/s',
    mag: 'μT',
    altitude: 'm',
    v_velocity: 'm/s',
    v_accel: 'm/s²',
    quat: '',
    tvc: 'deg',
    pyro: ''
};

const prettyNames = {
    vbat: 'Battery Voltage',
    temperature: 'Temperature',
    pressure: 'Pressure',
    acc: 'Accelerometer',
    gyr: 'Gyroscope',
    mag: 'Magnotometer',
    altitude: 'Altitude',
    v_velocity: 'Vertical Velocity',
    v_accel: 'Vertical Acceleration',
    quat: 'Quaternion',
    tvc: 'TVC Angles',
    pyro: 'Pyro State'
};

function processRocketData(obj) {
    if (!obj || typeof obj !== 'object') return;

    chartBuffer.push(obj);
    if (chartBuffer.length > bufferMaxLength) chartBuffer.shift();

    chartBuffer.sort((a, b) => a.T_plus - b.T_plus);

    updateAllCharts();
    updatePyroLabel();
    updateFlightEvents(obj);
}

function updateChartForKey(key) {
    const chart = chartInstances[key];
    if (!chart) return;

    let labels = [];
    let series = [];
    let fields = [];
    if (["acc","gyr","mag","quat","tvc"].includes(key)) {
        if (key === "quat") fields = ["w","x","y","z"];
        else if (key === "tvc") fields = ["x","y"];
        else fields = ["x","y","z"];
        series = fields.map(() => []);
    } else {
        series = [[]];
    }

    let buffer = chartBuffer;
    if (buffer.length > bufferMaxLength) {
        buffer = buffer.slice(buffer.length - bufferMaxLength);
    }
    while (buffer.length < bufferMaxLength) {
        buffer.unshift(null);
    }
    for (const entry of buffer) {
        if (entry) {
            labels.push(entry.T_plus);
            if (["acc","gyr","mag","quat","tvc"].includes(key)) {
                fields.forEach((f, idx) => {
                    series[idx].push(entry[key][f]);
                });
            } else if (key === "pressure") {
                series[0].push(entry[key] / 1000); // convert to kPa
            } else {
                series[0].push(entry[key]);
            }
        } else {
            labels.push(null);
            if (["acc","gyr","mag","quat","tvc"].includes(key)) {
                fields.forEach((f, idx) => {
                    series[idx].push(null);
                });
            } else {
                series[0].push(null);
            }
        }
    }

	chart.data.labels = labels;
	chart.data.datasets.forEach((ds, idx) => {
		ds.data = series[idx];
	});

	const lastEntry = buffer.filter(e => e).slice(-1)[0];
	if (lastEntry && chart.options && chart.options.scales && chart.options.scales.x) {
		chart.options.scales.x.min = lastEntry.T_plus - 3;
		chart.options.scales.x.max = lastEntry.T_plus;
	}

	if (chart.options && chart.options.scales && chart.options.scales.y) {
		let allValues = [];
		chart.data.datasets.forEach(ds => {
			allValues = allValues.concat(ds.data.filter(v => typeof v === 'number' && !isNaN(v)));
		});
		if (allValues.length > 0) {
			let min = Math.min(...allValues);
			let max = Math.max(...allValues);
			let mean = allValues.reduce((a, b) => a + b, 0) / allValues.length;
			let range = max - min;
			if (range < 1) {
				min = mean - 0.5;
				max = mean + 0.5;
			}
			chart.options.scales.y.min = min;
			chart.options.scales.y.max = max;
		} else {
			chart.options.scales.y.min = undefined;
			chart.options.scales.y.max = undefined;
		}
	}
	chart.update('none');

    let latest = buffer.length ? buffer[buffer.length - 1][key] : null;
    let latestText = '';
    if (["acc","gyr","mag","quat","tvc"].includes(key)) {
        latestText = fields.map(f => `${f}: ${latest ? latest[f].toFixed(2) : '---'}`).join(', ');
        if (units[key]) latestText += ` ${units[key]}`;
    } else if (key === "pressure") {
        latestText = latest !== undefined ? (latest / 1000).toFixed(2) : '---';
        latestText += ' kPa';
    } else {
        latestText = latest !== undefined ? latest.toFixed(2) : '---';
        if (units[key]) latestText += ` ${units[key]}`;
    }
    document.getElementById(`latest-${key}`).textContent = latestText;
}

function setupChartsUI() {
	const container = document.getElementById('charts-container');
	container.innerHTML = '';

	let row = null;
	let chartsInRow = 0;
	for (let i = 0; i < orderedKeys.length; i++) {
		if (chartsInRow === 0) {
			row = document.createElement('div');
			row.className = 'chart-row';
		}
		const key = orderedKeys[i];
		const chartWrapper = document.createElement('div');
		chartWrapper.className = 'chart-wrapper';

		// Chart title
		const title = document.createElement('div');
		title.className = 'chart-title';
		if (key === 'pyro') {
			title.textContent = prettyNames[key];
		} else {
			title.textContent = `${prettyNames[key]}${units[key] ? ' (' + units[key] + ')' : ''}`;
		}
		chartWrapper.appendChild(title);

		if (key !== 'pyro') {
			const canvas = document.createElement('canvas');
			canvas.id = `chart-${key}`;
			chartWrapper.appendChild(canvas);
		} else {
			const pyroLabel = document.createElement('div');
			pyroLabel.id = 'pyro-state-label';
			pyroLabel.style.fontSize = '1.5rem';
			pyroLabel.style.marginTop = '2rem';
			pyroLabel.style.color = '#ffce56';
			chartWrapper.appendChild(pyroLabel);
		}

		const valueDiv = document.createElement('div');
		valueDiv.id = `latest-${key}`;
		valueDiv.style.margin = '8px 0 0 0';
		valueDiv.style.color = '#ffce56';
		valueDiv.style.fontSize = '1rem';
		valueDiv.style.fontWeight = 'bold';
		valueDiv.style.alignSelf = 'flex-end';
		chartWrapper.appendChild(valueDiv);

		row.appendChild(chartWrapper);
		chartsInRow++;
		
		if (chartsInRow === 3 || i === orderedKeys.length - 1) {
			container.appendChild(row);
			chartsInRow = 0;
		}
	}

	for (const key of orderedKeys) {
		if (key === 'pyro') continue;
		chartConfigs[key] = getChartConfigForKey(key);
		const ctx = document.getElementById(`chart-${key}`).getContext('2d');
		chartInstances[key] = new Chart(ctx, chartConfigs[key]);
	}
}

function getChartConfigForKey(key) {
	let labels = [];
	let datasets = [];
	let colors = [
		'#ff6384', '#36a2eb', '#cc65fe', '#ffce56', '#2ecc40', '#e67e22', '#34495e', '#8e44ad'
	];
	let colorIdx = 0;

	if (["acc","gyr","mag","quat","tvc"].includes(key)) {
		let fields = [];
		if (key === "quat") fields = ["w","x","y","z"];
		else if (key === "tvc") fields = ["x","y"];
		else fields = ["x","y","z"];
		datasets = fields.map(f => ({
			label: f,
			data: [],
			borderColor: colors[colorIdx++ % colors.length],
			fill: false,
			pointRadius: 0,
			tension: 0.2
		}));
	} else {
		datasets = [{
			label: key,
			data: [],
			borderColor: colors[colorIdx++ % colors.length],
			fill: false,
			pointRadius: 0,
			tension: 0.2
		}];
	}

	return {
		type: 'line',
		data: {
			labels: [],
			datasets: datasets
		},
		options: {
			responsive: true,
			plugins: {
				legend: { display: true },
				tooltip: { enabled: true, mode: 'nearest', intersect: false },
			},
			scales: {
				x: {
					type: 'linear',
					title: { display: true, text: 'T+ (s)' },
					ticks: { color: '#fff' }
				},
				y: {
					title: { display: true, text: key },
					ticks: { color: '#fff', stepSize: 0.1 },
					beginAtZero: false,
					min: undefined,
					max: undefined
				}
			}
		}
	};
}

function updateAllCharts() {
	for (const key of orderedKeys) {
		if (key === 'pyro') continue;
		updateChartForKey(key);
	}
}

function updatePyroLabel() {
	const buffer = chartBuffer.slice(-bufferMaxLength);
	let latest = buffer.length ? buffer[buffer.length - 1].pyro : null;
	let stateMap = ['STANDBY', 'FIRING', 'CONFIRMED', 'TIMEOUT', 'BROKEN'];
	let text = '';
	if (latest) {
		text = `Motor: ${stateMap[latest.motor] || latest.motor}, Parachute: ${stateMap[latest.parachute] || latest.parachute}`;
	} else {
		text = 'Motor: --, Parachute: --';
	}
	document.getElementById('pyro-state-label').textContent = text;
}

function updateFlightEvents(data) {
	if (!data || !data.flags) return;
	
	const flags = data.flags;
	console.log(flags);
	const flagNames = ['armed', 'ignition', 'apogee', 'touchdown'];
	
	flagNames.forEach(flagName => {
		const circle = document.getElementById(`status-${flagName}`);
		if (circle) {
			if (flags[flagName]) {
				circle.classList.remove('inactive');
				circle.classList.add('active');
			} else {
				circle.classList.remove('active');
				circle.classList.add('inactive');
			}
		}
	});
}

window.onload = () => {
    initSocket();
    setupChartsUI();
};
