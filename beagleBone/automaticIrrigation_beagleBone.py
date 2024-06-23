import Adafruit_BBIO.GPIO as GPIO
import Adafruit_BBIO.ADC as ADC
import time


umiAnalog = "P9_39"
solenoid_pin = "P9_15"
 
limite_umidade_superior = 50
limite_umidade_inferior = 40

GPIO.setup(solenoid_pin, GPIO.OUT)
GPIO.output(solenoid_pin, GPIO.HIGH)

ADC.setup()

print("Inicializando sistema de irrigacao automatizado")

while True:
    # Tarefa 1: Leitura do sensor de umidade
    umiR = ADC.read_raw(umiAnalog)
    print("umiR: ", umiR)
    umidade_medida = int((100 * (4095 - umiR) / (4095 - 2450)))
    
    # Tarefa 2: Controle da valvula solenoide
    if umidade_medida < limite_umidade_inferior and GPIO.input(solenoid_pin) == GPIO.HIGH:
        GPIO.output(solenoid_pin, GPIO.LOW)
        print("Ligando solenoide")
        time.sleep(5)
    elif umidade_medida >= limite_umidade_superior and GPIO.input(solenoid_pin) == GPIO.LOW:
        GPIO.output(solenoid_pin, GPIO.HIGH)
        print("Desligando solenoide")

    print("UMIDADE MEDIDA: ", umidade_medida)


    # Delay para proxima medicao
    time.sleep(0.2)