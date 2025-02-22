import serial
import time

# Configuração da serial
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)

# Função para ler as temperaturas das zonas térmicas
def read_thermal_zones():
    temperatures = []
    for i in range(4):  # Supondo 4 zonas térmicas
        with open(f"/sys/class/thermal/thermal_zone{i}/temp", "r") as file:
            temp = int(file.read()) / 1000  # Converter para Celsius
            temperatures.append(temp)
    return temperatures

# Loop para enviar dados
while True:
    temps = read_thermal_zones()
    ser.write((",".join(map(str, temps)) + "\n"))  # Enviar como string
    time.sleep(2)