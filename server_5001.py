from flask import Flask, render_template, request, redirect
import requests

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        mode = request.form.get('mode')
        if mode in ('0', '1', '2', '3', '4', '5'):
            try:
                response = requests.get(f'http://localhost:5000/?mode={mode}')
                return render_template('index.html', message=f"Server responded: {response.text}")
            except requests.exceptions.RequestException:
                return render_template('index.html', message="Failed to connect to port 5000.")
        else:
            return render_template('index.html', message="Invalid mode. Use 0, 1, 2, 3, 4, or 5.")
    
    return render_template('index.html')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001)
