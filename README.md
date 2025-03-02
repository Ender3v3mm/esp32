# esp32

Verificar si hay un paquete de pip en opkg:
En algunos sistemas basados en opkg puedes intentar:

#bash


opkg update && opkg install python3-pip


Si ese paquete no existe o no se instala, sigue con el siguiente paso.

Instalar pip manualmente:

Descarga el script de instalación de pip (get-pip.py) y ejecútalo con Python3. Por ejemplo:

#bash

wget https://bootstrap.pypa.io/get-pip.py

python3 get-pip.py

Esto instalará pip para Python 3. Si "wget" no está disponible, puedes usar "curl" o descargar el archivo desde otro equipo y transferirlo.

Instala la librería websocket-client:
Una vez instalado pip, ejecuta:

#bash


pip install websocket-client

O, si aún no reconoce el comando "pip", usa:

#bash

python3 -m pip install websocket-client

Con pip instalado, ya podrás ejecutar el comando de una sola línea para enviar el mensaje "UP" desde la terminal:

#bash

python3 -c "import websocket; ws = websocket.create_connection('ws://192.168.1.29/ws'); ws.send('UP'); ws.close()"
