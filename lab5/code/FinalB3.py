from flask import Flask, render_template, Response, request
import socket, time, itertools
from camera_pi import Camera
from threading import Thread

app = Flask(__name__)


server_address_1 = ('127.0.0.2', 8001)  # UDP for video
sock_1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock_1.bind(server_address_1)

server_address = ('127.0.0.1', 8004)    # TCP for control/data
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(server_address)
sock.listen(5)
connection, address = sock.accept()

info = "b0c0d0"
IP_Address = '10.227.121.126'
PORT = 8080

@app.route('/')
def index():
    if request.headers.get('accept') == 'text/event-stream':
        def events():
            for i, c in enumerate(itertools.cycle('\|/-')):
                yield "data: %s\n\n" % (info)
        return Response(events(), content_type='text/event-stream')
    return render_template('FinalB2.html')  # main D-pad

@app.route('/dpad')
def dpad():
    return render_template('FinalB3.html')  # joystick control

@app.route('/joystick')
def joystick():
    return render_template('FinalB1.html')  # joystick control

@app.route('/phone')
def phone():
    return render_template('FinalB2.html')  # phone sensor control

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


def launch_socket_server(connection):
    global info, frame
    print('Listening...')
    a='b0c0d0'
    while True:        
        info = connection.recv(6).decode("utf-8")
        print('info:', info)
        if info != a and len(info)>0:
            a = info

@app.route('/joydata', methods=['POST'])
def joydata():
    """Handles D-Pad or Joystick JSON commands"""
    if request.is_json:
        connection.send(str(request.get_json()).encode('utf-8'))
        return "OK"
    return "Unsupported"


@app.route('/phonedata',methods = ['POST', 'GET'])
def PhoneFunction():
    content_type = request.headers.get('Content-Type')
    if (content_type == 'application/json'):
        json = request.get_json()
        print("Data ", json)
        connection.send(str(json).encode('utf-8'))
        return "Content supported\n"
    else:
        return "Content not supported\n"


@app.route('/UpFunction')
def UpFunction():
    print('In UpFunction')
    cmd = 'u'
    connection.send(cmd.encode('utf-8'))  
    return "None"
    
@app.route('/LeftFunction')
def LeftFunction():
    print('In LeftFunction')
    cmd = 'a'
    connection.send(cmd.encode('utf-8'))
    return "Left pressed"

@app.route('/RightFunction')
def RightFunction():
    print('In RightFunction')
    cmd = 'd'
    connection.send(cmd.encode('utf-8'))
    return "Right pressed"

@app.route('/DownFunction')
def DownFunction():
    print('In DownFunction')
    cmd = 's'
    connection.send(cmd.encode('utf-8'))
    return "Down pressed"

@app.route('/StopFunction')
def StopFunction():
    print('In StopFunction')
    cmd = 'x'
    connection.send(cmd.encode('utf-8'))
    return "Stop pressed"


@app.after_request
def add_nav_links(response):
    """Appends navigation links to the bottom of every rendered page."""
    if response.content_type.startswith('text/html'):
        nav_html = """
        <hr>
        <p style="font-size: 150%;">
            <a href="/dpad">Main D-Pad</a> |
            <a href="/joystick">Joystick Control</a> |
            <a href="/phone">Phone Sensor Control</a>
        </p>
        """
        data = response.get_data(as_text=True)
        if "</body>" in data:
            data = data.replace("</body>", nav_html + "</body>")
            response.set_data(data)
    return response

if __name__ == "__main__":
    t = Thread(target=launch_socket_server,args=(connection,address))
    t.daemon = True
    t.start()


    app.run(debug=True, host=IP_Address, port=PORT, use_reloader=False,ssl_context='adhoc')
