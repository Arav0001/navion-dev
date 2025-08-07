import csv

THRUST_CURVE_FILE = 'data/AeroTech_G12ST.csv'
CLEAN_THRUST_CURVE_FILE = 'data/AeroTech_G12ST_clean.csv'

with open(THRUST_CURVE_FILE, 'r') as infile, open(CLEAN_THRUST_CURVE_FILE, 'w', newline='') as outfile:
    reader = csv.reader(infile)
    writer = csv.writer(outfile)

    for row in reader:
        if (len(row) != 2):
            continue
        try:
            float(row[0])
            float(row[1])
            writer.writerow(row)
        except:
            continue