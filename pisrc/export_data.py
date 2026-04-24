#
# file: export_data.py
# Exports the given day's worth of sensor data to json files
#

import sqlite3
import json
from datetime import datetime
import sys

def execute_raw_query(table, start, end):
    cur.execute(f"""
                SELECT *
                FROM {table}
                WHERE timestamp >= {int(start.timestamp())} and timestamp <= {int(end.timestamp())}
                """)

def execute_bin_query(bin_minutes, table, start, end):
    avg_cmd = "AVG(panelV * panelA)" if table == "static_data" else "AVG(panelV * panelA - systemV * systemA)"
    cur.execute(f"""
            SELECT
                (timestamp / {bin_minutes * 60}) * {bin_minutes * 60} as bin_start,
                ROUND({avg_cmd}, 3) as avg_power,
                {'ROUND(AVG(azimuth), 3) as avg_azimuth,ROUND(AVG(zenith), 3) as avg_zenith,' if table == "dynamic_data" else ''}
                ROUND(AVG(temp), 3) as avg_temp,
                ROUND(AVG(humidity), 3) as avg_humidity,
                ROUND(AVG((tl_ldr + bl_ldr + tr_ldr + br_ldr) / 4), 3) as avg_ldr,
                ROUND(AVG(((tl_ldr + bl_ldr) / 2) - ((tr_ldr + br_ldr) / 2)), 3) as avg_horizLDRDiff,
                ROUND(AVG(((tl_ldr + tr_ldr) / 2) - ((bl_ldr + br_ldr) / 2)), 3) as avg_vertLDRDiff
            FROM {table}
            WHERE timestamp >= {int(start.timestamp())} and timestamp <= {int(end.timestamp())}
            GROUP BY bin_start
            ORDER BY bin_start
            """)

def export(table, start, end, cur, date_string):
    execute_raw_query(table, start, end)
    rows = cur.fetchall()
    data = [dict(row) for row in rows]

    with open(f"{table.split('_')[0]}_{date_string}_raw.json", "w") as f:
        json.dump(data, f)

    execute_bin_query(1, table, start, end)
    rows = cur.fetchall()
    data = [dict(row) for row in rows]

    with open(f"{table.split('_')[0]}_{date_string}_bin1.json", "w") as f:
        json.dump(data, f)

    execute_bin_query(15, table, start, end)
    rows = cur.fetchall()
    data = [dict(row) for row in rows]

    with open(f"{table.split('_')[0]}_{date_string}_bin15.json", "w") as f:
        json.dump(data, f)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: export_data.py MM-DD-YYYY")
        exit()

    date_string = sys.argv[1]
    date = datetime.strptime(date_string, "%m-%d-%Y")

    conn = sqlite3.connect("solar.db")
    conn.row_factory = sqlite3.Row
    cur = conn.cursor()

    start = datetime.combine(date, datetime.min.time())
    end = datetime.combine(date, datetime.max.time())

    export("static_data", start, end, cur, date_string)
    export("dynamic_data", start, end, cur, date_string)
