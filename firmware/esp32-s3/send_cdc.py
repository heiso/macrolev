import os
import sys
import time

if len(sys.argv) < 3:
    print("Usage: python3 send_cdc.py <serial_port> <file_path>")
    sys.exit(1)

SERIAL_PORT = sys.argv[1]
FILE_PATH = sys.argv[2]
CHUNK_SIZE = 64
MARKER = b'[EOF]'

# Open serial port
fd = os.open(SERIAL_PORT, os.O_WRONLY | os.O_NOCTTY)

with open(FILE_PATH, 'rb') as f:
    while True:
        chunk = f.read(CHUNK_SIZE)
        if not chunk:
            break
        os.write(fd, chunk)
        print(f"Sent {len(chunk)} bytes")
        time.sleep(0.01)  # 10ms delay

os.write(fd, MARKER)
print("Sent marker [EOF]. Done.")
os.close(fd)
