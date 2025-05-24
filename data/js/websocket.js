let socket;
let reconnectInterval;

function initWebSocket() {
    connect();
}

function connect() {
    if (socket) {
        socket.close();
    }
    
    socket = new WebSocket(`ws://${window.location.host}/ws`);
    
    socket.onopen = () => {
        addLogMessage('WebSocket connection established');
        clearInterval(reconnectInterval);
        
        requestStatus();
        requestLogHistory();
    };
    
    socket.onclose = () => {
        addLogMessage('WebSocket connection closed');
        
        reconnectInterval = setInterval(() => {
            connect();
        }, 5000);
    };
    
    socket.onerror = (error) => {
        addLogMessage('WebSocket error occurred');
        console.error('WebSocket error:', error);
    };
    
    socket.onmessage = (event) => {
        try {
            const data = JSON.parse(event.data);
            handleWebSocketMessage(data);
        } catch (error) {
            console.error('Error parsing WebSocket message:', error);
        }
    };
}

function handleWebSocketMessage(data) {
    switch (data.event) {
        case 'log_update':
            addLogMessage(data.message);
            break;
        case 'log_batch':
            if (data.messages && Array.isArray(data.messages)) {
                const logContainer = document.getElementById('log-container');
                
                logContainer.innerHTML = '';
                
                data.messages.forEach(message => {
                    addLogMessage(message, false);
                });
                
                logContainer.scrollTop = logContainer.scrollHeight;
            }
            break;
        case 'stats_update':
            // Process stats data sent from broadcastStats
            // Extract only the data fields, excluding the event name
            const statsData = {};
            Object.keys(data).forEach(key => {
                if (key !== 'event') {
                    statsData[key] = data[key];
                }
            });
            
            // Log stats updates to the SystemLog
            const statsMsg = `Stats update: Water level: ${data.water_level?.toFixed(2)}m (${(data.water_percent * 100)?.toFixed(1)}%), Distance: ${data.measured_distance?.toFixed(2)}m, Sensor: ${data.sensor_connected ? 'connected' : 'disconnected'}`;
            addLogMessage(statsMsg);
            
            // Update the UI
            updateUI(statsData);
            break;
        case 'sensor_reading':
            document.getElementById('water-level').textContent = `${data.water_level.toFixed(2)} m`;
            document.getElementById('water-percent').textContent = `${(data.water_percent * 100).toFixed(1)}%`;
            break;
    }
}

function requestStatus() {
    if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(JSON.stringify({
            event: 'request_status'
        }));
    }
}

function requestConfig() {
    if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(JSON.stringify({
            event: 'request_config'
        }));
    }
}

function requestLogHistory() {
    if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(JSON.stringify({
            event: 'request_logs'
        }));
    }
}
