#
# file: data_processing.py
# Recieves data from solar panel systems and inputs them into the database
#

import paho.mqtt.client as mqtt
import sqlite3
import json
from datetime import datetime
import time

def create_tables():
    cur.execute("""
    CREATE TABLE IF NOT EXISTS dynamic_data(
                timestamp TIMESTAMP PRIMARY KEY,
                temp REAL,
                humidity REAL,
                tl_ldr REAL,
                bl_ldr REAL,
                tr_ldr REAL,
                br_ldr REAL,
                azimuth REAL,
                zenith REAL,
                panelV REAL,
                panelA REAL,
                systemV REAL,
                systemA REAL
                )
                """)

    cur.execute("""
    CREATE TABLE IF NOT EXISTS static_data(
                timestamp TIMESTAMP PRIMARY KEY,
                temp REAL,
                humidity REAL,
                tl_ldr REAL,
                bl_ldr REAL,
                tr_ldr REAL,
                br_ldr REAL,
                panelV REAL,
                panelA REAL
                )
                """)

    cur.execute("""
    CREATE TABLE IF NOT EXISTS logger(
                timestamp TIMESTAMP PRIMARY KEY,
                system BOOLEAN,
                message VARCHAR(100)
                )
                """)

def on_message(_, __, msg: mqtt.MQTTMessage):
    if(msg.topic == "solar/logger"):
        try:
            data = json.loads(msg.payload.decode())
            timestamp = time.time()
            cur.execute("INSERT INTO logger VALUES (?,?,?)", (timestamp, data["system"], data["message"]))

            with open("log.txt", "a") as f:
                f.write(str(datetime.now()) + " - " + msg.payload.decode() + "\n")
        except Exception as e:
            print("solar/logger Error putting into DB", e)
            pass
        
        return
            
    try:
        data = json.loads(msg.payload.decode())
        timestamp = int(time.time())
        print(str(timestamp) + " - " + msg.payload.decode())

        if(msg.topic == "solar/dynamic"):
            cur.execute("INSERT INTO dynamic_data VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)",
                    (
                        timestamp,
                        data["temp"],
                        data["humidity"],
                        data["tl_ldr"],
                        data["bl_ldr"],
                        data["tr_ldr"],
                        data["br_ldr"],
                        data["azimuth"],
                        data["zenith"],
                        data["panelV"],
                        data["panelA"],
                        data["systemV"],
                        data["systemA"],
                    ))
        else:
            cur.execute("INSERT INTO static_data VALUES (?,?,?,?,?,?,?,?,?)",
                    (
                        timestamp,
                        data["temp"],
                        data["humidity"],
                        data["tl_ldr"],
                        data["bl_ldr"],
                        data["tr_ldr"],
                        data["br_ldr"],
                        data["panelV"],
                        data["panelA"],
                    ))
        conn.commit()
    except Exception as e:
        print("Error putting into DB", e)
        pass

if __name__ == "__main__":
    conn = sqlite3.connect("solar.db")
    conn.row_factory = sqlite3.Row
    cur = conn.cursor()

    create_tables()

    print("Running Data Processing...")
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.connect("192.168.1.162", 1883)

    client.subscribe("solar/logger")
    client.subscribe("solar/dynamic")
    client.subscribe("solar/static")

    client.on_message = on_message
    client.loop_forever()