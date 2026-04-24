#
# file: live_dashboard.py
# Runs a live GUI that displays the current day's solar data
#

import tkinter as tk
from tkinter import ttk
import sqlite3
import datetime as dt
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.ticker import MaxNLocator

class DatabaseApp:
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.root.geometry("800x550")
        self.root.title("Solar Dashboard")
        

        self.offset = dt.timezone(dt.timedelta(hours=-4))

        self.conn = sqlite3.connect("solar.db")
        self.conn.row_factory = sqlite3.Row
        self.cursor = self.conn.cursor()

        self.boldFont = ("Helvetica", 11, "bold")

        tk.Label(root, text="Today's Power Data", font=self.boldFont).place(relx=0.15)

        ttk.Separator(root, orient='vertical').place(relx=0.5, rely=0, relwidth=0.2, relheight=1)
        ttk.Separator(root, orient='horizontal').place(relx=0.51, rely=0.5, relheight=0.2, relwidth=0.485)

        tk.Label(root, text="Static", anchor='center', font=self.boldFont).place(relx=0.625, rely=0.015, anchor='center')
        
        self.staticPowerLabel = tk.Label(root, text="_ W", anchor='center')
        self.staticPowerLabel.place(relx=0.625, rely=0.1, anchor='center')

        self.staticTempLabel = tk.Label(root, text="Temp: _°", anchor='center')
        self.staticTempLabel.place(relx=0.625, rely=0.15, anchor='center')

        self.staticHumidityLabel = tk.Label(root, text="Humidity: _%", anchor='center')
        self.staticHumidityLabel.place(relx=0.625, rely=0.2, anchor='center')

        self.staticAvgLDRLabel = tk.Label(root, text="Avg LDR: _ lx", anchor='center')
        self.staticAvgLDRLabel.place(relx=0.625, rely=0.25, anchor='center')

        ttk.Separator(root, orient='vertical').place(relx=0.75, rely=0, relwidth=0.2, relheight=0.485)

        tk.Label(root, text="Dynamic", anchor='center', font=self.boldFont).place(relx=0.875, rely=0.015, anchor='center')

        tk.Label(root, text="Generated", font=self.boldFont).place(relx=0.8125, rely=0.1, anchor='center')
        tk.Label(root, text="Consumed", font=self.boldFont).place(relx=0.9425, rely=0.1, anchor='center')

        self.dynamicPowerGenLabel = tk.Label(root, text="_ W", anchor='center')
        self.dynamicPowerGenLabel.place(relx=0.8125, rely=0.15, anchor='center')

        ttk.Separator(root, orient='horizontal').place(relx=0.86, rely=0.1, relheight=0.2, width=25)
        ttk.Separator(root, orient='vertical').place(relx=0.875, rely=0.11, height=40, relwidth=0.2)

        self.dynamicPowerConLabel = tk.Label(root, text="_ W", anchor='center')
        self.dynamicPowerConLabel.place(relx=0.9375, rely=0.15, anchor='center')

        tk.Label(root, text='Total', anchor='center', font=self.boldFont).place(relx=0.875, rely=0.2, anchor='center')
        self.dynamicPowerTotLabel = tk.Label(root, text="_ W", anchor='center')
        self.dynamicPowerTotLabel.place(relx=0.875, rely=0.24, anchor='center')

        self.dynamicTempLabel = tk.Label(root, text="Temp: _°", anchor='center')
        self.dynamicTempLabel.place(relx=0.875, rely=0.29, anchor='center')

        self.dynamicHumidityLabel = tk.Label(root, text="Humidity: _%", anchor='center')
        self.dynamicHumidityLabel.place(relx=0.875, rely=0.34, anchor='center')

        self.dynamicAvgLDRLabel = tk.Label(root, text="Avg LDR: _ lx", anchor='center')
        self.dynamicAvgLDRLabel.place(relx=0.875, rely=0.39, anchor='center')

        tk.Label(root, text="Log", anchor='center', font=self.boldFont).place(relx=0.75, rely=0.52, anchor='center')
        
        scrollbar = tk.Scrollbar(root, orient=tk.VERTICAL)
        scrollbar.place(relx=0.95, rely=0.55, height=220)
        scrollbarH = tk.Scrollbar(root, orient=tk.HORIZONTAL)
        scrollbarH.place(relx=0.51, rely=0.975, width=350)
        self.loggerListbox = tk.Listbox(root, yscrollcommand=scrollbar.set, xscrollcommand=scrollbarH.set)
        self.loggerListbox.place(relx=0.51, rely=0.55, height=220, width=350)
        scrollbar.config(command=self.loggerListbox.yview)
        scrollbarH.config(command=self.loggerListbox.xview)

        self.fig, (self.static_ax, self.dynamic_ax) = plt.subplots(nrows=2, ncols=1)
        self.canvas = FigureCanvasTkAgg(self.fig, master=root)
        self.canvas.draw()

        self.canvas.get_tk_widget().place(relx=0.01, rely=0.05, width=380)

        self.updateFuncId = ""
        self.updateGraphFuncId = ""
        

    def __del__(self):
        self.conn.close()

    def on_closing(self):
        root.quit()
        root.destroy()

    def getRowsFromToday(self, table):
         # Start of today (local time)
        start = dt.datetime.combine(dt.datetime.today(), dt.datetime.min.time())
        start_ts = int(start.timestamp())

        # Start of tomorrow
        end = dt.datetime.combine(dt.datetime.today(), dt.datetime.max.time())
        end_ts = int(end.timestamp())

        self.cursor.execute(f"""
            SELECT *
            FROM {table}
            WHERE timestamp >= {start_ts} AND timestamp <= {end_ts}
            """
        )
        return self.cursor.fetchall()

    def update_graphs(self):
        self.dynamic_ax.clear()
        self.static_ax.clear()

        start = dt.datetime.combine(dt.datetime.today(), dt.datetime.min.time())
        start_ts = int(start.timestamp())
        bucket_minutes = 15

        self.cursor.execute(f"""
            SELECT 
                (timestamp / {bucket_minutes * 60}) * {bucket_minutes * 60} as bin_start,
                AVG(panelV * panelA) as avg_power
            FROM static_data
            WHERE timestamp >= {start_ts}
            GROUP BY bin_start
            ORDER BY bin_start
            """
        )
        static_rows = self.cursor.fetchall()

        self.cursor.execute(f"""
            SELECT 
                (timestamp / {bucket_minutes * 60}) * {bucket_minutes * 60} as bin_start,
                AVG(panelV * panelA - systemV * systemA) as avg_power
            FROM dynamic_data
            WHERE timestamp >= {start_ts}
            GROUP BY bin_start
            ORDER BY bin_start
            """
        )
        dynamic_rows = self.cursor.fetchall()

        x = []
        y = []
        threshold = 1e-7 # Sometimes floating point calculations mess up
        for row in static_rows:
            time = dt.datetime.fromtimestamp(row[0])
            x.append(f"{time.hour}:{time.minute}")
            power = row[1]
            y.append(0.0 if 0 < abs(power) < threshold else power)

        self.static_ax.bar(x, y)
        self.static_ax.set_ylim(-2, 20)
        self.static_ax.set_xlabel("Time")
        self.static_ax.set_ylabel("Power (W)")
        self.static_ax.tick_params(axis='x', labelrotation=45)
        self.static_ax.set_title("Static")
        self.static_ax.xaxis.set_major_locator(MaxNLocator(nbins=10)) 

        x = []
        y = []
        threshold = 1e-7 # Sometimes floating point calculations mess up
        for row in dynamic_rows:
            time = dt.datetime.fromtimestamp(row[0])
            x.append(f"{time.hour}:{time.minute}")
            power = row[1]
            y.append(0.0 if 0 < abs(power) < threshold else power)

        self.dynamic_ax.bar(x, y)
        self.dynamic_ax.set_ylim(-2, 20)
        self.dynamic_ax.set_xlabel("Time")
        self.dynamic_ax.set_ylabel("Power (W)")
        self.dynamic_ax.tick_params(axis='x', labelrotation=45)
        self.dynamic_ax.set_title("Dynamic")
        self.dynamic_ax.xaxis.set_major_locator(MaxNLocator(nbins=10)) 

        self.fig.tight_layout()
        self.canvas.draw()

        try:
            self.root.after(60000, self.update_graphs)
        except:
            pass

    def update(self):
        try:
            self.cursor.execute("SELECT * FROM static_data ORDER BY timestamp DESC LIMIT 1")
            data = self.cursor.fetchone()

            self.staticPowerLabel.config(text=f"{(data['panelV'] * data['panelA']):.2f} W")
            self.staticTempLabel.config(text=f"Temp: {data['temp']}°")
            self.staticHumidityLabel.config(text=f"Humidity: {data['humidity']}%")
            avg = (data['tl_ldr'] + data['tr_ldr'] + data['bl_ldr'] + data['br_ldr']) / 4
            self.staticAvgLDRLabel.config(text=f"Avg LDR: {avg:.2f} lx")
        except:
            pass

        try:
            self.cursor.execute("SELECT * FROM dynamic_data ORDER BY timestamp DESC LIMIT 1")
            data = self.cursor.fetchone()

            gen = (data['panelV'] * data['panelA'])
            con = (data['systemV'] * data['systemA'])
            self.dynamicPowerGenLabel.config(text=f"{gen:.2f} W")
            self.dynamicPowerConLabel.config(text=f"{con:.2f} W")
            self.dynamicPowerTotLabel.config(text=f"{(gen - con):.2f} W")

            self.dynamicTempLabel.config(text=f"Temp: {data['temp']}°")
            self.dynamicHumidityLabel.config(text=f"Humidity: {data['humidity']}%")

            avg = (data['tl_ldr'] + data['tr_ldr'] + data['bl_ldr'] + data['br_ldr']) / 4
            self.dynamicAvgLDRLabel.config(text=f"Avg LDR: {avg:.2f} lx")

        except:
            pass

        try:
            self.loggerListbox.delete(0, tk.END)
            data = self.getRowsFromToday("logger")
            for row in data:
                self.loggerListbox.insert(tk.END, f"{dt.datetime.fromtimestamp(row['timestamp']).time().strftime('%H:%M:%S')} - ({'Dynamic' if row['system'] else 'Static'}): {row['message']}")

            self.loggerListbox.see(tk.END)
        except:
            pass

        try:
            self.root.after(1000, self.update)
        except:
            pass

if __name__ == "__main__":
    root = tk.Tk()
    app = DatabaseApp(root)
    
    app.updateFuncId = root.after(1000, app.update)
    app.updateGraphFuncId = root.after(1000, app.update_graphs)
    root.mainloop()
