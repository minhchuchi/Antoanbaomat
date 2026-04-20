from flask import Flask, render_template_string, request, jsonify
import requests
from Crypto.Cipher import DES
from Crypto.Util.Padding import pad
from Crypto.Random import get_random_bytes
import base64

app = Flask(__name__)

# ĐỊA CHỈ IP MÁY BẠN (Dựa vào ảnh bạn chụp)
RECEIVER_IP = "http://172.16.5.185:5000/receive"

HTML_SENDER = '''
<!DOCTYPE html>
<html>
<head>
    <title>DNU SENDER - NGƯỜI GỬI</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
</head>
<body class="bg-light p-5">
    <div class="container" style="max-width: 600px;">
        <div class="card shadow p-4">
            <h3 class="text-primary">BÊN GỬI (SENDER)</h3>
            <p class="text-muted">Gửi tới: {{ target }}</p>
            <textarea id="txt" class="form-control mb-3" rows="4" placeholder="Nhập tin nhắn bí mật..."></textarea>
            <button onclick="push()" class="btn btn-primary w-100 py-2">MÃ HÓA & GỬI NGAY</button>
        </div>
    </div>
    <script>
        async function push() {
            const val = document.getElementById('txt').value;
            const res = await fetch('/send_to_server', {
                method: 'POST',
                headers: {'Content-Type': 'application/x-www-form-urlencoded'},
                body: `msg=${encodeURIComponent(val)}`
            });
            const data = await res.json();
            alert(data.status === "success" ? "Đã gửi thành công!" : "Không kết nối được tới Receiver!");
        }
    </script>
</body>
</html>
'''

@app.route('/')
def index():
    return render_template_string(HTML_SENDER, target=RECEIVER_IP)

@app.route('/send_to_server', methods=['POST'])
def send_to_server():
    msg = request.form.get('msg', '').encode('utf-8')
    key = get_random_bytes(8)
    iv = get_random_bytes(8)
    cipher = DES.new(key, DES.MODE_CBC, iv)
    ciphertext = base64.b64encode(cipher.encrypt(pad(msg, 8))).decode('utf-8')
    
    payload = {
        "key": base64.b64encode(key).decode('utf-8'),
        "iv": base64.b64encode(iv).decode('utf-8'),
        "ciphertext": ciphertext
    }
    
    try:
        # Gửi dữ liệu qua mạng LAN sang máy Receiver
        requests.post(RECEIVER_IP, json=payload, timeout=5)
        return jsonify({"status": "success"})
    except:
        return jsonify({"status": "fail"})

if __name__ == '__main__':
    # Bạn mình chạy cục bộ trên máy họ
    app.run(port=5001)