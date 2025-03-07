from flask import Flask, request, abort, render_template, redirect, url_for
import time

app = Flask(__name__)

# Store device states with last seen timestamps
devices = {
    '001': {'mode': 0, 'description': 'Main LED Array', 'port': 5000, 'last_seen': None, 'ip': None, 'color': '#FF0000'},
    '002': {'mode': 0, 'description': 'Aux Strip', 'port': 5001, 'last_seen': None, 'ip': None, 'color': '#FF0000'},
    '003': {'mode': 0, 'description': 'Cockpit Lights', 'port': 5002, 'last_seen': None, 'ip': None, 'color': '#FF0000'},
    '004': {'mode': 0, 'description': 'Engine Glow', 'port': 5003, 'last_seen': None, 'ip': None, 'color': '#FF0000'},
    '005': {'mode': 0, 'description': 'Cabin Mood', 'port': 5004, 'last_seen': None, 'ip': None, 'color': '#FF0000'},
    '006': {'mode': 0, 'description': 'Cargo Bay', 'port': 5006, 'last_seen': None, 'ip': None, 'color': '#FF0000'},
}

# Updated COMMAND_MAP to match ESP-01 effects
COMMAND_MAP = {
    0: "NONE",
    1: "SLOW",              # BabelFishSwarm
    2: "FAST",              # Rainbow
    3: "PANGLACTIC",        # PanGalacticGargleBlaster
    4: "IMPROBABILITY",     # InfiniteImprobabilityDrive
    5: "VOGONPOETRY",       # VogonPoetrySlam
    6: "GOLDTRAIL",         # HeartOfGoldTrail
    7: "FIGHTKAMPF",        # Unchanged
    8: "HEARTWAVE",         # Unchanged
    9: "BEEMATRIX",         # Unchanged
    10: "COLORPULSE",       # DontPanicPulse (with custom color)
    11: "TEXTMATRIX",       # Unchanged
    12: "TEXTMATRIXFAST",   # Unchanged
    13: "TEXTMATRIXBLUE"    # Unchanged
}

@app.template_filter('is_active')
def is_active_filter(last_seen):
    if last_seen and (time.time() - last_seen) < 60:
        return 'Active'
    return 'Inactive'

@app.route('/', methods=['GET', 'POST'])
def index():
    message = None
    if request.method == 'POST':
        selected_devices = request.form.getlist('devices')
        mode = request.form.get('mode')
        color = request.form.get('color', '').strip().upper()

        print(f"DEBUG: Received mode={mode}, color={color}, devices={selected_devices}")

        # Validate hex color
        if not (color.startswith('#') and len(color) == 7 and all(c in '0123456789ABCDEF' for c in color[1:])):
            print(f"WARNING: Invalid color received ({color}), ignoring color update.")
            color = None

        # Update devices
        if mode and mode.isdigit() and int(mode) in COMMAND_MAP:
            mode = int(mode)
            if 'all' in selected_devices:
                for dev_id in devices:
                    devices[dev_id]['mode'] = mode
                    if color:
                        devices[dev_id]['color'] = color
                    print(f"DEBUG: Updated device {dev_id} to mode={mode}, color={devices[dev_id]['color']}")
                message = f"All devices updated to {COMMAND_MAP[mode]} mode"
            else:
                updated_devices = []
                for dev_id in selected_devices:
                    if dev_id in devices:
                        devices[dev_id]['mode'] = mode
                        if color:
                            devices[dev_id]['color'] = color
                        updated_devices.append(dev_id)
                        print(f"DEBUG: Updated device {dev_id} to mode={mode}, color={devices[dev_id]['color']}")
                message = f"Updated {', '.join(updated_devices)} to {COMMAND_MAP[mode]} mode"
        else:
            message = "Invalid mode selected"

    return render_template('index.html', devices=devices, command_map=COMMAND_MAP, message=message)

@app.route('/device/<device_id>', methods=['GET'])
def get_device_command(device_id):
    if device_id not in devices:
        abort(404)

    devices[device_id]['last_seen'] = time.time()
    devices[device_id]['ip'] = request.remote_addr

    stored_color = devices[device_id]['color']
    mode = devices[device_id]['mode']

    print(f"DEBUG: Device {device_id} requested command. Mode: {mode}, Color: {stored_color}")

    if mode == 10:  # COLORPULSE with custom color
        return f"COLORPULSE:{stored_color}\n"
    return f"{COMMAND_MAP[mode]}\n"

@app.template_filter('timestamp_to_time')
def timestamp_to_time_filter(timestamp):
    from datetime import datetime
    if not timestamp:
        return 'Never'
    return datetime.fromtimestamp(timestamp).strftime('%Y-%m-%d %H:%M:%S')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=4999)