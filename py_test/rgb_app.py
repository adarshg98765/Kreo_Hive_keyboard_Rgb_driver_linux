import tkinter as tk
from tkinter import colorchooser, messagebox
import subprocess


SYSFS_FILE_PATH = "/sys/bus/hid/devices/0003:320F:5055.0003/rgb_color"

#change file path based on vendorid:productid(use lsusb command)

class RGBControlApp:
    def __init__(self, root):
        """Initializes the GUI application."""
        self.root = root
        self.root.title("Keyboard RGB Control")
        self.root.geometry("350x150")
        self.root.resizable(False, False)

        self.color_preview = tk.Label(
            root,
            text="Current Color",
            bg="black",  
            fg="white",
            width=20,
            height=3,
            relief="sunken",
            borderwidth=2
        )
        self.color_preview.pack(pady=10)

        self.choose_button = tk.Button(
            root,
            text="Choose Color",
            command=self.select_color
        )
        self.choose_button.pack(pady=10)

        self.last_color_rgb = (0, 0, 0)

    def select_color(self):
        """Opens the color palette and triggers the update."""

        color_data = colorchooser.askcolor(
            initialcolor=self.rgb_to_hex(self.last_color_rgb)
        )

        if color_data and color_data[0]:
            rgb_tuple = color_data[0]
            hex_string = color_data[1]

            self.update_device_color(rgb_tuple)
            
            self.update_gui_preview(hex_string, rgb_tuple)

    def update_gui_preview(self, hex_color, rgb_tuple):
        """Updates the color preview box in the GUI."""
        self.color_preview.config(background=hex_color)
        brightness = (rgb_tuple[0] * 299 + rgb_tuple[1] * 587 + rgb_tuple[2] * 114) / 1000
        text_color = "white" if brightness < 128 else "black"
        self.color_preview.config(fg=text_color)

    def update_device_color(self, rgb_tuple):
        """
        Formats the RGB values and writes them to the sysfs file
        using 'sudo tee' to handle permissions correctly.
        """
        r, g, b = int(rgb_tuple[0]), int(rgb_tuple[1]), int(rgb_tuple[2])
        
        rgb_string = f"{r},{g},{b}"
        
        print(f"Attempting to write '{rgb_string}' to {SYSFS_FILE_PATH}")

        command = ['sudo', 'tee', SYSFS_FILE_PATH]

        try:
            result = subprocess.run(
                command,
                input=rgb_string,
                text=True,
                check=True,
                capture_output=True 
            )
            print("Successfully updated device color.")
            self.last_color_rgb = (r, g, b)
            
        except FileNotFoundError:
            error_title = "Command Not Found"
            error_message = (
                f"Error: Could not find the 'sudo' or 'tee' command.\n"
                "Please ensure these standard utilities are installed and in your PATH."
            )
            messagebox.showerror(error_title, error_message)
            
        except subprocess.CalledProcessError as e:
            error_title = "Permission or Driver Error"
            error_message = (
                f"Failed to write to the device file.\n\n"
                f"File: {SYSFS_FILE_PATH}\n"
                f"Error: {e.stderr.strip()}\n\n"
                "Possible reasons:\n"
                "1. You entered the wrong password for 'sudo'.\n"
                "2. The kernel driver is not loaded or has crashed.\n"
                "3. The file path is incorrect."
            )
            messagebox.showerror(error_title, error_message)

    def rgb_to_hex(self, rgb):
        """Converts an (r, g, b) tuple to a #rrggbb hex string."""
        return f'#{int(rgb[0]):02x}{int(rgb[1]):02x}{int(rgb[2]):02x}'


if __name__ == "__main__":
    main_window = tk.Tk()
    app = RGBControlApp(main_window)
    main_window.mainloop()