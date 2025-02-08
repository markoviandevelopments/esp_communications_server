from flask import Flask, render_template, request, redirect
import requests

app = Flask(__name__)

MAIN_SERVER = 'http://localhost:5000'

@app.route('/', methods=['GET', 'POST'])
def index():
    message = None

    if request.method == 'POST':
        # Check if the LED strip mode form was submitted.
        if 'mode' in request.form:
            mode = request.form.get('mode')
            if mode in ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9'):
                try:
                    # Include device=strip and mode in the query string.
                    response = requests.get(f'{MAIN_SERVER}/?device=strip&mode={mode}')
                    message = f"LED Strip updated. Main server responded: {response.text.strip()}"
                except requests.exceptions.RequestException:
                    message = "Failed to connect to main server for LED strip mode."
            else:
                message = "Invalid LED strip mode. Use 0-9."

        # Check if the toggle matrix button was pressed.
        elif 'toggle_matrix' in request.form:
            try:
                response = requests.post(f'{MAIN_SERVER}/control/matrix')
                message = "Matrix mode toggled."
            except requests.exceptions.RequestException:
                message = "Failed to connect to main server for toggling matrix mode."

        # Check if the matrix command form was submitted.
        elif 'matrix_command' in request.form:
            matrix_command = request.form.get('matrix_command')
            try:
                response = requests.post(f'{MAIN_SERVER}/set-matrix', data={'command': matrix_command})
                message = f"Matrix command set to '{matrix_command}'."
            except requests.exceptions.RequestException:
                message = "Failed to connect to main server for setting matrix command."

    return render_template('index.html', message=message)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001)
