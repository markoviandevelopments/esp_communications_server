from flask import Flask, request, abort, render_template, redirect, url_for

app = Flask(__name__)

# Store modes for all ESP devices and their descriptions
devices = {
    '001': {'mode': 0, 'description': 'LED Array', 'status': 'inactive', 'port': 5000},
    '002': {'mode': 0, 'description': 'Matrix Panel', 'status': 'inactive', 'port': 5001},
    # Add more devices as needed
}

COMMAND_MAP = {
    0: "NONE",
    1: "SLOW",
    2: "FAST",
    3: "VALENTINE",
    4: "CANDLE",
    5: "PINKWAVES",
    6: "FIGHTKAMPF",
    7: "HEARTWAVE",
    8: "ROMPULSE",
    9: "CUPIDSARROW"
}

@app.route('/', methods=['GET', 'POST'])
def index():
    message = None
    if request.method == 'POST':
        selected_devices = request.form.getlist('devices')
        mode = request.form.get('mode')
        
        if not mode.isdigit() or int(mode) not in COMMAND_MAP:
            message = "Invalid mode selection"
        else:
            mode = int(mode)
            if 'all' in selected_devices:
                for dev in devices:
                    devices[dev]['mode'] = mode
                message = f"All devices updated to {COMMAND_MAP[mode]} mode"
            else:
                updated_devices = []
                for dev_id in selected_devices:
                    if dev_id in devices:
                        devices[dev_id]['mode'] = mode
                        updated_devices.append(dev_id)
                message = f"Updated {', '.join(updated_devices)} to {COMMAND_MAP[mode]} mode"
    
    return render_template('index.html', 
                         devices=devices,
                         command_map=COMMAND_MAP,
                         message=message)

@app.route('/device/<device_id>', methods=['GET'])
def get_device_command(device_id):
    if device_id not in devices:
        abort(404)
    return f"{COMMAND_MAP[devices[device_id]['mode']]}\n"

@app.route('/update_status/<device_id>/<status>', methods=['POST'])
def update_status(device_id, status):
    if device_id in devices and status in ['active', 'inactive']:
        devices[device_id]['status'] = status
        return f"Status updated for {device_id}"
    abort(400)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=4999)