import tkinter as tk
from tkinter import ttk
import serial
import threading
import time

SERIAL_PORT = 'COM2'
BAUD_RATE = 9600

class TrafficControlApp:
    def __init__(self, root):
        self.root = root
        self.root.title("VÁROSIRÁNYÍTÁSI KÖZPONT - IoT Gateway")
        self.root.geometry("500x450")
        self.root.configure(bg="#2c3e50")

        self.ser = None
        self.connected = False
        self.running = True

        tk.Label(root, text="KERESZTEZŐDÉS #100", font=("Arial", 20, "bold"), bg="#2c3e50", fg="#ecf0f1").pack(pady=20)

        # Adatok
        self.data_frame = tk.Frame(root, bg="#34495e", bd=2, relief="groove")
        self.data_frame.pack(pady=10, padx=20, fill="x")

        self.lbl_light = tk.Label(self.data_frame, text="Fényerő: --", font=("Courier", 15), bg="#34495e", fg="#f1c40f")
        self.lbl_light.pack(pady=5)

        self.lbl_status = tk.Label(self.data_frame, text="Mód: --", font=("Courier", 15), bg="#34495e", fg="#2ecc71")
        self.lbl_status.pack(pady=5)
        
        self.lbl_event = tk.Label(self.data_frame, text="Utolsó Esemény: --", font=("Arial", 10), bg="#34495e", fg="#bdc3c7")
        self.lbl_event.pack(pady=5)

        # Gombok
        btn_frame = tk.Frame(root, bg="#2c3e50")
        btn_frame.pack(pady=20)

        tk.Button(btn_frame, text="KÉNYSZERÍTETT ÉJSZAKA", command=self.set_night, bg="#c0392b", fg="white", font=("Arial", 12, "bold"), width=25).pack(pady=5)
        tk.Button(btn_frame, text="KÉNYSZERÍTETT NAPPAL", command=self.set_day, bg="#2980b9", fg="white", font=("Arial", 12, "bold"), width=25).pack(pady=5)
        tk.Button(btn_frame, text="AUTOMATA (Szenzor)", command=self.set_auto, bg="#27ae60", fg="white", font=("Arial", 12, "bold"), width=25).pack(pady=5)

        # Serial indítása
        self.connect_serial()
        
        self.thread = threading.Thread(target=self.read_serial_loop, daemon=True)
        self.thread.start()

    def connect_serial(self):
        try:
            self.ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            self.connected = True
            print(f"Sikeres csatlakozás: {SERIAL_PORT}")
        except Exception as e:
            print(f"Hiba: {e}")
            self.lbl_status.config(text="Hiba: Nincs kapcsolat", fg="red")

    def read_serial_loop(self):
        while self.running:
            if self.connected and self.ser.in_waiting > 0:
                try:
                    line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                    self.process_data(line)
                except Exception:
                    pass
            time.sleep(0.1)

    def process_data(self, line):
        if line.startswith("F:"):
            try:
                parts = line.split("|")
                feny = parts[0].split(":")[1]
                status = parts[1].split(":")[1]
                
                status_text = "AUTOMATA"
                color = "#2ecc71"
                if status == 'E':
                    status_text = "ÉJSZAKAI (Kényszerítve)"
                    color = "#e67e22"
                elif status == 'N':
                    status_text = "NAPPALI (Kényszerítve)"
                    color = "#3498db"

                self.root.after(0, lambda: self.lbl_light.config(text=f"Fényerő: {feny}"))
                self.root.after(0, lambda: self.lbl_status.config(text=f"Mód: {status_text}", fg=color))
            except: pass
            
        elif line.startswith("EVENT:"):
            msg = line.split(":")[1]
            self.root.after(0, lambda: self.lbl_event.config(text=f"Esemény: {msg}"))

    def send_command(self, cmd):
        if self.connected:
            self.ser.write(cmd)
            print(f"Küldve: {cmd}")

    def set_night(self): self.send_command(b'E')
    def set_day(self): self.send_command(b'N')
    def set_auto(self): self.send_command(b'A')

if __name__ == "__main__":
    root = tk.Tk()
    app = TrafficControlApp(root)
    root.mainloop()