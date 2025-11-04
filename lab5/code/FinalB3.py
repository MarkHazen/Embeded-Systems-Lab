from flask import Flask, render_template, Response, request
import socket, time, itertools
from camera_pi import Camera
from threading import Thread

app = Flask(__name__)


server_address_1 = ('127.0.0.2', 8001)  # UDP for video
sock_1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock_1.bind(server_address_1)

server_address = ('127.0.0.1', 8000)    # TCP for control/data
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(server_address)
sock.listen(5)
connection, address = sock.accept()

info = "b0c0d0"
IP_Address = '10.227.96.142'
PORT = 8080




@app.route('/')
def index():
    """Main D-Pad page"""
    if request.headers.get('accept') == 'text/event-stream':
        def events():
            global info
            while True:
                yield f"data: {info}\n\n"
                time.sleep(0.1)
        return Response(events(), content_type='text/event-stream')
    return render_template('index.html')


@app.route('/joystick')
def joystick():
    return render_template('joystick.html')


@app.route('/phone')
def phone():
    return render_template('phone.html')


def gen(camera):
    max_len = 65507
    while True:
        frame, _ = sock_1.recvfrom(max_len)
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')


@app.route('/video_feed')
def video_feed():
    return Response(gen(Camera()),
                    mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/joydata', methods=['POST'])
def joydata():
    """Handles D-Pad or Joystick JSON commands"""
    if request.is_json:
        connection.send(str(request.get_json()).encode('utf-8'))
        return "OK"
    return "Unsupported"


@app.route('/phonedata', methods=['POST'])
def phonedata():
    """Handles phone orientation JSON"""
    if request.is_json:
        data = request.get_json()
        connection.send(str(data).encode('utf-8'))
        return "OK"
    return "Unsupported"


def socket_listener(connection):
    global info
    prev = "b0c0d0"
    while True:
        new = connection.recv(6).decode("utf-8")
        if new and new != prev:
            info = new
            prev = new


if __name__ == "__main__":
    Thread(target=socket_listener, args=(connection,), daemon=True).start()
    app.run(host=IP_Address, port=PORT, debug=True,
            use_reloader=False, ssl_context='adhoc')
