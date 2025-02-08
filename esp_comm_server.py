from flask import Flask, request, abort

app = Flask(__name__)

# Global settings for LED control
BLINK_MODE = 1        # 0 = none, 1 = SLOW, 2 = FAST, 3 = VALENTINE, etc.
MATRIX_MODE = 0       # 0 = mirror strips, 1 = independent matrix command
MATRIX_COMMAND = "SLOW"  # Default command for matrix when in independent mode

def get_strip_command(mode):
    """
    Maps the integer blink mode to its corresponding command string.
    """
    if mode == 2:
        return "FAST"
    elif mode == 1:
        return "SLOW"
    elif mode == 3:
        return "VALENTINE"
    elif mode == 4:
        return "CANDLE"
    elif mode == 5:
        return "PINKWAVES"
    elif mode == 6:
        return "FIGHTKAMPF"
    elif mode == 7:
        return "HEARTWAVE"
    elif mode == 8:
        return "ROMPULSE"
    elif mode == 9:
        return "CUPIDSARROW"
    else:
        return ""  # BLINK_MODE == 0 returns an empty command

@app.route('/', methods=['GET'])
def serve_command():
    """
    Main endpoint that returns a command based on the device type.
    ESP‑01 devices should poll with a URL parameter:
      ?device=strip  (for LED strip)
      ?device=matrix (for matrix panels)
      
    Optionally, a query parameter 'mode' (an integer 0–9) can update the BLINK_MODE for strips.
    """
    global BLINK_MODE, MATRIX_MODE, MATRIX_COMMAND

    # Determine which device is requesting a command.
    device = request.args.get('device', 'strip').lower()

    # If a 'mode' parameter is provided and the device is a strip, update BLINK_MODE.
    mode_param = request.args.get('mode')
    if mode_param is not None and device == 'strip':
        try:
            new_mode = int(mode_param)
            if new_mode in (0, 1, 2, 3, 4, 5, 6, 7, 8, 9):
                BLINK_MODE = new_mode
            else:
                abort(400, "Invalid mode. Use 0 (none), 1 (slow), 2 (fast), 3 (valentine), 4 (candle glow), 5 (pink waves), 6 (fightkampf), 7 (heart wave), 8 (romantic pulse), or 9 (cupids arrow).")
        except ValueError:
            abort(400, "Mode must be an integer.")

    # Select the command based on device type.
    if device == 'strip':
        command = get_strip_command(BLINK_MODE)
    elif device == 'matrix':
        # In mirror mode, the matrix follows the same command as the LED strips.
        # In independent mode, use MATRIX_COMMAND.
        if MATRIX_MODE == 0:
            command = get_strip_command(BLINK_MODE)
        else:
            command = MATRIX_COMMAND
    else:
        abort(400, "Invalid device type. Use 'strip' or 'matrix'.")

    return command + "\n", 200

@app.route('/control/matrix', methods=['POST'])
def toggle_matrix_mode():
    """
    Toggle the matrix mode between mirroring the LED strips (0) or using an independent command (1).
    """
    global MATRIX_MODE
    MATRIX_MODE = 1 - MATRIX_MODE  # Toggle between 0 and 1
    return redirect('/')

@app.route('/set-matrix', methods=['POST'])
def set_matrix_command():
    """
    Set an independent command for the matrix device. (Only used when MATRIX_MODE is 1.)
    Expects a form parameter named 'command'.
    """
    global MATRIX_COMMAND
    MATRIX_COMMAND = request.form.get('command', 'SLOW')
    return redirect('/')

# @app.route('/matrix', methods=['GET'])
# def matrix_endpoint():
#     global MATRIX_MODE, BLINK_MODE, MATRIX_COMMAND
#     if MATRIX_MODE == 0:  # Mirror strips
#         return get_strip_command(BLINK_MODE) + "\n", 200
#     else:  # Independent mode
#         return MATRIX_COMMAND + "\n", 200

# @app.route('/control/matrix', methods=['POST'])
# def toggle_matrix_mode():
#     global MATRIX_MODE
#     MATRIX_MODE = 1 - MATRIX_MODE  # Toggle between 0 and 1
#     return redirect('/')

# @app.route('/set-matrix', methods=['POST'])
# def set_matrix_command():
#     global MATRIX_COMMAND
#     MATRIX_COMMAND = request.form.get('command', 'SLOW')
#     return redirect('/')

# @app.route('/', methods=['GET'])
# def serve_blink_mode():
#     global BLINK_MODE

#     # Optional: allow updating BLINK_MODE via a URL parameter, e.g.:
#     # http://your_ip:5000/?mode=2
#     mode_param = request.args.get('mode')
#     if mode_param is not None:
#         try:
#             new_mode = int(mode_param)
#             if new_mode in (0, 1, 2, 3, 4, 5, 6, 7, 8, 9):
#                 BLINK_MODE = new_mode
#                 return f"Blink mode updated to {BLINK_MODE}\n", 200
#             else:
#                 abort(400, "Invalid mode. Use 0 (none), 1 (slow), 2 (fast), 3 (valentine), 4 (candle glow), 5 (pink waves), 6 (fightkampf [WRITTEN BY PRESTON]), 7 (heart wave), 8 (romantic pulse), or 9 (cupids arrow).")
#         except ValueError:
#             abort(400, "Mode must be an integer (0, 1, 2, 3, 4, 5, 6, 7, 8, or 9).")

    # # Respond with the command based on the current blink mode.
    # if BLINK_MODE == 2:
    #     return "FAST\n", 200
    # elif BLINK_MODE == 1:
    #     return "SLOW\n", 200
    # elif BLINK_MODE == 3:
    #     return "VALENTINE\n", 200
    # elif BLINK_MODE == 4:
    #     return "CANDLE\n", 200
    # elif BLINK_MODE == 5:
    #     return "PINKWAVES\n", 200
    # elif BLINK_MODE == 6:
    #     return "FIGHTKAMPF\n", 200
    # elif BLINK_MODE == 7:
    #     return "HEARTWAVE\n", 200
    # elif BLINK_MODE == 8:
    #     return "ROMPULSE\n", 200
    # elif BLINK_MODE == 9:
    #     return "CUPIDSARROW\n", 200
    # else:
    #     # For BLINK_MODE == 0, we return an empty response.
    #     # You could also return a specific message or a 204 status if preferred.
    #     return "", 200

if __name__ == '__main__':
    # Run the server on all interfaces so that it is accessible externally.
    app.run(host='0.0.0.0', port=5000)
