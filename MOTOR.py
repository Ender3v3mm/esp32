#!/usr/bin/env python3
import sys
import time
import websocket

if len(sys.argv) < 2:
    print("Uso: send_ws.py <mensaje>")
    sys.exit(1)

mensaje = sys.argv[1]
timeout = 30  # segundos de espera máxima

try:
    ws = websocket.create_connection("ws://192.168.1.29/ws", timeout=timeout)
except Exception as e:
    print("Error al conectar:", e)
    sys.exit(1)

ws.send(mensaje)
print("Comando enviado:", mensaje)

# Espera la respuesta "DONE"
start_time = time.time()
respuesta = None
while time.time() - start_time < timeout:
    try:
        respuesta = ws.recv()
        if respuesta == "DONE":
            print("Movimiento completado, todo correcto")
            break
    except websocket.WebSocketTimeoutException:
        print("Tiempo de espera agotado")
        break

ws.close()
