import json
import serial
import tkinter as tk
from tkinter import filedialog, messagebox

class Configurator(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("ESP32 Macropad Configurator")
        self.geometry("400x300")
        tk.Label(self, text="Serial Port:").pack()
        self.port_entry = tk.Entry(self)
        self.port_entry.pack()
        tk.Button(self, text="Load JSON", command=self.load_json).pack()
        tk.Button(self, text="Send to Device", command=self.send).pack()
        self.text = tk.Text(self)
        self.text.pack(expand=True, fill="both")
        self.json_data = None

    def load_json(self):
        path = filedialog.askopenfilename(filetypes=[("JSON", "*.json")])
        if not path:
            return
        with open(path, 'r') as f:
            self.json_data = f.read()
        self.text.delete('1.0', tk.END)
        self.text.insert(tk.END, self.json_data)

    def send(self):
        if not self.json_data:
            self.json_data = self.text.get('1.0', tk.END)
        port = self.port_entry.get()
        if not port:
            messagebox.showerror("Error", "Specify serial port")
            return
        try:
            with serial.Serial(port, 115200, timeout=3) as ser:
                ser.write(self.json_data.encode('utf-8') + b'\n')
            messagebox.showinfo("Done", "Mapping sent")
        except Exception as e:
            messagebox.showerror("Error", str(e))

if __name__ == "__main__":
    app = Configurator()
    app.mainloop()
