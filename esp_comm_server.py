from flask import Flask, request, abort

app = Flask(__name__)

MATRIX_MODE = 0  # 0 = mirror strips, 1 = independent
BLINK_MODE = 1  # Set your initial mode here
MATRIX_COMMAND = "SLOW"  # Default matrix command

@app.route('/matrix', methods=['GET'])
def matrix_endpoint():
    global MATRIX_MODE, BLINK_MODE, MATRIX_COMMAND
    if MATRIX_MODE == 0:  # Mirror strips
        return get_strip_command(BLINK_MODE) + "\n", 200
    else:  # Independent mode
        return MATRIX_COMMAND + "\n", 200

@app.route('/control/matrix', methods=['POST'])
def toggle_matrix_mode():
    global MATRIX_MODE
    MATRIX_MODE = 1 - MATRIX_MODE  # Toggle between 0 and 1
    return redirect('/')

@app.route('/set-matrix', methods=['POST'])
def set_matrix_command():
    global MATRIX_COMMAND
    MATRIX_COMMAND = request.form.get('command', 'SLOW')
    return redirect('/')

@app.route('/', methods=['GET'])
def serve_blink_mode():
    global BLINK_MODE

    # Optional: allow updating BLINK_MODE via a URL parameter, e.g.:
    # http://your_ip:5000/?mode=2
    mode_param = request.args.get('mode')
    if mode_param is not None:
        try:
            new_mode = int(mode_param)
            if new_mode in (0, 1, 2, 3, 4, 5, 6, 7, 8, 9):
                BLINK_MODE = new_mode
                return f"Blink mode updated to {BLINK_MODE}\n", 200
            else:
                abort(400, "Invalid mode. Use 0 (none), 1 (slow), 2 (fast), 3 (valentine), 4 (candle glow), 5 (pink waves), 6 (fightkampf [WRITTEN BY PRESTON]), 7 (heart wave), 8 (romantic pulse), or 9 (cupids arrow).")
        except ValueError:
            abort(400, "Mode must be an integer (0, 1, 2, 3, 4, 5, 6, 7, 8, or 9).")

    # Respond with the command based on the current blink mode.
    if BLINK_MODE == 2:
        return "FAST\n", 200
    elif BLINK_MODE == 1:
        return "SLOW\n", 200
    elif BLINK_MODE == 3:
        return "VALENTINE\n", 200
    elif BLINK_MODE == 4:
        return "CANDLE\n", 200
    elif BLINK_MODE == 5:
        return "PINKWAVES\n", 200
    elif BLINK_MODE == 6:
        return "FIGHTKAMPF\n", 200
    elif BLINK_MODE == 7:
        return "HEARTWAVE\n", 200
    elif BLINK_MODE == 8:
        return "ROMPULSE\n", 200
    elif BLINK_MODE == 9:
        return "CUPIDSARROW\n", 200
    else:
        # For BLINK_MODE == 0, we return an empty response.
        # You could also return a specific message or a 204 status if preferred.
        return "", 200

if __name__ == '__main__':
    # Run the server on all interfaces so that it is accessible externally.
    app.run(host='0.0.0.0', port=5000)
