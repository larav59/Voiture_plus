"""
Simulateur Marvelmind pour les tests sans matériel réel.
Ce simulateur se contente d'envoyer des paquets de données Marvelmind via un port série virtuel.
Il génère des positions en mouvement circulaire pour simuler des variations de position et d'angle.
Les paquets sont envoyés toutes les secondes.

Code basé sur le code de la librirairie Marvelmind notamment pour le calcul du CRC Modbus et la structure des paquets.
"""

import serial
import time
import struct
import math

SERIAL_PORT = '/dev/ttyACM0_PY'
BAUDRATE = 9600 

def calculate_crc_modbus(data):
	crc = 0xffff
	for byte in data:
		crc ^= byte
		for _ in range(8):
			if crc & 0x0001:
				crc = (crc >> 1) ^ 0xA001
			else:
				crc >>= 1
	return crc

def create_packet(x_cm, y_cm, angle_deg):
	# Pour trouver la justification des champs, voir le code du thread de la lib marvelmind

	# HEADER (5 bytes)
	header = b'\xff\x47\x01\x00\x10'

	# PAYLOAD (16 bytes)
	# Structure: Timestamp(4) + X(2) + Y(2) + Z(2) + Flags(1) + Addr(1) + Angle(2) + RESERVED(2)
	timestamp = int(time.time() * 1000) & 0xFFFFFFFF
	z_cm = 1500 # Hauteur OSEF 
	flags = 0x00
	address = 10 
	angle_val = int(angle_deg * 10) & 0x0FFF 

	# struct.pack permet de créer des données binaires :
	# '<' : little-endian
	# I : unsigned int (4 bytes)
	# h : short (2 bytes)
	# B : unsigned char (1 byte)
	# x : pad byte (1 byte)
	payload = struct.pack('<IhhhBBHxx', timestamp, x_cm, y_cm, z_cm, flags, address, angle_val)
	
	# CRC
	full_content = header + payload
	crc = calculate_crc_modbus(full_content)
	
	# PAQUET FINAL
	packet = full_content + struct.pack('<H', crc)
	
	return packet

def main():
	try:
		ser = serial.Serial(SERIAL_PORT, BAUDRATE)
		print(f"Simulateur Marvelmind {SERIAL_PORT}")
		print("Envoi de données... (Ctrl+C pour arrêter)")
		
		t = 0
		while True:
			radius = 200 
			
			x = int(radius * math.cos(t))
			y = int(radius * math.sin(t))
			angle = (t * 180 / math.pi) % 360
			
			packet = create_packet(x, y, angle)
			ser.write(packet)
			
			print(f"Envoyé: X={x} cm, Y={y} cm, Ang={angle:.1f}°")

			t += 0.1
			time.sleep(1.0) # Envoi toutes les secondes
			
	except KeyboardInterrupt:
		print("\nArrêt.")
	except Exception as e:
		print(f"Erreur: {e}")

if __name__ == "__main__":
	main()