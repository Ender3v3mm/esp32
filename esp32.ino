#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AccelStepper.h>

// ===== Configuración de pines para el motor =====
#define STEP_PIN   14
#define DIR_PIN    12
#define ENABLE_PIN 27

// WiFi
const char* ssid = "TU WIFI";
const char* password = "TU CONTRASEÑA";

// Crear servidor web en el puerto 80 y un WebSocket en "/ws"
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Crear objeto para el motor usando AccelStepper
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
bool motorEnabled = false;

// Valor por defecto de movimiento en pasos
const int defaultDistance = 2000;

// Función para extraer el valor numérico de un mensaje
int extractDistance(String msg) {
  int spaceIdx = msg.indexOf(' ');
  if (spaceIdx != -1) {
    String param = msg.substring(spaceIdx + 1);
    int dist = param.toInt();
    if (dist != 0)
      return dist;
  }
  return defaultDistance;
}

// Función para manejar eventos del WebSocket
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    String msg = "";
    for (size_t i = 0; i < len; i++) {
      msg += (char)data[i];
    }
    Serial.printf("Mensaje WS recibido: %s\n", msg.c_str());
    
    // Interpretar comandos
    if (msg.startsWith("UP")) {
      int distance = extractDistance(msg);
      digitalWrite(ENABLE_PIN, LOW); // Activa el motor
      motorEnabled = true;
      stepper.moveTo(stepper.currentPosition() + distance);
    } else if (msg.startsWith("DOWN")) {
      int distance = extractDistance(msg);
      digitalWrite(ENABLE_PIN, LOW);
      motorEnabled = true;
      stepper.moveTo(stepper.currentPosition() - distance);
    } else if (msg.startsWith("STOP")) {
      stepper.stop();
      digitalWrite(ENABLE_PIN, HIGH);
      motorEnabled = false;
      ws.textAll("DONE");
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configurar pin ENABLE (LOW activa el driver)
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH); // Motor deshabilitado inicialmente
  
  // Configurar motor
  stepper.setMaxSpeed(2000);
  stepper.setAcceleration(750);
  
  // Conectar a WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
  Serial.println(WiFi.localIP());
  
  // Configurar WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  // Servir página web básica
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", 
      "<html><head><title>Control de Motor</title></head><body>"
      "<h1>Control de Motor Nema 17</h1>"
      "<p>Ejemplos: 'UP 2500' o 'DOWN 1500'</p>"
      "<button onclick=\"ws.send('UP')\">Subir (default)</button> "
      "<button onclick=\"ws.send('DOWN')\">Bajar (default)</button> "
      "<button onclick=\"ws.send('STOP')\">Parar</button>"
      "<script>var ws=new WebSocket('ws://'+location.hostname+'/ws');</script>"
      "</body></html>");
  });
  
  server.begin();
}

void loop() {
  // Ejecuta el movimiento del motor si está activo
  if (motorEnabled) {
    stepper.run();
    // Cuando se alcanza la posición, deshabilitar motor y notificar
    if (stepper.distanceToGo() == 0) {
      digitalWrite(ENABLE_PIN, HIGH);
      motorEnabled = false;
      Serial.println("Movimiento completado, motor desactivado");
      ws.textAll("DONE");
    }
  }
}
