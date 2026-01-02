import subprocess
import struct
import tkinter as tk
from tkinter import font as tkfont
import sys

def run_ascii_pipe(exe_path, image_path):
    return subprocess.Popen([exe_path, image_path], stdout=subprocess.PIPE)

def read_frame(proc):
    header = proc.stdout.read(8) # width and height
    if len(header) < 8:
        return None, None

    width, height = struct.unpack("II", header)
    frame = []

    for _ in range(height):
        row_chars = []
        for _ in range(width):
            len_byte = proc.stdout.read(1)
            if not len_byte:
                return None, None
            
            length = struct.unpack("B", len_byte)[0]
            utf8_bytes = proc.stdout.read(length)
            if len(utf8_bytes) < length:
                return None, None
            row_chars.append(utf8_bytes.decode("utf-8"))

        frame.append("".join(row_chars))
    return width, frame

def update_ascii_text():
    global proc, text_widget

    width, frame = read_frame(proc)
    if frame is None:
        root.after(100, update_ascii_text)
        return

    ascii_art = "\n".join(frame)

    font_size = 8

    new_font = tkfont.Font(family="Courier", size=font_size)
    
    text_widget.configure(font=new_font)
    text_widget.delete("1.0", tk.END)
    text_widget.insert(tk.END, ascii_art)

def on_click(event):
    proc.terminate()
    root.destroy()

if __name__ == "__main__":
    exe_path = "ascii.exe"
    image_path = sys.argv[1]

    proc = run_ascii_pipe(exe_path, image_path)

    root = tk.Tk()
    root.title("ASCII Art Viewer")

    root.attributes('-fullscreen', True)

    text_widget = tk.Text(root, bg="black", fg="white", wrap=tk.NONE)
    text_widget.pack(fill=tk.BOTH, expand=True)

    root.bind("<Button-1>", on_click)
    root.bind("<Escape>", lambda e: on_click(e))

    root.after(0, update_ascii_text)
    root.mainloop()
